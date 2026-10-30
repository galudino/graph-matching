# graph-matching: C implementation

Parallel maximal matching of weighted graphs with one-way and N-way
handshaking, using POSIX threads. This is the C implementation's API
reference; the [assignment guide](https://github.com/galudino/graph-matching/blob/main/c/docs/assignment.md)
(also as a [PDF](https://github.com/galudino/graph-matching/blob/main/c/docs/assignment.pdf))
explains the algorithms and what each assignment function must do. The source
is on [GitHub](https://github.com/galudino/graph-matching).

## Components

| Component | Headers | Contents |
|-----------|---------|----------|
| support   | status_code.h, log.h, stopwatch.h, prefix_sum.h | Status codes, logging, timing, prefix sums |
| graph     | edge_list.h, graph.h | Edge lists, and graphs stored as adjacency arrays |
| matching  | matching.h | Matchings: validation, file I/O, and the greedy baseline |
| handshake | handshake.h, handshake_worker.h | The parallel driver and the state it shares between threads |
| generator | matrix_generator.hpp | Random test graphs (C++) |

## The assignment

The functions to implement are declared in three headers:

- worklist.h: splitting the work between threads, and removing finished
  vertices.
- one_way.h: one-way handshaking.
- n_way.h: N-way handshaking (extra credit).

Each function's documentation says exactly what it must write. They operate on
a handshake_worker, whose handshake_shared holds the input graph, the partners
being computed, and the per-pass scratch arrays.
