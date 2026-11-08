"""The ledger of what matches: each library's files, their copies and their (file, address) pairs,
kept as one JSON shard per source file (docs/pipeline-plan.md 3.3).

Layout under docs/data/imports/:

    <lib>/library.json            the library's own fields: base, totals (each copy's plan-time count),
                                  compile_errors
    <lib>/<key>.json              one source file's block, key = the file's base name in lower case
                                  (key_of): name, copy, in_tree, ambiguous, tiebreak, shared,
                                  shared_callers, short_known, expected, best, merged, headers_from;
                                  plus `description` for a file written from the client (RECONSTRUCTED)
    <lib>/headers/<name>.json     one header row: {"copy": its origin} (+ `description`); <name> keeps
                                  its case and the app's subdirectories (app/headers/client/ui_mesh.h.json).
                                  A shard with only a description is a RECONSTRUCTED entry with no row.

The counts (in_tree_total, ambiguous_total) are not stored: load() computes them. JSON is written
sorted, indent 1 (one array element per line), LF, final newline, so git merges additions on both
sides of a shard and two landings conflict only when they touch the same source file.

load(src) returns the single-file dict every reader used (imports.json's shape, counts computed):
src is None (the tree's ledger), a shard directory, a snapshot file (.work/imports.<sha>.json) or a
git revision (its shards, or its docs/data/imports.json before the split).

    python ledger.py export [SRC] [-o FILE]       the single-file snapshot (stdout without -o)
    python ledger.py export --rev SHA -o FILE     a revision's ledger (run_branch.py's baselines)
    python ledger.py split FILE [--root DIR] [--descriptions PYFILE]
                                                  one-shot: a single-file ledger into shards
    python ledger.py compare OLD [SRC]            OLD (a snapshot) against SRC's export: every field
                                                  equal, stored counts against the computed ones
    python ledger.py describe LIB NAME TEXT       a reconstructed file's description (RECONSTRUCTED):
                                                  app shared/auxdata/x.cpp -> app/x.json,
                                                  app shared/auxdata/x.h -> app/headers/shared/auxdata/x.h.json
"""
import argparse
import ast
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
if HERE not in sys.path:
    sys.path.insert(0, HERE)

REL = 'docs/data/imports'                       # the shards, relative to a tree's root
LEDGER = os.path.join(REPO, 'docs', 'data', 'imports')
OLD_REL = 'docs/data/imports.json'              # the single file before the split (old revisions)
STATE = os.path.join(REPO, 'docs', 'data', 'imports.json')
LIBRARY = 'library.json'
HEADERS = 'headers'
COUNTS = ('in_tree_total', 'ambiguous_total')
_RESERVED = re.compile(r'^(con|prn|aux|nul|com[0-9]|lpt[0-9])$', re.I)


def _text(path):
    with open(path, encoding='utf-8', newline='') as f:
        return f.read()


def _write(path, text):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write(text)


def dumps(obj):
    """The ledger's JSON text: sorted keys, indent 1, LF, final newline."""
    return json.dumps(obj, indent=1, sort_keys=True) + '\n'


def totals(block):
    """A library's computed counts: distinct in_tree addresses, distinct ambiguous addresses."""
    files = block.get('files', {}).values()
    return {'in_tree_total': len({a for f in files for a in f.get('in_tree', [])}),
            'ambiguous_total': len({a for f in files for a in f.get('ambiguous', [])})}


def with_counts(state):
    for block in state.values():
        block.update(totals(block))
    return state


# ------------------------------------------------------------------------------------------ shards

def shard_paths(state, descriptions=None):
    """{relative path under the ledger: shard object} for a single-file state (+ {(lib, name): text})."""
    descriptions = dict(descriptions or {})
    out = {}
    for lib, block in state.items():
        out[f'{lib}/{LIBRARY}'] = {k: v for k, v in block.items() if k not in ('files', 'headers') + COUNTS}
        for key, f in block.get('files', {}).items():
            if key == LIBRARY[:-5] or _RESERVED.match(key) or key != key.lower():
                raise SystemExit(f'{lib}: file key {key!r} cannot be a shard name')
            shard = dict(f)
            d = descriptions.pop((lib, f.get('name', key + '.cpp')), None)
            if d is not None:
                shard['description'] = d
            out[f'{lib}/{key}.json'] = shard
        seen = {}
        for name, copy in block.get('headers', {}).items():
            if any(_RESERVED.match(p.split('.')[0]) for p in name.split('/')):
                raise SystemExit(f'{lib}: header {name!r} cannot be a shard name')
            if name.lower() in seen:
                raise SystemExit(f'{lib}: headers {name!r} and {seen[name.lower()]!r} differ only by case')
            seen[name.lower()] = name
            shard = {'copy': copy}
            d = descriptions.pop((lib, name), None)
            if d is not None:
                shard['description'] = d
            out[f'{lib}/{HEADERS}/{name}.json'] = shard
    for (lib, name), d in descriptions.items():   # a description with no row in the state
        if name.endswith('.cpp'):
            raise SystemExit(f'{lib}/{name}: a description without a file block')
        out[f'{lib}/{HEADERS}/{name}.json'] = {'description': d}
    return out


