#!/usr/bin/env bash
#
# Download the assignment's test matrices from the SuiteSparse Matrix
# Collection into data/<name>/<name>.mtx.
#
#   ./scripts/fetch_matrices.sh             # all six, smallest first
#   ./scripts/fetch_matrices.sh cant hood   # only these
#
# The larger matrices (ldoor, af_shell10) are hundreds of megabytes once
# extracted. Downloaded matrices are ignored by git.
#

set -euo pipefail

declare -A groups=(
    [dawson5]=GHS_indef
    [cant]=Williams
    [consph]=Williams
    [hood]=GHS_psdef
    [ldoor]=GHS_psdef
    [af_shell10]=Schenk_AFE
)

names=("$@")
if [[ ${#names[@]} -eq 0 ]]; then
    names=(dawson5 cant consph hood ldoor af_shell10)
fi

cd "$(git rev-parse --show-toplevel)/data"

failed=()

for name in "${names[@]}"; do
    group=${groups[$name]:-}
    if [[ -z $group ]]; then
        echo "error: unknown matrix '$name' (expected one of: ${!groups[*]})" >&2
        exit 1
    fi

    if [[ -f $name/$name.mtx ]]; then
        echo "$name: already in data/$name/"
        continue
    fi

    url="https://sparse.tamu.edu/MM/$group/$name.tar.gz"
    echo "$name: downloading $url"

    if ! curl -fL --retry 2 -o "$name.tar.gz" "$url"; then
        rm -f "$name.tar.gz"
        failed+=("$name")
        continue
    fi

    tar -xzf "$name.tar.gz"
    rm "$name.tar.gz"
    echo "$name: extracted to data/$name/$name.mtx"
done

if [[ ${#failed[@]} -gt 0 ]]; then
    echo >&2
    echo "error: could not download: ${failed[*]}" >&2
    echo "Check each matrix's page for current links, e.g. https://sparse.tamu.edu/Williams/cant" >&2
    exit 1
fi
