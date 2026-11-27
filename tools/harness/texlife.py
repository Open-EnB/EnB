r"""Textures created against released, shot by shot, from the draw logs' "texture_life" (d3d8shot.c, 2026-11-26).

    python tools/harness/texlife.py DIR [DIR...] [--kind WxH[:FORMAT]] [--all]

DIR is an e2e session folder or a stored run (its shot_*.json draw logs, in frame order). The proxy counts every
IDirect3DDevice8::CreateTexture since the device's creation and every texture's last Release, per kind (size,
levels, format, usage, pool); each draw log holds the counts at its frame. Per shot this prints the totals and, for
each kind whose counts moved since the shot before, created +N released +M (the kinds of --kind only, when given;
FORMAT is D3DFORMAT's number, e.g. 23 R5G6B5, 21 A8R8G8B8). A texture a client creates and never gives back is a
kind whose live count (created - released) grows over cycles that should leave it where it was (test-plan SP-05,
bug B-12: StarmapDataClass::Create_Mesh 0x7BEF50's map pictures, one RObj2Texture texture per object the map draws
large enough, released when the object leaves). `--all` prints every kind's running counts at every shot.
A draw log from a proxy before the counter has no "texture_life": the shot is listed as 'no counts'.
"""
import argparse
import glob
import json
import os
import re
import sys


def kind_key(k):
    return (k['w'], k['h'], k['levels'], k['format'], k['usage'], k['pool'])


def kind_name(key):
    w, h, levels, fmt, usage, pool = key
    return f'{w}x{h} fmt {fmt} lv {levels} use {usage} pool {pool}'


def parse_kind(text):
    """'128x128' or '128x128:23' -> (w, h, format or None)"""
    m = re.fullmatch(r'(\d+)x(\d+)(?::(\d+))?', text)
    if not m:
        raise argparse.ArgumentTypeError(f'kind {text!r}: WxH or WxH:FORMAT')
    return int(m.group(1)), int(m.group(2)), int(m.group(3)) if m.group(3) else None


def wanted(key, kinds):
    if not kinds:
        return True
    return any(key[0] == w and key[1] == h and (f is None or key[3] == f) for w, h, f in kinds)


def shot_logs(folder):
    """[(frame, shot name, path)] in frame order"""
    out = []
    for p in glob.glob(os.path.join(folder, 'shot_*.json')):
        m = re.match(r'shot_(.+)_(\d+)\.json$', os.path.basename(p))
        if m:
            out.append((int(m.group(2)), m.group(1), p))
    return sorted(out)


def life_of(path):
    with open(path, encoding='utf-8') as f:
        d = json.load(f)
    return d.get('texture_life')


def rows(folder, kinds=None, show_all=False):
    """One dict a shot: name, frame, created, released, live, moved [(kind, +created, +released, live)]"""
    prev, out = {}, []
    for frame, name, path in shot_logs(folder):
        life = life_of(path)
        if life is None:
            out.append({'name': name, 'frame': frame, 'counts': False})
            continue
        now = {kind_key(k): (k['created'], k['released']) for k in life['kinds']}
        moved = []
        for key, (c, r) in sorted(now.items()):
            pc, pr = prev.get(key, (0, 0))
            if wanted(key, kinds) and (show_all or (c, r) != (pc, pr)):
                moved.append((key, c - pc, r - pr, c - r))
        out.append({'name': name, 'frame': frame, 'counts': True, 'created': life['created'],
                    'released': life['released'], 'live': life['live'], 'moved': moved})
        prev = now
    return out


def report(folder, kinds=None, show_all=False):
    lines = [folder]
    for r in rows(folder, kinds, show_all):
        if not r['counts']:
            lines.append(f"  {r['name']} ({r['frame']}): no counts")
            continue
        lines.append(f"  {r['name']} ({r['frame']}): created {r['created']}, released {r['released']}, "
                     f"live {r['live']}")
        for key, dc, dr, live in r['moved']:
            lines.append(f"    {kind_name(key)}: created +{dc}, released +{dr}, live {live}")
    return lines


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('dirs', nargs='+')
    ap.add_argument('--kind', action='append', type=parse_kind, default=[])
    ap.add_argument('--all', action='store_true')
    a = ap.parse_args(argv)
    for d in a.dirs:
        print('\n'.join(report(d, a.kind, a.all)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
