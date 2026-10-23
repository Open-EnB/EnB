"""Lists one compiled function's relocations with the symbols they refer to.

    python objrel.py file.obj SYMBOL

objdiff.py shows every relocated field as REL, and shows an object's `call rel32` and the
client's `call dword ptr [__imp__...]` both as calls to REL. This names what each relocation in
our function points to (e.g. `__imp__AIL_lock@0` proves an import call), with its offset in the
function and its COFF type (6 = absolute, 20 = relative).
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from objmatch import CoffObject  # noqa: E402


def main():
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    obj = CoffObject(sys.argv[1])
    names = {s['index']: s['name'] for s in obj.symbols}
    found = False
    for name, sec, start, end in obj.functions():
        if name != sys.argv[2]:
            continue
        found = True
        for va, symidx, typ in sorted(obj.sections[sec]['relocs']):
            if start <= va < end:
                print(f'{va - start:#06x} {typ:2d} {names.get(symidx, symidx)}')
    if not found:
        raise SystemExit(f'{sys.argv[2]}: not defined in {sys.argv[1]} (objdiff.py --list shows the names)')


if __name__ == '__main__':
    main()