def from_shards(items):
    """(state, descriptions) from (relative path, JSON text) pairs."""
    state, desc = {}, {}
    for rel, text in items:
        parts = rel.split('/')
        if len(parts) < 2 or not rel.endswith('.json'):
            continue
        lib = parts[0]
        obj = json.loads(text)
        block = state.setdefault(lib, {})
        if parts[1] == HEADERS and len(parts) > 2:
            name = '/'.join(parts[2:])[:-5]
            if 'copy' in obj:
                block.setdefault('headers', {})[name] = obj['copy']
            if 'description' in obj:
                desc[(lib, name)] = obj['description']
        elif len(parts) == 2 and parts[1] == LIBRARY:
            block.update(obj)
        elif len(parts) == 2:
            key = parts[1][:-5]
            d = obj.pop('description', None)
            block.setdefault('files', {})[key] = obj
            if d is not None:
                desc[(lib, obj.get('name', key + '.cpp'))] = d
    for block in state.values():
        block.setdefault('files', {})
        block['headers'] = dict(sorted(block.get('headers', {}).items()))
        block['files'] = dict(sorted(block['files'].items()))
    return dict(sorted(state.items())), dict(sorted(desc.items()))


def _walk(root):
    for d, _, fs in os.walk(root):
        for f in fs:
            if f.endswith('.json'):
                p = os.path.join(d, f)
                yield os.path.relpath(p, root).replace(os.sep, '/'), _text(p)


def _git(args, repo, **kw):
    return subprocess.run(['git', '-C', repo] + args, capture_output=True, text=True, encoding='utf-8', **kw)


def _rev_items(rev, repo):
    """(relative path, text) of the shards at a revision; None when it has none."""
    ls = _git(['ls-tree', '-r', '--name-only', rev, '--', REL + '/'], repo)
    if ls.returncode:
        raise SystemExit(f'ledger: {rev!r} is neither a path nor a revision: {ls.stderr.strip()}')
    names = [n for n in ls.stdout.splitlines() if n.endswith('.json')]
    if not names:
        return None
    out = subprocess.run(['git', '-C', repo, 'cat-file', '--batch'], input=''.join(f'{rev}:{n}\n' for n in names).encode(),
                         capture_output=True).stdout
    items, i = [], 0
    for n in names:
        nl = out.index(b'\n', i)
        size = int(out[i:nl].split()[2])
        items.append((n[len(REL) + 1:], out[nl + 1:nl + 1 + size].decode('utf-8')))
        i = nl + 1 + size + 1
    return items


def read(src=None, repo=None):
    """(state without counts, descriptions) from src (see load)."""
    repo = repo or REPO
    if src is None:
        src = os.path.join(repo, *REL.split('/'))
        if not os.path.isdir(src) and os.path.isfile(os.path.join(repo, *OLD_REL.split('/'))):
            src = os.path.join(repo, *OLD_REL.split('/'))
    if os.path.isdir(src):
        return from_shards(_walk(src))
    if os.path.isfile(src):
        state = json.loads(_text(src))
        for block in state.values():
            for c in COUNTS:
                block.pop(c, None)
        return state, {}
    items = _rev_items(src, repo)
    if items is not None:
        return from_shards(items)
    old = _git(['show', f'{src}:{OLD_REL}'], repo)
    if old.returncode:
        raise SystemExit(f'ledger: no ledger at {src}')
    state = json.loads(old.stdout)
    for block in state.values():
        for c in COUNTS:
            block.pop(c, None)
    return state, {}


