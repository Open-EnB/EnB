"""String references per function, without a disassembler: every byte offset of .text whose 4 bytes equal a
string's VA, preceded by an imm32-taking opcode (push 68, mov r32 B8-BF, mov r/m32 C7 /0 with a short modrm,
lea / mov from a disp32), credited to the function starting at or before it (symbols.json functions and thunk
targets). Writes .work/names/strrefs.tsv (paths.work): target (function VA), slot (its thunk, if any), string VA, string."""
import bisect
import csv
import json
import re
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from clientimage import ClientImage  # noqa: E402
from linklayout import thunk_table, pe_text  # noqa: E402

CLIENT = paths.CLIENT_EXE


def main():
    ci = ClientImage(CLIENT, check=False)
    data = ci.data
    strings = {}
    for sec in ('.rdata', '.data'):
        try:
            s0, s1, raw = ci.section(sec)
        except Exception:
            continue
        blob = data[raw:raw + (s1 - s0)]
        for m in re.finditer(rb'[\x20-\x7e\t\r\n]{6,}\x00', blob):
            strings[s0 + m.start()] = m.group()[:-1].decode('latin1')
    body, base = pe_text(CLIENT)
    table = thunk_table(body, base)
    slot_of = {}
    for s, t in table:
        slot_of.setdefault(t, s)
    starts = sorted({int(x['address'], 16) for x in json.load(open(paths.SYMBOLS))
                     if x['kind'] == 'function'} | set(slot_of))
    arr = np.frombuffer(body, dtype=np.uint8)
    n = len(arr) - 4
    v = (arr[0:n].astype(np.uint32) | (arr[1:n + 1].astype(np.uint32) << 8) |
         (arr[2:n + 2].astype(np.uint32) << 16) | (arr[3:n + 3].astype(np.uint32) << 24))
    sv = np.array(sorted(strings), dtype=np.uint32)
    hit = np.nonzero(np.isin(v, sv))[0]
    out = []
    for off in hit:
        if off < 1:
            continue
        op = body[off - 1]
        ok = op == 0x68 or 0xB8 <= op <= 0xBF or (off >= 2 and body[off - 2] in (0xC7, 0x8D, 0x8B, 0xA1)) \
            or (off >= 3 and body[off - 3] == 0xC7) or (off >= 4 and body[off - 4] == 0xC7)
        if not ok:
            continue
        va = base + int(off)
        j = bisect.bisect_right(starts, va) - 1
        if j < 0:
            continue
        f = starts[j]
        sva = int(v[off])
        out.append((f, slot_of.get(f), sva, strings[sva]))
    with open(paths.work('strrefs.tsv'), 'w', newline='') as fo:
        w = csv.writer(fo, delimiter='\t', lineterminator='\n')
        w.writerow(['target', 'slot', 'string_va', 'string'])
        for f, s, sva, st in sorted(set(out)):
            w.writerow([f'0x{f:X}', f'0x{s:X}' if s else '', f'0x{sva:X}', st.replace('\t', ' ').replace('\n', '\\n')
                        .replace('\r', '\\r')])
    print(f'strings {len(strings)}, references {len(set(out))}, functions {len({x[0] for x in out})}, '
          f'thunked {len({x[0] for x in out if x[1]})}')


if __name__ == '__main__':
    main()
