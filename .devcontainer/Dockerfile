FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

# GCC 14 provides <print> and the rest of the C++23 library that GCC 13
# (Ubuntu 24.04's default) lacks. devcontainer.json selects it via CC/CXX.
# libclang-rt provides the sanitizer runtimes for the asan-ubsan preset with Clang.
# gh (GitHub CLI) is in the default Codespaces image but not this base image.
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        g++-14 \
        clang \
        libclang-rt-18-dev \
        clang-format \
        clang-tidy \
        cmake \
        ninja-build \
        gdb \
        gh \
        doxygen \
        graphviz \
    && rm -rf /var/lib/apt/lists/*
