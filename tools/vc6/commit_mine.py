"""Commit only your own hunks when the git index is shared with other builders (D62, D112(2)).

    python tools/vc6/commit_mine.py MSGFILE [SPEC.json] [--base REV] [--file NAME ...] [--addr ADDR ...]
                                    [--path FILE ...] [--refresh-totals] [--dry]

A bare `git commit` takes whatever the other builders have staged, and `git commit -- <paths>` takes
the whole working-tree copy of a shared file (importer.py, an app header, docs/data/*) including their
uncommitted hunks. This script builds the commit in a private index instead (one per run): it reads
BASE (default HEAD) into it, adds the `whole` paths as they are in the working tree, applies only the
matching hunks of each `hunks` path, edits the row files and imports.json as below, writes the tree,
and moves the branch with a compare-and-swap: `git commit-tree TREE -p OLD` then
`git update-ref <branch> NEW OLD`, OLD being the HEAD the commit was built on. When another commit
landed in between, it rebuilds once on the new HEAD and retries, then fails loudly; it never moves
HEAD any other way. Afterwards the shared index's entries for the committed paths are pointed at the
new HEAD (other builders' unstaged edits of the same files stay in the working tree).

SPEC.json (optional when --file / --refresh-totals / --addr say everything):
    {"whole": ["ww3d2/textureloader.cpp", "docs/library-matching.md"],
     "hunks": {"tools/vc6/importer.py": ["textureloader", "WWSHADE_API_PATCHES"]},
     "addrs": ["0x72ABB0"], "files": ["app/client/client_lego_impl.cpp"]}
A hunk of `git diff -U0 BASE -- path` is yours when any of its +/- lines contains one of the keys.

docs/data/imports.json is edited by entry, not hunk:
  * `--file NAME` (repeatable), or a `hunks` key of imports.json that is a file-block name, makes the
    `"NAME": {` entry of a library's "files" map equal to the working copy's (inserted, replaced or
    removed), so another builder's adjacent new block never travels. It also takes the "headers" map
    rows whose stem is NAME's or its block's file stem (`"packets/pkfindmember.h": "reconstructed"`
    for pkfindmember); a NAME that is itself a header key (`--file packets/x.h`) takes that row.
  * `--refresh-totals` takes every library's count lines (ambiguous_total, in_tree_total, totals)
    from the working copy; otherwise hunks with "in_tree_total" are dropped (every verify rewrites them).
  * other imports.json keys keep the keyword-hunk behaviour.
docs/data/attempts.csv, not-exact.md and names-review.md are edited by row: the committed file is
BASE's plus or minus only the caller's rows (an attempts.csv row whose address or file is the
caller's; a .md row whose path is one of the caller's files), so neighbours' adjacent rows never
travel. The caller's addresses: --addr, SPEC "addrs", 0x keys anywhere in SPEC, and the in_tree
addresses of the --file blocks; the caller's files: --path, SPEC "files" and "whole", the --file
blocks' sources (lib/name) and their headers. These three files are taken this way whether they are
named in SPEC or not (a listed one with no rows of yours commits nothing). `--dry` prints the choices.

MSGFILE holds the full commit message (subject, body, the Co-Authored-By line).
Written by builder C on 2026-10-25 (.work/c/commit_mine.py), promoted by the coordinator; entries,
rows, totals and the compare-and-swap by K2 on 2026-10-27 (D112(2)).
"""
import argparse
import json
import os
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import commit_rows as cr  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
IMPORTS = 'docs/data/imports.json'
HUNK = re.compile(r'^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@(.*)$')


