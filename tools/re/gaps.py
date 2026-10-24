"""Locates the client's version of every compiled function that is not byte-exact.

A library object's code section is linked whole, so its functions sit in the client in the same
order as in the object. Each run of unmatched functions therefore lies between the client
addresses of its matched neighbours. Within that window the client's functions are paired with
the unmatched ones (in order) and compared instruction by instruction, giving each gap a likely
address and a similarity: the worklist for reconstructing E&B's versions.

    python gaps.py --title wwlib --out ../../docs/data/gaps-wwlib file.obj...
"""
import argparse
import bisect
import difflib
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage  # noqa: E402
from objdiff import client_lines, norm, obj_lines  # noqa: E402
from objmatch import CoffObject, demangle, match_objects  # noqa: E402


def similarity(img, obj, sec, s, e, addr, client_len):
    left = [norm(t) for _, _, t in obj_lines(obj, sec, s, e)]
    right = [norm(t) for _, _, t in client_lines(img, addr, client_len)]
    return difflib.SequenceMatcher(None, left, right, autojunk=False).ratio()


def increasing_run(points):
    """Longest subsequence of (index, address) pairs whose addresses increase with the index."""
    best, prev = [], {}
    tails = []          # tails[k]: position in points of the smallest tail of a run of length k+1
    for p, (_, addr) in enumerate(points):
        k = bisect.bisect_left([points[t][1] for t in tails], addr)
        prev[p] = tails[k - 1] if k else None
        if k == len(tails):
            tails.append(p)
        else:
            tails[k] = p
    p = tails[-1] if tails else None
    while p is not None:
        best.append(points[p])
        p = prev[p]
    return best[::-1]


def gaps_for(img, starts, path):
    obj = CoffObject(path)
    funcs = obj.functions()
    results = match_objects(img, [path])
    rows = []
    # With /O2 (which implies /Gy) every function is its own COMDAT section; the linker keeps an
    # object's sections in order, so the sequence is by section index, then offset.
    items = sorted([(name, sec, s, e, r) for (name, sec, s, e), r in zip(funcs, results)],
                   key=lambda x: (x[1], x[2]))
    exact = [(i, int(it[4]['addresses'][0], 16)) for i, it in enumerate(items) if it[4]['status'] == 'exact']
    todo = [i for i, it in enumerate(items) if it[4]['status'] == 'none']
    if not todo:
        return rows
    if not exact:
        for i in todo:
            it = items[i]
            rows.append({'object': os.path.basename(path), 'function': demangle(it[0]), 'symbol': it[0],
                         'length': it[4]['length'], 'status': it[4]['status'], 'address': None,
                         'similarity': None, 'note': 'no matched function in this object'})
        return rows
    # Inline helpers from headers match too but are placed wherever the linker first met them:
    # keep the anchors near the object's main cluster, in increasing order.
    addrs = sorted(a for _, a in exact)
    med = addrs[len(addrs) // 2]
    near = [(i, a) for i, a in exact if abs(a - med) < 0x20000]
    anchors = increasing_run(near)
    length = {i: items[i][3] - items[i][2] for i in range(len(items))}
    span_lo = min(a for _, a in near) - 0x800
    span_hi = max(a + length[i] for i, a in near) + 0x800
    matched = {a for _, a in exact}
    used = set()
    for j in todo:
        name, sec, s, e, r = items[j]
        before = [(i, a) for i, a in anchors if i < j]
        after = [(i, a) for i, a in anchors if i > j]
        lo = before[-1][1] + length[before[-1][0]] if before else span_lo
        hi = after[0][1] if after else span_hi
        if hi <= lo:
            lo, hi = span_lo, span_hi
        i0 = bisect.bisect_left(starts, lo - 16)
        cands = [c for c in starts[i0:i0 + 400] if c < hi and c not in matched and c not in used][:64]
        best = (0.0, None)
        for c in cands:
            k = bisect.bisect_right(starts, c)
            clen = (starts[k] - c) if k < len(starts) else (e - s)
            sim = similarity(img, obj, sec, s, e, c, max(clen, 8))
            if sim > best[0]:
                best = (sim, c)
        if best[1] is not None:
            used.add(best[1])
        rows.append({'object': os.path.basename(path), 'function': demangle(name), 'symbol': name,
                     'length': r['length'], 'status': r['status'],
                     'address': f'0x{best[1]:06X}' if best[1] else None,
                     'similarity': round(best[0], 2) if best[1] else None,
                     'window': f'0x{lo:06X}-0x{hi:06X}'})
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('objects', nargs='+')
    ap.add_argument('--title', required=True)
    ap.add_argument('--out', required=True)
    a = ap.parse_args()
    img = ClientImage()
    starts, _ = img.function_starts(verified=True)
    rows = []
    seen = set()
    for p in sorted(a.objects):
        for r in gaps_for(img, starts, p):
            # an inline function from a header appears in every object that uses it: once is enough
            if r.get('symbol') in seen:
                continue
            seen.add(r.get('symbol'))
            rows.append(r)
    json.dump(rows, open(a.out + '.json', 'w'), indent=1)
    with open(a.out + '.md', 'w', encoding='utf-8') as f:
        f.write(f'# {a.title}: functions not byte-exact, located (generated by tools/re/gaps.py)\n\n')
        f.write('Each function our source compiles that the client does not contain byte for byte, '
                'paired with the client function found between its matched neighbours. Similarity '
                'is over instructions (relocations masked; 1.0 = same instructions, differing only '
                'in data such as constants or branch layout). `status`: `none` not found, '
                '`multiple` found more than once (then the pairing decides), `short` too short to '
                'search.\n\n')
        f.write('| Object | Function | Length | Status | Client | Similarity |\n|---|---|---|---|---|---|\n')
        for r in sorted(rows, key=lambda r: (r['object'], -(r['similarity'] or 0))):
            f.write(f"| {r['object']} | `{r['function'][:60]}` | {r['length']} | {r['status']} | "
                    f"{r['address'] or '-'} | {r['similarity'] if r['similarity'] is not None else '-'} |\n")
    n = len(rows)
    hi = sum(1 for r in rows if (r['similarity'] or 0) >= 0.9)
    print(f'{a.title}: {n} gaps located; {hi} at similarity >= 0.9')


if __name__ == '__main__':
    main()
