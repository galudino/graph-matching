#!/usr/bin/env bash
#
# Run this project's build and the instructor's reference binary on the same
# input, then compare the two matchings.
#
#   ./scripts/compare_with_reference.sh data/test.mtx oneWay 4
#   ./scripts/compare_with_reference.sh data/cant/cant.mtx nWays 2 4
#
# The arguments after the input file are passed to both programs. Build
# first (cmake --build --preset debug). Set GRAPH_MATCHING_BIN to compare a
# different build, such as build/release/app/graph_matching.
#

set -euo pipefail

if [[ $# -lt 3 ]]; then
    echo "usage: $0 <input.mtx> oneWay <threads>" >&2
    echo "       $0 <input.mtx> nWays <hands> <threads>" >&2
    exit 1
fi

root=$(git rev-parse --show-toplevel)
binary=${GRAPH_MATCHING_BIN:-$root/build/debug/app/graph_matching}
reference=${GRAPH_MATCHING_REFERENCE:-$root/reference/match-sol}

input=$1
shift

if [[ ! -x $binary ]]; then
    echo "error: $binary not found; build first: cmake --build --preset debug" >&2
    exit 1
fi

out_dir=$(mktemp -d)
trap 'rm -rf "$out_dir"' EXIT

echo "== yours: $binary"
"$binary" "$input" "$out_dir/yours.out" "$@"

echo
echo "== reference: $reference"
"$reference" "$input" "$out_dir/reference.out" "$@"

echo
if cmp -s "$out_dir/yours.out" "$out_dir/reference.out"; then
    echo "Same matching as the reference ($(wc -l <"$out_dir/yours.out") vertices)."
else
    echo "Different from the reference (line N is vertex N - 1; < yours, > reference):"
    diff "$out_dir/yours.out" "$out_dir/reference.out" | head -20
    exit 1
fi
