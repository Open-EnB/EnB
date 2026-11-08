"""Stand-in collisions (pipeline-plan §4 item 6a): functions a change defines anew in app/ that a hand stand-in
still defines.

    python tools/vc6/standins.py check [--rev A..B] [--repo DIR]

The stand-ins: programs/clientpoc/poc_standins.cpp, poc_world*.cpp, poc_gadget_*.cpp, stubs/app_stubs.cpp,
tests/clienttest/client_stubs.cpp (STANDINS). A landing that rebuilds a function one of them stands in for
links twice (LNK2005 in the POC or clienttest, HF1, HF2) or silently keeps the stand-in: the landing deletes the
stand-in in the same change.

New in A..B: (1) definitions (easource.function_spans: brace depth plus the head before each `{`, comments and
preprocessor lines blanked) in app/ files (.cpp .c .h .hpp .inl) that B has more of than A; (2) with ledgers given
(wt.py bring-over: the verified ledger), app pairs B has and A has not, named by docs/data/symbols.json at B (an
address it does not name yet is covered by (1) only). Each is looked up in the stand-in files as they are at B (a
landing that deletes the stand-in passes) by qualified name (`Class::Method`, a free function's plain name) and
parameter types: names are compared, then the parameter lists (names, defaults and spacing dropped) when both sides
have one, so `Value_To_String(char *, int, MoneyClass)` and the stand-in `Value_To_String(char *, int, const
MoneyClass &)` differ (another mangled name, no LNK2005). A pair from (2) has no parameter list: name only.
Not refused (printed as notes):
  * a collision that A already had: the same name and parameters defined in app/ and by the same stand-in file at A
    (an established coexistence that links: the app object is not pulled, e.g. client_stubs.cpp's soptions.cpp
    members, the POC's G_Assert_Report);
  * a stand-in the change adds itself (a deliberate one, e.g. clienttest's renamed Unknown_7BCA70);
  * a stand-in whose definition line, or the line just above it, says `standins: keep` (say why there).
The link check (wt.py bring-over (b)) catches what names cannot: an app object newly pulled by a new caller.

--rev: default the recorded base of the builder worktree the command runs in (.work/wt/NAME.base)..HEAD, else
HEAD~1..HEAD. Exit 1 with the list (symbol, where it is new, stand-in file:line) on a collision, 0 otherwise.
wt.py bring-over calls check() after its verify and refuses the landing on a collision.
"""
import argparse
import collections
import fnmatch
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HERE), 're'))
import easource  # noqa: E402

STANDINS = ('programs/clientpoc/poc_standins.cpp', 'programs/clientpoc/poc_world*.cpp',
            'programs/clientpoc/poc_gadget_*.cpp', 'stubs/app_stubs.cpp', 'tests/clienttest/client_stubs.cpp')
STANDIN_DIRS = sorted({p.rsplit('/', 1)[0] for p in STANDINS})
APP_EXT = ('.cpp', '.c', '.h', '.hpp', '.inl')
SYMBOLS = 'docs/data/symbols.json'
KEEP = 'standins: keep'
TYPE_WORDS = {'int', 'char', 'short', 'long', 'unsigned', 'signed', 'double', 'float', 'bool', 'void', 'const',
              'volatile', '__int64', 'wchar_t', 'struct', 'class', 'enum', 'union'}
Def = collections.namedtuple('Def', 'name line sig keep')


def _git(repo, *args, check=True):
    r = subprocess.run(['git', *args], cwd=repo, capture_output=True)
    if check and r.returncode:
        raise SystemExit(f'standins: git {" ".join(args)}: {r.stderr.decode(errors="replace").strip()}')
    return r


def show(repo, rev, path):
    """The file's text at REV (None: absent), or the working tree's when REV is None."""
    if rev is None:
        full = os.path.join(repo, *path.split('/'))
        return open(full, encoding='latin1').read() if os.path.isfile(full) else None
    r = _git(repo, 'cat-file', 'blob', f'{rev}:{path}', check=False)
    return r.stdout.decode('latin1') if r.returncode == 0 else None


