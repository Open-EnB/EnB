"""Hub helpers: the not-exact functions (attempts.csv partial / parked / blocked) ranked by their not-exact callers.

The user's lead (2026-11-22, from PR #248's work): making a shared helper exact can make its callers exact
too, when they are close. A caller's call bytes are relocated (masked), so the gain does not come through the
call itself: it comes through the helper's form, which also decides its inlined copies, the callers'
inline budget (workflow.md 3.1.1) and the prototype the callers compile against (argument pushes, the return
register). The direct callers measure how widely the helper is shared; inlined copies have no call and do not
show here.

    python tools/re/hubs.py [--status partial,parked,blocked] [--top N] [--near 0.8] [--lib L] [--min-open K]
                                                   the table, best hub first
    python tools/re/hubs.py --show ADDR...         one helper's callers: exact or not, status, score, size
    python tools/re/hubs.py ... --out FILE.md      the table as Markdown too

Per helper: size, its own latest status / score / blocker, `callers` (distinct calling functions),`exact` (callers already exact: nothing to gain there), `open` (callers not exact), `near`
(open callers whose latest attempts score >= --near), `rows` (open callers with any attempts row) and
`gain` (the sum of the open callers' latest scores, an unscored one counting 0). Ranked by near, then gain,
then open.

Data: call sets from siblings.py's features() (E8 calls between verified function starts, through the
incremental thunks; cached in .work/sib/features.pkl), exact = the ledger's in_tree addresses plus
symbols.json's exact functions, statuses and scores from docs/data/attempts.csv (the latest row per address;
a helper exact since its last row is left out). Scores are each row's own measure (objdiff similarity or
masked fraction), so compare them loosely. E9 tail jumps are not counted. Planning only: the verify decides.
"""
import argparse
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import siblings  # noqa: E402

HEX = siblings.HEX


def exact_set():
    ex = set(siblings.load_ledger())
    for s in json.load(open(os.path.join(siblings.DATA, 'symbols.json'))):
        if s['kind'] == 'function' and s.get('source') == 'exact':
            ex.add(int(s['address'], 16))
    return ex


def latest_rows():
    out = {}
    for a, rows in siblings.load_attempts().items():
        out[a] = max(rows, key=lambda r: r['date'])
    return out


def score(r):
    try:
        return float(r['score']) if r and r.get('score') else None
    except ValueError:
        return None


def build(statuses, near):
    fe = siblings.features()
    names = siblings.load_names()
    exact = exact_set()
    last = latest_rows()
    callers, sites = {}, {}
    for f, d in fe.items():
        for c in d['calls']:
            if c != f:
                callers.setdefault(c, set()).add(f)
    hubs = []
    for a, r in last.items():
        if r['status'] not in statuses or a in exact:
            continue
        cs = callers.get(a, set())
        opn = [c for c in cs if c not in exact]
        sc = [score(last.get(c)) for c in opn]
        hubs.append({
            'addr': a, 'name': names.get(a) or r.get('symbol') or '?', 'lib': r.get('library') or '?',
            'file': r.get('file') or '', 'size': fe.get(a, {}).get('size', 0), 'status': r['status'],
            'score': score(r), 'blocker': r.get('blocker') or '', 'callers': len(cs),
            'exact': len(cs) - len(opn), 'open': len(opn), 'rows': sum(1 for c in opn if c in last),
            'near': sum(1 for s in sc if s is not None and s >= near), 'gain': round(sum(s or 0 for s in sc), 2),
        })
    hubs.sort(key=lambda h: (-h['near'], -h['gain'], -h['open'], h['addr']))
    return hubs, callers, fe, names, exact, last


def fmt_score(s):
    return '-' if s is None else f'{s:.2f}'


def table(hubs, md=False):
    head = ['addr', 'name', 'lib', 'size', 'status', 'score', 'blocker', 'callers', 'exact', 'open', 'rows', 'near', 'gain']
    rows = [[HEX(h['addr']), h['name'][:48], h['lib'], str(h['size']), h['status'], fmt_score(h['score']),
             h['blocker'], str(h['callers']), str(h['exact']), str(h['open']), str(h['rows']), str(h['near']),
             f"{h['gain']:.2f}"]
            for h in hubs]
    if md:
        return '\n'.join(['| ' + ' | '.join(head) + ' |', '|' + '---|' * len(head)] +
                         ['| ' + ' | '.join(x.replace('|', '\\|') for x in r) + ' |' for r in rows])
    w = [max(len(x) for x in col) for col in zip(head, *rows)] if rows else [len(x) for x in head]
    return '\n'.join('  '.join(x.ljust(n) for x, n in zip(r, w)).rstrip() for r in [head] + rows)


def show(addrs, callers, fe, names, exact, last):
    for a in addrs:
        cs = sorted(callers.get(a, set()), key=lambda c: (c in exact, -(score(last.get(c)) or 0), c))
        r = last.get(a)
        own = f"{r['status']} {fmt_score(score(r))} {r.get('blocker') or ''}".strip() if r else \
            ('exact' if a in exact else 'no attempts row')
        print(f'{HEX(a)} {names.get(a) or (r or {}).get("symbol") or "?"} '
              f'(size {fe.get(a, {}).get("size", 0)}; {own}): {len(cs)} caller(s)')
        for c in cs:
            cr = last.get(c)
            st = 'exact' if c in exact else (f"{cr['status']} {fmt_score(score(cr))}" if cr else 'not exact, no row')
            nm = names.get(c) or (cr or {}).get('symbol') or '?'
            fl = f"  [{cr['file']}]" if cr and cr.get('file') else ''
            print(f'   {HEX(c):10s} {st:18s} size {fe.get(c, {}).get("size", 0):5d}  {nm}{fl}')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--status', default='partial,parked,blocked')
    ap.add_argument('--near', type=float, default=0.8, help='score from which an open caller counts as near')
    ap.add_argument('--top', type=int, default=30)
    ap.add_argument('--lib', help='only helpers of this library (attempts.csv library column)')
    ap.add_argument('--min-open', type=int, default=1, help='only helpers with at least this many open callers')
    ap.add_argument('--show', nargs='+', metavar='ADDR')
    ap.add_argument('--out', help='also write the table as Markdown')
    a = ap.parse_args()
    hubs, callers, fe, names, exact, last = build(set(a.status.split(',')), a.near)
    if a.show:
        show([int(x, 16) for x in a.show], callers, fe, names, exact, last)
        return
    hubs = [h for h in hubs if h['open'] >= a.min_open and (not a.lib or h['lib'] == a.lib)]
    top = hubs[:a.top] if a.top else hubs
    print(table(top))
    print(f'\n{len(hubs)} helper(s) with >= {a.min_open} open caller(s); near = open callers scored >= {a.near}')
    if a.out:
        with open(a.out, 'w', encoding='utf-8', newline='\n') as f:
            f.write(f'# Hub helpers (tools/re/hubs.py --status {a.status} --near {a.near})\n\n'
                    + table(hubs if not a.top else top, md=True) + '\n')


if __name__ == '__main__':
    main()
