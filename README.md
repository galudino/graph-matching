# graph-matching

Parallel graph matching with one-way and N-way handshaking, using POSIX
threads. This revisits a course project: the algorithm is implemented in C
first, as the assignment asks, then refactored to C++.

## The assignment

Three files are the assignment; everything else is the starter code's harness:

| File | Functions |
|------|-----------|
| `components/match/src/oneway.c` | `extend_one_hand`, `check_handshaking` |
| `components/match/src/filter.c` | `count_unmatched_vertices`, `update_remain_nodes_index` |
| `components/match/src/nways.c` | `generate_n_way_graph`, `prune_n_way_graph` (extra credit) |

Each function is declared, with its inputs and outputs, in the matching header
under `components/match/include/graph_matching/`.

The files start as empty functions so that the project builds. Until they are
implemented, the program hangs instead of producing a matching: the filter
step never reports that the work list is empty.

### Names compared with the starter code

The code uses snake_case throughout, so names differ from the starter code's
camelCase:

| Starter code | Here |
|------|------|
| `GraphData`, `nNodes`, `nEdges` | `graph_data`, `n_nodes`, `n_edges` |
| `nodeToProcess`, `newNodeToProcess` | `node_to_process`, `new_node_to_process` |
| `strongNeighbor`, `nodeCount`, `startLocations` | `strong_neighbor`, `node_count`, `start_locations` |
| `nWays`, `nWayGraph`, `nWayGraphDegree` | `n_ways`, `n_way_graph`, `n_way_graph_degree` |
| `threadId`, `threadNum`, `nodeNum` | `thread_id`, `thread_num`, `node_num` |
| `readmm`, `DataStructure.h`, `*wrapper.c` | `read_mm`, `graph_data.h`, `*_wrapper.c` |

Input arrays are now `const int *` in the function signatures.

## Build, test, and run

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

The unit tests cover the harness (graph reading, sorting, prefix sums,
result I/O, and `check_result`), not the assignment functions.

Run with the same arguments as the original `match` program:

```bash
./build/debug/app/graph_matching data/test.mtx test.out oneWay 4      # 4 threads
./build/debug/app/graph_matching data/test.mtx test.out nWays 2 4     # 2 hands, 4 threads
```

The result has one line per vertex: its partner, or `-2`
(`NO_MATCHED_NODES`) if it has none.

### From VS Code

Run these from the Command Palette with **Tasks: Run Task**. Each one builds
first, then asks for the matrix, number of threads, and (for N-way) hands per
vertex:

| Task | Does |
|------|------|
| **Run: One-Way Matching** | Writes the result to `<build dir>/one_way.out` |
| **Run: N-Way Matching** | Writes the result to `<build dir>/n_ways.out` |
| **Run: Compare One-Way with Reference** | Runs `compare_with_reference.sh` on your build |
| **Run: Compare N-Way with Reference** | The same, for N-way |
| **Data: Fetch Test Matrices** | Runs `fetch_matrices.sh` for all six matrices |

To stop a run that hangs, use the trash-can icon on its terminal.

To debug, use the **CMake: Debug graph_matching (one-way)** or **(N-way)**
launch configuration. Both run on `data/test.mtx` with 2 threads; the N-way
one uses 2 hands.

### From CMake

The `run_one_way` and `run_n_ways` targets build the program and run it:

```bash
cmake --build --preset debug --target run_one_way
cmake --build --preset debug --target run_n_ways
```

They write `one_way.out` or `n_ways.out` in the build directory. To change
the input, set these when configuring:

```bash
cmake --preset debug \
    -DGRAPH_MATCHING_RUN_MATRIX=data/cant/cant.mtx \
    -DGRAPH_MATCHING_RUN_THREADS=8 \
    -DGRAPH_MATCHING_RUN_HANDS=3
```

CMake turns a relative `GRAPH_MATCHING_RUN_MATRIX` into an absolute path from
the directory you run `cmake` in, which is the repository root when you use
presets.

## Test matrices

`data/test.mtx` is a 9-vertex example. Six larger test matrices come from the
SuiteSparse Matrix Collection:

```bash
./scripts/fetch_matrices.sh              # all six, smallest first
./scripts/fetch_matrices.sh cant hood    # only these
```

They are extracted to `data/<name>/<name>.mtx` and ignored by git. `ldoor`
and `af_shell10` are hundreds of megabytes each. If a download fails, check
the matrix's page (for example <https://sparse.tamu.edu/Williams/cant>)
for current links.

## Checking against the reference

`reference/match-sol` is the reference binary from the starter code (x86-64
Linux). To run it and your build on the same input and compare the results:

```bash
./scripts/compare_with_reference.sh data/test.mtx oneWay 4
./scripts/compare_with_reference.sh data/cant/cant.mtx nWays 2 4
```

The script exits nonzero and shows the first differences if the matchings
differ.

## Layout

```text
app/main.c                  Command-line program (harness)
components/match/           Matching library
    include/graph_matching/   Headers
    src/oneway.c              Assignment
    src/filter.c              Assignment
    src/nways.c               Assignment (extra credit)
    src/*_wrapper.c           Thread drivers (harness)
    src/utils.c, src/sort.c   Graph I/O and helpers (harness)
third_party/mmio/           NIST Matrix Market I/O library, unchanged
tests/                      doctest unit tests for the harness
data/test.mtx               Small example matrix
docs/                       Doxygen configuration
reference/match-sol         Reference binary
scripts/                    Matrix download and reference comparison
```

## Changes from the starter code

The harness behaves the same as the original: on `test.mtx` and on generated
matrices with ties, duplicate entries, self-loops, and pattern-only values,
`read_mm` builds the same graph and the sequential mode writes the same
matching. The differences:

- Formatting, Doxygen comments, and snake_case names in this project's style.
- `fscanf` results are checked, so malformed input stops with an error
  instead of leaving values unset.
- `read_mm` no longer prints an uninitialized type code when a file's banner
  is invalid, and does not call `malloc(0)` for a matrix with no edges.
- The sorts return early on empty input instead of reading past the array.
- The thread drivers free their barrier, and the N-way driver's timing line
  says "N Way" instead of "One Way".
- Timing uses `clock_gettime(CLOCK_MONOTONIC)` instead of `gettimeofday`.
- `err()` uses `vsnprintf`, so long messages are truncated instead of
  overflowing its buffer.

## Credits and license

The harness, test matrix, and reference binary are adapted from
course-provided starter code. `mmio` is from NIST's
[Matrix Market](https://math.nist.gov/MatrixMarket/). The MIT license in
[LICENSE](LICENSE) covers this project's own code only.
