#!/usr/bin/env bash
#
# Render c/docs/assignment.md, with its SVG figures, to c/docs/assignment.pdf.
#
#   ./c/scripts/build_assignment_pdf.sh
#
# Needs pandoc, Node.js, and Playwright's Chromium. To redraw the figures
# first, run python3 c/docs/figures/generate.py.
#

set -euo pipefail

# Work from c/, where the guide's paths start.
cd "$(dirname "$0")/.."

for tool in pandoc node; do
    if ! command -v "$tool" >/dev/null; then
        echo "error: $tool is not installed" >&2
        exit 1
    fi
done

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

# Center the italic "Figure N: ..." lines under the figures.
cat >"$work/captions.lua" <<'LUA'
function Para(para)
    local only = para.content[1]
    if #para.content == 1 and only.t == "Emph" and pandoc.utils.stringify(only):match("^Figure %d") then
        return pandoc.Div(para, pandoc.Attr("", {"caption"}))
    end
end
LUA

pandoc docs/assignment.md \
    --from gfm \
    --to html5 \
    --standalone \
    --embed-resources \
    --resource-path docs \
    --css docs/pdf.css \
    --lua-filter "$work/captions.lua" \
    --no-highlight \
    --metadata pagetitle="Parallel Graph Matching by Handshaking" \
    --output "$work/assignment.html"

cat >"$work/print.js" <<'JS'
const { chromium } = require('playwright');

(async () => {
    const [html, pdf] = process.argv.slice(2);
    const browser = await chromium.launch();
    const page = await browser.newPage();
    await page.goto('file://' + html, { waitUntil: 'load' });
    await page.pdf({
        path: pdf,
        format: 'Letter',
        margin: { top: '0.8in', bottom: '0.8in', left: '0.85in', right: '0.85in' },
        displayHeaderFooter: true,
        headerTemplate: '<span></span>',
        footerTemplate:
            '<div style="width:100%; text-align:center; font-size:9px; color:#59636e;">' +
            '<span class="pageNumber"></span> / <span class="totalPages"></span></div>',
    });
    await browser.close();
})().catch((error) => {
    console.error(error.message);
    process.exit(1);
});
JS

if ! node "$work/print.js" "$work/assignment.html" "$PWD/docs/assignment.pdf"; then
    echo "error: printing failed; Playwright's Chromium is needed (npm install playwright)" >&2
    exit 1
fi

echo "Wrote c/docs/assignment.pdf"
