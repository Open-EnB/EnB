#!/usr/bin/env python3
"""Pre-push check: what must not reach the public repository.

    python tools/prepush_check.py            # tree and history, prints a verdict
    python tools/prepush_check.py --tree     # current tree only (fast)
    python tools/prepush_check.py -v         # also list every informational finding

The project publishes source and notes only. This scans every file ever committed (the push
publishes all of history, not just the tip) for:

  big files      any blob over 5 MB
  binaries       exe dll lib obj vhdx pcapng jsonl ... (by extension) and files with NUL bytes that
                 are not on the small allow list
  captures       network and gameplay recordings (pcap, pcapng, cap, jsonl, .rec, ...)
  secrets        private keys, cloud/API tokens, `password = "..."` style assignments
  install paths  tracked file *names* inside the EA or Net-7 install trees (these are the
                 vendor's files and must stay out). Text that merely mentions an absolute path
                 (docs/binary.md names the install folder as provenance) is informational.

Exit status 1 if anything in the first four classes, or an install-tree file name, is found.
Standard library only. Needs `git` on PATH.
"""
import argparse
import os
import re
import subprocess
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
BIG = 5 * 1024 * 1024
BINARY_EXT = {'.exe', '.dll', '.lib', '.obj', '.vhdx', '.vhd', '.pcapng', '.pcap', '.cap', '.jsonl',
              '.pdb', '.ilk', '.exp', '.sys', '.ocx', '.res', '.mix', '.big', '.iso', '.zip', '.7z',
              '.rar', '.cab', '.bik', '.mp3', '.wav', '.tga', '.dds', '.bmp', '.png', '.jpg', '.gif',
              '.dmp', '.etl', '.snapshot', '.avhdx', '.vmdk', '.ova'}
CAPTURE_EXT = {'.pcap', '.pcapng', '.cap', '.jsonl', '.rec', '.etl', '.dmp'}
# generated pictures the project publishes on purpose
ALLOW_BINARY = {'.svg'}
SECRET_RES = [
    ('private key', re.compile(r'-----BEGIN (?:[A-Z]+ )?PRIVATE KEY-----')),
    ('AWS access key', re.compile(r'\bAKIA[0-9A-Z]{16}\b')),
    ('GitHub token', re.compile(r'\b(?:ghp|gho|ghu|ghs|ghr)_[A-Za-z0-9]{30,}\b|\bgithub_pat_[A-Za-z0-9_]{40,}\b')),
    ('Slack token', re.compile(r'\bxox[abprs]-[A-Za-z0-9-]{10,}\b')),
    ('API key (sk-)', re.compile(r'\bsk-[A-Za-z0-9]{32,}\b')),
    ('Google API key', re.compile(r'\bAIza[0-9A-Za-z_-]{35}\b')),
    ('Discord webhook', re.compile(r'discord(?:app)?\.com/api/webhooks/\d+/[\w-]{20,}')),
    ('assigned secret', re.compile(r'(?i)\b(?:password|passwd|pwd|secret|api[_-]?key|access[_-]?token|auth[_-]?token)\b'
                                   r'\s*[:=]\s*["\'][^"\'\s$%{<]{6,}["\']')),
]
# placeholders and test values that look like assignments but are not secrets
SECRET_IGNORE = re.compile(r'(?i)(changeme|example|placeholder|xxxx|\*\*\*|your[_ -]|<.*>|dummy|test)')
INSTALL_NAME_RES = [re.compile(p, re.I) for p in (
    r'(^|/)EA GAMES(/|$)', r'(^|/)Earth (&|and) Beyond(/|$)', r'(^|/)Net-?7(/|$)',
    r'(^|/)Program Files', r'(^|/)Live-Era-Data(/|$)', r'(^|/)a private tool(/|$)')]
INSTALL_TEXT_RES = [re.compile(p, re.I) for p in (
    r'[A-Za-z]:\\Program Files(?: \(x86\))?\\EA GAMES', r'[A-Za-z]:\\Net-?7\b', r'[A-Za-z]:\\EnBDevelopment\\Net7',
    r'/c/Program Files(?: \(x86\))?/EA GAMES')]


def git(*args, binary=False, inp=None):
    r = subprocess.run(['git', *args], cwd=ROOT, capture_output=True, input=inp, check=True)
    return r.stdout if binary else r.stdout.decode('utf-8', 'replace')


