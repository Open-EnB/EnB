r"""The camera's path in e2e runs (`e2e.py --camera-track`'s camera-track.txt), retail runs against ours.

    python tools/harness/camtrack.py show DIR [--from T] [--to T] [--every S] [--view K]
    python tools/harness/camtrack.py compare --retail DIR... --ours DIR... [--scenario NAME | --anchor NAME=SPEC...]
                                     [--tol-pos U] [--tol-ang D] [--json FILE]
    python tools/harness/camtrack.py ship --retail DIR... --ours DIR... --shots NAME... --ref NAME
                                     the moving object's place in shots' draw logs (sp_land: the ship along its
                                     landing spline, B-01, while the camera holds: land_1s land_2s5 --ref surface)

DIR is a scenario's e2e output folder (camera-track.txt, events.log). Each line of camera-track.txt is
`TICK T FRAME N` and N view matrices (16 floats, row vectors: D3D's view matrix), in the order the frame first set
them. The views whose rotation is not the identity are the frame's 3-D views (the orthographic UI views have the
identity rotation); each is its own series, view 0 the first of them, view 1 the second. In space view 0 is the
camera (a later one is stale, a second scene's); on a planet surface view 0 is a top-down view over the ship
(70-88 degrees down, slowly turning) and view 1 the camera. Retail and ours set them in the same order, so each
series compares with its own: a view that is missing or extra on ours is a difference too.

An anchor is a moment in events.log: SPEC is SOURCE:REGEX[#K] (the K-th matching line of that source, 1 by default;
SOURCE `any` for every source), with an optional window `@LO,HI` in seconds around it (default -1,8) and `%rel` to
compare places relative to the view's last place before the anchor (a camera a cut puts at an offset from the ship).
Per anchor and view:
  * plateaus: where the view held still (within 1 unit / 0.5 degree for at least 0.15 s) in the window, their
    places on each run; the camera controllers draw on one generator of a fixed seed (0xBDB1C0), so a held place is
    the same on every retail run to the tenth of a unit, and ours must be too;
  * path: every pose of ours in the window: its place against the retail runs' paths (the distance to the nearest
    point of each retail run's polyline), so a move sampled at other frames still compares, and its angle against
    the retail runs' angle at the same time from the anchor, give or take TIME_SLACK (0.1 s: the anchor is a
    packet's log time, a cut lands a little either side of it), for a camera that holds its place and turns after
    the ship; the tolerance at a time T from the anchor is retail's own spread there (each retail pose within 0.25 s of
    T against the other retail runs, and each retail frame's distance from the line between its two neighbours: a
    camera that shakes, as the dock cameras do by a unit or two on every run) plus --tol-pos / --tol-ang (0.1 unit,
    0.1 degree): a held place stays strict
    where the phase before it (a warp's arrival, a chase camera) varies between runs;
  * timing: the seconds from the anchor to the first plateau after it (the move's length), retail's range and ours.
The verdict per anchor: SAME, or DIFF with the first differing frame (the view, its time from the anchor, the frame
number, the field: x / y / z / yaw / pitch, ours and the nearest retail value). A tolerance is never widened to
hide a difference: a DIFF is a reconstruction item (test-plan section 5).

--scenario takes the anchors of SCENARIOS below (the sp_* camera scenarios of test-plan SP-01 to SP-03).
"""
import argparse
import glob
import json
import math
import os
import re
import sys

IDENT = (1, 0, 0, 0, 1, 0, 0, 0, 1)
SPREAD_WINDOW = 0.25        # seconds: the retail poses that set the tolerance at a time from the anchor
# seconds: an angle is compared with the other run's poses this far either side of the same time from the anchor (the
# anchor is the proxy's log time of a packet, not the frame the client acted in: a cut lands up to about a tenth of
# a second either side of it on two runs)
TIME_SLACK = 0.1
# seconds: a run's path is the reference for the other runs' frames over its window widened this much on each side, so
# a frame near the window's edge on one run (whose moment falls a little earlier or later) still finds the path
REF_MARGIN = 0.5