def _param(p):
    """One parameter's type: default, name and spacing dropped."""
    p = re.sub(r'\s+', ' ', p.split('=', 1)[0]).strip()
    p = re.sub(r'\s*([*&,<>()\[\]])\s*', r'\1', p)
    p = re.sub(r'([\w*&])(?: |(?<=[*&]))[A-Za-z_]\w*\[[^\]]*\]$', r'\1*', p)     # char buf[] -> char*
    p = re.sub(r'\[[^\]]*\]$', '*', p)                               # char[] -> char*
    p = re.sub(r'^(?:struct|class|enum|union) ', '', p)
    m = re.match(r'^(.*[\w*&>])(?:\s+|(?<=[*&]))([A-Za-z_]\w*)$', p)
    if m and m.group(2) not in TYPE_WORDS and re.sub(r'\b(const|volatile)\b', '', m.group(1)).strip(' *&'):
        p = m.group(1).strip()
    return p


def signature(head):
    """The normalised parameter list of a definition's head text (a tuple), or None when it does not parse."""
    i = head.find('(')
    if i < 0:
        return None
    args = easource.balanced_args(head, i)
    if args is None:
        return None
    parts, depth, cur = [], 0, ''
    for c in args:
        if c in '<(':
            depth += 1
        elif c in '>)':
            depth -= 1
        if c == ',' and depth == 0:
            parts.append(cur)
            cur = ''
        else:
            cur += c
    parts.append(cur)
    out = tuple(_param(p) for p in parts if p.strip())
    return () if out == ('void',) else out


def definitions(text):
    """[Def(qualified name, line, parameter types or None, keep marker)] of the function bodies in TEXT."""
    clean = easource.strip_comments(text)
    lines = text.split('\n')
    out = []
    for name, head, body, _ in easource.function_spans(text):
        line = text.count('\n', 0, head) + 1
        keep = any(KEEP in lines[k] for k in (line - 2, line - 1) if 0 <= k < len(lines))
        h = clean[head:body]
        last = name.split('::')[-1]
        j = h.find(last)
        out.append(Def(name.lstrip(':'), line, signature(h[j + len(last):] if j >= 0 else h), keep))
    return out


def same(a, b):
    """Two definitions of one name collide: parameter lists equal, or one unknown."""
    return a is None or b is None or a == b


def standin_files(repo, rev=None):
    if rev is None:
        names = []
        for d in STANDIN_DIRS:
            full = os.path.join(repo, *d.split('/'))
            names += [d + '/' + f for f in sorted(os.listdir(full))] if os.path.isdir(full) else []
    else:
        names = _git(repo, 'ls-tree', '--name-only', rev, '--', *[d + '/' for d in STANDIN_DIRS]).stdout.decode().split()
    return [p for p in names if any(fnmatch.fnmatchcase(p, g) for g in STANDINS)]


def standin_index(repo, rev=None):
    """{qualified name: [(stand-in path, Def)]} at REV (None: the working tree)."""
    idx = collections.defaultdict(list)
    for p in standin_files(repo, rev):
        for d in definitions(show(repo, rev, p) or ''):
            idx[d.name].append((p, d))
    return idx


def new_in_diff(repo, old, new):
    """{qualified name: [(app path, parameter types)]}: definitions NEW has more of than OLD, per changed app/ file."""
    out = collections.defaultdict(list)
    names = _git(repo, 'diff', '--name-only', '--no-renames', old, new, '--', 'app/').stdout.decode().split('\n')
    for p in names:
        if not p.lower().endswith(APP_EXT):
            continue
        after = collections.Counter((d.name, d.sig) for d in definitions(show(repo, new, p) or ''))
        before = collections.Counter((d.name, d.sig) for d in definitions(show(repo, old, p) or ''))
        for n, sig in after - before:
            out[n].append((p, sig))
    return out


def app_pairs(led):
    """{address: file} of the app library's in-tree pairs (ledger.load's dict)."""
    out = {}
    for f, v in (led or {}).get('app', {}).get('files', {}).items():
        for x in v.get('in_tree', []):
            out[str(x).upper().replace('0X', '0x')] = f
    return out


def new_in_pairs(old_ledger, new_ledger, names):
    """{qualified name: [(app file (address), None)]} of the app pairs NEW_LEDGER has and OLD_LEDGER has not;
    NAMES: {address: name}."""
    out = collections.defaultdict(list)
    before = app_pairs(old_ledger)
    for addr, f in app_pairs(new_ledger).items():
        if addr not in before and names.get(addr):
            out[names[addr]].append((f'app/{f} ({addr})', None))
    return out


