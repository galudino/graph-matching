# graph-matching

Parallel graph matching with one-way and N-way handshaking, using POSIX
threads. Each implementation is its own project, with its own components,
programs, tests, and API reference; they share only the test data and tools
at the repository root.

| Implementation | Folder | Program | API reference |
|----------------|--------|---------|---------------|
| C | [`c/`](c/README.md) | `graph_matching_c` | [galudino.github.io/graph-matching/c](https://galudino.github.io/graph-matching/c/) |
| C++ (in progress) | [`cpp/`](cpp/README.md) | `graph_matching_cpp` | [galudino.github.io/graph-matching/cpp](https://galudino.github.io/graph-matching/cpp/) |

**The assignment** is part of the C implementation: see
[c/README.md](c/README.md) and the guide in
[c/docs/assignment.md](c/docs/assignment.md) (also as a
[PDF](c/docs/assignment.pdf)).

## Build and test

The presets build and test every implementation at once:

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Programs land in `build/<preset>/bin/`. Other presets are `release`,
`asan-ubsan` (address and undefined-behavior sanitizers), and `assignment`
(adds the C assignment's tests). Test names start with their
implementation's folder, `c/` or `cpp/`, so `ctest --preset debug -R '^c/'`
runs only the C tests.

Each implementation also builds on its own, such as `cmake -S c -B build/c-only`,
and `-DGRAPH_MATCHING_BUILD_C=OFF` or `-DGRAPH_MATCHING_BUILD_CPP=OFF` leaves
one out of the root build.

## Test matrices

`data/test.mtx` is a 9-vertex example. For anything larger, generate random
matrices (see [c/README.md](c/README.md#generating-test-matrices)) or
download the SuiteSparse ones:

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

## Reference binary

`reference/match-sol` is a reference implementation (x86-64 Linux).
`compare_with_reference.sh` runs it and a build on the same input and diffs
the results. It compares the C build by default; set `GRAPH_MATCHING_BIN` to
compare another program:

```bash
./scripts/compare_with_reference.sh data/test.mtx one-way 4
./scripts/compare_with_reference.sh data/generated/small.mtx n-way 2 4
```

## Layout

```text
c/                             C implementation (see c/README.md)
cpp/                           C++ implementation (see cpp/README.md)
data/test.mtx                  Small example matrix; downloads and generated matrices go here
reference/match-sol            Reference binary
scripts/                       Matrix download, reference comparison
docs/index.html                Front page of the API reference site
CMakeLists.txt                 Builds every implementation
CMakePresets.json              debug, release, asan-ubsan, assignment
```

## Releases

Each implementation is released on its own. Pushing a tag named
`<folder>/v<version>`, such as `c/v1.1.0` or `cpp/v0.1.0`, publishes a GitHub
release. The version must match `VERSION` in that folder's `CMakeLists.txt`,
and an annotated tag's message becomes the release notes. The C release
attaches the assignment PDF:

```bash
git tag -a c/v1.1.0 -m "Release notes"
git push origin c/v1.1.0
```

A release drafted in the GitHub UI works too; the workflow only attaches the
PDF to it. Releases before the split into folders use plain tags, such as
`v1.0.0`.

## Credits and license

The harness, test matrix, and reference binary are adapted from
course-provided starter code. `mmio` is from NIST's
[Matrix Market](https://math.nist.gov/MatrixMarket/). The MIT license in
[LICENSE](LICENSE) covers this project's own code only.
