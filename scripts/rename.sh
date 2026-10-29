#!/usr/bin/env bash
#
# Rename the template's project, namespace, include directory, and targets,
# and claim its file headers and LICENSE for the new project's author.
#
#   ./scripts/rename.sh my_sketch ["Your Name"]
#
# Replaces cpp_starter/cpp-starter, starter (namespace, starter:: aliases,
# starter_* targets, include/starter/) and STARTER_ (guards, options) with
# the new name. Sets every \author in the file headers, and the LICENSE
# copyright holder, to the author: the second argument, or else
# `git config user.name`. Sets every \date to today. Then deletes this
# script. Run it once, from a clean tree.
#

set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 || ! $1 =~ ^[a-z][a-z0-9_]*$ ]]; then
    echo "usage: $0 <name> [author]   (name in lower_snake_case, e.g. my_sketch)" >&2
    exit 1
fi

name=$1
upper=$(tr '[:lower:]' '[:upper:]' <<<"$name")
dashed=${name//_/-}

cd "$(git rev-parse --show-toplevel)"

author=${2:-$(git config user.name || true)}

if [[ -n $(git status --porcelain) ]]; then
    echo "error: commit or stash your changes first" >&2
    exit 1
fi

git ls-files -z -- ':!LICENSE' ':!scripts/rename.sh' |
    xargs -0 perl -pi -e "
        s/cpp-starter/${dashed}/g;
        s/cpp_starter/${name}/g;
        s/STARTER_/${upper}_/g;
        s/\\bstarter/${name}/g;
    "

for dir in components/*/include/starter; do
    git mv "$dir" "$(dirname "$dir")/${name}"
done

# Values go through the environment so names with quotes, slashes, or
# parentheses reach perl unchanged.
export RENAME_AUTHOR=$author
export RENAME_TODAY
export RENAME_YEAR
RENAME_TODAY=$(date +%F)
RENAME_YEAR=$(date +%Y)

headers=()
while IFS= read -r -d '' file; do
    headers+=("$file")
done < <(git ls-files -z -- '*.c' '*.h' '*.cpp' '*.hpp')

perl -pi -e 's/^(\/\/\/ \\date\s+)\d{4}-\d{2}-\d{2}$/$1$ENV{RENAME_TODAY}/' "${headers[@]}"

if [[ -n $author ]]; then
    perl -pi -e 's/^(\/\/\/ \\author\s+).*$/$1$ENV{RENAME_AUTHOR}/' "${headers[@]}"
    perl -pi -e 's/^Copyright \(c\) \d{4} .*$/Copyright (c) $ENV{RENAME_YEAR} $ENV{RENAME_AUTHOR}/' LICENSE
fi

git rm -q scripts/rename.sh
rm -rf build

echo "Renamed to '${name}'."

if [[ -n $author ]]; then
    echo "File headers and LICENSE now credit '${author}'."
else
    echo "warning: no author given and git user.name is unset;" \
        "update \\author in the file headers and LICENSE by hand." >&2
fi

echo "Review with 'git diff --stat', then reconfigure:"
echo "    cmake --preset debug && cmake --build --preset debug"
