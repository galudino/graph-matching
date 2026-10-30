# graph-matching: C++ implementation

Parallel graph matching with one-way and N-way handshaking, in C++23. It is
written separately from the [C implementation](../c/README.md) and shares
none of its code, only the test data and tools at the repository root.

Commands below run from the repository root. See the
[root README](../README.md) for building, test matrices, and releases.

## Status

In progress. So far there is the project structure and the command-line
program, `graph_matching_cpp`, which takes the same arguments as the C program
but does not match yet:

```bash
./build/debug/bin/graph_matching_cpp --help
```

### From VS Code

The same tasks and launch configurations as the C program, for
`graph_matching_cpp`. Run the tasks with **Tasks: Run Task**; they build
first, then ask for the matrix, number of threads, and (for N-way) hands per
vertex:

| Task | Does |
|------|------|
| **Run (C++): One-Way Matching** | Writes the result to `<build dir>/one_way_cpp.out` |
| **Run (C++): N-Way Matching** | Writes the result to `<build dir>/n_ways_cpp.out` |
| **Run (C++): Greedy Matching** | Writes the result to `<build dir>/greedy_cpp.out` |
| **Run (C++): Compare One-Way with Reference** | Runs `compare_with_reference.sh` on this program |
| **Run (C++): Compare N-Way with Reference** | The same, for N-way |
| **CMake: Documentation (C++)** | Builds the API reference |

To debug, use the **CMake: Debug graph_matching_cpp (one-way)** or
**(N-way)** launch configuration, which run on `data/test.mtx` with 2
threads.

**The API reference** is at
**https://galudino.github.io/graph-matching/cpp/**. To build it locally, run
`cmake --build --preset debug --target docs_cpp` and open
`build/debug/cpp/docs/html/index.html`.

## Layout

```text
app/main.cpp                   Command-line matcher (graph_matching_cpp)
components/                    Libraries, one folder each (none yet)
tests/                         Tests
docs/mainpage.md               Front page of the API reference
cmake/CompilerWarnings.cmake   Warning flags
```

## Conventions

So the two implementations build side by side without clashes:

| | Name |
|---|---|
| Component targets | `graph_matching_cpp_<name>`, alias `graph_matching_cpp::<name>` |
| Headers | `components/<name>/include/graph_matching_cpp/` |
| Programs | `graph_matching_cpp`, and a `_cpp` suffix on any others |
| Custom targets | A `_cpp` suffix, such as `docs_cpp` |
| CMake options and variables | `GRAPH_MATCHING_CPP_` prefix |
| Test names | `cpp/` prefix, such as `doctest_discover_tests(<target> TEST_PREFIX "cpp/")` |

It also builds on its own:

```bash
cmake -S cpp -B build/cpp-only -G Ninja && cmake --build build/cpp-only
```
