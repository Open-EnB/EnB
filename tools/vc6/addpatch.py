"""Applies patches to the tree and records them in one of importer.py's patch blocks.

    python addpatch.py --block WWSHADE_API_PATCHES SPEC.py
    python addpatch.py --block NEW_PATCHES --title "what the block is for (decision, doc)" SPEC.py

SPEC.py (keep it in .work/) defines P = [(lib, file, old, new, why), ...]: `old` is replaced by
`new` in the tree file, and `why` is the evidence (a client address). Write SPEC.py with a file
tool, not a shell heredoc: escapes such as \\t and \\n get mangled by the shell.

Every `old` must occur exactly once in the file at the moment it is applied (patches apply in
order, so a later one sees the earlier ones' results). Newlines are matched to the file's own.
Nothing is written unless every patch applies. The patches are appended to the block's entry for
(lib, file), so `importer.py assemble`/`replay` re-apply them after the file's PATCHES. A block
that doesn't exist yet is created (with --title as its comment) before the reconstructed files.
Run `importer.py replay` afterwards: it must report 0 differences.
"""
import argparse
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
IMP = os.path.join(HERE, 'importer.py')
sys.path.insert(0, HERE)
import importer  # noqa: E402

ANCHOR = '# Files reconstructed by us from the client'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--block', required=True, help="the importer.py dict, e.g. WWSHADE_API_PATCHES")
    ap.add_argument('--title', help='comment for a new block: what it is for, which decision')
    ap.add_argument('spec')
    a = ap.parse_args()
    block = a.block
    spec = runpy.run_path(a.spec)['P']

    t = open(IMP, encoding='utf-8', newline='').read()
    nl = '\r\n' if '\r\n' in t else '\n'
    t = t.replace('\r\n', '\n')
    opening = f'{block} = {{\n'
    if opening not in t:
        if not a.title:
            raise SystemExit(f'{block} does not exist yet: give --title to create it')
        new = (''.join(f'# {line}\n' for line in a.title.splitlines())
               + opening + '\n}\n'
               + f'for _key, _edits in {block}.items():\n'
               + '    PATCHES.setdefault(_key, []).extend(_edits)\n'
               + f'# end of {block}\n')
        assert t.count(ANCHOR) == 1
        t = t.replace(ANCHOR, new + ANCHOR)

    # Apply to the tree, in memory first.
    texts = {}
    for lib, name, old, new, why in spec:
        real = [x for x in os.listdir(importer.dest(lib)) if x.lower() == name.lower()]
        if not real:
            raise SystemExit(f'{lib}/{name}: no such file in the tree')
        path = os.path.join(importer.dest(lib), real[0])
        if path not in texts:
            texts[path] = open(path, encoding='latin1', newline='').read()
        s = texts[path]
        fnl = '\r\n' if '\r\n' in s else '\n'
        o, n = old.replace('\n', fnl), new.replace('\n', fnl)
        if s.count(o) != 1:
            raise SystemExit(f'{lib}/{name}: anchor occurs {s.count(o)} times: {old!r}')
        texts[path] = s.replace(o, n)

    # Record: one dict entry per (lib, file), items appended in order.
    head, rest = t.split(opening, 1)
    body, tail = rest.split('\n}\nfor _key, _edits', 1)
    body = body + '\n' if body else ''
    for lib, name, old, new, why in spec:
        key = repr((lib, name.lower()))
        item = f"        ({old!r},\n         {new!r},\n         {why!r}),\n"
        start = f'    {key}: [\n'
        if start in body:
            i = body.index(start)
            j = body.index('    ],\n', i)
            body = body[:j] + item + body[j:]
        else:
            body += start + item + '    ],\n'
    t = head + opening + body.rstrip('\n') + '\n}\nfor _key, _edits' + tail

    open(IMP, 'w', encoding='utf-8', newline='').write(t.replace('\n', nl))
    for path, s in texts.items():
        open(path, 'w', encoding='latin1', newline='').write(s)
    print(f'{len(spec)} patch(es) applied to the tree and recorded in {block}')


if __name__ == '__main__':
    main()
