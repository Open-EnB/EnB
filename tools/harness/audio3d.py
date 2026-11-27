"""AU-02 (test plan): 3D sound at each start: the sample's position, volume and distances and the listener's place
and facing, from the mss32log proxy's audio.jsonl (C2), compared with retail's stored runs within retail's spread.

    python tools/harness/audio3d.py show LOG [--scenario NAME]     the 3D starts of a session (or one scenario's cut)
    python tools/harness/audio3d.py compare RUN [--store DIR] [--set S] [--names N ...] [--json OUT]

The record (`spatial`, kept in each audio-NAME.json since 2026-11-27; baseline.audio_summary): one row per start
of a 3D sample (handle kind t): [sound, ms, position, volume, [min, max] distances, listener position, listener
forward]. The position is the sample's last AIL_set_3D_position since its load, or the first one after the start in
the same frame when the client places it only after starting it (positions to 1/1000, the listener's to 1).

compare: RUN (an e2e --out folder, a session folder or a stored run) against the store's runs of each scenario that
carry `spatial` (older runs do not: run the capture again). Per sound, its starts pooled (concurrent starts come in
either order and the client draws each start's volume at random): per field (position, listener position, listener
forward) the spread is, leave one out, the farthest any stored start is from its nearest start of the same sound in
the other stored runs (0.01 at least: the rounding); each of RUN's starts is SAME when its nearest stored start is
within that spread. The volume within the stored starts' range, the distances one of the stored values. A sound in
fewer than two stored runs is FEW (named, not compared); one no stored run has is OURS-ONLY, one every stored run
has and RUN lacks RETAIL-ONLY (the order and counts are AU-01's: baseline.py verify --audio-only). The scenario is
SAME when no sound differs, or (3 stored runs or more) when RUN against any n - 1 stored runs differs in no more
sounds than a stored run does against the others (leave one out: retail's own noise).
Exit 0 when nothing differs.
"""
import argparse
import json
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

MIN_SPREAD = 0.01


def _r(v, nd=3):
    return round(float(v), nd)


def spatial(events):
    """The 3D starts of an event list (audiolog.load's, one scenario's cut or a session's) -> rows (module doc)."""
    state, lis, fwd, pending, out = {}, None, None, {}, []
    for e in events:
        ev, h = e.get('ev'), e.get('h')
        if h == '-':
            if ev == 'position':
                lis = [_r(e.get('x', 0), 0), _r(e.get('y', 0), 0), _r(e.get('z', 0), 0)]
            elif ev == 'orientation':
                fwd = [_r(e.get('fx', 0)), _r(e.get('fy', 0)), _r(e.get('fz', 0))]
            continue
        if not isinstance(h, str) or not h.startswith('t'):
            continue
        st = state.setdefault(h, {})
        if ev in ('load', 'init'):
            st.clear()
            pending.pop(h, None)
        elif ev == 'position':
            st['pos'] = [_r(e.get('x', 0)), _r(e.get('y', 0)), _r(e.get('z', 0))]
            row = pending.pop(h, None)
            if row is not None and e.get('f') == row[1]:
                row[0][2] = st['pos']
        elif ev == 'volume':
            st['vol'] = e.get('v')
        elif ev == 'distances':
            st['dist'] = [e.get('min'), e.get('max')]
        elif ev == 'start':
            row = [e.get('snd'), e.get('t'), st.get('pos'), st.get('vol'), st.get('dist'), lis, fwd]
            out.append(row)
            if row[2] is None:
                pending[h] = (row, e.get('f'))
        elif ev in ('stop', 'end'):
            pending.pop(h, None)
    return out


def _dist(a, b):
    if a is None or b is None:
        return None if a is not b else 0.0
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(a, b)))


def by_sound(rows, names=None):
    """rows -> {sound name: [row]} in order."""
    out = {}
    for row in rows:
        out.setdefault((names or {}).get(row[0], row[0]), []).append(row)
    return out


FIELDS = (('position', 2), ('listener', 5), ('forward', 6))


def _nearest(v, pool):
    ds = [d for d in (_dist(v, p) for p in pool) if d is not None]
    return min(ds) if ds else None


def compare_sounds(mine, stored):
    """mine: {sound: [row]}; stored: {run: {sound: [row]}} -> [{'sound', 'verdict', 'detail'}] (module doc)."""
    out = []
    for snd in sorted(set(mine) | {s for v in stored.values() for s in v}, key=str):
        have = {r: v[snd] for r, v in stored.items() if snd in v}
        if snd not in mine:
            if len(have) == len(stored):
                out.append({'sound': snd, 'verdict': 'RETAIL-ONLY', 'detail': f'in {len(have)} stored runs'})
            continue
        if not have:
            out.append({'sound': snd, 'verdict': 'OURS-ONLY', 'detail': f'{len(mine[snd])} starts'})
            continue
        if len(have) < 2:
            out.append({'sound': snd, 'verdict': 'FEW', 'detail': f'{len(have)} stored run'})
            continue
        bad = []
        for fname, i in FIELDS:
            spread = MIN_SPREAD
            for r, rows in have.items():         # leave one out: each stored start's nearest in the other runs
                pool = [x[i] for o, rs in have.items() if o != r for x in rs]
                for x in rows:
                    d = _nearest(x[i], pool)
                    if d is not None:
                        spread = max(spread, d)
            pool = [x[i] for rs in have.values() for x in rs]
            far = [(d, x) for x in mine[snd] for d in [_nearest(x[i], pool)] if d is None or d > spread]
            if far:
                d, x = max(far, key=lambda t: -1 if t[0] is None else t[0])
                bad.append(f'{fname}: {len(far)} of {len(mine[snd])} starts outside the stored spread {spread:.3f} '
                           f'(e.g. {x[i]} at {x[1]} ms, {"-" if d is None else f"{d:.3f}"} from the nearest)')
        vols = [x[3] for rs in have.values() for x in rs if x[3] is not None]
        if vols:
            lo, hi = min(vols), max(vols)
            out_v = [x[3] for x in mine[snd] if x[3] is not None and not lo <= x[3] <= hi]
            if out_v:
                bad.append(f'volume: {len(out_v)} starts outside the stored {lo}..{hi} (e.g. {out_v[0]})')
        dists = {json.dumps(x[4]) for rs in have.values() for x in rs}
        odd = {json.dumps(x[4]) for x in mine[snd]} - dists
        if odd:
            bad.append(f'distances {sorted(odd)} against {sorted(dists)}')
        counts = sorted(len(rs) for rs in have.values())
        out.append({'sound': snd, 'verdict': 'DIFF' if bad else 'SAME',
                    'detail': f'{len(mine[snd])} starts (stored {counts[0]}..{counts[-1]})'
                    + ('; ' + '; '.join(bad) if bad else '')})
    return out


