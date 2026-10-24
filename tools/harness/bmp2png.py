"""Converts a 24-bit .bmp (vidtest's screenshots) to .png, without Pillow (M2.1).

    python tools/harness/bmp2png.py IN.bmp [OUT.png]      (default OUT: IN with .png)
"""
import os
import struct
import sys
import zlib


def read_bmp(path):
    d = open(path, 'rb').read()
    off = struct.unpack_from('<I', d, 10)[0]
    w, h = struct.unpack_from('<ii', d, 18)
    bpp = struct.unpack_from('<H', d, 28)[0]
    if bpp != 24:
        raise SystemExit(f'{path}: {bpp} bpp, only 24 handled')
    stride = ((w * 3) + 3) & ~3
    rows = []
    for y in range(abs(h)):
        sy = abs(h) - 1 - y if h > 0 else y        # bottom-up unless the height is negative
        row = d[off + sy * stride: off + sy * stride + w * 3]
        rgb = bytearray(w * 3)
        rgb[0::3] = row[2::3]
        rgb[1::3] = row[1::3]
        rgb[2::3] = row[0::3]
        rows.append(bytes(rgb))
    return w, abs(h), rows


def write_png(path, w, h, rows):
    def chunk(tag, data):
        return struct.pack('>I', len(data)) + tag + data + struct.pack('>I', zlib.crc32(tag + data) & 0xFFFFFFFF)
    raw = b''.join(b'\0' + r for r in rows)
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) \
        + chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b'')
    open(path, 'wb').write(png)


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    src = sys.argv[1]
    dst = sys.argv[2] if len(sys.argv) > 2 else os.path.splitext(src)[0] + '.png'
    w, h, rows = read_bmp(src)
    write_png(dst, w, h, rows)
    print(f'{dst}: {w}x{h}')


if __name__ == '__main__':
    main()
