#!/usr/bin/env python3
"""Housekeeping checks for the documentation and the tree.

    python tools/docs_check.py            # exit status 1 on any problem
    python tools/docs_check.py --sizes    # also print the builders' startup-set sizes

* every relative link in the Markdown files (README, CONTRIBUTING, CLAUDE.md, docs/, the tools'
  READMEs, .claude/skills) resolves to a file or directory in the checkout;
* no tracked text file mixes CRLF and bare LF line endings (the tree has two conventions,
  see .gitattributes, but never both in one file);
* the files .gitattributes lists as LF really are LF.

Moved out of the working notes (.work/audit/check.py) so CI can run it. Standard library only.
"""
import argparse
import fnmatch
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
    bad = []
    for f in files:
        try:
            b = read(f)
        except OSError:
            continue
        if not is_text(b):
            continue
        crlf = b.count(b'\r\n')
        lf = len(re.findall(rb'(?<!\r)\n', b))
        if crlf and lf:
            bad.append('%s: mixed line endings (%d CRLF, %d bare LF)' % (f, crlf, lf))
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


def check_gitattributes(files):
    """The files listed `text eol=lf` must be LF, and every tracked text file that is LF should be listed."""
    path = os.path.join(ROOT, '.gitattributes')
    if not os.path.exists(path):
        return ['.gitattributes is missing']
    listed = set()
    for line in open(path, encoding='utf-8'):
        line = line.strip()
        if line and not line.startswith('#') and 'eol=lf' in line:
            listed.add(line.split()[0])
    bad = []
    present = set(files)
    for f in sorted(listed):
        if f in present and is_text(read(f)) and b'\r\n' in read(f):
            bad.append('%s: listed as LF in .gitattributes but has CRLF' % f)
    for f in files:
        b = read(f)
        if is_text(b) and b.count(b'\n') and b'\r\n' not in b and f not in listed and \
                not any(fnmatch.fnmatch(f, pat) for pat in listed):
            bad.append('%s: LF file not listed in .gitattributes' % f)
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
    problems = check_endings(files) + check_links(files) + check_gitattributes(files)
    for p in problems:
        print(p)
    print('docs_check: %d files, %d problem(s)' % (len(files), len(problems)))
    if a.sizes:
        sizes()
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
