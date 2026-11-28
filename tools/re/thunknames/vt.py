"""Vtables of client.exe (symbols.json's ??_7 addresses): entries read until a non-thunk pointer or the next
known vtable. Entries are thunk slots (the incremental link's vtables point at the thunks), or
.text addresses that are not thunked (library code: _purecall 0x9CCD10 and others)."""
import collections
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from clientimage import ClientImage  # noqa: E402
from linklayout import thunk_table, pe_text  # noqa: E402

CLIENT = paths.CLIENT_EXE


def vtables():
    ci = ClientImage(CLIENT, check=False)
    body, base = pe_text(CLIENT)
    slots = {s for s, _ in thunk_table(body, base)}
    syms = json.load(open(paths.SYMBOLS))
    vt = sorted((int(x['address'], 16), x['symbol']) for x in syms if x['kind'] == 'vtable')
    starts = {a for a, _ in vt}
    out = []
    for a, sym in vt:
        ents = []
        va = a
        while True:
            if va != a and va in starts:
                break
            try:
                e = ci.u32(va)
            except Exception:
                break
            if e not in slots and not (0x401000 <= e < 0xAE68DD):
                break
            ents.append(e)
            va += 4
        out.append((a, sym, ents))
    return out


def families(vts, pure=0x9CCD10, owner=True):
    """{(vtable index vi, entry index k): family id} for vtables `vts` ([(addr, sym, entries)]): vtables A and B are
    one family at index k when they are joined by entries shared at the same index j >= k. A function inherited at
    index j means a common ancestor whose vtable reaches past j, so slots 0..j are the same virtuals in both; a
    shared entry at j < k says nothing about k (2026-11-27: every RefCountClass descendant shares Delete_This at
    [0], and one family over all indexes gave Policy's Evaluate the names of mappers' Get_Scene_ID / Lookup).
    Second rule (`owner`, on by default): the holders of a shared entry also agree below the length L of the
    shortest vtable holding it (their common ancestor's vtable is at least that long): this keeps a base and a
    derived class that overrides every base slot from k on (UIProcessClass / UIAnimProcessClass).
    The id is comparable only between pairs of the same k."""
    at = collections.defaultdict(set)
    for vi, (_, _, ents) in enumerate(vts):
        for k, e in enumerate(ents):
            if e != pure:
                at[k, e].add(vi)
    by_k = collections.defaultdict(list)
    for (k, _), vs in at.items():
        if len(vs) > 1:
            by_k[k].append(sorted(vs))
            low = min(len(vts[v][2]) for v in vs)
            if owner and low - 1 > k:               # unions carry down to every lower k (the loop runs k down)
                by_k[low - 1].append(sorted(vs))
    parent = list(range(len(vts)))

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x
    out = {}
    for k in range(max((len(e) for *_, e in vts), default=0) - 1, -1, -1):
        for vs in by_k.get(k, ()):
            for v in vs[1:]:
                parent[find(v)] = find(vs[0])
        for vi, (_, _, ents) in enumerate(vts):
            if k < len(ents):
                out[vi, k] = find(vi)
    return out


if __name__ == '__main__':
    v = vtables()
    print(len(v), sum(len(e) for _, _, e in v), sum(1 for *_, e in v if not e))
    for a, s, e in v[:3]:
        print(hex(a), s, [hex(x) for x in e[:6]])