def load(src=None, repo=None):
    """The ledger as imports.json's dict (counts computed). src: None, a shard directory, a snapshot
    file or a git revision (of `repo`, default this tree)."""
    return with_counts(read(src, repo)[0])


def loads_text(src=None, repo=None):
    """load() as the single-file text (export's)."""
    return dumps(load(src, repo))


def descriptions(src=None, repo=None):
    """{(library, file name): description}: the files written from the client (was RECONSTRUCTED)."""
    return read(src, repo)[1]


_reconstructed = None


def __getattr__(name):
    """RECONSTRUCTED: computed from the shards on first use."""
    global _reconstructed
    if name == 'RECONSTRUCTED':
        if _reconstructed is None:
            _reconstructed = descriptions()
        return _reconstructed
    raise AttributeError(f"module 'ledger' has no attribute {name!r}")


def save(state, root=None):
    """Write a state's shards under root (default the tree's ledger). Descriptions already in the
    shards are kept; a shard the state no longer has keeps only its description, or goes. Only
    changed files are written."""
    root = root or LEDGER
    old_state, desc = from_shards(_walk(root)) if os.path.isdir(root) else ({}, {})
    want = shard_paths(state, {k: v for k, v in desc.items() if k[0] in state})
    have = {rel for rel, _ in _walk(root)} if os.path.isdir(root) else set()
    for rel, obj in want.items():
        p = os.path.join(root, *rel.split('/'))
        text = dumps(obj)
        if os.path.isfile(p) and _text(p) == text:
            continue
        _write(p, text)
    for rel in have - set(want):
        if rel.split('/')[0] in state:
            os.remove(os.path.join(root, *rel.split('/')))


# ------------------------------------------------------------------------------------------ the verify's state

def load_state():
    from libdefs import drop_not_linked
    return app_state(drop_not_linked(load()))


def app_state(state):
    """The APP_LIBS libraries' entries, from the descriptions (RECONSTRUCTED): one file per .cpp
    (keeping its last verify result), every other file a header of origin 'reconstructed'."""
    from libdefs import APP_LIBS, key_of
    recon = __getattr__('RECONSTRUCTED')
    for lib in APP_LIBS:
        names = sorted(n for l, n in recon if l == lib)
        if not names:
            continue
        s = state.setdefault(lib, {})
        s['base'], s['totals'] = 'reconstructed', {}
        old, s['files'] = s.get('files', {}), {}
        for n in names:
            if not n.endswith('.cpp'):
                continue
            k = key_of(n)
            if k in s['files']:
                raise SystemExit(f'{lib}: {n} and {s["files"][k]["name"]} share the key {k} (see APP_LIBS)')
            s['files'][k] = {'copy': 'reconstructed', 'name': n, 'expected': {}, 'merged': []}
            for kept in ('in_tree', 'ambiguous', 'tiebreak', 'shared', 'shared_callers', 'short_known'):
                if kept in old.get(k, {}):
                    s['files'][k][kept] = old[k][kept]
        s['headers'] = {n: 'reconstructed' for n in names if not n.endswith('.cpp')}
    return state


def save_state(state):
    from vc6lock import hold_lock
    hold_lock()
    save(state)


# ------------------------------------------------------------------------------------------ commands

def literal_descriptions(pyfile):
    """RECONSTRUCTED's literal from a Python file (importer.py or ledger.py before the split)."""
    for node in ast.parse(_text(pyfile)).body:
        if isinstance(node, ast.Assign) and any(getattr(t, 'id', None) == 'RECONSTRUCTED' for t in node.targets):
            return ast.literal_eval(node.value)
    raise SystemExit(f'{pyfile}: no RECONSTRUCTED')


def split(path, root=None, descriptions_from=None):
    root = root or LEDGER
    if os.path.exists(root) and os.listdir(root):
        raise SystemExit(f'{root} is not empty')
    state = json.loads(_text(path))
    desc = literal_descriptions(descriptions_from) if descriptions_from else {}
    shards = shard_paths(state, desc)
    lower = {}
    for rel in shards:
        if rel.lower() in lower:
            raise SystemExit(f'{rel} and {lower[rel.lower()]} differ only by case')
        lower[rel.lower()] = rel
    for rel, obj in shards.items():
        _write(os.path.join(root, *rel.split('/')), dumps(obj))
    print(f'{len(shards)} shards in {root}: {len(state)} libraries, '
          f'{sum(len(b.get("files", {})) for b in state.values())} files, '
          f'{sum(len(b.get("headers", {})) for b in state.values())} header rows, {len(desc)} descriptions')


