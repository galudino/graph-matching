# cpp-starter

A C17/C++23 CMake template for sketching projects in GitHub Codespaces,
including from an iPad. Open a Codespace and it arrives configured and
built: run, debug, and test from the CMake Tools status bar without
touching a terminal.

## Features

- C17 and C++23 (GCC 14, so `std::print` works)
- One example of each library kind: compiled C++, C with an `extern "C"`
  header, and header-only C++
- CMake presets with Ninja: `debug`, `release`, `asan-ubsan`
- doctest unit tests, discovered per test case by CTest and the VS Code
  Test Explorer
- clang-format, clang-tidy, and Doxygen
- GitHub Actions CI: GCC 14 and Clang 18, all presets, warnings as errors,
  plus a format and lint job

## Quick start in Codespaces

From the template's page on GitHub, pick one:

- **To try an idea:** **Use this template** → **Open in a codespace**. No
  repository is created yet. If the sketch is worth keeping, use
  **Publish Branch** in the Source Control view to create one. An
  unpublished Codespace is deleted after a period of inactivity (30 days
  by default), and its work with it.
- **To start a project:** **Use this template** → **Create a new
  repository**, then in the new repository **Code** → **Codespaces** →
  **Create codespace on main**.

Either way, each sketch gets its own Codespace. Then:

1. Wait for the `postCreateCommand` to finish configuring and building.
2. Use the CMake Tools buttons in the status bar to build, run, debug, and
   test. Every task in `.vscode/tasks.json` is also available from the
   Command Palette under **Tasks: Run Task**.

### Sketch first, publish later

1. **Use this template** → **Open in a codespace**. The template's files
   arrive already committed.