def renumber(hunks):
    """Rewrite the kept hunks' headers so their new-file positions follow the kept hunks only.

    `git apply` places a hunk at its new-file line (`+c`) and searches outward for the preimage; a
    -U0 insertion hunk has no preimage, so it lands exactly at `c`. When hunks before it were
    skipped (other builders'), `c` still counts their lines and the insertion lands inside another
    entry (d48e284 broke imports.json this way, 2026-10-26). So `c` is recomputed from the old-file
    position and the running delta of the kept hunks alone.
    """
    out, delta = [], 0
    for h in hunks:
        header = h[0].rstrip('\r\n')
        m = HUNK.match(header)
        if not m:
            sys.exit(f'unparsed hunk header: {header!r}')
        a = int(m[1])
        b = int(m[2]) if m[2] is not None else 1
        d = int(m[4]) if m[4] is not None else 1
        if b == 0:
            c = a + delta + 1      # insertion after old line a
        elif d == 0:
            c = a + delta - 1      # deletion of old lines a..a+b-1
        else:
            c = a + delta
        eol = h[0][len(header):]
        out.append([f'@@ -{a},{b} +{c},{d} @@{m[5]}{eol}'] + h[1:])
        delta += d - b
    return out


class Git:
    def __init__(self, index):
        self.index = index
        self.env = dict(os.environ, GIT_INDEX_FILE=index)

    def __call__(self, *args, inp=None, shared=False, check=True):
        r = subprocess.run(['git', *args], cwd=ROOT, env=None if shared else self.env, input=inp,
                           capture_output=True)
        if r.returncode and check:
            sys.exit(f'git {" ".join(args)}: {r.stderr.decode(errors="replace")}')
        return r if not check else r.stdout

    def text(self, *args, **kw):
        return self(*args, **kw).decode('utf-8', 'surrogateescape')

    def blob(self, rev_path):
        """Blob text at REV:path ('' when absent)."""
        r = self('cat-file', 'blob', rev_path, check=False)
        return r.stdout.decode('utf-8', 'surrogateescape') if r.returncode == 0 else ''

    def work(self, p):
        """The working copy of p in blob form (the clean filter applied: CRLF as the repository stores it)."""
        if not os.path.exists(os.path.join(ROOT, p)):
            return ''
        sha = self.text('hash-object', '-w', '--', p).strip()
        return self.blob(sha)

    def put(self, p, text, base):
        """Stage text (blob form) as p in the private index."""
        sha = self.text('hash-object', '-w', '--no-filters', '--stdin',
                        inp=text.encode('utf-8', 'surrogateescape')).strip()
        mode = '100644'
        ls = self.text('ls-tree', base, '--', p).split()
        if ls:
            mode = ls[0]
        self('update-index', '--add', '--cacheinfo', f'{mode},{sha},{p}')


def caller_set(spec, a, base_imports, work_imports, blocks):
    """(addresses, files) that make an attempts.csv / not-exact.md / names-review.md row the caller's."""
    addrs = set(a.addr) | set(spec.get('addrs', []))
    for keys in spec.get('hunks', {}).values():
        addrs |= {k for k in keys if re.fullmatch(r'0x[0-9A-Fa-f]{6,8}', k)}
    files = set(a.path) | set(spec.get('files', [])) | set(spec.get('whole', []))
    for text in (base_imports, work_imports):
        if not text:
            continue
        d = json.loads(text)
        for lib, name in blocks:
            ent = d.get(lib, {}).get('files', {}).get(name)
            if ent:
                addrs |= set(ent.get('in_tree', []))
                if ent.get('name'):
                    files.add(f'{lib}/{ent["name"]}')
            for h in d.get(lib, {}).get('headers', {}):
                if (lib, h) in blocks:
                    files.add(f'{lib}/{h}')
    return addrs, files


def stem(p):
    return os.path.splitext(os.path.basename(p))[0].lower()


def resolve_names(names, base_t, work_t):
    """(file blocks, header rows) as (lib, key) pairs for the caller's --file NAMEs."""
    blocks, headers = [], []
    maps = {}
    for t in (base_t, work_t):
        if t:
            for lib, v in json.loads(t).items():
                if isinstance(v, dict):
                    m = maps.setdefault(lib, {'files': {}, 'headers': set()})
                    m['files'].update(v.get('files', {}))
                    m['headers'] |= set(v.get('headers', {}))
    for name in names:
        hit = False
        for lib, m in maps.items():
            if name in m['files']:
                hit = True
                blocks.append((lib, name))
                stems = {name.lower(), stem(m['files'][name].get('name') or name)}
                headers += [(lib, h) for h in sorted(m['headers']) if stem(h) in stems]
            if name in m['headers']:
                hit = True
                headers.append((lib, name))
        if not hit:
            sys.exit(f'{IMPORTS}: no file block or header named {name!r} in the base or the working copy')
    return blocks, sorted(set(headers))