# the camera moments of the live space scenarios (test-plan SP-01 .. SP-03); each SPEC as --anchor's
SCENARIOS = {
    # counted from the session's start (events.log holds the login too): CAMERA_CONTROL #1 is the login's sector entry
    'sp_gate': [
        # the warp's end: the warp camera (CameraController02Class 0x797B50) cuts from the chase camera to its exit
        # place 4.1-4.2 s before the server's WARP_INDEX (retail, four runs: the place within 85 units); the
        # server's own warp timing moves by 0.3 s from run to run, so the anchor is its WARP_INDEX
        ('warp_end', r"antprox:sent WARP_INDEX$#1@-6,-1"),
        # the gate's verb (ACTION) and the ship's move toward the opening gate (the chase camera)
        ('gate_verb', r"antprox:received ACTION$#1@-1,7"),
        # the server's CAMERA_CONTROL 4 (5.8 s after the verb): the stargate camera's two or three frames before the
        # loading screen
        ('stargate_camera', r"antprox:sent CAMERA_CONTROL$#2@-0.5,1"),
        ('stargate_offset', r"antprox:sent CAMERA_CONTROL$#2@-0.3,1%rel"),
        # Saturn's sector-entry camera from 0.15 s on (before it the stargate camera's last pose stays on the screen:
        # stargate_offset compares that one)
        ('saturn_entry', r"antprox:sent CAMERA_CONTROL$#3@0.15,8"),
    ],
    'sp_land': [
        # each window ends before the scenario's next input (Q at about +4.5 s, W at about +6.5 s): the camera's
        # moves after it follow the harness's input timing, not the client
        ('endriago_entry', r"antprox:sent CAMERA_CONTROL$#2@-1,4.3"),
        # the Land verb: the planet-landing camera's held place while the ship flies its spline (B-01)
        ('land', r"antprox:received ACTION$#1@-1,6"),
        # from the CAMERA_CONTROL on: before it the surface's first view (a top-down one, turning with the server's
        # time since its start) is all there is
        ('surface_entry', r"antprox:sent CAMERA_CONTROL$#3@0,6"),
        # the launchpad's CAMERA_CONTROL 4 on a planet: the take-off camera (B-02), placed at an offset from the
        # ship (relative: the ship's own place at that moment follows the server's timing)
        ('takeoff', r"antprox:sent CAMERA_CONTROL$#4@-1,2"),
        ('takeoff_offset', r"antprox:sent CAMERA_CONTROL$#4@-0.3,1%rel"),
        ('space_entry', r"antprox:sent CAMERA_CONTROL$#5@0.15,8"),
    ],
    'sp_dock': [
        ('warp_end', r"antprox:sent WARP_INDEX$#1@-6,-1"),
        # the Dock verb's CAMERA_CONTROL 5: the station-landing camera, about 5 s (from the CAMERA_CONTROL on: the verb
        # is clicked 0.2 s after the warp's end, which the server's timing places)
        ('dock', r"antprox:sent CAMERA_CONTROL$#2@0.15,6"),
        ('launch_entry', r"antprox:sent CAMERA_CONTROL$#3@0.15,8"),
    ],
}


def real(v):
    return v is not None and any(abs(a - b) > 1e-4 for a, b in zip(v[0:3] + v[4:7] + v[8:11], IDENT))


def pose(v):
    """(position, yaw, pitch) of a view matrix (row vectors): the camera's place is -t R^T, it looks along -z."""
    r = (v[0:3], v[4:7], v[8:11])
    pos = tuple(-(v[12] * r[i][0] + v[13] * r[i][1] + v[14] * r[i][2]) for i in range(3))
    look = (-v[2], -v[6], -v[10])
    yaw = math.degrees(math.atan2(look[1], look[0]))
    pitch = math.degrees(math.asin(max(-1.0, min(1.0, look[2]))))
    return pos, yaw, pitch