2. From the workspace root, name the sketch and commit
   ([details below](#start-a-new-sketch)):

   ```bash
   ./scripts/rename.sh graph_matching
   git add -A && git commit -m "Rename project to graph_matching"
   ```

3. Work as usual, committing as you go.
4. If the sketch is worth keeping, use **Publish Branch** in the Source
   Control view (or the cloud icon next to the branch name in the status
   bar) and choose a repository name. The Codespace you're in is now
   connected to the new repository: keep working and pushing from it.
5. If it isn't, delete the Codespace from
   [github.com/codespaces](https://github.com/codespaces), or let it
   expire.

The workspace folder stays `/workspaces/cpp-starter` for the life of the
Codespace, even after you publish. Leave it alone: renaming it while the
Codespace is open can leave the editor without a workspace.

A Codespace's folder is named after the repository it was created from,
so a different folder name needs a repository first. To get
`/workspaces/graph-matching`, publish the sketch as `graph-matching`,
push everything, then open a new Codespace from that repository and
delete the old one. Before deleting a Codespace for any reason, commit
and push everything: uncommitted changes, stashes, and unpushed branches
exist only inside it.

### iPad tips

- iPad keyboards usually have no function keys, so F5/F7-style shortcuts
  are out. The status bar buttons and **Tasks: Run Task** cover the same
  actions.
- The Command Palette is also in the menu (☰) → **View** → **Command Palette**.
- Safari may not draw VS Code's Markdown preview fully. To preview a file
  in its own tab, rendered by GitHub itself:

  ```bash
  gh extension install yusukebe/gh-markdown-preview   # once per Codespace
  gh markdown-preview README.md
  ```
- New Codespaces build the container image first, which takes a couple of
  minutes. To skip that, enable prebuilds under the repository's
  **Settings** → **Codespaces** → **Set up prebuild**.

## Start a new sketch

1. Rename the project and make it yours:

   ```bash
   ./scripts/rename.sh my_sketch "Your Name"
   ```

   This replaces `cpp_starter`, `starter`, and `STARTER_` throughout the
   repository and moves each `include/starter/` to `include/my_sketch/`.
   It sets every `\author` in the file headers and the `LICENSE` copyright
   holder to the name you give, or to `git config user.name` if you leave
   it out, and sets every `\date` to today. Then it deletes itself.
   Commit or stash first; it refuses to run on a dirty tree.

2. Delete the example components you don't need. Each one is a folder
   plus a line in three places:

   | Remove | And its line in |
   |--------|-----------------|
   | `components/<name>/` | `CMakeLists.txt` (`add_subdirectory`) |
   | `tests/<name>_tests.cpp` | `tests/CMakeLists.txt` (source and link) |
   | | `app/CMakeLists.txt` and `app/main.cpp`, if the app uses it |

3. Update the `project()` description in `CMakeLists.txt` and
   `PROJECT_BRIEF` in `docs/Doxyfile.in`, and replace this README.

## Layout

```text
app/                    Command-line executable (main.cpp)
components/
    greeting/           Compiled C++ library        starter::greeting
    ring_buffer/        Header-only C++ library     starter::ring_buffer
    sensor/             C library                   starter::sensor
tests/                  doctest unit tests, one file per component
cmake/                  Shared CMake helpers (compiler warnings)
docs/                   Doxygen configuration
scripts/rename.sh       One-time project rename
```

Each component has its own `CMakeLists.txt` that defines a
`starter_<name>` target with a `starter::<name>` alias, and puts its public
headers under `include/starter/`. Link against the alias:

```cmake
target_link_libraries(my_target PRIVATE starter::ring_buffer)
```

### Adding a component

Copy the component folder closest to what you need (`ring_buffer` for
header-only, `greeting` for compiled C++, `sensor` for C), rename the
files and targets inside it, and add an `add_subdirectory` line to the
root `CMakeLists.txt`.

## Command line

```bash
cmake --preset debug                    # configure
cmake --build --preset debug            # build
./build/debug/app/starter               # run
ctest --preset debug                    # test
```

Replace `debug` with any preset:

| Preset | Purpose |
|--------|---------|
| `debug` | Day-to-day development |
| `release` | Optimized build |
| `asan-ubsan` | Debug build with AddressSanitizer and UndefinedBehaviorSanitizer; any report fails the test |

To fail the build on compiler warnings (as CI does), add
`-DCMAKE_COMPILE_WARNING_AS_ERROR=ON` when configuring.

### Formatting and static analysis

```bash
git ls-files '*.c' '*.cpp' '*.h' '*.hpp' | xargs clang-format -i
run-clang-tidy -quiet -p build/debug "^$PWD/(app|components|tests)/.*\.(c|cpp)$"
```

### Documentation

```bash
cmake --build --preset debug --target docs
```

Output goes to `build/debug/docs/html/index.html`.

## Targets

| Target | Description |
|--------|-------------|
| `starter` | Command-line executable |
| `starter_greeting` | Compiled C++ library (`starter::greeting`) |
| `starter_ring_buffer` | Header-only C++ library (`starter::ring_buffer`) |
| `starter_sensor` | C library (`starter::sensor`) |
| `starter_tests` | doctest unit tests |
| `docs` | Doxygen HTML, when Doxygen is installed |

`STARTER_BUILD_TESTS` and `STARTER_BUILD_DOCS` default to `ON` when this
is the top-level project and `OFF` when it is added to another project
with `add_subdirectory` or `FetchContent`.

## Building outside Codespaces

The devcontainer has everything installed. Elsewhere you need:

- CMake 3.25+ and Ninja
- GCC 14+, or Clang 18+ with a standard library that provides `<print>`
  (libstdc++ 14 or libc++ 17)
- Network access on first configure, to fetch doctest
- Optional: clang-format, clang-tidy, Doxygen, GDB

## Limitations

- Only GCC 14 and Clang 18 on Linux are tested (locally and in CI). The
  MSVC warning flags in `cmake/CompilerWarnings.cmake` have never been
  built, and the `asan-ubsan` preset's flags are GCC/Clang-only.
- There are no `install()` or package export rules. Add them if a sketch
  grows into a library that other projects consume.

## Maintaining the template

GitHub only shows **Use this template** when **Template repository** is
ticked under the repository's **Settings** → **General**. A fork or copy
of this repository needs that setting turned on again.

## License

MIT. See [LICENSE](LICENSE).
