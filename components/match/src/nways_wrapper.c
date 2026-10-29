///
/// \file       nways_wrapper.c
/// \brief      Thread driver for N-way handshaking matching.
///
/// \details    Adapted from nwayswrapper.c in the CS 314 (Rutgers, Fall
///             2018) Project 3 code package.
///
/// \author     Yanhao Chen (original)
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/nways.h>

#include <graph_matching/filter.h>
#include <graph_matching/match.h>
#include <graph_matching/oneway.h>
#include <graph_matching/utils.h>

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void n_ways_handshake_wrapper(int n_threads, int n_ways, graph_data graph, int **res) {
    pthread_t *threads = malloc((size_t)n_threads * sizeof(pthread_t));
    n_way_thread_data *td = malloc((size_t)n_threads * sizeof(n_way_thread_data));

    pthread_barrier_t *barrier = malloc(sizeof(pthread_barrier_t));
    pthread_barrier_init(barrier, NULL, (unsigned)n_threads);

    // Arrays shared by every thread.
    int *strong_neighbor = malloc((size_t)graph.n_nodes * sizeof(int));
    int *node_to_process = malloc((size_t)graph.n_nodes * sizeof(int));
    int *new_node_to_process = malloc((size_t)graph.n_nodes * sizeof(int));
    int *node_count = malloc((size_t)n_threads * sizeof(int));
    int *start_locations = malloc(((size_t)n_threads + 1) * sizeof(int));
    int *n_way_graph = malloc((size_t)graph.n_nodes * (size_t)n_ways * sizeof(int));
    int *n_way_graph_degree = calloc((size_t)graph.n_nodes, sizeof(int));

    // The first pass processes every vertex.
    for (int i = 0; i < graph.n_nodes; i++) {
        node_to_process[i] = i;
    }

    for (int i = 0; i < n_threads; i++) {
        td[i].thread_id = i;
        td[i].thread_num = n_threads;
        td[i].barrier = barrier;
        td[i].n_ways = n_ways;
        td[i].graph = graph;
        td[i].res = *res;
        td[i].strong_neighbor = strong_neighbor;
        td[i].n_way_graph = n_way_graph;
        td[i].n_way_graph_degree = n_way_graph_degree;
        td[i].node_to_process = node_to_process;
        td[i].node_count = node_count;
        td[i].start_locations = start_locations;
        td[i].new_node_to_process = new_node_to_process;
    }

    fprintf(stderr, "Start %d Way Matching ... ", n_ways);

    struct timespec begin_time;
    struct timespec end_time;
    set_time(&begin_time);

    for (int i = 0; i < n_threads; i++) {
        const int rc = pthread_create(&threads[i], NULL, n_ways_handshake, &td[i]);
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
    fprintf(stderr, "%d Way Handshaking Matching Time: %.2f ms\n", n_ways,
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
    free(n_way_graph);
    free(n_way_graph_degree);
}

void *n_ways_handshake(void *thread_arg) {
    const n_way_thread_data *td = thread_arg;
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
    const int n_ways = td->n_ways;
    int *n_way_graph = td->n_way_graph;
    int *n_way_graph_degree = td->n_way_graph_degree;

    int remain_nodes_num = graph.n_nodes;

    long iter = 0;
    for (;; iter++) {
        // Step 1: Each vertex extends N hands, which forms the N-way graph.
        generate_n_way_graph(thread_id, thread_num, graph, n_ways, remain_nodes_num,
                             node_to_process, res, n_way_graph_degree, n_way_graph);
        pthread_barrier_wait(barrier);

        // Step 2: Remove N-way edges without a handshake; at the same time,
        // each vertex extends a hand to its strongest remaining neighbor.
        prune_n_way_graph(thread_id, thread_num, n_ways, remain_nodes_num, node_to_process,
                          n_way_graph_degree, n_way_graph, strong_neighbor);
        pthread_barrier_wait(barrier);

        // Step 3: Each vertex checks whether its hand was shaken.
        check_handshaking(thread_id, thread_num, remain_nodes_num, node_to_process, strong_neighbor,
                          res);
        pthread_barrier_wait(barrier);

        // Step 4: Filter out the matched vertices.
        // Step 4.1: Each thread counts the unmatched vertices in its share.
        count_unmatched_vertices(thread_id, thread_num, remain_nodes_num, node_to_process, res,
                                 node_count);
        pthread_barrier_wait(barrier);

        // Step 4.2: Prefix-sum the counts into each thread's write position.
        if (thread_id == 0) {
            exclusive_prefix_sum(node_count, start_locations, thread_num);
        }
        pthread_barrier_wait(barrier);

        // The matching is maximal once no unmatched vertex remains.
        if (start_locations[thread_num] == 0) {
            break;
        }

        // Step 4.3: Each thread copies its unmatched vertices into new_node_to_process.
        update_remain_nodes_index(thread_id, thread_num, node_to_process, start_locations, res,
                                  remain_nodes_num, new_node_to_process);
        pthread_barrier_wait(barrier);

        remain_nodes_num = start_locations[thread_num];
        swap_int_array(&node_to_process, &new_node_to_process);
    }

    pthread_exit((void *)(intptr_t)iter);
}
