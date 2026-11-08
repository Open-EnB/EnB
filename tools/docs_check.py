#!/usr/bin/env python3
"""Housekeeping checks for the documentation and the tree.

    python tools/docs_check.py            # exit status 1 on any problem
    python tools/docs_check.py --sizes    # also print the builders' startup-set sizes

* every relative link in the Markdown files (README, CONTRIBUTING, CLAUDE.md, docs/, the tools'
  READMEs, .claude/skills) resolves to a file or directory in the checkout;
* every stored text file is LF (D147, .gitattributes `* text=auto eol=lf`), unless marked -text;
* docs/data/attempts.csv parses, every row has the header's fields, no field spans lines, and no two
  rows share (date, address). The file merges by line union (.gitattributes `merge=union`), so a row
  changed on both sides of a merge comes out twice; this is where that is caught. attempts.py record
  refuses a (date, address) already logged, so a duplicate is always a merge's. The pairs below were
  logged twice the same day before that rule (2026-11-07) and are allowed.

Moved out of the working notes (.work/audit/check.py) so CI can run it. Standard library only.
"""
import argparse
import csv
import io
import os
import re
import subprocess
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
LINK = re.compile(r'\]\(([^)#\s]+)(#[^)]*)?\)')
FENCE = re.compile(r'^(```|~~~)')


def tracked():
    out = subprocess.run(['git', 'ls-files', '-z'], cwd=ROOT, capture_output=True, check=True).stdout
    return [p for p in out.decode('utf-8').split('\0') if p]


def read(path):
    with open(os.path.join(ROOT, path), 'rb') as f:
        return f.read()


def is_text(data):
    return b'\0' not in data[:8192]


def check_endings(files):
    """D147: every stored text file is LF. `git ls-files --eol` reports the index (what a clone gets),
    so a CRLF or mixed blob is a problem unless .gitattributes marks the file -text."""
    out = subprocess.run(['git', 'ls-files', '--eol', '-z'], cwd=ROOT, capture_output=True, check=True).stdout
    bad = []
    for rec in out.decode('utf-8', 'replace').split('\0'):
        if '\t' not in rec:
            continue
        meta, path = rec.split('\t', 1)
        fields = meta.split()
        index, attr = fields[0], (fields[2] if len(fields) > 2 else '')
        if index in ('i/crlf', 'i/mixed') and '-text' not in attr:
            bad.append('%s: stored with CR line endings (%s); the tree is LF (D147)' % (path, index[2:]))
    return bad


def check_links(files):
    bad = []
    for f in files:
        if not f.lower().endswith('.md'):
            continue
        text = read(f).decode('utf-8', 'replace')
        infence = False
        for n, line in enumerate(text.splitlines(), 1):
            if FENCE.match(line.strip()):
                infence = not infence
                continue
            if infence:
                continue
            line = re.sub(r'`[^`]*`', '', line)           # not inside inline code
            for m in LINK.finditer(line):
                target = m.group(1)
                if re.match(r'[a-z][a-z0-9+.-]*:', target, re.I):        # http:, https:, mailto:
                    continue
                p = os.path.normpath(os.path.join(ROOT, os.path.dirname(f), target.replace('%20', ' ')))
                if not os.path.exists(p):
                    bad.append('%s:%d: broken link %s' % (f, n, target))
    return bad


ATTEMPTS = 'docs/data/attempts.csv'
# (date, address) logged twice before attempts.py refused it (2026-11-07): distinct attempts, not merge copies
ATTEMPTS_TWICE = {
    ('2026-10-25 20:12', '0x9182C0'), ('2026-10-28', '0x716140'), ('2026-10-28', '0x716220'),
    ('2026-10-28', '0x716760'), ('2026-10-28', '0x7168A0'), ('2026-10-28', '0x717CC0'),
    ('2026-10-31', '0x6120E0'), ('2026-10-31', '0x612430'), ('2026-10-31 20:10', '0x8099B0'),
    ('2026-11-06', '0x5D8E90'), ('2026-11-06', '0x5D9410'), ('2026-11-06', '0x5D9610'),
    ('2026-11-06', '0x5D9D10'), ('2026-11-06', '0x5DA600'), ('2026-11-06', '0x5DAB50'),
    ('2026-11-06', '0x5D9100'), ('2026-11-06', '0x5DA0D0'),
}


def check_attempts(files):
    """attempts.csv merges by union (item 1 of docs/pipeline-plan.md): it must still parse, keep one
    record per line, and hold no (date, address) twice outside ATTEMPTS_TWICE."""
    if ATTEMPTS not in files:
        return []
    text = read(ATTEMPTS).decode('utf-8')
    if '<<<<<<<' in text or '>>>>>>>' in text:
        return ['%s: conflict markers' % ATTEMPTS]
    rows = list(csv.reader(io.StringIO(text, newline='')))
    if not rows:
        return ['%s: empty' % ATTEMPTS]
    head, bad, seen = rows[0], [], {}
    for n, r in enumerate(rows[1:], 2):
        if len(r) != len(head):
            bad.append('%s: record %d has %d fields, the header %d' % (ATTEMPTS, n, len(r), len(head)))
            continue
        if any(c in f for f in r for c in '\r\n'):
            bad.append('%s: record %d has a field spanning lines (union merges need one record per line)' % (ATTEMPTS, n))
        key = (r[0], r[1])
        if key in seen and key not in ATTEMPTS_TWICE:
            bad.append('%s: records %d and %d share date %s and address %s (a row changed on both sides of a '
                       'merge? keep one)' % (ATTEMPTS, seen[key], n, r[0], r[1]))
        seen.setdefault(key, n)
    return bad


def sizes():
    """Bytes a builder reads at start-up, by kind (docs/builders.md section 1)."""
    def section(path, head, level):
        t = read(path).decode('utf-8')
        i = t.index(head)
        m = re.compile(r'^' + '#' * level + r' ', re.M).search(t, i + len(head))
        return len(t[i:m.start() if m else len(t)].encode('utf-8'))
    wf = 'docs/workflow.md'
    t = read(wf).decode('utf-8')
    heads = re.findall(r'^### 3\.\d.*$', t, re.M)
    sz = {h.split()[1]: section(wf, h, 3) for h in heads}
    common = (len(read('CLAUDE.md')) + len(read('docs/builders.md')) + section(wf, '## 1.', 2) +
              section(wf, '## 2.', 2) + sz['3.0'] + 3000)
    app = common + sum(sz[k] for k in ('3.1', '3.2', '3.3', '3.4', '3.6')) + sz['3.9'] // 2 + \
        section('docs/conventions.md', '## 1.', 2) + 2500
    lib = common + sum(sz[k] for k in ('3.1', '3.2', '3.4', '3.5', '3.6')) + sz['3.9'] // 2 + 3000
    har = common + sum(sz[k] for k in ('3.5', '3.7', '3.8')) + len(read('tools/harness/README.md'))
    print('startup bytes: app %d, library %d, harness %d' % (app, lib, har))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--sizes', action='store_true', help='print the builders\' startup-set sizes')
    a = ap.parse_args()
    files = tracked()
    problems = check_endings(files) + check_links(files) + check_attempts(files)
    for p in problems:
        print(p)
    print('docs_check: %d files, %d problem(s)' % (len(files), len(problems)))
    if a.sizes:
        sizes()
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