def symbol_names(repo, rev):
    text = show(repo, rev, SYMBOLS)
    if not text:
        return {}
    out = {}
    for e in json.loads(text):
        if e.get('kind') == 'function' and e.get('name'):
            out[e['address'].upper().replace('0X', '0x')] = e['name']
    return out


def app_defs_at(repo, rev, name):
    """[Def] of NAME in app/ at REV (git grep on its last component, then parsed)."""
    last = name.split('::')[-1]
    r = _git(repo, 'grep', '-l', '-F', last, rev, '--', 'app/', check=False)
    out = []
    for line in r.stdout.decode('latin1').splitlines():
        p = line.split(':', 1)[1] if line.startswith(rev + ':') else line
        if p.lower().endswith(APP_EXT):
            out += [d for d in definitions(show(repo, rev, p) or '') if d.name == name]
    return out


def check(repo, old, new, old_ledger=None, new_ledger=None):
    """(collisions, notes) of OLD..NEW. collisions: [(name, [where new], [(stand-in path, line)])]; notes: the
    exempt ones, one line each."""
    news = new_in_diff(repo, old, new)
    if new_ledger is not None:
        for n, w in new_in_pairs(old_ledger, new_ledger, symbol_names(repo, new)).items():
            if n not in news:          # a definition the diff names already carries its parameters
                news[n].extend(w)
    idx = standin_index(repo, new)
    found, notes = [], []
    old_idx = None
    for n in sorted(news):
        hits = []
        for p, d in idx.get(n, ()):
            where = sorted({w for w, sig in news[n] if same(sig, d.sig)})
            if not where:
                continue
            if d.keep:
                notes.append(f'{n}: {p}:{d.line} is marked `{KEEP}`')
                continue
            if old_idx is None:
                old_idx = standin_index(repo, old)
            olds = [od for op, od in old_idx.get(n, ()) if op == p and same(od.sig, d.sig)]
            if not olds:
                notes.append(f'{n}: the stand-in {p}:{d.line} is new in this change too (deliberate)')
                continue
            if any(same(a.sig, d.sig) for a in app_defs_at(repo, old, n)):
                notes.append(f'{n}: app/ and {p}:{d.line} both defined it before this change (it links)')
                continue
            hits.append((p, d.line, where))
        if hits:
            found.append((n, sorted({w for _, _, ws in hits for w in ws}), [(p, l) for p, l, _ in hits]))
    return found, notes


def report(found):
    """The refusal's lines."""
    lines = [f'{len(found)} function(s) new in app/ still defined by a stand-in (delete the stand-in in the same '
             f'change, or mark it `{KEEP}` with why):']
    for n, where, stands in found:
        lines.append(f'  {n}: new in {", ".join(where[:3])}; stand-in at '
                     + ', '.join(f'{p}:{l}' for p, l in stands[:3]))
    return lines


def default_range(repo):
    top = os.path.normpath(_git(repo, 'rev-parse', '--show-toplevel').stdout.decode().strip())
    common = _git(repo, 'rev-parse', '--path-format=absolute', '--git-common-dir').stdout.decode().strip()
    root = os.path.normpath(os.path.dirname(common))
    rec = os.path.join(root, '.work', 'wt', os.path.basename(top) + '.base')
    if top != root and os.path.isfile(rec):
        base = open(rec, encoding='utf-8').read().split()
        if base:
            return base[0], 'HEAD'
    return 'HEAD~1', 'HEAD'


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('cmd', choices=['check'])
    ap.add_argument('--rev', help='A..B (default: the worktree\'s recorded base..HEAD, else HEAD~1..HEAD)')
    ap.add_argument('--repo', default=os.getcwd())
    a = ap.parse_args(argv)
    if a.rev:
        if '..' not in a.rev:
            raise SystemExit('standins: --rev takes A..B')
        old, new = a.rev.split('..', 1)
        new = new or 'HEAD'
    else:
        old, new = default_range(a.repo)
    found, notes = check(a.repo, old, new)
    for n in notes:
        print('note:', n)
    if found:
        print('\n'.join(report(found)))
        return 1
    print(f'stand-ins: no collision in {old[:12]}..{new[:12]}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
