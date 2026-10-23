"""Compiles one variant of a tree file and keeps its object, for objdiff.py.

    python trydiff.py LIB FILE VARIANTS.py NAME

VARIANTS.py is the same file tryvariants.py takes (V = {name: [(old, new), ...]}). The variant NAME
is applied to the tree file, the file is compiled alone with the library's flags into
%TEMP%\\enb_import\\td_NAME, and the tree file is restored, whatever happens. Use it when
tryvariants.py shows no gain: diff the variant's function against the client with
`objdiff.py %TEMP%\\enb_import\\td_NAME\\FILE.obj SYMBOL ADDRESS`.
"""
import os
import runpy
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import importer  # noqa: E402
import vc6  # noqa: E402


def main():
    if len(sys.argv) != 5:
        raise SystemExit(__doc__)
    lib, name, spec, var = sys.argv[1:5]
    d = importer.dest(lib)
    real = [x for x in os.listdir(d) if x.lower() == name.lower()]
    if not real:
        raise SystemExit(f'{lib}/{name}: no such file in the tree')
    path = os.path.join(d, real[0])
    orig = open(path, encoding='latin1', newline='').read()
    nl = '\r\n' if '\r\n' in orig else '\n'
    text = orig
    for old, new in runpy.run_path(spec)['V'][var]:
        old, new = old.replace('\n', nl), new.replace('\n', nl)
        if text.count(old) != 1:
            raise SystemExit(f'anchor occurs {text.count(old)} times: {old[:80]!r}')
        text = text.replace(old, new)
    out = os.path.join(importer.WORK, 'td_' + var)
    try:
        open(path, 'w', encoding='latin1', newline='').write(text)
        if os.path.isdir(out):
            shutil.rmtree(out)
        _, log = vc6.compile_sources([path], importer.tree_includes(), out=out,
                                     flags=importer.LIB_FLAGS.get(lib, vc6.DEFAULT_FLAGS))
        errors = [line for line in log.splitlines() if ' error ' in line]
        print('\n'.join(errors[:20]))
        print(out)
    finally:
        open(path, 'w', encoding='latin1', newline='').write(orig)


if __name__ == '__main__':
    main()