def blobs(history):
    """[(sha, path)] of every blob in the tip tree, or in all of history."""
    if not history:
        out = []
        for line in git('ls-tree', '-r', '-z', 'HEAD').split('\0'):
            if not line:
                continue
            meta, path = line.split('\t', 1)
            parts = meta.split()
            if parts[1] == 'blob':
                out.append((parts[2], path))
        return out
    seen, out = set(), []
    for line in git('rev-list', '--objects', '--all').splitlines():
        sha, _, path = line.partition(' ')
        if path and sha not in seen:
            seen.add(sha)
            out.append((sha, path))
    # keep only blobs
    types = git('cat-file', '--batch-check=%(objectname) %(objecttype) %(objectsize)',
                inp='\n'.join(s for s, _ in out).encode()).splitlines()
    info = {}
    for t in types:
        s, ty, sz = t.split()
        info[s] = (ty, int(sz))
    return [(s, p) for s, p in out if info.get(s, ('', 0))[0] == 'blob']


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--tree', action='store_true', help='current tree only, not history')
    ap.add_argument('-v', '--verbose', action='store_true')
    a = ap.parse_args()
    history = not a.tree
    bl = blobs(history)
    sizes = {}
    chk = git('cat-file', '--batch-check=%(objectname) %(objectsize)', inp='\n'.join(s for s, _ in bl).encode())
    for line in chk.splitlines():
        s, sz = line.split()
        sizes[s] = int(sz)

    problems = {'big files': [], 'binaries': [], 'captures': [], 'secrets': [], 'install-tree names': []}
    info = {'install path mentions': [], 'suspect assignments skipped as placeholders': []}
    paths_of = {}
    for s, p in bl:
        paths_of.setdefault(s, []).append(p)

    # 1. names and sizes (cheap)
    for s, p in bl:
        ext = os.path.splitext(p)[1].lower()
        if sizes[s] > BIG:
            problems['big files'].append(f'{p} ({sizes[s] / 1048576:.1f} MB) {s[:10]}')
        if ext in CAPTURE_EXT:
            problems['captures'].append(f'{p} {s[:10]}')
        elif ext in BINARY_EXT and ext not in ALLOW_BINARY:
            problems['binaries'].append(f'{p} {s[:10]}')
        norm = p.replace('\\', '/')
        if any(r.search(norm) for r in INSTALL_NAME_RES):
            problems['install-tree names'].append(f'{p} {s[:10]}')

    # 2. contents: one cat-file --batch pass over blobs under 2 MB that are not already flagged
    want = [s for s, p in bl if sizes[s] <= BIG]
    seen_nul = set()
    proc = subprocess.Popen(['git', 'cat-file', '--batch'], cwd=ROOT, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    import threading

    def feed():
        proc.stdin.write(('\n'.join(want) + '\n').encode())
        proc.stdin.close()
    threading.Thread(target=feed, daemon=True).start()
    out = proc.stdout
    while True:
        head = out.readline()
        if not head:
            break
        sha, _ty, size = head.decode().split()
        data = out.read(int(size) + 1)[:-1]
        name = paths_of[sha][0]
        ext = os.path.splitext(name)[1].lower()
        if b'\0' in data[:8192]:
            if ext not in BINARY_EXT and ext not in ALLOW_BINARY and sha not in seen_nul:
                seen_nul.add(sha)
                problems['binaries'].append(f'{name} (binary content) {sha[:10]}')
            continue
        text = data.decode('utf-8', 'replace')
        for label, rx in SECRET_RES:
            for m in rx.finditer(text):
                line_start = text.rfind('\n', 0, m.start()) + 1
                line_end = text.find('\n', m.end())
                line = text[line_start:line_end if line_end >= 0 else len(text)].strip()[:120]
                if label == 'assigned secret' and SECRET_IGNORE.search(m.group(0)):
                    info['suspect assignments skipped as placeholders'].append(f'{name}: {line}')
                    continue
                problems['secrets'].append(f'{label}: {name} {sha[:10]}: {line}')
                break
        for rx in INSTALL_TEXT_RES:
            m = rx.search(text)
            if m:
                info['install path mentions'].append(f'{name}: {m.group(0)}')
                break

    # 3. verdict
    scope = 'tree and full history' if history else 'current tree only'
    print(f'prepush_check: {len(bl)} blobs scanned ({scope})')
    bad = 0
    for k, v in problems.items():
        v = sorted(set(v))
        print(f'  {k:20s} {len(v)}')
        for line in v[:25]:
            print(f'      {line}')
        if len(v) > 25:
            print(f'      ... and {len(v) - 25} more')
        bad += len(v)
    for k, v in info.items():
        v = sorted(set(v))
        print(f'  [info] {k}: {len(v)} file(s)')
        if a.verbose:
            for line in v:
                print(f'      {line}')
    print('VERDICT: ' + ('FAIL, do not push' if bad else 'PASS'))
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