def parse_track_line(line):
    """(t, frame, [the poses of the frame's 3-D views, in order]) of one camera-track.txt line; None if malformed."""
    s = line.split()
    if len(s) < 4:
        return None
    try:
        n = int(s[3])
        if len(s) != 4 + 16 * n:
            return None
        t, frame = float(s[1]), int(s[2])
        fl = [float(x) for x in s[4:]]
    except ValueError:
        return None
    views = [fl[i:i + 16] for i in range(0, len(fl), 16)]
    return t, frame, [pose(v) for v in views if real(v)]


def load_track(d):
    out = []
    p = os.path.join(d, 'camera-track.txt')
    if os.path.exists(p):
        with open(p) as f:
            for line in f:
                r = parse_track_line(line)
                if r:
                    out.append(r)
    return out


def load_events(d):
    ev = []
    p = os.path.join(d, 'events.log')
    if os.path.exists(p):
        with open(p, encoding='utf-8', errors='replace') as f:
            for line in f:
                m = re.match(r'\s*([\d.]+) (\S+)\s+(.*)', line)
                if m:
                    ev.append((float(m.group(1)), m.group(2), m.group(3).rstrip()))
    return ev


def parse_spec(spec):
    """SOURCE:REGEX[#K][@LO,HI][%rel] -> (source, regex, k, lo, hi); see relative_spec for %rel."""
    spec = spec[:-4] if spec.endswith('%rel') else spec
    lo, hi = -1.0, 8.0
    m = re.search(r'@(-?[\d.]+),(-?[\d.]+)$', spec)
    if m:
        lo, hi = float(m.group(1)), float(m.group(2))
        spec = spec[:m.start()]
    k = 1
    m = re.search(r'#(\d+)$', spec)
    if m:
        k = int(m.group(1))
        spec = spec[:m.start()]
    src, _, rx = spec.partition(':')
    return src, rx, k, lo, hi


def relative_spec(spec):
    """A SPEC ending `%rel` compares places relative to the view's last place before the anchor: a camera that a
    cut puts at an offset from the ship (the take-off and stargate cameras) is compared by that offset, since the
    ship's own place at the server's CAMERA_CONTROL varies with the server's timing. The place tolerance grows by
    twice the largest reference uncertainty (make_relative: ours' and a retail run's reference each)."""
    return spec.endswith('%rel')


def make_relative(samples):
    """(SAMPLES with each place less the last place before time 0, the reference's uncertainty in units): the
    reference moves on until the cut, which falls somewhere between the last frame before time 0 and the first
    after it, so the uncertainty is the reference's speed times that gap. Unchanged, 0.0 when there is no frame
    before time 0."""
    before = [(t, p) for t, fr, p in samples if t < 0]
    if not before:
        return samples, 0.0
    t1, p1 = before[-1]
    unc = 0.0
    if len(before) >= 2:
        t0, p0 = before[-2]
        after = [t for t, fr, p in samples if t >= 0]
        if after and t1 > t0:
            unc = math.dist(p1[0], p0[0]) / (t1 - t0) * (after[0] - t1)
    o = p1[0]
    return [(t, fr, (tuple(p[0][i] - o[i] for i in range(3)), p[1], p[2])) for t, fr, p in samples], unc


def find_anchor(events, spec):
    src, rx, k, lo, hi = parse_spec(spec)
    n = 0
    for t, s, text in events:
        if (src == 'any' or s == src) and re.search(rx, text):
            n += 1
            if n == k:
                return t
    return None


def dist(a, b):
    """(units, degrees) between two poses."""
    d = math.dist(a[0], b[0])
    dy = abs((a[1] - b[1] + 180) % 360 - 180)
    return d, max(dy, abs(a[2] - b[2]))


