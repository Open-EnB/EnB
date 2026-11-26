"""behaudit.py's `seeds` check: stashed forms the tree has not adopted (run 2026-11-25, at the coordinator's request).

Two sources in docs/attempts/ (D185 stashes), each row a function whose stash scores better than the tree or whose
patch the tree never took:
  variant   docs/attempts/<ADDR>/variant_*.txt whose first line says `similarity S vs the tree's T` with S > T, whose
            body (whitespace dropped) is not in the tree file that defines its first `Class::Method(` line, and whose
            address the ledger does not credit exact (SD's .work/sd/varcheck.py + varfilter.py: six cuts that
            unadopted.py's score-gap list missed)
  seed      every hunk of every stashed seed patch (JSON `*seed_patch` strings, `+++ seed/...` sections, `SEED PATCH:`
            blocks): its removed / added lines (longer than 6 characters, whitespace dropped) looked up in the target
            file; UNADOPTED = every removed line still there and an added one missing, UNADOPTED-removal = a pure
            removal whose lines are all still there (R3's .work/r3/seedcheck2.py, per hunk)
Rows carry the latest attempts.csv row of the address (status, score, evidence) so a parked one shows as parked.
"""
import csv
import glob
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HERE), 'vc6'))
TOPS = ('app', 'ww3d2', 'wwlib', 'wwmath', 'wwaudio', 'wwshade', 'wwsaveload', 'wwdebug', 'libs', 'binkmovie')


def norm(s):
    return re.sub(r'\s+', '', s)


def tree_files(tree):
    out = {}
    for top in TOPS:
        for root, _, fs in os.walk(os.path.join(tree, top)):
            for x in fs:
                out.setdefault(x.lower(), []).append(os.path.join(root, x).replace('\\', '/'))
    return out


def exact_addresses(tree):
    import ledger
    out = set()
    for lib in ledger.load(os.path.join(tree, 'docs', 'data', 'imports')).values():
        for f in (lib.get('files', {}) if isinstance(lib, dict) else {}).values():
            out.update(int(a, 16) for a in f.get('in_tree', []))
    return out


def latest_attempts(tree):
    out = {}
    p = os.path.join(tree, 'docs', 'data', 'attempts.csv')
    if os.path.exists(p):
        with open(p, encoding='utf-8', errors='replace') as fh:
            for r in csv.DictReader(fh):
                try:
                    out[int(r.get('address') or '', 16)] = r
                except ValueError:
                    pass
    return out


def variant_rows(tree, files, exact):
    rows = []
    texts = {}
    for f in sorted(glob.glob(os.path.join(tree, 'docs', 'attempts', '*', 'variant_*.txt'))):
        t = open(f, encoding='utf-8', errors='replace').read()
        m = re.search(r"similarity ([0-9.]+) vs the tree's ([0-9.]+)", t.split('\n', 1)[0])
        if not m or float(m.group(1)) <= float(m.group(2)):
            continue
        try:
            addr = int(os.path.basename(os.path.dirname(f)), 16)
        except ValueError:
            continue
        if addr in exact:
            continue
        body = '\n'.join(line for line in t.split('\n') if not line.startswith('//'))
        first = next((line for line in body.split('\n') if '::' in line and '(' in line), '').strip()
        hit = None
        if first:
            for lst in files.values():
                for p in lst:
                    if p.endswith(('.cpp', '.h')):
                        if p not in texts:
                            texts[p] = open(p, encoding='utf-8', errors='replace').read()
                        if first in texts[p]:
                            hit = p
                            break
                if hit:
                    break
        if hit and norm(body) in norm(texts[hit]):
            continue
        rows.append((addr, 'variant', m.group(1), m.group(2), os.path.relpath(hit, tree) if hit else '?',
                     os.path.relpath(f, tree)))
    return rows


def resolve(tree, files, path):
    path = path.replace('\\', '/')
    for pre in ('a/', 'b/', ''):
        if path.startswith(pre) and os.path.exists(os.path.join(tree, path[len(pre):])):
            return [os.path.join(tree, path[len(pre):]).replace('\\', '/')]
    c = files.get(path.rsplit('/', 1)[-1].lower(), [])
    return [t for t in c if t.lower().endswith(path.lower())] or c


