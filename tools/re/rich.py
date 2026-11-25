"""The DOS stub's rich header: decode it, recompute its key, and compare two images' (image builder IM4,
2026-11-25; docs/binary.md §5.3).

LINK 6.00 writes, after the 64-byte DOS header and its 64-byte stub program, 'DanS' and three zero dwords, one
(comp.id, count) pair per (product, build) its inputs carried (an object's @comp.id symbol, an import member's
version, ...), 'Rich' and the key; every dword before 'Rich' is XORed with the key. The key is

    0x80 + sum(rol(byte, i) for i, byte in the 0x80 bytes of the DOS header and stub, e_lfanew's 4 left out)
         + sum(rol(comp.id, count % 32) for each pair)              (mod 2**32)

so the block is a function of the pair list (its order included) and the stub's bytes: an image reproduces
retail's 0x80-0x128 only when its link had retail's set of (product, build, count), in LINK's order.

    python tools/re/rich.py [EXE ...]            each image's pairs, and whether its key recomputes
    python tools/re/rich.py --diff OURS.exe      our pairs against retail's: count differences per (product, build)
"""
import argparse
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clientimage  # noqa: E402

# Product ids of the VC6 era (the rich header's high word), as far as the client's inputs use them.
PRODUCTS = {0: 'unmarked object', 1: 'imported functions (Import0)', 4: 'LINK 6.00 import member',
            6: 'CVTRES 5.00', 10: 'C compiler 12.00', 11: 'C++ compiler 12.00', 12: 'cvtomf / aliasobj 6',
            14: 'MASM 6.13', 19: 'LINK 5.12 import library', 28: 'C compiler 12.00 (newer)',
            29: 'C++ compiler 12.00 (newer)', 37: 'unknown 37'}


def rol(v, n):
    n %= 32
    return ((v << n) | (v >> (32 - n))) & 0xffffffff if n else v


def decode(data):
    """(start, end, key, pairs) of the rich block: start = 'DanS' offset, end = past the key; pairs
    [(product, build, count)] in the file's order. None when there is none."""
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    at = data.find(b'Rich', 0x80, pe)
    if at < 0:
        return None
    key = struct.unpack_from('<I', data, at + 4)[0]
    start = 0x80
    if struct.unpack_from('<I', data, start)[0] ^ key != 0x536e6144:      # 'DanS'
        return None
    words = [struct.unpack_from('<I', data, o)[0] ^ key for o in range(start + 16, at, 4)]
    pairs = [(words[i] >> 16, words[i] & 0xffff, words[i + 1]) for i in range(0, len(words), 2)]
    return start, at + 8, key, pairs


def key_of(dos, pairs):
    """The key for the 0x80 bytes of DOS header and stub `dos` and the pairs (product, build, count)."""
    k = 0x80
    for i, b in enumerate(dos[:0x80]):
        if 0x3c <= i < 0x40:
            continue
        k = (k + rol(b, i)) & 0xffffffff
    for p, b, c in pairs:
        k = (k + rol((p << 16) | b, c)) & 0xffffffff
    return k


def encode(dos, pairs):
    """The block LINK writes for these pairs: 'DanS', three zero dwords, the pairs, 'Rich', the key."""
    key = key_of(dos, pairs)
    words = [0x536e6144, 0, 0, 0]
    for p, b, c in pairs:
        words += [(p << 16) | b, c]
    return b''.join(struct.pack('<I', w ^ key) for w in words) + b'Rich' + struct.pack('<I', key)


def diff(ours, client):
    """[(product, build, client count, our count)] for every pair whose count differs, client's order first."""
    a = {(p, b): c for p, b, c in client}
    o = {(p, b): c for p, b, c in ours}
    keys = [(p, b) for p, b, _ in client] + [(p, b) for p, b, _ in ours if (p, b) not in a]
    return [(p, b, a.get((p, b), 0), o.get((p, b), 0)) for p, b in keys if a.get((p, b), 0) != o.get((p, b), 0)]


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('exe', nargs='*')
    ap.add_argument('--diff', metavar='OURS')
    ap.add_argument('--client', default=clientimage.DEFAULT_CLIENT)
    a = ap.parse_args()
    if a.diff:
        c = decode(open(a.client, 'rb').read())[3]
        o = decode(open(a.diff, 'rb').read())[3]
        print(f'retail {len(c)} pairs, ours {len(o)}; differing counts (product/build: retail, ours):')
        for p, b, x, y in diff(o, c):
            print(f'  {p:>3}/{b:<5} {PRODUCTS.get(p, "?"):30} {x:>5} {y:>5}  ({y - x:+d})')
        return
    for path in a.exe or [a.client]:
        d = open(path, 'rb').read()
        r = decode(d)
        if not r:
            print(f'{path}: no rich header')
            continue
        start, end, key, pairs = r
        pe = struct.unpack_from('<I', d, 0x3c)[0]
        ok = key_of(d[:0x80], pairs) == key and encode(d[:0x80], pairs) == d[start:end]
        print(f'{path}: {len(pairs)} pairs, block {start:#x}-{end:#x}, e_lfanew {pe:#x} ({pe - end} bytes after), '
              f'key {key:#010x} {"recomputes" if ok else "DOES NOT recompute"}')
        for p, b, c in pairs:
            print(f'  {p:>3}/{b:<5} {c:>5}  {PRODUCTS.get(p, "?")}')


if __name__ == '__main__':
    main()