def window(track, t0, lo, hi, view=0):
    """[(t from the anchor, frame, pose of VIEW)] in [t0+lo, t0+hi]; frames without that view are left out."""
    return [(t - t0, fr, ps[view]) for t, fr, ps in track if t0 + lo <= t <= t0 + hi and len(ps) > view]


def views_in(track, t0, lo, hi):
    return max((len(ps) for t, fr, ps in track if t0 + lo <= t <= t0 + hi), default=0)


def plateaus(samples, still=1.0, still_ang=0.5, min_len=0.15):
    """[(start, end, pose)] where the view held within STILL units / STILL_ANG degrees for MIN_LEN s."""
    out, cur = [], None
    for t, fr, p in samples:
        if cur and all(x <= y for x, y in zip(dist(p, cur[2]), (still, still_ang))):
            cur[1] = t
        else:
            if cur and cur[1] - cur[0] >= min_len:
                out.append(tuple(cur))
            cur = [t, t, p]
    if cur and cur[1] - cur[0] >= min_len:
        out.append(tuple(cur))
    return out


def seg_nearest(p, a, b):
    """The point of segment a-b (poses) nearest p's position: (units, the interpolated pose)."""
    ax, bx, px = a[0], b[0], p[0]
    ab = [bx[i] - ax[i] for i in range(3)]
    l2 = sum(c * c for c in ab)
    u = 0.0 if l2 == 0 else max(0.0, min(1.0, sum((px[i] - ax[i]) * ab[i] for i in range(3)) / l2))
    q = tuple(ax[i] + u * ab[i] for i in range(3))
    dyaw = (b[1] - a[1] + 180) % 360 - 180
    return math.dist(px, q), (q, a[1] + u * dyaw, a[2] + u * (b[2] - a[2]))


def path_nearest(p, path):
    """(units, degrees, the nearest pose) of pose P against the polyline PATH ([(t, frame, pose)]): the nearest
    point by place; among points at the same place (a camera that turns while it holds still) the nearest angle."""
    best = None
    pts = [x[2] for x in path]
    if len(pts) == 1:
        pts = pts * 2
    for a, b in zip(pts, pts[1:]):
        d, q = seg_nearest(p, a, b)
        g = dist(p, q)[1]
        if best is None or (round(d, 3), g) < (round(best[0], 3), best[1]):
            best = (d, g, q)
    if best is None:
        return float('inf'), float('inf'), None
    return best


def first_field(p, q, place=True):
    """The field that differs most (x / y / z when PLACE, else yaw / pitch) and the two values."""
    rows = [('x', p[0][0], q[0][0]), ('y', p[0][1], q[0][1]), ('z', p[0][2], q[0][2])] if place else         [('yaw', p[1], q[1]), ('pitch', p[2], q[2])]

    def gap(r):
        g = abs(r[1] - r[2])
        return min(g, 360 - g) if r[0] == 'yaw' else g
    return max(rows, key=gap)


def at_time(samples, t, gap=0.2):
    """The pose of SAMPLES ([(t, frame, pose)], in time order) at time T, linear between the two frames around it;
    None outside them or across a gap longer than GAP seconds."""
    for (t1, f1, p1), (t2, f2, p2) in zip(samples, samples[1:]):
        if t1 <= t <= t2:
            if t2 - t1 > gap:
                return None
            u = 0.0 if t2 == t1 else (t - t1) / (t2 - t1)
            dyaw = (p2[1] - p1[1] + 180) % 360 - 180
            return (tuple(p1[0][i] + u * (p2[0][i] - p1[0][i]) for i in range(3)), p1[1] + u * dyaw,
                    p1[2] + u * (p2[2] - p1[2]))
    return None


