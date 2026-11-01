"""Writes a COFF object of weak-external aliases (VC6's link has no /alternatename).

    python tools/vc6/coff_alias.py OUT.obj ALIAS=TARGET [ALIAS=TARGET ...]

Each ALIAS becomes a weak external (IMAGE_SYM_CLASS_WEAK_EXTERNAL, IMAGE_WEAK_EXTERN_SEARCH_ALIAS)
whose default is TARGET: a reference to ALIAS that nothing else defines resolves to TARGET's address.
The POC client (build.py link --program clientpoc) uses it for one client global that two app files
declare with different types (two mangled names, one variable: 0xBE67C4's FontManagerClass * and
LoadingViewFontManagerClass *). The object has no sections.
"""
import struct
import sys

IMAGE_FILE_MACHINE_I386 = 0x14C
IMAGE_SYM_CLASS_EXTERNAL = 2
IMAGE_SYM_CLASS_WEAK_EXTERNAL = 105
IMAGE_WEAK_EXTERN_SEARCH_ALIAS = 3


def write(path, pairs):
    strings = bytearray()
    symbols = bytearray()

    def name_field(name):
        raw = name.encode('latin1')
        if len(raw) <= 8:
            return raw.ljust(8, b'\0')
        offset = 4 + len(strings)
        strings.extend(raw + b'\0')
        return struct.pack('<II', 0, offset)

    count = 0
    for alias, target in pairs:
        target_index = count
        # the target: an undefined external
        symbols += name_field(target) + struct.pack('<IhHBB', 0, 0, 0, IMAGE_SYM_CLASS_EXTERNAL, 0)
        # the alias: a weak external with one auxiliary record (TagIndex, Characteristics)
        symbols += name_field(alias) + struct.pack('<IhHBB', 0, 0, 0, IMAGE_SYM_CLASS_WEAK_EXTERNAL, 1)
        symbols += struct.pack('<II', target_index, IMAGE_WEAK_EXTERN_SEARCH_ALIAS) + b'\0' * 10
        count += 3
    header = struct.pack('<HHIIIHH', IMAGE_FILE_MACHINE_I386, 0, 0, 20, count, 0, 0)
    with open(path, 'wb') as f:
        f.write(header + bytes(symbols) + struct.pack('<I', 4 + len(strings)) + bytes(strings))


if __name__ == '__main__':
    if len(sys.argv) < 3:
        raise SystemExit(__doc__)
    write(sys.argv[1], [a.split('=', 1) for a in sys.argv[2:]])
