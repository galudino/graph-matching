///
/// \file       handshake.c
/// \brief      Thread driver for one-way and N-way handshaking.
///
/// \details    Starts the threads, runs the passes with a barrier between
///             steps, and swaps the worklists between passes. The steps
///             themselves are the assignment functions.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/handshake.h>

#include <graph_matching/handshake_worker.h>
#include <graph_matching/log.h>
#include <graph_matching/matching.h>
#include <graph_matching/n_way.h>
#include <graph_matching/one_way.h>
#include <graph_matching/prefix_sum.h>
#include <graph_matching/stopwatch.h>
#include <graph_matching/worklist.h>

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

///
/// \brief      Run passes until no vertex is left unmatched.
///
/// \param      argument    The thread's handshake_worker.
///
/// \return     NULL.
///
static void *handshake_worker_run(void *argument) {
    handshake_worker *const self = argument;
    handshake_shared *const shared = self->shared;
    pthread_barrier_t *const barrier = &shared->barrier;

    for (self->pass_index = 0;; ++self->pass_index) {
        // Step 1: every vertex extends a hand.
        if (shared->hand_count == 1) {
            one_way_extend_hand(self);
        } else {
            n_way_find_candidates(self);
            pthread_barrier_wait(barrier);
            n_way_extend_hand(self);
        }
        pthread_barrier_wait(barrier);

        // Step 2: vertices whose hands meet are matched.
        one_way_check_handshakes(self);
        pthread_barrier_wait(barrier);

        // Step 3: count the vertices still unmatched in each range...
        shared->unmatched_counts[self->thread_index] = 0;
        worklist_count_unmatched(self);
        pthread_barrier_wait(barrier);

        // ...and turn the counts into each thread's write position.
        if (self->thread_index == 0) {
            prefix_sum_exclusive(shared->unmatched_counts, shared->write_offsets,
                                 shared->thread_count);
        }
        pthread_barrier_wait(barrier);

        const int remaining = shared->write_offsets[shared->thread_count];
        if (remaining == 0) {
            break;
        }

        // Step 4: gather the unmatched vertices into the next worklist.
        worklist_compact(self);
        pthread_barrier_wait(barrier);

        self->active_count = remaining;
        int *const finished = self->active_vertices;
        self->active_vertices = self->next_active_vertices;
        self->next_active_vertices = finished;
    }

    return NULL;
}

///
/// \brief      Allocate an int array of at least one element.
///
/// \param      count       Number of elements.
///
/// \return     The array, or NULL if allocation failed.
///
static int *allocate_ints(size_t count) { return malloc((count > 0 ? count : 1) * sizeof(int)); }

///
/// \brief      Release the shared arrays; safe on partially allocated state.
///
/// \param      shared      Shared state to release.
///
static void handshake_shared_destroy(handshake_shared *const shared) {
    free(shared->strongest_neighbor);
    free(shared->candidates);
    free(shared->candidate_counts);
    free(shared->unmatched_counts);
    free(shared->write_offsets);
}

status_code handshake_match(matching *const self, const graph *input_graph,
                            const handshake_options *options, handshake_statistics *statistics) {
    if (options->thread_count < 1 || options->hand_count < 1 ||
        self->vertex_count != input_graph->vertex_count) {
        return STATUS_INVALID_ARGUMENT;
    }

    const int thread_count = options->thread_count;
    const size_t vertex_count = (size_t)input_graph->vertex_count;
    const size_t hand_count = (size_t)options->hand_count;

    handshake_shared shared = {
        .input_graph = input_graph,
        .partners = self->partners,
        .thread_count = thread_count,
        .hand_count = options->hand_count,
        .strongest_neighbor = allocate_ints(vertex_count),
        .candidates = options->hand_count > 1 ? allocate_ints(vertex_count * hand_count) : NULL,
        .candidate_counts = options->hand_count > 1 ? allocate_ints(vertex_count) : NULL,
        .unmatched_counts = allocate_ints((size_t)thread_count),
        .write_offsets = allocate_ints((size_t)thread_count + 1),
    };

    int *worklist_a = allocate_ints(vertex_count);
    int *worklist_b = allocate_ints(vertex_count);
    pthread_t *threads = malloc((size_t)thread_count * sizeof(pthread_t));
    handshake_worker *workers = malloc((size_t)thread_count * sizeof(handshake_worker));

    const bool allocated = shared.strongest_neighbor != NULL && shared.unmatched_counts != NULL &&
                           shared.write_offsets != NULL && worklist_a != NULL &&
                           worklist_b != NULL && threads != NULL && workers != NULL &&
                           (options->hand_count == 1 ||
                            (shared.candidates != NULL && shared.candidate_counts != NULL));

    if (!allocated) {
        handshake_shared_destroy(&shared);
        free(worklist_a);
        free(worklist_b);
        free(threads);
        free(workers);
        return STATUS_OUT_OF_MEMORY;
    }

    // The first pass processes every vertex.
    for (size_t v = 0; v < vertex_count; ++v) {
        worklist_a[v] = (int)v;
    }

    pthread_barrier_init(&shared.barrier, NULL, (unsigned)thread_count);

    for (int i = 0; i < thread_count; ++i) {
        workers[i] = (handshake_worker){
            .shared = &shared,
            .thread_index = i,
            .active_count = input_graph->vertex_count,
            .active_vertices = worklist_a,
            .next_active_vertices = worklist_b,
            .pass_index = 0,
        };
    }

    stopwatch timer;
    stopwatch_start(&timer);

    for (int i = 0; i < thread_count; ++i) {
        const int rc = pthread_create(&threads[i], NULL, handshake_worker_run, &workers[i]);
        if (rc != 0) {
            loge("Could not create thread %d (error %d).\n", i, rc);
            abort();
        }
    }

    for (int i = 0; i < thread_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    if (statistics != NULL) {
        statistics->elapsed_ms = stopwatch_elapsed_ms(&timer);
        // Every thread runs the same passes; the last one found nothing left.
        statistics->pass_count = workers[0].pass_index + 1;
    }

    pthread_barrier_destroy(&shared.barrier);
    handshake_shared_destroy(&shared);
    free(worklist_a);
    free(worklist_b);
    free(threads);
    free(workers);

    return STATUS_OK;
}