def build(git, a, spec, base):
    """Fill the private index from BASE plus the caller's work; returns the paths committed."""
    git('read-tree', base)
    paths = [p for p in spec.get('whole', []) if p not in cr.ROW_FILES]
    for p in paths:
        git('add', '--', p)
    hunk_specs = {p: k for p, k in spec.get('hunks', {}).items() if p not in cr.ROW_FILES}
    base_imp, work_imp = git.blob(f'{base}:{IMPORTS}'), git.work(IMPORTS)
    names = list(a.file)
    if IMPORTS in hunk_specs:
        bl, wl = base_imp.splitlines(), work_imp.splitlines()
        for k in hunk_specs[IMPORTS]:
            pat = re.compile(r'^   ' + re.escape(json.dumps(k)) + r': \{$')
            if any(pat.match(x) for x in bl) or any(pat.match(x) for x in wl):
                names.append(k)
        hunk_specs[IMPORTS] = [k for k in hunk_specs[IMPORTS] if k not in names]
        if not hunk_specs[IMPORTS]:
            del hunk_specs[IMPORTS]
    blocks, headers = resolve_names(names, base_imp, work_imp)
    for p, keys in hunk_specs.items():
        paths.append(p)
        d = git.text('diff', '-U0', base, '--', p)
        head, hunks, cur = [], [], None
        for line in d.splitlines(keepends=True):
            if line.startswith('@@'):
                cur = [line]
                hunks.append(cur)
            elif cur is None:
                head.append(line)
            else:
                cur.append(line)
        if p == IMPORTS and not any('in_tree_total' in k for k in keys):
            n = len(hunks)
            hunks = [h for h in hunks if not any('"in_tree_total"' in x for x in h[1:] if x[:1] in '+-')]
            if n != len(hunks):
                print(f'{p}: dropped {n - len(hunks)} in_tree_total hunk(s)')
        keep = [h for h in hunks if any(k in ''.join(x for x in h[1:] if x[:1] in '+-') for k in keys)]
        print(f'{p}: {len(keep)} of {len(hunks)} hunks are yours')
        if a.dry:
            for h in keep:
                print('   ', h[0].strip(), '|', ' '.join(x.strip() for x in h[1:])[:120])
        if keep:
            patch = ''.join(head) + ''.join(''.join(h) for h in renumber(keep))
            git('apply', '--cached', '--unidiff-zero', '-', inp=patch.encode('utf-8', 'surrogateescape'))
    # imports.json: file blocks, header rows, totals
    if blocks or headers or a.refresh_totals:
        cur = git.blob(f':{IMPORTS}')
        changes = []
        try:
            for mapname, sel in (('files', blocks), ('headers', headers)):
                for lib in sorted({lib for lib, _ in sel}):
                    cur, ch = cr.set_entries(cur, work_imp, lib, mapname, [k for l2, k in sel if l2 == lib])
                    changes += [f'{lib}.{mapname} {c}' for c in ch]
            if a.refresh_totals:
                cur, ch = cr.set_totals(cur, work_imp)
                changes += ch
        except ValueError as e:
            sys.exit(f'{IMPORTS}: {e}')
        print(f'{IMPORTS}: {len(changes)} entr{"y" if len(changes) == 1 else "ies"} taken: {", ".join(changes) or "none"}')
        if changes:
            git.put(IMPORTS, cur, base)
            paths.append(IMPORTS)
    # row files
    addrs, files = caller_set(spec, a, base_imp, work_imp, blocks + headers)
    owned = cr.owner(addrs, files)
    named = set(spec.get('whole', [])) | set(spec.get('hunks', {}))
    for p in cr.ROW_FILES:
        if p in spec.get('hunks', {}):
            ks = spec['hunks'][p]
            owned_p = cr.owner(addrs | {k for k in ks if cr.norm_addr(k)}, files | {k for k in ks if '/' in k})
        else:
            owned_p = owned
        b, w = git.blob(f'{base}:{p}'), git.work(p)
        if b == w:
            continue
        text, added, removed = cr.merge_rows(p, b, w, owned_p)
        if added or removed or p in named:
            print(f'{p}: {len(added)} row(s) added, {len(removed)} removed (yours only)')
        if a.dry:
            for raw in removed:
                print('    -', raw.strip()[:120])
            for raw in added:
                print('    +', raw.strip()[:120])
        if text != b:
            git.put(p, text, base)
            paths.append(p)
    return paths


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('msgfile')
    ap.add_argument('spec', nargs='?')
    ap.add_argument('--base', help='build on this revision instead of HEAD (no rebuild on a race)')
    ap.add_argument('--file', action='append', default=[], help='imports.json file block or header row to take (repeatable)')
    ap.add_argument('--addr', action='append', default=[], help='an address whose attempts.csv rows are yours (repeatable)')
    ap.add_argument('--path', action='append', default=[], help='a source path whose rows are yours (repeatable)')
    ap.add_argument('--refresh-totals', action='store_true', help="take imports.json's count lines from the working copy")
    ap.add_argument('--dry', action='store_true', help='show the choices and stop before committing')
    a = ap.parse_args()
    spec = json.load(open(a.spec)) if a.spec else {}
    os.makedirs(os.path.join(ROOT, '.work'), exist_ok=True)
    index = os.path.join(ROOT, '.work', f'commit_mine.{os.getpid()}.index')
    git = Git(index)
    ref = git('symbolic-ref', '-q', 'HEAD', check=False).stdout.decode().strip() or 'HEAD'
    try:
        for attempt in (1, 2):
            old = git.text('rev-parse', 'HEAD').strip()
            base = a.base or old
            if os.path.exists(index):
                os.remove(index)
            paths = build(git, a, spec, base)
            tree = git.text('write-tree').strip()
            stat = git.text('diff', '--cached', '--stat', base)
            if not stat.strip():
                sys.exit('nothing to commit: no whole path changed and no hunk, entry or row matched')
            print(stat)
            if a.dry:
                return
            new = git.text('commit-tree', tree, '-p', old, '-F', os.path.abspath(a.msgfile)).strip()
            hook = os.environ.get('COMMIT_MINE_BEFORE_SWAP')    # tests only: move the branch here
            if hook and (attempt == 1 or os.environ.get('COMMIT_MINE_BEFORE_SWAP_EVERY')):
                subprocess.run(hook, shell=True, cwd=ROOT)
            r = git('update-ref', '-m', 'commit_mine', ref, new, old, check=False)
            if r.returncode == 0:
                break
            now = git.text('rev-parse', 'HEAD').strip()
            print(f'compare-and-swap failed: {ref} is {now[:9]}, not {old[:9]} (built on); ', end='')
            if a.base or attempt == 2:
                sys.exit(f'commit_mine: FAILED, {ref} moved while committing; nothing committed '
                         f'(commit {new[:9]} is dangling). Rerun.')
            print('rebuilding once on the new HEAD')
        print(git.text('log', '-1', '--format=%h %s', new).strip())
        # the shared index still holds the old blobs for these paths: point them at the new HEAD
        for _ in range(5):
            r = git('reset', '-q', 'HEAD', '--', *paths, shared=True, check=False)
            if r.returncode in (0, 1):
                break
            time.sleep(2)
        else:
            print(f'warning: shared index not refreshed for {paths}: {r.stderr.decode(errors="replace")}')
    finally:
        if os.path.exists(index):
            os.remove(index)


if __name__ == '__main__':
    main()
