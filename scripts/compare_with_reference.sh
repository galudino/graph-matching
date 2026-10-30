#!/usr/bin/env bash
#
# Run this project's build and the reference binary on the same input, then
# compare the two matchings.
#
#   ./scripts/compare_with_reference.sh data/test.mtx one-way 4
#   ./scripts/compare_with_reference.sh data/generated/small.mtx n-way 2 4
#
# The arguments after the input file are the matcher's method arguments; the
# reference binary gets the equivalent in its own syntax (oneWay, nWays).
# Build first (cmake --build --preset debug). Set GRAPH_MATCHING_BIN to
# compare a different build, such as build/release/app/graph_matching.
#

set -euo pipefail

usage() {
    echo "usage: $0 <input.mtx> one-way <threads>" >&2
    echo "       $0 <input.mtx> n-way <hands> <threads>" >&2
    exit 1
}

[[ $# -ge 3 ]] || usage

root=$(git rev-parse --show-toplevel)
binary=${GRAPH_MATCHING_BIN:-$root/build/debug/app/graph_matching}
reference=${GRAPH_MATCHING_REFERENCE:-$root/reference/match-sol}

input=$1
method=$2
shift 2

case $method in
one-way)
    [[ $# -eq 1 ]] || usage
    reference_args=(oneWay "$1")
    ;;
n-way)
    [[ $# -eq 2 ]] || usage
    reference_args=(nWays "$1" "$2")
    ;;
*)
    usage
    ;;
esac

if [[ ! -x $binary ]]; then
    echo "error: $binary not found; build first: cmake --build --preset debug" >&2
    exit 1
fi

out_dir=$(mktemp -d)
trap 'rm -rf "$out_dir"' EXIT

echo "== yours: $binary"
"$binary" "$input" "$out_dir/yours.out" "$method" "$@" || true

echo
echo "== reference: $reference"
"$reference" "$input" "$out_dir/reference.out" "${reference_args[@]}"

echo
if cmp -s "$out_dir/yours.out" "$out_dir/reference.out"; then
    echo "Same matching as the reference ($(wc -l <"$out_dir/yours.out") vertices)."
else
    echo "Different from the reference (line N is vertex N - 1; < yours, > reference):"
    diff "$out_dir/yours.out" "$out_dir/reference.out" | head -20
    exit 1
fi
