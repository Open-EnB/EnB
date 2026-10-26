"""The attempt log: every near miss a builder tried and abandoned, so nobody retries it blind.

    python attempts.py record ADDR --symbol S --file F --status exact|partial|parked|blocked
                       [--score 0..1] [--forms N] [--blocker CLASS] [--evidence "..."]
                       [--stash PATH] [--model M] [--date "YYYY-MM-DD HH:MM"]
    python attempts.py show ADDR|SYMBOL        every row for it, newest first, and its stash files
    python attempts.py list [--status X] [--lib L] [--blocker B]
    python attempts.py check PATH... [--since "YYYY-MM-DD HH:MM"] [--ack ADDR...]

Rows go to docs/data/attempts.csv (tracked; written only by this tool): date, address, symbol,
file (repository-relative), library (from the file: `app` for app/..., else the file's directory),
status, score (objdiff similarity or masked fraction, 0..1), forms tried, blocker family, evidence,
stash, model. Statuses: `exact` (solved: keep the row, it says which form won), `partial` (still
being worked on or left at a score), `parked` (stopped after the stop rule; a new lead is needed),
`blocked` (waits on something else: a layout, an unbuilt class, a decision).

Blocker families are explain.py's classes: relocation, misplaced-start, frame, field-offset,
literal, register, order, inline-budget, shape; or `layout` / `source` (an unknown class or
missing code), or empty.

A stash is the source form worth keeping (a patch, a `.cpp.txt` or a forms spec `.py.txt`): small
text only, copied to docs/attempts/<ADDR>/ (never objects, logs or listings; 64 KB at most).

`check` is for builders before a retry: for each given source (tracked files: the lines changed
against HEAD, plus git's function headers of those hunks; untracked: the whole file) it collects
the client addresses written there (0x401000-0xBFFFFF) and the logged symbols named there, and
fails (exit 1) when one of them has a `parked` or `blocked` row newer than --since (when you last
read `show` for it; without --since every such row counts). `--ack ADDR` accepts one knowingly.

Rule (builders.md §2): read `attempts.py show` before retrying any function; record every near
miss you abandon.
"""
import argparse
import csv
import datetime
import os
import re
import shutil
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
CSV = os.path.join(REPO, 'docs', 'data', 'attempts.csv')
STASH = os.path.join(REPO, 'docs', 'attempts')
FIELDS = ['date', 'address', 'symbol', 'file', 'library', 'status', 'score', 'forms', 'blocker',
          'evidence', 'stash', 'model']
STATUSES = ('exact', 'partial', 'parked', 'blocked')
BLOCKERS = ('', 'relocation', 'misplaced-start', 'frame', 'field-offset', 'literal', 'register',
            'order', 'inline-budget', 'shape', 'layout', 'source')
APP_TOPS = ('app',)
STASH_MAX = 64 * 1024
STASH_REFUSED = ('.obj', '.lib', '.exe', '.pdb', '.log', '.od', '.dis', '.json', '.pkl')
ADDR_RE = re.compile(r'\b0x([0-9A-Fa-f]{6})\b')


def norm_addr(a):
    a = a.strip()
    v = int(a, 16)
    return f'0x{v:06X}'


def rel(path):
    full = os.path.abspath(path)
    try:
        r = os.path.relpath(full, REPO)
    except ValueError:
        return path.replace('\\', '/')
    return r.replace('\\', '/')


def library(file):
    parts = file.replace('\\', '/').split('/')
    if not file:
        return ''
    if parts[0].lower() in APP_TOPS:
        return parts[0].lower()
    return parts[-2].lower() if len(parts) > 1 else ''


def load():
    if not os.path.exists(CSV):
        return []
    with open(CSV, newline='', encoding='utf-8') as f:
        return list(csv.DictReader(f))


def append(row):
    new = not os.path.exists(CSV)
    os.makedirs(os.path.dirname(CSV), exist_ok=True)
    with open(CSV, 'a', newline='', encoding='utf-8') as f:
        w = csv.DictWriter(f, FIELDS, lineterminator='\n')
        if new:
            w.writeheader()
        w.writerow(row)


def stash(addr, src):
    """Copy SRC into docs/attempts/<addr>/; returns the repository-relative path."""
    if not src:
        return ''
    full = os.path.abspath(src)
    if full.startswith(os.path.abspath(STASH) + os.sep):
        return rel(full)
    if not os.path.isfile(full):
        raise SystemExit(f'--stash {src}: no such file')
    ext = os.path.splitext(full)[1].lower()
    if ext in STASH_REFUSED:
        raise SystemExit(f'--stash {src}: objects, logs and listings are not stashed (only source text)')
    if os.path.getsize(full) > STASH_MAX:
        raise SystemExit(f'--stash {src}: {os.path.getsize(full)} bytes; stash a patch or the function, not the file')
    name = os.path.basename(full)
    if ext not in ('.patch', '.diff', '.txt', '.md'):
        name += '.txt'  # a .cpp or .py in docs/ must not look like tree source
    d = os.path.join(STASH, addr[2:])
    os.makedirs(d, exist_ok=True)
    dst = os.path.join(d, name)
    shutil.copyfile(full, dst)
    return rel(dst)