def describe(lib, name, text, root=None):
    """Write or update a reconstructed file's shard (its description; the RECONSTRUCTED entry): NAME is the
    path under the library with '/' (shared/auxdata/x.cpp). A .cpp gets <lib>/<key>.json with its name, a
    header <lib>/headers/NAME.json with copy 'reconstructed'; verify fills in the rest."""
    root = root or LEDGER
    name = name.replace('\\', '/')
    if name.lower().endswith('.cpp'):
        key = os.path.splitext(os.path.basename(name))[0].lower()
        p = os.path.join(root, lib, key + '.json')
        obj = json.loads(_text(p)) if os.path.isfile(p) else {}
        if obj.get('name', name) != name:
            raise SystemExit(f'{lib}: {name} and {obj["name"]} share the key {key}')
        obj['name'] = name
    else:
        p = os.path.join(root, lib, HEADERS, *name.split('/')) + '.json'
        obj = json.loads(_text(p)) if os.path.isfile(p) else {}
        obj.setdefault('copy', 'reconstructed')
    obj['description'] = text
    _write(p, dumps(obj))
    print(os.path.relpath(p, REPO).replace(os.sep, '/'))


def compare(old_path, src=None):
    """OLD snapshot against SRC's export: 0 when equal but for the counts, which must equal the computed."""
    old = json.loads(_text(old_path))
    new = load(src)
    problems = []
    if sorted(old) != sorted(new):
        problems.append(f'libraries differ: {sorted(set(old) ^ set(new))}')
    for lib in sorted(set(old) & set(new)):
        o, n = old[lib], new[lib]
        for k in sorted(set(o) | set(n)):
            if k in COUNTS:
                if k in o and o[k] != n[k]:
                    problems.append(f'{lib}: stored {k} {o[k]} != computed {n[k]}')
            elif o.get(k) != n.get(k):
                if k == 'files':
                    bad = [f for f in set(o[k]) | set(n[k]) if o[k].get(f) != n[k].get(f)]
                    problems.append(f'{lib}: {len(bad)} file blocks differ: {sorted(bad)[:10]}')
                else:
                    problems.append(f'{lib}: {k} differs')
        print(f"{lib:11s} files {len(n['files'])}, headers {len(n.get('headers', {}))}; in_tree_total stored "
              f"{o.get('in_tree_total')} computed {n['in_tree_total']}; ambiguous_total stored "
              f"{o.get('ambiguous_total')} computed {n['ambiguous_total']}")
    same_text = json.dumps(old, indent=1, sort_keys=True) == json.dumps(new, indent=1, sort_keys=True)
    print(f'compare: {len(problems)} problem(s); export text identical to {old_path}: {same_text}')
    for p in problems[:30]:
        print('  ' + p)
    return 1 if problems else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    e = sub.add_parser('export', help='the single-file snapshot')
    e.add_argument('src', nargs='?', help='shard directory, snapshot file or git revision (default: the tree)')
    e.add_argument('--rev', help='a git revision (same as SRC)')
    e.add_argument('-o', '--out', help='write here (default stdout)')
    s = sub.add_parser('split', help='one-shot: a single-file ledger into shards')
    s.add_argument('file')
    s.add_argument('--root', help='the shard directory (default docs/data/imports)')
    s.add_argument('--descriptions', metavar='PYFILE', help="a Python file whose RECONSTRUCTED literal gives the descriptions")
    d = sub.add_parser('describe', help="a reconstructed file's description (its RECONSTRUCTED entry)")
    d.add_argument('lib')
    d.add_argument('name', help="the path under the library with '/', e.g. shared/auxdata/x.cpp")
    d.add_argument('text')
    c = sub.add_parser('compare', help='a snapshot against an export: equal but for the counts')
    c.add_argument('old')
    c.add_argument('src', nargs='?')
    a = ap.parse_args()
    if a.cmd == 'export':
        text = loads_text(a.rev or a.src)
        if a.out:
            _write(os.path.abspath(a.out), text)
        else:
            sys.stdout.write(text)
    elif a.cmd == 'split':
        split(a.file, a.root, a.descriptions)
    elif a.cmd == 'describe':
        describe(a.lib, a.name, a.text)
    elif a.cmd == 'compare':
        sys.exit(compare(a.old, a.src))


if __name__ == '__main__':
    main()