BAD = ('DIFF', 'OURS-ONLY', 'RETAIL-ONLY')


def _n_bad(mine, stored):
    return sum(r['verdict'] in BAD for r in compare_sounds(mine, stored))


def noise_counts(mine, stored):
    """(retail's noise: the most sounds a stored run differs in against the other stored runs, RUN's fewest against
    any n - 1 of them); (None, None) with fewer than 3 stored runs."""
    if len(stored) < 3:
        return None, None
    noise = loo = None
    for r in stored:
        others = {o: v for o, v in stored.items() if o != r}
        n, m = _n_bad(stored[r], others), _n_bad(mine, others)
        noise = n if noise is None else max(noise, n)
        loo = m if loo is None else min(loo, m)
    return noise, loo


def _mine(run):
    """RUN -> ({scenario: (spatial rows, names)}, the stored run's folder or None)."""
    import baseline
    import e2e
    dirs, man = baseline._run_input(run)
    out = {}
    if man is not None:
        for en in (man.get('scenarios') or [dict(man, name=man.get('scenario'))]):
            if en.get('audio'):
                a = baseline.read_json(os.path.join(dirs[0], en['audio']))
                if 'spatial' in a:
                    out[en['name']] = (a['spatial'], a.get('names', {}))
        return out, os.path.abspath(dirs[0])
    results = e2e.run_results(dirs)
    for d in dirs:
        here = [(n, results[n]) for n in e2e.run_results([d]) if n in results]
        here.sort(key=lambda nr: nr[1].get('tick_start') or 0)
        summ, _ = baseline.session_audio(d, here)
        for n, a in summ.items():
            out[n] = (a.get('spatial', []), a.get('names', {}))
    return out, None


def compare(run, store, names=None, want_set=None, out=print):
    import baseline
    import ssim
    mine, self_dir = _mine(run)
    rows, bad = [], 0
    for name, (sp, nm) in mine.items():
        if names and name not in names:
            continue
        h = ssim._holder(store, name, want_set)
        if h is None:
            out(f'{name}: no single stored folder (give --set)')
            bad += 1
            continue
        stored = {}
        for en in baseline.run_entries(h[3], name):
            if os.path.abspath(en['dir']) == self_dir or not en.get('audio'):
                continue
            a = baseline.read_json(en['audio'])
            if 'spatial' in a:
                stored[en['run']] = by_sound(a['spatial'], a.get('names', {}))
        if len(stored) < 2:
            out(f'{name}: {len(stored)} stored runs with spatial rows (2 needed: capture again)')
            bad += 1
            continue
        mine_bs = by_sound(sp, nm)
        res = compare_sounds(mine_bs, stored)
        tally = {}
        for r in res:
            tally[r['verdict']] = tally.get(r['verdict'], 0) + 1
            if r['verdict'] in BAD:
                out(f'{name} {r["sound"]}: {r["verdict"]} {r["detail"]}')
        n_bad = sum(tally.get(v, 0) for v in BAD)
        noise, loo = noise_counts(mine_bs, stored)
        same = n_bad == 0 or (noise is not None and loo <= noise)
        bad += not same
        out(f'{name}: {"SAME" if same else "DIFF"} over {len(stored)} stored runs: '
            + ', '.join(f'{v} {c}' for v, c in sorted(tally.items()))
            + ('' if noise is None else f'; leave one out: {loo} sounds differ against {len(stored) - 1} stored '
               f'runs, a stored run against the others up to {noise}'))
        rows.append({'scenario': name, 'runs': sorted(stored), 'tally': tally, 'verdict': 'SAME' if same else 'DIFF',
                     'noise': noise, 'loo': loo, 'items': res})
    return rows, bad


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('show')
    s.add_argument('log')
    c = sub.add_parser('compare')
    c.add_argument('run')
    c.add_argument('--store')
    c.add_argument('--set')
    c.add_argument('--names', nargs='*')
    c.add_argument('--json')
    a = ap.parse_args(argv)
    if a.cmd == 'show':
        import audiolog
        path = a.log if a.log.endswith('.jsonl') else os.path.join(a.log, audiolog.LOG_NAME)
        ev = audiolog.load(path)
        import baseline
        nm = baseline.audio_names(ev)
        for row in spatial(ev):
            print(json.dumps([nm.get(row[0], row[0])] + row[1:]))
        return 0
    import baseline
    store = a.store or baseline.store_root(argparse.Namespace(store=None))
    rows, bad = compare(a.run, store, a.names, a.set)
    if a.json:
        with open(a.json, 'w', encoding='utf-8') as f:
            json.dump(rows, f, indent=1)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
