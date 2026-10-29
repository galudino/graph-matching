///
/// \file       oneway_wrapper.c
/// \brief      Thread driver for one-way handshaking matching.
///
/// \details    Adapted from onewaywrapper.c in the CS 314 (Rutgers, Fall
///             2018) Project 3 code package.
///
/// \author     Yanhao Chen (original)
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/oneway.h>

#include <graph_matching/filter.h>
#include <graph_matching/match.h>
#include <graph_matching/utils.h>

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void one_way_handshake_wrapper(int n_threads, graph_data graph, int **res) {
    pthread_t *threads = malloc((size_t)n_threads * sizeof(pthread_t));
    one_way_thread_data *td = malloc((size_t)n_threads * sizeof(one_way_thread_data));

    pthread_barrier_t *barrier = malloc(sizeof(pthread_barrier_t));
    pthread_barrier_init(barrier, NULL, (unsigned)n_threads);

    // Arrays shared by every thread.
    int *strong_neighbor = malloc((size_t)graph.n_nodes * sizeof(int));
    int *node_to_process = malloc((size_t)graph.n_nodes * sizeof(int));
    int *new_node_to_process = malloc((size_t)graph.n_nodes * sizeof(int));
    int *node_count = malloc((size_t)n_threads * sizeof(int));
    int *start_locations = malloc(((size_t)n_threads + 1) * sizeof(int));

    // The first pass processes every vertex.
    for (int i = 0; i < graph.n_nodes; i++) {
        node_to_process[i] = i;
    }

    for (int i = 0; i < n_threads; i++) {
        td[i].thread_id = i;
        td[i].thread_num = n_threads;
        td[i].barrier = barrier;
        td[i].graph = graph;
        td[i].res = *res;
        td[i].strong_neighbor = strong_neighbor;
        td[i].node_to_process = node_to_process;
        td[i].node_count = node_count;
        td[i].start_locations = start_locations;
        td[i].new_node_to_process = new_node_to_process;
    }

    fprintf(stderr, "Start One Way Matching ... ");

    struct timespec begin_time;
    struct timespec end_time;
    set_time(&begin_time);

    for (int i = 0; i < n_threads; i++) {
        const int rc = pthread_create(&threads[i], NULL, one_way_handshake, &td[i]);
        if (rc != 0) {
            ERROR("pthread_create failed with thread %d with return code %d\n", i, rc);
            exit(EXIT_FAILURE);
        }
    }

    // Every thread runs the same number of passes, so any thread's count will do.
    long iter = 0;
    for (int i = 0; i < n_threads; i++) {
        void *thread_result = NULL;
        const int rc = pthread_join(threads[i], &thread_result);
        if (rc != 0) {
            ERROR("pthread_join failed with thread %d with return code %d\n", i, rc);
            exit(EXIT_FAILURE);
        }
        iter = (long)(intptr_t)thread_result;
    }

    set_time(&end_time);

    fprintf(stderr, "SUCCESS\n");
    fprintf(stderr, "Find a maximal matching after %ld iterations\n", iter);
    fprintf(stderr, "One Way Handshaking Matching Time: %.2f ms\n",
            get_time(&begin_time, &end_time));

    pthread_barrier_destroy(barrier);
    free(barrier);
    free(threads);
    free(td);
    free(strong_neighbor);
    free(node_to_process);
    free(new_node_to_process);
    free(node_count);
    free(start_locations);
}

void *one_way_handshake(void *thread_arg) {
    const one_way_thread_data *td = thread_arg;
    const int thread_id = td->thread_id;
    const int thread_num = td->thread_num;
    pthread_barrier_t *barrier = td->barrier;
    const graph_data graph = td->graph;
    int *res = td->res;
    int *strong_neighbor = td->strong_neighbor;
    int *node_to_process = td->node_to_process;
    int *node_count = td->node_count;
    int *start_locations = td->start_locations;
    int *new_node_to_process = td->new_node_to_process;

    int remain_nodes_num = graph.n_nodes;

    long iter = 0;
    for (;; iter++) {
        // Step 1: Each vertex extends a hand to its strongest neighbor.
        extend_one_hand(thread_id, thread_num, graph, remain_nodes_num, node_to_process, res,
                        strong_neighbor);
        pthread_barrier_wait(barrier);

        // Step 2: Each vertex checks whether its hand was shaken.
        check_handshaking(thread_id, thread_num, remain_nodes_num, node_to_process, strong_neighbor,
                          res);
        pthread_barrier_wait(barrier);

        // Step 3: Filter out the matched vertices.
        // Step 3.1: Each thread counts the unmatched vertices in its share.
        count_unmatched_vertices(thread_id, thread_num, remain_nodes_num, node_to_process, res,
                                 node_count);
        pthread_barrier_wait(barrier);

        // Step 3.2: Prefix-sum the counts into each thread's write position.
        if (thread_id == 0) {
            exclusive_prefix_sum(node_count, start_locations, thread_num);
        }
        pthread_barrier_wait(barrier);

        // The matching is maximal once no unmatched vertex remains.
        if (start_locations[thread_num] == 0) {
            break;
        }

        // Step 3.3: Each thread copies its unmatched vertices into new_node_to_process.
        update_remain_nodes_index(thread_id, thread_num, node_to_process, start_locations, res,
                                  remain_nodes_num, new_node_to_process);
        pthread_barrier_wait(barrier);

        remain_nodes_num = start_locations[thread_num];
        swap_int_array(&node_to_process, &new_node_to_process);
    }

    pthread_exit((void *)(intptr_t)iter);
}
