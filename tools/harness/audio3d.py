"""AU-02 (test plan): 3D sound at each start: the sample's position, volume and distances and the listener's place
and facing, from the mss32log proxy's audio.jsonl (C2), compared with retail's stored runs within retail's spread.

    python tools/harness/audio3d.py show LOG [--scenario NAME]     the 3D starts of a session (or one scenario's cut)
    python tools/harness/audio3d.py compare RUN [--store DIR] [--set S] [--names N ...] [--json OUT]

The record (`spatial`, kept in each audio-NAME.json since 2026-11-27; baseline.audio_summary): one row per start
of a 3D sample (handle kind t): [sound, ms, position, volume, [min, max] distances, listener position, listener
forward]. The position is the sample's last AIL_set_3D_position since its load, or the first one after the start in
the same frame when the client places it only after starting it (positions to 1/1000, the listener's to 1).

compare: RUN (an e2e --out folder, a session folder or a stored run) against the store's runs of each scenario that
carry `spatial` (older runs do not: run the capture again). The starts are matched by (sound, its k-th start in the
scenario). Per matched start and per field (position, listener position, listener forward) the spread is the
largest distance between two stored runs' values; RUN's value is SAME when its distance to the nearest stored value
is within that spread (0.01 at least: the rounding); volume and distances SAME when equal to a stored run's. A start
held by fewer than two stored runs is FEW (named, not compared); a start no stored run has is OURS-ONLY, one every
stored run has and RUN lacks RETAIL-ONLY (the order and counts are AU-01's: baseline.py verify --audio-only).
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


def keyed(rows, names=None):
    """rows -> {(sound name, k): row}: the k-th start of each sound (k from 1)."""
    seen, out = {}, {}
    for row in rows:
        snd = (names or {}).get(row[0], row[0])
        seen[snd] = seen.get(snd, 0) + 1
        out[(snd, seen[snd])] = row
    return out


FIELDS = (('position', 2), ('listener', 5), ('forward', 6))


def compare_rows(mine, stored):
    """mine: {key: row}; stored: {run: {key: row}} -> [{'key', 'verdict', 'detail'}] (module doc)."""
    out = []
    keys = sorted(set(mine) | {k for v in stored.values() for k in v}, key=lambda k: (k[0] or '', k[1]))
    for key in keys:
        have = {r: v[key] for r, v in stored.items() if key in v}
        if key not in mine:
            if len(have) == len(stored):
                out.append({'key': key, 'verdict': 'RETAIL-ONLY', 'detail': f'in {len(have)} stored runs'})
            continue
        if not have:
            out.append({'key': key, 'verdict': 'OURS-ONLY', 'detail': ''})
            continue
        if len(have) < 2:
            out.append({'key': key, 'verdict': 'FEW', 'detail': f'{len(have)} stored run'})
            continue
        row, bad = mine[key], []
        for fname, i in FIELDS:
            vals = [x[i] for x in have.values()]
            pairs = [_dist(a, b) for j, a in enumerate(vals) for b in vals[j + 1:]]
            pairs = [p for p in pairs if p is not None]
            spread = max([MIN_SPREAD] + pairs)
            ds = [d for d in (_dist(row[i], v) for v in vals) if d is not None]
            near = min(ds) if ds else None
            if near is None or near > spread:
                bad.append(f'{fname} {row[i]} {"" if near is None else f"{near:.3f} "}from the nearest stored '
                           f'(spread {spread:.3f})')
        for fname, i in (('volume', 3), ('distances', 4)):
            if row[i] not in [x[i] for x in have.values()]:
                bad.append(f'{fname} {row[i]} against {sorted({json.dumps(x[i]) for x in have.values()})}')
        out.append({'key': key, 'verdict': 'DIFF' if bad else 'SAME', 'detail': '; '.join(bad)})
    return out


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
                stored[en['run']] = keyed(a['spatial'], a.get('names', {}))
        if len(stored) < 2:
            out(f'{name}: {len(stored)} stored runs with spatial rows (2 needed: capture again)')
            bad += 1
            continue
        res = compare_rows(keyed(sp, nm), stored)
        tally = {}
        for r in res:
            tally[r['verdict']] = tally.get(r['verdict'], 0) + 1
            if r['verdict'] in ('DIFF', 'OURS-ONLY', 'RETAIL-ONLY'):
                out(f'{name} {r["key"][0]} #{r["key"][1]}: {r["verdict"]} {r["detail"]}')
        n_bad = sum(tally.get(v, 0) for v in ('DIFF', 'OURS-ONLY', 'RETAIL-ONLY'))
        bad += n_bad
        out(f'{name}: {"SAME" if not n_bad else "DIFF"} over {len(stored)} stored runs: '
            + ', '.join(f'{v} {c}' for v, c in sorted(tally.items())))
        rows.append({'scenario': name, 'runs': sorted(stored), 'tally': tally,
                     'items': [dict(r, key=list(r['key'])) for r in res]})
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