def near_in_time(samples, t, slack=None, steps=8):
    """The poses SAMPLES passes through within SLACK (TIME_SLACK) seconds of T: the frames there and STEPS points
    along each step between two frames that touches the interval (a fast turn between two frames counts), not
    across a gap of more than 0.2 s."""
    slack = TIME_SLACK if slack is None else slack
    out = []
    for (t1, f1, p1), (t2, f2, p2) in zip(samples, samples[1:]):
        if t2 < t - slack or t1 > t + slack or t2 - t1 > 0.2:
            continue
        dyaw = (p2[1] - p1[1] + 180) % 360 - 180
        for k in range(steps + 1):
            u = k / steps
            tt = t1 + u * (t2 - t1)
            if abs(tt - t) <= slack:
                out.append((tuple(p1[0][i] + u * (p2[0][i] - p1[0][i]) for i in range(3)), p1[1] + u * dyaw,
                            p1[2] + u * (p2[2] - p1[2])))
    if not out:
        out = [p for tt, fr, p in samples if abs(tt - t) <= slack]
    return out


def angle_gap(p, samples, t):
    """(degrees, the pose) of the nearest angle among SAMPLES' poses near time T; (None, None) if none."""
    return min(((dist(p, q)[1], q) for q in near_in_time(samples, t)), key=lambda x: x[0], default=(None, None))


def compare_view(wins, retail, ours, tol_pos, tol_ang, view, res, refs=None):
    """One view's series. The place: each pose of ours against the retail runs' paths (time-free: frames fall at
    other times on each run). The angle: against the retail runs' angle at the same time from the anchor (a camera
    that holds its place and turns after the ship has its angle as a function of time). The tolerance at a time:
    retail's own spread there (SPREAD_WINDOW) plus TOL_POS / TOL_ANG. WINS: {run: samples}. Fills RES['runs'][run]
    and returns the first difference or None."""
    refs = refs or wins
    rw = [r for r in retail if wins[r]]
    spread_pts = []
    for a in rw:
        others = [b for b in rw if b != a]
        for t, fr, p in wins[a]:
            if not others:
                continue
            d = min(path_nearest(p, refs[b])[0] for b in others)
            gs = [g for g, q in (angle_gap(p, refs[b], t) for b in others) if g is not None]
            spread_pts.append((t, d, min(gs) if gs else 0.0))

    # each retail run's own frame-to-frame noise: a frame's distance from the line between its two neighbours at
    # its time (a camera that shakes by a unit or two, the dock cameras', has it on every run; a smooth move has
    # none whatever the frame timing; a cut, where the neighbours lie more than 200 units apart, is not noise)
    for a in rw:
        w = wins[a]
        for (t0, f0, p0), (t1, f1, p1), (t2, f2, p2) in zip(w, w[1:], w[2:]):
            if t2 - t0 <= 0.2 and t2 > t0 and math.dist(p0[0], p2[0]) <= 200.0:
                u = (t1 - t0) / (t2 - t0)
                mid = tuple(p0[0][i] + u * (p2[0][i] - p0[0][i]) for i in range(3))
                spread_pts.append((t1, math.dist(p1[0], mid), 0.0))

    def spread_at(t):
        near = [(d, g) for tt, d, g in spread_pts if abs(tt - t) <= SPREAD_WINDOW]
        return (max((d for d, g in near), default=0.0), max((g for d, g in near), default=0.0))
    res.setdefault('retail_spread', {})[view] = (round(max((d for t, d, g in spread_pts), default=0.0), 3),
                                                 round(max((g for t, d, g in spread_pts), default=0.0), 3))
    rp = {n: plateaus(wins[n]) for n in rw}
    first = None
    for o in ours:
        row = res['runs'][o].setdefault('views', {}).setdefault(view, {})
        w = wins.get(o, [])
        if not w or not rw:
            if w or rw:
                row['verdict'] = 'DIFF'
                f = {'run': o, 'view': view, 'what': 'no frames of this view on ' + ('retail' if w else 'ours')}
                first = first or f
            continue
        worst, f1, over = (0.0, 0.0), None, []
        for t, fr, p in w:
            d, _, q = min((path_nearest(p, refs[r]) for r in rw), key=lambda x: x[0])
            g, qa = min(((g2, q2) for g2, q2 in (angle_gap(p, refs[r], t) for r in rw) if g2 is not None),
                        key=lambda x: x[0], default=(0.0, None))
            worst = (max(worst[0], d), max(worst[1], g))
            sp = spread_at(t)
            if d > sp[0] + tol_pos or g > sp[1] + tol_ang:
                over.append((round(t, 3), round(d, 3), round(g, 3)))
            if f1 is None and (d > sp[0] + tol_pos or g > sp[1] + tol_ang):
                fld, ov, rv = first_field(p, q, True) if d > sp[0] + tol_pos else first_field(p, qa, False)
                f1 = {'run': o, 'view': view, 't': round(t, 3), 'frame': fr, 'field': fld, 'ours': round(ov, 3),
                      'retail': round(rv, 3), 'units': round(d, 3), 'degrees': round(g, 3),
                      'retail_spread_there': (round(sp[0], 3), round(sp[1], 3))}
        row['path_worst'] = (round(worst[0], 3), round(worst[1], 3))
        row['over'] = over
        row['plateau_vs_retail'] = []
        for a0, b0, p in plateaus(w):
            near = min(((dist(p, q), r) for r in rw for _, _, q in rp[r]), key=lambda x: x[0],
                       default=((float('inf'), float('inf')), None))
            row['plateau_vs_retail'].append((round(a0, 2), round(near[0][0], 3), round(near[0][1], 3)))
        row['verdict'] = 'DIFF' if f1 else 'SAME'
        first = first or f1
    return first


