#!/usr/bin/env python3
"""
Generate the SVG figures for docs/assignment.md.

    python3 docs/figures/generate.py

Every value drawn here is written out by hand: the example graph is
data/test.mtx, and the pass-by-pass states are the ones the assignment tests
check (tests/assignment/handshake_tests.cpp). Nothing here runs the matching
algorithms.
"""

from pathlib import Path

OUT = Path(__file__).resolve().parent

FONT = "font-family='Helvetica, Arial, sans-serif'"
INK = "#1f2328"
MUTED = "#6e7781"
EDGE = "#8c959f"
MATCH = "#1a7f37"
HAND = "#0969da"
REJECT = "#cf222e"

# One color per thread, and a lighter tint of each for cell backgrounds.
THREAD = ["#0969da", "#bc4c00", "#8250df", "#1a7f37"]
THREAD_TINT = ["#ddf4ff", "#fff1e5", "#fbefff", "#dafbe1"]

# One tint per vertex, for the adjacency arrays.
VERTEX_TINT = ["#ffd8d3", "#d3e5ff", "#d2f4d3", "#fff0b3", "#ead3ff",
               "#ffd9ec", "#cdf1f0", "#e8e8e8", "#ffe2c2"]

# data/test.mtx: a 3x3 grid. (u, v, weight), u < v.
EDGES = [
    (0, 1, 1), (1, 2, 2),
    (0, 3, 2), (1, 4, 4), (2, 5, 4),
    (3, 4, 3), (4, 5, 4),
    (3, 6, 1), (4, 7, 3), (5, 8, 1),
    (6, 7, 1), (7, 8, 2),
]

# Neighbor lists as graph_read_matrix_market() builds them: strongest first,
# ties by vertex ID.
NEIGHBORS = [[3, 1], [4, 2, 0], [5, 1], [4, 0, 6], [1, 5, 3, 7], [2, 4, 8],
             [3, 7], [4, 8, 6], [7, 5]]
WEIGHTS = [[2, 1], [4, 2, 1], [4, 2], [3, 2, 1], [4, 4, 3, 3], [4, 4, 1],
           [1, 1], [3, 2, 1], [2, 1]]


class Svg:
    """A minimal SVG document builder."""

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.parts = []

    def add(self, element):
        self.parts.append(element)

    def text(self, x, y, content, size=14, anchor="middle", color=INK, weight="normal"):
        self.add(f"<text x='{x}' y='{y}' font-size='{size}' text-anchor='{anchor}' "
                 f"fill='{color}' font-weight='{weight}' {FONT}>{content}</text>")

    def rect(self, x, y, w, h, fill="#ffffff", stroke=INK, width=1, radius=0, dash=None):
        dash_attr = f" stroke-dasharray='{dash}'" if dash else ""
        self.add(f"<rect x='{x}' y='{y}' width='{w}' height='{h}' rx='{radius}' fill='{fill}' "
                 f"stroke='{stroke}' stroke-width='{width}'{dash_attr}/>")

    def line(self, x1, y1, x2, y2, color=EDGE, width=2, marker=None, dash=None):
        marker_attr = f" marker-end='url(#{marker})'" if marker else ""
        dash_attr = f" stroke-dasharray='{dash}'" if dash else ""
        self.add(f"<line x1='{x1}' y1='{y1}' x2='{x2}' y2='{y2}' stroke='{color}' "
                 f"stroke-width='{width}'{marker_attr}{dash_attr}/>")

    def circle(self, x, y, r, fill="#ffffff", stroke=INK, width=1.5):
        self.add(f"<circle cx='{x}' cy='{y}' r='{r}' fill='{fill}' stroke='{stroke}' "
                 f"stroke-width='{width}'/>")

    def save(self, name):
        markers = "".join(
            f"<marker id='{mid}' viewBox='0 0 10 10' refX='9' refY='5' markerWidth='7' "
            f"markerHeight='7' orient='auto-start-reverse'><path d='M 0 0 L 10 5 L 0 10 z' "
            f"fill='{color}'/></marker>"
            for mid, color in (("hand", HAND), ("reject", REJECT), ("flow", MUTED)))
        body = "\n".join(self.parts)
        svg = (f"<svg xmlns='http://www.w3.org/2000/svg' width='{self.width}' "
               f"height='{self.height}' viewBox='0 0 {self.width} {self.height}'>\n"
               f"<defs>{markers}</defs>\n"
               f"<rect width='100%' height='100%' fill='#ffffff'/>\n{body}\n</svg>\n")
        (OUT / name).write_text(svg)


