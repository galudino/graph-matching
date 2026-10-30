# graph-matching

Parallel graph matching with one-way and N-way handshaking, using POSIX
threads. The algorithms are written in C first, in an object-oriented style
that maps onto C++ classes, ahead of a C++ version.

**The assignment** is described in [docs/assignment.md](docs/assignment.md)
(also as a [PDF](docs/assignment.pdf)): what each function must do, worked
examples, and how to check your work.

| File | Functions |
|------|-----------|
| `components/handshake/src/worklist.c` | `worklist_range`, `worklist_count_unmatched`, `worklist_compact` |
| `components/handshake/src/one_way.c` | `one_way_extend_hand`, `one_way_check_handshakes` |
| `components/handshake/src/n_way.c` | `n_way_find_candidates`, `n_way_extend_hand` (extra credit) |

These start as empty functions. Until they are implemented, a handshaking
run stops after one pass and reports that the vertices are still unmatched.

## Build, test, and run

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

The unit tests cover everything except the assignment. The assignment's own
tests are in a separate preset, since they fail until it is done:

```bash
cmake --preset assignment && cmake --build --preset assignment && ctest --preset assignment
```

Run the matcher with one of three methods:

```bash
# One-way on 4 threads; N-way with 2 hands on 4 threads; the greedy baseline.
./build/debug/app/graph_matching data/test.mtx out.txt one-way 4
./build/debug/app/graph_matching data/test.mtx out.txt n-way 2 4
./build/debug/app/graph_matching data/test.mtx out.txt greedy
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
| **Run: One-Way Matching** | Writes the result to `<build dir>/one_way.out` |
| **Run: N-Way Matching** | Writes the result to `<build dir>/n_ways.out` |
| **Run: Greedy Matching** | Writes the result to `<build dir>/greedy.out` |
| **Run: Compare One-Way with Reference** | Runs `compare_with_reference.sh` on your build |
| **Run: Compare N-Way with Reference** | The same, for N-way |
| **Test: Assignment** | Builds and runs the assignment tests |
| **Data: Generate Test Matrices** | Writes the small, medium, and large generated matrices |
| **Data: Fetch Test Matrices** | Runs `fetch_matrices.sh` for the SuiteSparse matrices |

To debug, use the **CMake: Debug graph_matching (one-way)** or **(N-way)**
launch configuration, which run on `data/test.mtx` with 2 threads, or
**CMake: Debug graph_matching_assignment_tests**.

### From CMake

The `run_one_way`, `run_n_ways`, and `run_greedy` targets build the program
and run it, writing `one_way.out`, `n_ways.out`, or `greedy.out` in the build
directory:

```bash
cmake --build --preset debug --target run_one_way
```

To change the input, set these when configuring:

```bash
cmake --preset debug \
    -DGRAPH_MATCHING_RUN_MATRIX=data/generated/medium.mtx \
    -DGRAPH_MATCHING_RUN_THREADS=8 \
    -DGRAPH_MATCHING_RUN_HANDS=3
```

CMake turns a relative `GRAPH_MATCHING_RUN_MATRIX` into an absolute path from
the directory you run `cmake` in, which is the repository root when you use
presets.

## Test matrices

`data/test.mtx` is a 9-vertex example. For anything larger, generate random
matrices or download the SuiteSparse ones.

### Generated matrices

`generate_matrix` writes a random graph as a symmetric Matrix Market file:

```bash
# 50,000 vertices with average degree 12; then the same with weights 1-100 and seed 7.
./build/debug/app/generate_matrix data/generated/mine.mtx 50000 12
./build/debug/app/generate_matrix data/generated/mine.mtx 50000 12 100 7
```

Weights are whole numbers from 1 to the maximum (default 10), so many edges
tie and the tie-breaking by vertex ID gets exercised; raise the maximum for
fewer ties. The same arguments always produce the same file.

The `generate_matrices` target (or the **Data: Generate Test Matrices** task)
writes a standard set to `data/generated/`, which git ignores:

| File | Vertices | Average degree | Size |
|------|----------|----------------|------|
| `small.mtx` | 1,000 | 8 | 39 KB |
| `medium.mtx` | 100,000 | 16 | 11 MB |
| `large.mtx` | 1,000,000 | 16 | 122 MB |

```bash
cmake --build --preset debug --target generate_matrices
```

### SuiteSparse matrices

Six larger matrices come from the SuiteSparse Matrix Collection:

```bash
./scripts/fetch_matrices.sh              # all six, smallest first
./scripts/fetch_matrices.sh cant hood    # only these
```

They are extracted to `data/<name>/<name>.mtx` and ignored by git. `ldoor`
and `af_shell10` are hundreds of megabytes each.

The download host has refused requests from cloud machines, including
Codespaces. If it refuses you, download the `.tar.gz` from a browser on
another network instead (the matrix's page, such as
<https://sparse.tamu.edu/Williams/cant>, has the link), drag it into `data/`
in the Explorer, and extract it there:

```bash
cd data && tar -xzf cant.tar.gz && rm cant.tar.gz
```

## Checking results

- **Greedy:** one-way handshaking produces exactly the greedy matching, so
  `cmp` its output with `greedy`'s on any graph.
- **Reference binary:** `reference/match-sol` is a reference implementation
  (x86-64 Linux). `compare_with_reference.sh` runs it and your build on the
  same input and diffs the results:

  ```bash
  ./scripts/compare_with_reference.sh data/test.mtx one-way 4
  ./scripts/compare_with_reference.sh data/generated/small.mtx n-way 2 4
  ```

## Layout

Each module is a set of typedef'd structs with functions that take the
instance first (`graph *self`, or `const graph *self` when they don't change
it), so that each maps onto a C++ class.

```text
app/main.c                     Command-line matcher
app/generate_matrix.cpp        Random test matrix generator (C++)
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
data/test.mtx                  Small example matrix
docs/assignment.md             Assignment guide; figures/ and the PDF build alongside
reference/match-sol            Reference binary
scripts/                       Matrix download, reference comparison, PDF build
```

## Rebuilding the guide

The figures in `docs/figures/` are drawn by `docs/figures/generate.py`, and
`scripts/build_assignment_pdf.sh` renders `docs/assignment.md` to
`docs/assignment.pdf`. The PDF build needs pandoc, Node.js, and Playwright's
Chromium, which the devcontainer does not include.

```bash
python3 docs/figures/generate.py
./scripts/build_assignment_pdf.sh
```

## Credits and license

The harness, test matrix, and reference binary are adapted from
course-provided starter code. `mmio` is from NIST's
[Matrix Market](https://math.nist.gov/MatrixMarket/). The MIT license in
[LICENSE](LICENSE) covers this project's own code only.