def compare_anchor(runs, spec, tol_pos=0.1, tol_ang=0.1):
    """runs: [(name, is_retail, track, events)]. Per view: plateaus, path compare, timing; the verdict."""
    _, _, _, lo, hi = parse_spec(spec)
    res = {'spec': spec, 'runs': {}, 'verdict': None, 'first_difference': None}
    anchors = {}
    for name, retail, track, events in runs:
        t0 = find_anchor(events, spec)
        anchors[name] = t0
        res['runs'][name] = {'anchor': t0}
        if t0 is not None:
            res['runs'][name]['views_seen'] = views_in(track, t0, lo, hi)
    retail = [n for n, r, tr, ev in runs if r and anchors[n] is not None]
    ours = [n for n, r, tr, ev in runs if not r]
    if not retail:
        res['verdict'] = 'NO RETAIL'
        return res
    for n in ours:
        if anchors[n] is None:
            res['first_difference'] = res['first_difference'] or {'run': n, 'what': 'no anchor on ours'}
    nviews = max(res['runs'][n].get('views_seen', 0) for n in res['runs'])
    tracks = {n: tr for n, r, tr, ev in runs}
    for v in range(nviews):
        wins = {n: window(tracks[n], anchors[n], lo, hi, v) for n in anchors if anchors[n] is not None}
        refs = {n: window(tracks[n], anchors[n], lo - REF_MARGIN, hi + REF_MARGIN, v) for n in wins}
        unc = 0.0
        if relative_spec(spec):
            rel = {n: make_relative(window(tracks[n], anchors[n], min(lo, -0.3) - REF_MARGIN, hi + REF_MARGIN, v))
                   for n in wins}
            refs = {n: w for n, (w, u) in rel.items()}
            wins = {n: [x for x in w if lo <= x[0] <= hi] for n, w in refs.items()}
            unc = 2 * max((u for w, u in rel.values()), default=0.0)
            res.setdefault('reference_uncertainty', {})[v] = round(unc, 3)
        for n, w in wins.items():
            pl = plateaus(w)
            after = [a for a, b, p in pl if a > 0.05]
            res['runs'][n].setdefault('views', {})[v] = {
                'samples': len(w), 'plateaus': [(round(a, 2), round(b, 2), fmt(p)) for a, b, p in pl],
                'move_s': round(after[0], 2) if after else None}
        f = compare_view(wins, retail, [o for o in ours if anchors[o] is not None], tol_pos + unc, tol_ang, v, res,
                         refs)
        res['first_difference'] = res['first_difference'] or f
    if ours:
        res['verdict'] = 'DIFF' if res['first_difference'] else 'SAME'
    else:
        res['verdict'] = 'RETAIL ONLY'
    return res


