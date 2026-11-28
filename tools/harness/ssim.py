"""RD-02 (test plan): the shot compare by a perceptual metric, SSIM, with the thresholds per shot and per window set
from retail's own runs (principle 3), alongside e2e's 16 x 16 cell rule.

    python tools/harness/ssim.py pair A.png B.png [--step 2] [--win 8]
    python tools/harness/ssim.py compare RUN [--store DIR] [--set S] [--names N ...] [--json OUT]

pair: the mean SSIM of two shots and the worst window (its pixel box).
compare: every shot of RUN (an e2e --out folder, a session folder or a stored run) against the store's runs of the
same scenario (the scenario's own folder, else the one set folder that ran it; --set picks one; a stored RUN is
left out of its own reference set). Per shot key (NAME, NAME#2 for a second shot of that name) and per window:
  a window is LOW for a run X against reference runs O when X's best SSIM there against O is below the lowest
  SSIM there of two runs of O (O's own spread in that window). A window is STILL for O when that lowest SSIM is
  at least STILL (0.999: the same picture there in every run of O), else MOVING.
  noise  = leave one out over the stored runs: each stored run r against the others, its count of LOW windows,
           STILL ones and all; the noise is the largest such count of each. Needs 3 stored runs (2 give one pair:
           NO NOISE, the mean only).
  ours   = RUN against every such set of the others (n - 1 stored runs, as each stored run had), its fewest LOW
           windows, STILL ones and all.
  SAME when ours <= noise in both, DIFF above (the LOW windows' pixel box printed): where every retail run draws
  the same picture (no STILL noise), a change is a DIFF from its first window; where retail's own picture moves,
  more LOW windows than a retail run has are. RETAIL-ONLY / OURS-ONLY when a side lacks the shot. The mean SSIM of RUN against its nearest stored run and the stored runs' lowest pair is printed
  beside it (a reading aid: the mean of 5,120 windows hides a small changed region).
Shots e2e's shot_mask.txt marks `skip` or `mean T` are left out (MASKED); its rectangles are left out of the
windows. The image is the shot's luma (0.299 R + 0.587 G + 0.114 B) at every STEP-th pixel and row; SSIM is taken
over non-overlapping WIN x WIN windows of it (C1 = (0.01 * 255)^2, C2 = (0.03 * 255)^2). Exit 0 when every
compared shot is SAME, 1 otherwise.
"""
import argparse
import itertools
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

C1 = (0.01 * 255) ** 2
C2 = (0.03 * 255) ** 2
STEP = 2
WIN = 8
STILL = 0.999

_cache = {}