def split_diff(body):
    for m in re.finditer(r'^\+\+\+ (\S+)\n(.*?)(?=^--- |^\+\+\+ |\Z)', body, re.S | re.M):
        yield (m.group(1), m.group(2))


def patches_in(text):
    for m in re.finditer(r'"(\w*seed_patch)"\s*:\s*"((?:[^"\\]|\\.)*)"', text):
        try:
            body = json.loads('"' + m.group(2) + '"')
        except ValueError:
            continue
        if body.strip():
            for p in split_diff(body):
                yield (m.group(1),) + p
    for m in re.finditer(r'^\+\+\+ (seed/\S+)\n(.*?)(?=^--- |^\+\+\+ |^REPLAY PYTHON|^=+|\Z)', text, re.S | re.M):
        yield ('+++ seed', m.group(1)[5:], m.group(2))
    for m in re.finditer(r'^SEED PATCH:\n(.*?)(?=^[A-Z][A-Z ]+:\s*$|\Z)', text, re.S | re.M):
        for p in split_diff(m.group(1)):
            if not p[0].startswith('seed/'):
                yield ('SEED PATCH',) + p


def hunks(body):
    parts = re.split(r'^(@@[^\n]*)\n', body, flags=re.M)
    for i in range(1, len(parts) - 1, 2):
        yield parts[i], parts[i + 1]


def seed_rows(tree, files, exact):
    rows, seen, texts = [], set(), {}
    for f in sorted(glob.glob(os.path.join(tree, 'docs', 'attempts', '**', '*'), recursive=True)):
        if not os.path.isfile(f):
            continue
        try:
            t = open(f, encoding='utf-8', errors='replace').read().replace('\r', '')
        except OSError:
            continue
        if 'seed' not in t.lower():
            continue
        try:
            addr = int(os.path.relpath(f, os.path.join(tree, 'docs', 'attempts')).replace('\\', '/').split('/')[0], 16)
        except ValueError:
            addr = 0
        for kind, target, body in patches_in(t):
            tf = resolve(tree, files, target)
            for hdr, hb in hunks(body):
                key = (target.rsplit('/', 1)[-1], norm(hb))
                if key in seen:
                    continue
                seen.add(key)
                rem = [norm(x[1:]) for x in hb.split('\n') if x.startswith('-') and len(norm(x[1:])) > 6]
                add = [norm(x[1:]) for x in hb.split('\n') if x.startswith('+') and len(norm(x[1:])) > 6]
                if not (rem or add) or not tf:
                    continue
                if tf[0] not in texts:
                    texts[tf[0]] = norm(open(tf[0], encoding='utf-8', errors='replace').read())
                rp = sum(x in texts[tf[0]] for x in rem)
                ap = sum(x in texts[tf[0]] for x in add)
                if add:
                    status = 'UNADOPTED' if ap < len(add) and rp == len(rem) else ''
                else:
                    status = 'UNADOPTED-removal' if rp == len(rem) else ''
                if status and addr not in exact:
                    rows.append((addr, 'seed', '%d/%d removed lines kept' % (rp, len(rem)),
                                 '%d/%d added lines present' % (ap, len(add)), os.path.relpath(tf[0], tree) + ' ' + hdr,
                                 os.path.relpath(f, tree)))
    return rows


def rows(tree):
    """behaudit rows (dicts) of the seeds check."""
    files = tree_files(tree)
    exact = exact_addresses(tree)
    att = latest_attempts(tree)
    out = []
    for addr, kind, a, b, target, stash in variant_rows(tree, files, exact) + seed_rows(tree, files, exact):
        last = att.get(addr, {})
        out.append({'check': 'seeds', 'kind': kind, 'obj': target.replace('\\', '/')[:90], 'addr': '0x%06X' % addr,
                    'function': last.get('symbol', ''), 'status': last.get('status', ''), 'ratio': last.get('score', ''),
                    'how': '', 'off': '', 'ours': '', 'client_va': '', 'client': '', 'ours_val': a, 'client_val': b,
                    'expected': '', 'basis': stash.replace('\\', '/'),
                    'note': (last.get('date', '') + ' ' + (last.get('evidence') or '')[:100]).strip(),
                    'auto': '', 'known': ''})
    return out
