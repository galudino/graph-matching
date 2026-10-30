# graph-matching: C implementation

Parallel graph matching with one-way and N-way handshaking, in C17 with
POSIX threads. Each module is a set of typedef'd structs with functions that
take the instance first (`graph *self`, or `const graph *self` when they don't
change it).

Commands below run from the repository root. See the
[root README](../README.md) for building, test matrices, and releases.

## The assignment

The assignment is described in [docs/assignment.md](docs/assignment.md)
(also as a [PDF](docs/assignment.pdf)): what each function must do, worked
examples, and how to check your work.

| File | Functions |
|------|-----------|
| `c/components/handshake/src/worklist.c` | `worklist_range`, `worklist_count_unmatched`, `worklist_compact` |
| `c/components/handshake/src/one_way.c` | `one_way_extend_hand`, `one_way_check_handshakes` |
| `c/components/handshake/src/n_way.c` | `n_way_find_candidates`, `n_way_extend_hand` (extra credit) |

These start as empty functions. Until they are implemented, a handshaking
run stops after one pass and reports that the vertices are still unmatched.

The unit tests cover everything except the assignment. The assignment's own
tests are in a separate preset, since they fail until it is done:

```bash
cmake --preset assignment && cmake --build --preset assignment && ctest --preset assignment
```

**The API reference** is at
**https://galudino.github.io/graph-matching/c/**. To build it locally, run
`cmake --build --preset debug --target docs_c` and open
`build/debug/c/docs/html/index.html`.

## Running

Run the matcher with one of three methods:

```bash
# One-way on 4 threads; N-way with 2 hands on 4 threads; the greedy baseline.
./build/debug/bin/graph_matching_c data/test.mtx out.txt one-way 4
./build/debug/bin/graph_matching_c data/test.mtx out.txt n-way 2 4
./build/debug/bin/graph_matching_c data/test.mtx out.txt greedy
```

The output has one line per vertex: its partner, or `-2`
(`VERTEX_UNMATCHABLE`) if it has none. The program then checks the result
and reports its size, weight, and whether it is maximal; it exits with an
error if the result is invalid.

### From VS Code

Run these from the Command Palette with **Tasks: Run Task**. The run tasks
build first, then ask for the matrix, number of threads, and (for N-way)
hands per vertex:

| Task | Does |
|------|------|
| **Run (C): One-Way Matching** | Writes the result to `<build dir>/one_way_c.out` |
| **Run (C): N-Way Matching** | Writes the result to `<build dir>/n_ways_c.out` |
| **Run (C): Greedy Matching** | Writes the result to `<build dir>/greedy_c.out` |
| **Run (C): Compare One-Way with Reference** | Runs `compare_with_reference.sh` on your build |
| **Run (C): Compare N-Way with Reference** | The same, for N-way |
| **Test (C): Assignment** | Builds and runs the assignment tests |

To debug, use the **CMake: Debug graph_matching_c (one-way)** or **(N-way)**
launch configuration, which run on `data/test.mtx` with 2 threads, or
**CMake: Debug graph_matching_c_assignment_tests**.

### From CMake

The `run_one_way_c`, `run_n_ways_c`, and `run_greedy_c` targets build the
program and run it, writing `one_way_c.out`, `n_ways_c.out`, or `greedy_c.out`
in the build directory:

```bash
cmake --build --preset debug --target run_one_way_c
```

To change the input, set these when configuring:

```bash
cmake --preset debug \
    -DGRAPH_MATCHING_C_RUN_MATRIX=data/generated/medium.mtx \
    -DGRAPH_MATCHING_C_RUN_THREADS=8 \
    -DGRAPH_MATCHING_C_RUN_HANDS=3
```

CMake turns a relative `GRAPH_MATCHING_C_RUN_MATRIX` into an absolute path
from the directory you run `cmake` in, which is the repository root when you
use presets.

## Generating test matrices

`generate_matrix_c` writes a random graph as a symmetric Matrix Market file:

```bash
# 50,000 vertices with average degree 12; then the same with weights 1-100 and seed 7.
./build/debug/bin/generate_matrix_c data/generated/mine.mtx 50000 12
./build/debug/bin/generate_matrix_c data/generated/mine.mtx 50000 12 100 7
```

Weights are whole numbers from 1 to the maximum (default 10), so many edges
tie and the tie-breaking by vertex ID gets exercised; raise the maximum for
fewer ties. The same arguments always produce the same file.

The `generate_matrices_c` target (or the **Data: Generate Test Matrices**
task) writes a standard set to `data/generated/`, which git ignores:

| File | Vertices | Average degree | Size |
|------|----------|----------------|------|
| `small.mtx` | 1,000 | 8 | 39 KB |
| `medium.mtx` | 100,000 | 16 | 11 MB |
| `large.mtx` | 1,000,000 | 16 | 122 MB |

```bash
cmake --build --preset debug --target generate_matrices_c
```

## Checking results

- **Greedy:** one-way handshaking produces exactly the greedy matching, so
  `cmp` its output with `greedy`'s on any graph.
- **Reference binary:** `scripts/compare_with_reference.sh` runs
  `reference/match-sol` and this build on the same input and diffs the
  results:

  ```bash
  ./scripts/compare_with_reference.sh data/test.mtx one-way 4
  ./scripts/compare_with_reference.sh data/generated/small.mtx n-way 2 4
  ```

## Layout

```text
app/main.c                     Command-line matcher (graph_matching_c)
app/generate_matrix.cpp        Random test matrix generator (generate_matrix_c, C++)
components/
    support/                   status_code, log (logd/loge/logw/logi), stopwatch, prefix_sum
    graph/                     edge_list, and graph (adjacency arrays, Matrix Market reader)
    matching/                  matching: results, file I/O, validation, greedy baseline
    handshake/                 handshake: the parallel driver, plus the assignment
        src/worklist.c           Assignment
        src/one_way.c            Assignment
        src/n_way.c              Assignment (extra credit)
    generator/                 Random graph generation (C++)
third_party/mmio/              NIST Matrix Market I/O library, unchanged
tests/                         doctest unit tests
tests/assignment/              Assignment tests (assignment preset)
docs/assignment.md             Assignment guide; figures/ and the PDF build alongside
docs/mainpage.md               Front page of the API reference
scripts/                       Assignment PDF build
```

CMake targets and options are named for this project (`graph_matching_c_*`,
`GRAPH_MATCHING_C_*`), so it builds alongside the other implementations. It
also builds on its own:

```bash
cmake -S c -B build/c-only -G Ninja && cmake --build build/c-only
```

## Rebuilding the guide

The figures in `docs/figures/` are drawn by `docs/figures/generate.py`, and
`scripts/build_assignment_pdf.sh` renders `docs/assignment.md` to
`docs/assignment.pdf`. The PDF build needs pandoc, Node.js, and Playwright's
Chromium, which the devcontainer does not include.

```bash
python3 c/docs/figures/generate.py
./c/scripts/build_assignment_pdf.sh
```
