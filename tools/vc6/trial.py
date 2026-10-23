"""Tries replacing a group of files in the repository with another EA copy's versions.

    python trial.py COPY LIB/FILE... [--keep-if-better] [--libs LIB...]

Copies the named files (headers and sources, e.g. ww3d2/part_buf.h, wwlib/sharebuf.h) from COPY
(R, G or Z) over the repository's, re-applies the importer's patches and annotations to them,
verifies the affected libraries and reports the client functions gained and lost per library.
The change is kept only with --keep-if-better and only if something is gained and nothing is
lost; otherwise every file is restored. A kept trial is recorded in docs/data/imports.json (the
source file's copy, or the header's origin) so the import report stays true.
"""
import argparse
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import annotations  # noqa: E402
import importer  # noqa: E402

DEPENDANTS = {
    'wwdebug': importer.LIBS,
    'wwlib': importer.LIBS,
    'wwmath': ['wwmath', 'wwsaveload', 'wwaudio', 'ww3d2', 'wwshade'],
    'wwsaveload': ['wwsaveload', 'wwaudio', 'ww3d2', 'wwshade'],
    'wwaudio': ['wwaudio', 'ww3d2', 'wwshade'],
    'ww3d2': ['ww3d2', 'wwaudio', 'wwshade'],
    'wwshade': ['wwshade'],
}


def matched(state, lib):
    return set().union(*[set(f.get('in_tree', [])) for f in state[lib]['files'].values()])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('copy', choices=['R', 'G', 'Z'])
    ap.add_argument('files', nargs='+', help='LIB/FILE')
    ap.add_argument('--keep-if-better', action='store_true')
    ap.add_argument('--libs', nargs='*', help='libraries to verify (default: the dependants of every library touched)')
    a = ap.parse_args()

    saved = []          # (lib, original name, bytes)
    placed = []         # (lib, new name)
    for spec in a.files:
        lib, name = spec.replace('\\', '/').split('/', 1)
        d = importer.dest(lib)
        src_dir = os.path.join(importer.COPY_ROOT[a.copy], importer.LIB_DIR[lib][a.copy])
        src_files = {f.lower(): f for f in os.listdir(src_dir)}
        if name.lower() not in src_files:
            raise SystemExit(f'{name} is not in {a.copy} {lib}')
        for old in [x for x in os.listdir(d) if x.lower() == name.lower()]:
            saved.append((lib, old, open(os.path.join(d, old), 'rb').read()))
            os.remove(os.path.join(d, old))
        new = src_files[name.lower()]
        shutil.copy2(os.path.join(src_dir, new), os.path.join(d, new))
        try:
            importer.apply_patches(lib, os.path.join(d, new), new)
        except SystemExit as e:
            print(f'  (patch not applied to {lib}/{new}: {e})')
        annotations.apply(lib, os.path.join(d, new), new)
        placed.append((lib, new))

    state = importer.load_state()
    libs = a.libs or sorted({l for lib, _ in placed for l in DEPENDANTS[lib]}, key=importer.LIBS.index)
    before = {l: matched(state, l) for l in libs}
    trial = json.loads(json.dumps(state))
    importer.verify(trial, libs)
    errors = sorted({os.path.basename(e.replace(chr(92), '/'))[:160] for l in libs for e in trial[l].get('compile_errors', [])})
    for e in errors[:15]:
        print('   ', e)
    gained = lost = 0
    for l in libs:
        after = matched(trial, l)
        g, x = len(after - before[l]), len(before[l] - after)
        gained += g
        lost += x
        print(f'{l:11s} {len(before[l]):5d} -> {len(after):5d}   gained {g}, lost {x}')

    if a.keep_if_better and gained > 0 and lost == 0:
        for lib, new in placed:
            f = trial[lib]['files'].get(importer.key_of(new))
            if new.lower().endswith('.cpp') and f:
                f['copy'], f['name'], f['merged'] = a.copy, new, []
            else:
                trial[lib].setdefault('headers', {})[new] = a.copy
        importer.save_state(trial)
        print(f'KEPT: {gained} gained, none lost')
        return
    for lib, new in placed:
        p = os.path.join(importer.dest(lib), new)
        if os.path.exists(p):
            os.remove(p)
    for lib, old, data in saved:
        open(os.path.join(importer.dest(lib), old), 'wb').write(data)
    print('restored' + ('' if a.keep_if_better else ' (dry run)'))


if __name__ == '__main__':
    main()