def cmd_record(a):
    addr = norm_addr(a.address)
    if a.score is not None and not 0 <= a.score <= 1:
        raise SystemExit('--score is 0..1')
    if a.blocker not in BLOCKERS:
        raise SystemExit(f'--blocker: one of {", ".join(b for b in BLOCKERS if b)}')
    file = rel(a.file) if a.file and os.path.exists(a.file) else (a.file or '').replace('\\', '/')
    row = {'date': a.date or datetime.datetime.now().strftime('%Y-%m-%d %H:%M'),
           'address': addr, 'symbol': a.symbol or '', 'file': file, 'library': a.lib or library(file),
           'status': a.status, 'score': '' if a.score is None else f'{a.score:.2f}',
           'forms': '' if a.forms is None else str(a.forms), 'blocker': a.blocker,
           'evidence': (a.evidence or '').replace('\r', ' ').replace('\n', ' '),
           'stash': stash(addr, a.stash), 'model': a.model or os.environ.get('ENB_MODEL', '')}
    append(row)
    print(f"recorded {addr} {row['symbol']} {row['status']} ({len(load())} rows)")


def matches(row, key):
    if re.fullmatch(r'(0x)?[0-9A-Fa-f]{5,8}', key):
        return row['address'] == norm_addr(key)
    return key.lower() in row['symbol'].lower()


def fmt(r):
    head = f"{r['date']}  {r['address']}  {r['status']:7s}"
    extra = [f"score {r['score']}" if r['score'] else '', f"{r['forms']} forms" if r['forms'] else '',
             r['blocker'], r['model']]
    s = f"{head}  {r['symbol']}  [{r['file']}]  {', '.join(x for x in extra if x)}"
    if r['evidence']:
        s += f"\n      {r['evidence']}"
    if r['stash']:
        s += f"\n      stash: {r['stash']}"
    return s


def cmd_show(a):
    rows = [r for r in load() if matches(r, a.key)]
    if not rows:
        print(f'no attempts logged for {a.key}')
        return
    for r in sorted(rows, key=lambda r: r['date'], reverse=True):
        print(fmt(r))
    for addr in sorted({r['address'] for r in rows}):
        d = os.path.join(STASH, addr[2:])
        if os.path.isdir(d):
            for f in sorted(os.listdir(d)):
                print(f'  {rel(os.path.join(d, f))}')


def latest(rows):
    """The newest row per address."""
    out = {}
    for r in sorted(rows, key=lambda r: r['date']):
        out[r['address']] = r
    return out


def cmd_list(a):
    rows = load()
    newest = latest(rows)
    for r in sorted(newest.values(), key=lambda r: (r['library'], r['address'])):
        if a.status and r['status'] != a.status:
            continue
        if a.lib and r['library'] != a.lib.lower():
            continue
        if a.blocker and r['blocker'] != a.blocker:
            continue
        n = sum(1 for x in rows if x['address'] == r['address'])
        print(f"{r['address']}  {r['library']:9s} {r['status']:7s} {r['score'] or '-':>4s}  "
              f"{r['blocker'] or '-':14s} {r['symbol']}  ({n} rows, newest {r['date']})")


def touched_text(path):
    """Changed lines against HEAD (with git's function headers) for a tracked file, else the file."""
    full = os.path.abspath(path)
    r = subprocess.run(['git', '-C', REPO, 'ls-files', '--error-unmatch', full], capture_output=True)
    if r.returncode == 0:
        d = subprocess.run(['git', '-C', REPO, 'diff', '-U3', 'HEAD', '--', full], capture_output=True,
                           text=True, encoding='latin-1').stdout
        return '\n'.join(l for l in d.splitlines()
                         if l.startswith(('+', '-', '@@', ' ')) and not l.startswith(('+++', '---')))
    return open(full, encoding='latin-1').read()


def cmd_check(a):
    rows = load()
    newest = latest(rows)
    acked = {norm_addr(x) for x in a.ack or []}
    bad = []
    for p in a.paths:
        text = touched_text(p)
        addrs = {norm_addr(m) for m in ADDR_RE.findall(text) if 0x401000 <= int(m, 16) <= 0xBFFFFF}
        for r in newest.values():
            short = r['symbol'].split('::')[-1] if r['symbol'] else ''
            named = short and len(short) > 3 and re.search(r'\b' + re.escape(short) + r'\b', text)
            if r['address'] not in addrs and not named:
                continue
            if r['status'] not in ('parked', 'blocked') or r['address'] in acked:
                continue
            if a.since and r['date'] <= a.since:
                continue
            bad.append((p, r))
    for p, r in bad:
        print(f"{rel(p)}: {r['address']} {r['symbol']} is {r['status']} since {r['date']}: "
              f"read `attempts.py show {r['address']}`, then retry with --since or --ack")
    if bad:
        raise SystemExit(1)
    print(f'ok: {len(a.paths)} file(s), no parked or blocked attempt newer than the claim')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    r = sub.add_parser('record')
    r.add_argument('address')
    r.add_argument('--symbol')
    r.add_argument('--file')
    r.add_argument('--lib', help='the library, when --file does not give it')
    r.add_argument('--status', required=True, choices=STATUSES)
    r.add_argument('--score', type=float)
    r.add_argument('--forms', type=int)
    r.add_argument('--blocker', default='')
    r.add_argument('--evidence')
    r.add_argument('--stash')
    r.add_argument('--model')
    r.add_argument('--date')
    s = sub.add_parser('show')
    s.add_argument('key')
    li = sub.add_parser('list')
    li.add_argument('--status', choices=STATUSES)
    li.add_argument('--lib')
    li.add_argument('--blocker')
    c = sub.add_parser('check')
    c.add_argument('paths', nargs='+')
    c.add_argument('--since')
    c.add_argument('--ack', nargs='*')
    a = ap.parse_args()
    {'record': cmd_record, 'show': cmd_show, 'list': cmd_list, 'check': cmd_check}[a.cmd](a)


if __name__ == '__main__':
    main()