# --- Grid drawings -----------------------------------------------------------

def position(v, left, top, spacing=110):
    return left + (v % 3) * spacing, top + (v // 3) * spacing


def draw_grid(svg, left, top, matched=(), unmatchable=(), weights=True, faded=()):
    """Draw the test.mtx grid. matched: pairs drawn as matched edges."""
    matched = {tuple(sorted(p)) for p in matched}
    for u, v, w in EDGES:
        x1, y1 = position(u, left, top)
        x2, y2 = position(v, left, top)
        is_match = (u, v) in matched
        svg.line(x1, y1, x2, y2, color=MATCH if is_match else EDGE, width=6 if is_match else 2)
        if weights:
            mx, my = (x1 + x2) / 2, (y1 + y2) / 2
            dx, dy = (0, -15) if y1 == y2 else (20, 5)
            svg.text(mx + dx, my + dy, str(w), size=13, color=MUTED, weight="bold")
    for v in range(9):
        x, y = position(v, left, top)
        fill = "#eeeeee" if v in faded else ("#ffebe9" if v in unmatchable else "#ffffff")
        stroke = REJECT if v in unmatchable else INK
        svg.circle(x, y, 17, fill=fill, stroke=stroke, width=2 if v in unmatchable else 1.5)
        svg.text(x, y + 5, str(v), size=15, weight="bold", color=MUTED if v in faded else INK)


def draw_hand(svg, u, v, left, top, color=HAND, marker="hand"):
    """Arrow from u toward v, offset so that two opposite hands don't overlap."""
    x1, y1 = position(u, left, top)
    x2, y2 = position(v, left, top)
    length = ((x2 - x1) ** 2 + (y2 - y1) ** 2) ** 0.5
    ux, uy = (x2 - x1) / length, (y2 - y1) / length
    ox, oy = -uy * 7, ux * 7  # offset to the arrow's left
    svg.line(x1 + ux * 22 + ox, y1 + uy * 22 + oy, x2 - ux * 22 + ox, y2 - uy * 22 + oy,
             color=color, width=2.5, marker=marker)


def figure_graph():
    svg = Svg(720, 330)
    draw_grid(svg, 70, 60)
    svg.text(180, 320, "(a) data/test.mtx: 9 vertices, 12 weighted edges", size=14)
    draw_grid(svg, 430, 60, matched=[(0, 3), (1, 4), (2, 5), (7, 8)], unmatchable=[6])
    svg.text(540, 320, "(b) A maximal matching: 4 pairs, weight 12", size=14)
    svg.save("graph.svg")


def figure_one_way():
    svg = Svg(1060, 380)
    hands1 = [(0, 3), (1, 4), (2, 5), (3, 4), (4, 1), (5, 2), (6, 3), (7, 4), (8, 7)]
    draw_grid(svg, 70, 60, weights=True)
    for u, v in hands1:
        draw_hand(svg, u, v, 70, 60)
    svg.text(180, 330, "Pass 1: every vertex extends a hand", size=14)
    svg.text(180, 350, "1 and 4, 2 and 5 shake hands", size=14, color=MATCH)

    pass2 = [(0, 3), (3, 0), (6, 3), (7, 8), (8, 7)]
    draw_grid(svg, 420, 60, matched=[(1, 4), (2, 5)], weights=True, faded=[1, 2, 4, 5])
    for u, v in pass2:
        draw_hand(svg, u, v, 420, 60)
    svg.text(530, 330, "Pass 2: only 0, 3, 6, 7, 8 are active", size=14)
    svg.text(530, 350, "0 and 3, 7 and 8 shake hands; 6 does not", size=14, color=MATCH)

    draw_grid(svg, 770, 60, matched=[(0, 3), (1, 4), (2, 5), (7, 8)], unmatchable=[6],
              faded=[0, 1, 2, 3, 4, 5, 7, 8])
    svg.text(880, 330, "Pass 3: 6 has no free neighbor left", size=14)
    svg.text(880, 350, "6 becomes VERTEX_UNMATCHABLE; done", size=14, color=REJECT)
    svg.save("one_way_passes.svg")


# --- Arrays ------------------------------------------------------------------

def draw_array(svg, left, top, label, values, cell=34, tints=None, label_width=150,
               bold=False, index_row=False):
    svg.text(left + label_width - 10, top + 23, label, size=14, anchor="end", weight="bold")
    for i, value in enumerate(values):
        x = left + label_width + i * cell
        fill = tints[i] if tints else "#ffffff"
        svg.rect(x, top, cell, 34, fill=fill)
        svg.text(x + cell / 2, top + 23, str(value), size=14,
                 weight="bold" if bold else "normal")
        if index_row:
            svg.text(x + cell / 2, top - 6, str(i), size=11, color=MUTED)


def figure_adjacency():
    offsets = [0]
    for n in NEIGHBORS:
        offsets.append(offsets[-1] + len(n))
    flat_neighbors = [u for n in NEIGHBORS for u in n]
    flat_weights = [w for ws in WEIGHTS for w in ws]
    tints = [VERTEX_TINT[v] for v, n in enumerate(NEIGHBORS) for _ in n]

    cell = 34
    svg = Svg(170 + 24 * cell + 20, 250)
    draw_array(svg, 10, 30, "neighbor_offsets", offsets,
               tints=VERTEX_TINT + ["#ffffff"], index_row=True)
    draw_array(svg, 10, 110, "neighbors", flat_neighbors, tints=tints, index_row=True)
    draw_array(svg, 10, 160, "weights", flat_weights, tints=tints)

    # Bracket each vertex's list.
    svg.text(10 + 140, 232, "vertex", size=14, anchor="end", weight="bold")
    for v in range(9):
        x1 = 10 + 150 + offsets[v] * cell
        x2 = 10 + 150 + offsets[v + 1] * cell
        svg.line(x1 + 3, 212, x2 - 3, 212, color=INK, width=1.5)
        svg.text((x1 + x2) / 2, 232, str(v), size=13, weight="bold")
    svg.save("adjacency.svg")


def figure_candidates():
    cell = 34
    svg = Svg(910, 300)
    candidates = [3, 1, 4, 2, 5, 1, 4, 0, 1, 5, 2, 4, 3, 7, 4, 8, 7, 5]
    tints = [VERTEX_TINT[i // 2] for i in range(18)]
    draw_array(svg, 10, 30, "candidate_counts", [2] * 9, tints=VERTEX_TINT, index_row=True)
    draw_array(svg, 10, 110, "candidates", candidates, tints=tints, index_row=True)
    for v in range(9):
        x = 10 + 150 + 2 * v * cell
        svg.line(x + 3, 152, x + 2 * cell - 3, 152, color=INK, width=1.5)
        svg.text(x + cell, 170, f"v = {v}", size=12)

    svg.text(10 + 140, 238, "strongest_neighbor", size=14, anchor="end", weight="bold")
    result = ["3", "4", "5", "0", "1", "2", "-1", "8", "7"]
    for v, value in enumerate(result):
        x = 10 + 150 + v * cell
        fill = "#fff8c5" if value == "-1" else "#dafbe1"
        svg.rect(x, 215, cell, 34, fill=fill)
        svg.text(x + cell / 2, 238, value, size=14)
        svg.text(x + cell / 2, 209, str(v), size=11, color=MUTED)
    svg.text(10 + 150 + 9 * cell + 12, 238,
             "after n_way_extend_hand(); -1 = VERTEX_UNMATCHED: no hand this pass", size=12,
             anchor="start", color=MUTED)
    svg.text(10 + 150, 285, "Vertex 6 chose 3 and 7, but neither chose 6, so 6 waits for "
             "the next pass.", size=13, anchor="start")
    svg.save("n_way_candidates.svg")


def figure_worklist():
    cell = 40
    left = 200
    svg = Svg(left + 9 * cell + 300, 480)

    # Ranges.
    svg.text(left - 10, 28, "thread ranges", size=14, anchor="end", weight="bold")
    for t in range(3):
        x = left + 3 * t * cell
        svg.rect(x + 2, 12, 3 * cell - 4, 24, fill=THREAD_TINT[t], stroke=THREAD[t], radius=4)
        svg.text(x + 1.5 * cell, 29, f"thread {t}: [{3 * t}, {3 * t + 3})", size=12,
                 color=THREAD[t], weight="bold")

    active = list(range(9))
    tints = [THREAD_TINT[i // 3] for i in range(9)]
    svg.text(left - 10, 75, "active_vertices", size=14, anchor="end", weight="bold")
    for i, v in enumerate(active):
        svg.rect(left + i * cell, 52, cell, 34, fill=tints[i])
        svg.text(left + i * cell + cell / 2, 75, str(v), size=14)
        svg.text(left + i * cell + cell / 2, 102, str(i), size=11, color=MUTED)

    status = ["-1", "4", "5", "-1", "1", "2", "-1", "-1", "-1"]
    svg.text(left - 10, 145, "partners[v]", size=14, anchor="end", weight="bold")
    for i, value in enumerate(status):
        unmatched = value == "-1"
        svg.rect(left + i * cell, 122, cell, 34, fill="#fff8c5" if unmatched else "#ffffff")
        svg.text(left + i * cell + cell / 2, 145, value, size=14,
                 weight="bold" if unmatched else "normal")
    svg.text(left + 9 * cell + 12, 145, "after pass 1; -1 = VERTEX_UNMATCHED", size=12,
             anchor="start", color=MUTED)

    # Counts and offsets, one cell per thread.
    def per_thread(y, label, values, note, extra=None):
        svg.text(left - 10, y + 23, label, size=14, anchor="end", weight="bold")
        for t, value in enumerate(values):
            fill = THREAD_TINT[t] if t < 3 else "#ffffff"
            stroke = THREAD[t] if t < 3 else INK
            svg.rect(left + t * 60, y, 60, 34, fill=fill, stroke=stroke)
            svg.text(left + t * 60 + 30, y + 23, str(value), size=14)
        svg.text(left + len(values) * 60 + 12, y + 23, note, size=12, anchor="start",
                 color=MUTED)

    per_thread(200, "unmatched_counts", [1, 1, 3], "1. worklist_count_unmatched()")
    per_thread(270, "write_offsets", [0, 1, 2, 5],
               "2. prefix_sum_exclusive() (provided); last entry = total")

    svg.text(left - 10, 365, "next_active_vertices", size=14, anchor="end", weight="bold")
    owners = [0, 1, 2, 2, 2]
    for i, v in enumerate([0, 3, 6, 7, 8]):
        svg.rect(left + i * cell, 342, cell, 34, fill=THREAD_TINT[owners[i]],
                 stroke=THREAD[owners[i]])
        svg.text(left + i * cell + cell / 2, 365, str(v), size=14)
        svg.text(left + i * cell + cell / 2, 392, str(i), size=11, color=MUTED)
    svg.text(left + 5 * cell + 12, 365, "3. worklist_compact()", size=12, anchor="start",
             color=MUTED)

    svg.text(left, 435, "Each thread writes its unmatched vertices starting at "
             "write_offsets[thread_index],", size=13, anchor="start")
    svg.text(left, 455, "so no two threads write the same slot, and the order of the "
             "worklist is kept.", size=13, anchor="start")
    svg.save("worklist.svg")


def figure_pass():
    steps_one = ["one_way_extend_hand()", "one_way_check_handshakes()",
                 "worklist_count_unmatched()", "prefix_sum_exclusive()",
                 "worklist_compact()"]
    notes = ["each vertex picks one neighbor", "mutual hands become pairs",
             "per-thread counts", "thread 0 only (provided)", "next pass's worklist"]
    svg = Svg(860, 440)
    x = 40
    y = 40
    svg.text(x, 24, "One pass: every thread runs every step on its own range", size=14,
             anchor="start", weight="bold")
    for i, (step, note) in enumerate(zip(steps_one, notes)):
        top = y + i * 70
        svg.rect(x, top, 300, 44, fill=THREAD_TINT[0] if i < 2 else THREAD_TINT[2],
                 stroke=THREAD[0] if i < 2 else THREAD[2], radius=6)
        svg.text(x + 150, top + 27, step, size=14, weight="bold")
        svg.text(x + 320, top + 27, note, size=13, anchor="start", color=MUTED)
        if i < len(steps_one) - 1:
            svg.line(x + 150, top + 44, x + 150, top + 66, color=MUTED, width=2, marker="flow")
            svg.text(x + 162, top + 60, "barrier", size=12, color=MUTED, anchor="start")
    svg.text(x, y + 5 * 70 + 10, "After the prefix sum, if write_offsets[thread_count] is 0, "
             "every vertex is finished and the threads stop.", size=13, anchor="start")
    svg.text(x, y + 5 * 70 + 32, "N-way replaces the first step with n_way_find_candidates(), "
             "a barrier, then n_way_extend_hand().", size=13, anchor="start")
    svg.save("pass_steps.svg")


if __name__ == "__main__":
    figure_graph()
    figure_one_way()
    figure_adjacency()
    figure_candidates()
    figure_worklist()
    figure_pass()
    print("Wrote figures to", OUT)
