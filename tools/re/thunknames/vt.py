"""Vtables of client.exe (symbols.json's ??_7 addresses): entries read until a non-thunk pointer or the next
known vtable. Entries are thunk slots (the incremental link's vtables point at the thunks), or
.text addresses that are not thunked (library code: _purecall 0x9CCD10 and others)."""
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


if __name__ == '__main__':
    v = vtables()
    print(len(v), sum(len(e) for _, _, e in v), sum(1 for *_, e in v if not e))
    for a, s, e in v[:3]:
        print(hex(a), s, [hex(x) for x in e[:6]])