def fmt(p):
    if p is None:
        return '(no 3-D view)'
    pos, yaw, pitch = p
    return f'{pos[0]:.1f} {pos[1]:.1f} {pos[2]:.1f} y{yaw:.1f} p{pitch:.1f}'


def cmd_show(a):
    last = None
    for t, fr, ps in load_track(a.dir):
        if (a.start is not None and t < a.start) or (a.end is not None and t > a.end):
            continue
        if last is not None and t - last < a.every:
            continue
        last = t
        sel = ps if a.view is None else ps[a.view:a.view + 1]
        print(f'{t:9.3f} {fr:7d}  ' + ('  |  '.join(fmt(p) for p in sel) if sel else '(no 3-D view)'))
    return 0


def shot_drawlog(d, name):
    """The draw log (shot_NAME_FRAME.json) of shot NAME in run folder D, or None."""
    fs = sorted(glob.glob(os.path.join(d, f'shot_{name}_*.json')))
    fs = [f for f in fs if re.match(re.escape(name) + r'_\d+\.json$', os.path.basename(f)[5:])]
    if not fs:
        return None
    with open(fs[0]) as f:
        return json.load(f)


def moving_centre(log, ref, near=5000.0):
    """The middle (per axis median) of the world places of LOG's draws that are not in REF (another shot of the
    same run) and lie within NEAR units of LOG's first 3-D view: the moving object, e.g. the ship along its landing
    spline under a camera that holds still. None when nothing moved."""
    def places(d):
        return [tuple(w[12:15]) for w in (x.get('world') for x in d.get('draws', [])) if w]
    views = [v for v in log.get('views', []) if real(v)]
    cam = pose(views[0])[0] if views else None
    still = {tuple(round(c, 1) for c in p) for p in places(ref)}
    pts = [p for p in places(log) if tuple(round(c, 1) for c in p) not in still
           and cam is not None and math.dist(p, cam) <= near]
    if not pts:
        return None
    return tuple(sorted(p[i] for p in pts)[len(pts) // 2] for i in range(3))


def cmd_ship(a):
    """Per shot of --shots, each run's moving object's place (moving_centre against --ref), retail's spread and
    ours' distance to the nearest retail place."""
    runs = [(f'R{i}', d) for i, d in enumerate(a.retail, 1)] + [(f'O{i}', d) for i, d in enumerate(a.ours or [], 1)]
    rc = 0
    for name in a.shots:
        cs = {}
        for n, d in runs:
            log, ref = shot_drawlog(d, name), shot_drawlog(d, a.ref)
            cs[n] = moving_centre(log, ref, a.near) if log and ref else None
        rs = [c for n, c in cs.items() if n.startswith('R') and c]
        spread = max((math.dist(x, y) for i, x in enumerate(rs) for y in rs[i + 1:]), default=0.0)
        print(f'== {name} (moving against {a.ref}): retail spread {spread:.1f} u')
        for n, c in cs.items():
            line = f'  {n}: ' + ('(none)' if c is None else ' '.join(f'{v:.1f}' for v in c))
            if n.startswith('O') and c and rs:
                dn = min(math.dist(c, r) for r in rs)
                ok = dn <= spread + a.tol_pos
                line += f'  nearest retail {dn:.1f} u: {"SAME" if ok else "DIFF"}'
                rc |= 0 if ok else 1
            print(line)
    return rc


def cmd_compare(a):
    anchors = list(SCENARIOS.get(a.scenario, [])) if a.scenario else []
    for s in a.anchor or []:
        name, _, spec = s.partition('=')
        anchors.append((name, spec))
    if not anchors:
        print('camtrack: no anchors (--scenario or --anchor)')
        return 2
    runs = [(f'R{i}', True, load_track(d), load_events(d)) for i, d in enumerate(a.retail, 1)] + \
           [(f'O{i}', False, load_track(d), load_events(d)) for i, d in enumerate(a.ours or [], 1)]
    for i, d in enumerate(a.retail, 1):
        print(f'R{i} {d}')
    for i, d in enumerate(a.ours or [], 1):
        print(f'O{i} {d}')
    out = {}
    rc = 0
    for name, spec in anchors:
        r = compare_anchor(runs, spec, a.tol_pos, a.tol_ang)
        out[name] = r
        sp = '; '.join(f'view {v}: {s[0]} u / {s[1]} deg' for v, s in sorted(r.get('retail_spread', {}).items()))
        print(f'\n== {name} ({spec}): {r["verdict"]}' + (f'  (retail spread, its largest: {sp})' if sp else ''))
        for n, row in r['runs'].items():
            if row.get('anchor') is None:
                print(f'  {n}: no anchor')
                continue
            print(f'  {n}: anchor {row["anchor"]:.2f}')
            for v, vr in sorted(row.get('views', {}).items()):
                line = f'    view {v}: {vr.get("samples", 0)} frames, move {vr.get("move_s")} s'
                if 'path_worst' in vr:
                    line += (f'; path worst {vr["path_worst"][0]} u / {vr["path_worst"][1]} deg, '
                             f'{len(vr.get("over", []))} frames over: {vr["verdict"]}')
                elif 'verdict' in vr:
                    line += f': {vr["verdict"]}'
                print(line)
                for pl in vr.get('plateaus', [])[:a.plateaus]:
                    print(f'      held {pl[0]:6.2f}..{pl[1]:6.2f}  {pl[2]}')
                for pl in vr.get('plateau_vs_retail', [])[:a.plateaus]:
                    print(f'      ours held at {pl[0]:6.2f}: nearest retail plateau {pl[1]} u / {pl[2]} deg')
        if r['first_difference']:
            print(f'  first difference: {r["first_difference"]}')
        if r['verdict'] not in ('SAME', 'RETAIL ONLY'):
            rc = 1
    if a.json:
        with open(a.json, 'w') as f:
            json.dump(out, f, indent=1)
    return rc


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('show')
    s.add_argument('dir')
    s.add_argument('--from', dest='start', type=float)
    s.add_argument('--to', dest='end', type=float)
    s.add_argument('--every', type=float, default=0.0)
    s.add_argument('--view', type=int, help='only this 3-D view (0 the first)')
    c = sub.add_parser('compare')
    c.add_argument('--retail', nargs='+', required=True)
    c.add_argument('--ours', nargs='*')
    c.add_argument('--scenario', choices=sorted(SCENARIOS))
    c.add_argument('--anchor', action='append', help='NAME=SOURCE:REGEX[#K][@LO,HI]')
    c.add_argument('--tol-pos', type=float, default=0.1)
    c.add_argument('--tol-ang', type=float, default=0.1)
    c.add_argument('--plateaus', type=int, default=8, help='plateaus printed per run and view (default 8)')
    c.add_argument('--json')
    h = sub.add_parser('ship', help="a moving object's place in shots' draw logs (the ship under a held camera)")
    h.add_argument('--retail', nargs='+', required=True)
    h.add_argument('--ours', nargs='*')
    h.add_argument('--shots', nargs='+', required=True, help='shot names, e.g. land_1s land_2s5')
    h.add_argument('--ref', required=True, help="a shot of the same run where the object is elsewhere")
    h.add_argument('--near', type=float, default=5000.0)
    h.add_argument('--tol-pos', type=float, default=1.0)
    a = ap.parse_args(argv)
    return {'show': cmd_show, 'compare': cmd_compare, 'ship': cmd_ship}[a.cmd](a)


if __name__ == '__main__':
    sys.exit(main())