def luma(path, step=STEP):
    """A shot -> (width, height, rows of luma floats) at every STEP-th pixel and row (cached per path)."""
    key = (os.path.abspath(path), step)
    if key in _cache:
        return _cache[key]
    import e2e
    w, h, n, rows = e2e.read_shot(path)
    out = []
    for y in range(0, h, step):
        r = rows[y]
        rr, gg, bb = r[0::n * step], r[1::n * step], r[2::n * step]
        out.append([0.299 * a + 0.587 * b + 0.114 * c for a, b, c in zip(rr, gg, bb)])
    v = ((w + step - 1) // step, (h + step - 1) // step, out)
    _cache[key] = v
    return v


def ssim_windows(ra, rb, win=WIN, skip=None):
    """Two equal-size luma images (lists of rows) -> {(col, row): SSIM} over the WIN x WIN windows; skip(col, row)
    -> True leaves a window out."""
    h, w = len(ra), len(ra[0])
    n = float(win * win)
    out = {}
    for wy in range(h // win):
        ys = range(wy * win, wy * win + win)
        for wx in range(w // win):
            if skip and skip(wx, wy):
                continue
            x0, x1 = wx * win, wx * win + win
            sa = sb = saa = sbb = sab = 0.0
            for y in ys:
                la, lb = ra[y][x0:x1], rb[y][x0:x1]
                sa += sum(la)
                sb += sum(lb)
                saa += sum(v * v for v in la)
                sbb += sum(v * v for v in lb)
                sab += sum(p * q for p, q in zip(la, lb))
            ma, mb = sa / n, sb / n
            va, vb, cov = saa / n - ma * ma, sbb / n - mb * mb, sab / n - ma * mb
            out[(wx, wy)] = ((2 * ma * mb + C1) * (2 * cov + C2)) / ((ma * ma + mb * mb + C1) * (va + vb + C2))
    return out


def ssim_rows(ra, rb, win=WIN, skip=None):
    """-> (mean SSIM, worst SSIM, worst window (col, row), windows) of two luma images."""
    ws = ssim_windows(ra, rb, win, skip)
    if not ws:
        return 1.0, 1.0, None, 0
    where = min(ws, key=ws.get)
    return sum(ws.values()) / len(ws), ws[where], where, len(ws)


class Shot:
    """One shot key's images: the windows' SSIM of any two, computed once."""
    def __init__(self, paths, step=STEP, win=WIN, ignore=()):
        self.paths, self.step, self.win, self.ignore, self.pairs = paths, step, win, ignore, {}
        self.size = {k: luma(p, step)[:2] for k, p in paths.items()}

    def _skip(self, cx, cy):
        px = self.win * self.step
        x0, y0 = cx * px, cy * px
        return any(x0 < r[2] and r[0] < x0 + px and y0 < r[3] and r[1] < y0 + px for r in self.ignore)

    def windows(self, a, b):
        key = (a, b) if a < b else (b, a)
        if key not in self.pairs:
            if self.size[a] != self.size[b]:
                self.pairs[key] = None
            else:
                self.pairs[key] = ssim_windows(luma(self.paths[a], self.step)[2], luma(self.paths[b], self.step)[2],
                                               self.win, self._skip if self.ignore else None)
        return self.pairs[key]

    def mean(self, a, b):
        w = self.windows(a, b)
        return 0.0 if w is None else (sum(w.values()) / len(w) if w else 1.0)

    def low(self, x, others):
        """{'still': [...], 'moving': [...]}: the windows where x's best SSIM against OTHERS is below OTHERS' own
        lowest pair there, by the window's class for OTHERS; None when a size differs."""
        ws = [self.windows(x, o) for o in others]
        if any(w is None for w in ws):
            return None
        pair = [self.windows(a, b) for a, b in itertools.combinations(others, 2)]
        if any(w is None for w in pair):
            return None
        out = {'still': [], 'moving': []}
        for k in ws[0]:
            floor = min(w[k] for w in pair)
            if max(w[k] for w in ws) < floor:
                out['still' if floor >= STILL else 'moving'].append(k)
        return out

    def box(self, keys):
        if not keys:
            return None
        px = self.win * self.step
        xs, ys = [k[0] for k in keys], [k[1] for k in keys]
        return (min(xs) * px, min(ys) * px, (max(xs) + 1) * px, (max(ys) + 1) * px)


def judge(shot, mine, stored):
    """shot: a Shot holding MINE and the STORED keys -> {'verdict', 'low', 'noise', 'box', 'mean', 'nearest',
    'floor_mean'} (module doc)."""
    means = {r: shot.mean(mine, r) for r in stored}
    near = max(means, key=means.get)
    pair_means = [shot.mean(a, b) for a, b in itertools.combinations(stored, 2)]
    res = {'mean': round(means[near], 5), 'nearest': near,
           'floor_mean': round(min(pair_means), 5) if pair_means else None, 'runs': len(stored)}
    if len(stored) < 3:
        return dict(res, verdict='NO NOISE', low=None, noise=None, box=None)
    noise, best, boxes = {'still': 0, 'all': 0}, {}, {}
    for r in stored:
        others = [o for o in stored if o != r]
        lr, lm = shot.low(r, others), shot.low(mine, others)
        for d in (lr, lm):
            if d is not None:
                d['all'] = d['still'] + d.pop('moving')
        for c in noise:
            noise[c] = max(noise[c], len(lr[c]) if lr is not None else 1 << 30)
            if lm is not None and (c not in best or len(lm[c]) < best[c]):
                best[c], boxes[c] = len(lm[c]), lm[c]
    if not best:
        return dict(res, verdict='DIFF', low=None, noise=noise, box=None, size=True)
    over = [c for c in noise if best[c] > noise[c]]
    return dict(res, verdict='DIFF' if over else 'SAME', low=best, noise=noise,
                box=shot.box(boxes[over[0]] if over else boxes['all']))


def _run_shots(run):
    """RUN -> ({scenario: [(key, path)]}, the stored run's folder or None)."""
    import baseline
    import e2e
    dirs, man = baseline._run_input(run)
    out = {}
    if man is not None:
        for en in (man.get('scenarios') or [dict(man, name=man.get('scenario'))]):
            out[en['name']] = [(k, os.path.join(dirs[0], f)) for k, f in en.get('shots', [])]
        return out, os.path.abspath(dirs[0])
    for name, shots in e2e.run_shots(dirs).items():
        out[name] = baseline.shot_keys(shots)
    return out, None


def _holder(store, name, want_set=None):
    import baseline
    cands = [(c, e, s, d) for c, e, s, d in baseline.scenario_dirs(store)
             if name in baseline.dir_scenarios(d) and (not want_set or s == want_set)]
    own = [h for h in cands if h[2] == name]
    if own and not want_set:
        return own[0]
    return cands[0] if len(cands) == 1 else None


def compare(run, store, names=None, want_set=None, step=STEP, win=WIN, out=print):
    import baseline
    import e2e
    mask = e2e.load_shot_mask()
    mine, self_dir = _run_shots(run)
    rows, bad = [], 0
    for name, shots in mine.items():
        if names and name not in names:
            continue
        h = _holder(store, name, want_set)
        if h is None:
            out(f'{name}: no single stored folder (give --set)')
            bad += 1
            continue
        ents = [e for e in baseline.run_entries(h[3], name) if os.path.abspath(e['dir']) != self_dir]
        ref = {e['run']: dict((k, os.path.join(e['dir'], f)) for k, f in e['shots']) for e in ents}
        keys = [k for k, _ in shots]
        for k in sorted({k for v in ref.values() for k in v} - set(keys)):
            rows.append({'scenario': name, 'shot': k, 'verdict': 'RETAIL-ONLY'})
            out(f'{name} {k}: RETAIL-ONLY')
            bad += 1
        for k, path in shots:
            regions, rule = e2e.mask_for(k.split('#')[0], mask)
            if rule is not None:
                rows.append({'scenario': name, 'shot': k, 'verdict': 'MASKED', 'rule': list(rule)})
                out(f'{name} {k}: MASKED ({" ".join(map(str, rule))})')
                continue
            have = {r: v[k] for r, v in ref.items() if k in v}
            if not have:
                rows.append({'scenario': name, 'shot': k, 'verdict': 'OURS-ONLY'})
                out(f'{name} {k}: OURS-ONLY')
                bad += 1
                continue
            shot = Shot(dict(have, **{'\0run': path}), step, win, regions)
            j = judge(shot, '\0run', sorted(have))
            bad += j['verdict'] != 'SAME'
            rows.append(dict(j, scenario=name, shot=k))
            lw = j['low'] or {}
            nz = j['noise'] or {}
            out(f'{name} {k}: {j["verdict"]} low windows {lw.get("all")} (noise {nz.get("all")}), still '
                f'{lw.get("still")} (noise {nz.get("still")}) over {j["runs"]} runs; '
                f'mean {j["mean"]:.5f} (nearest {j["nearest"]}), stored lowest pair {j["floor_mean"]}'
                + (f'; box {j["box"]}' if j['verdict'] == 'DIFF' else ''))
    return rows, bad


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('pair')
    p.add_argument('a')
    p.add_argument('b')
    p.add_argument('--step', type=int, default=STEP)
    p.add_argument('--win', type=int, default=WIN)
    c = sub.add_parser('compare')
    c.add_argument('run')
    c.add_argument('--store')
    c.add_argument('--set')
    c.add_argument('--names', nargs='*')
    c.add_argument('--step', type=int, default=STEP)
    c.add_argument('--win', type=int, default=WIN)
    c.add_argument('--json')
    a = ap.parse_args(argv)
    if a.cmd == 'pair':
        wa, ha, ra = luma(a.a, a.step)
        wb, hb, rb = luma(a.b, a.step)
        if (wa, ha) != (wb, hb):
            print(json.dumps({'size': f'{wa * a.step}x{ha * a.step} vs {wb * a.step}x{hb * a.step}'}))
            return 1
        m, worst, where, n = ssim_rows(ra, rb, a.win)
        px = a.win * a.step
        box = (where[0] * px, where[1] * px, where[0] * px + px, where[1] * px + px) if where else None
        print(json.dumps({'ssim': m, 'worst': worst, 'box': box, 'windows': n}))
        return 0
    import baseline
    store = a.store or baseline.store_root(argparse.Namespace(store=None))
    rows, bad = compare(a.run, store, a.names, a.set, a.step, a.win)
    same = sum(r['verdict'] == 'SAME' for r in rows)
    masked = sum(r['verdict'] == 'MASKED' for r in rows)
    print(f'ssim: {same} SAME of {len(rows) - masked} compared, {masked} masked, {bad} not SAME')
    if a.json:
        with open(a.json, 'w', encoding='utf-8') as f:
            json.dump(rows, f, indent=1)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
