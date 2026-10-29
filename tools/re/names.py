"""Name certainty review list (decision D56).

Scans the committed E&B code (app/, stubs/, programs/, tests/) for

* tags `// NAME n: evidence` (n = 1..5, see docs/conventions.md "Name certainty"); an optional
  subject in brackets names what the tag is about when the code doesn't show it:
  `// NAME 2 [encode.h]: encode.cpp is the client's; header name assumed`;
* untagged neutral names (`Unknown...`, `m_Unknown...`, D10), which count as score 1;

and writes docs/data/names-review.md: every name scoring 3 or below, lowest first, with its
evidence. Names tagged 4 or 5 are settled and only counted.

    python tools/re/names.py            # rewrite docs/data/names-review.md
    python tools/re/names.py --print    # print the list instead
    python tools/re/names.py --tree DIR # scan another worktree (output still goes to this tree)

It scans tracked and untracked-not-ignored files (git ls-files -co --exclude-standard), so a new
file needs no `git add` first, and it also regenerates docs/data/not-exact.md (notexact.py's
generator), so one command refreshes both. `--print` prints the names list only.
"""
import argparse
import collections
import os
import re
import subprocess

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
OUT = os.path.join(ROOT, 'docs', 'data', 'names-review.md')
DIRS = ('app/', 'stubs/', 'programs/', 'tests/')
EXTS = ('.cpp', '.h', '.inl', '.c')
THRESHOLD = 3       # scores at or below this are listed for review

TAG = re.compile(r'//\s*NAME\s+([1-5])\s*(?:\[([^\]]+)\])?\s*:\s*(.*)$')
UNKNOWN = re.compile(r'\b\w*Unknown\w*\b')     # m_Unknown7C, Unknown_18, DeltaUnknownGlobal
KEYWORDS = {'const', 'virtual', 'static', 'inline', 'unsigned', 'signed', 'int', 'char', 'void', 'bool',
            'float', 'double', 'long', 'short', 'struct', 'class', 'enum', 'union', 'typedef', 'return',
            'public', 'private', 'protected', 'operator', 'template', 'typename', 'extern', 'explicit'}


def tracked_files(tree=None):
    """Tracked plus untracked-not-ignored source files under DIRS in `tree` (default: this tree)."""
    out = subprocess.run(['git', 'ls-files', '-co', '--exclude-standard'] + list(DIRS), cwd=tree or ROOT,
                         capture_output=True, text=True, check=True).stdout
    # a tracked file deleted in the working tree (not yet committed) is left out
    return [p for p in out.splitlines() if p.lower().endswith(EXTS)
            and os.path.exists(os.path.join(tree or ROOT, p))]


def subject_of(code, path):
    """The name a declaration line introduces: a class name, or the identifier before ( ; = [ {."""
    code = code.strip()
    if not code:
        return None
    if code.startswith('#'):
        return os.path.basename(path)       # a tag on the include guard is about the file itself
    m = re.match(r'(?:template\s*<[^>]*>\s*)?(?:class|struct|enum|union)\s+([A-Za-z_]\w*)', code)
    if m:
        return m.group(1)
    m = re.search(r'([A-Za-z_][\w:]*(?:::~?[A-Za-z_]\w*)?)\s*(?:\(|;|=|\[|\{|:(?!:))', code)
    if m and m.group(1) not in KEYWORDS:
        return m.group(1)
    words = [w for w in re.findall(r'[A-Za-z_][\w:]*', code) if w not in KEYWORDS]
    return words[-1] if words else None


def scan(path, tree=None):
    text = open(os.path.join(tree or ROOT, path), 'rb').read().decode('utf-8', errors='replace')
    lines = text.splitlines()
    tags, tagged_names = [], set()
    for i, line in enumerate(lines):
        m = TAG.search(line)
        if not m:
            continue
        score, subject, evidence = int(m.group(1)), m.group(2), m.group(3).strip()
        if not subject:
            code = line[:m.start()]
            if not code.strip():            # a tag on its own line describes the next code line
                for nxt in lines[i + 1:i + 6]:
                    s = nxt.strip()
                    if s and not s.startswith('//') and not s.startswith('/*') and not s.startswith('*'):
                        code = nxt
                        break
            subject = subject_of(code, path) or '?'
        tags.append((score, subject, path, i + 1, evidence))
        tagged_names.add(subject.split('::')[-1])
    unknowns = {}
    for i, line in enumerate(lines):
        code = line.split('//')[0]
        for name in UNKNOWN.findall(code):
            if name not in tagged_names and name not in unknowns:
                unknowns[name] = i + 1
    for name, ln in unknowns.items():
        tags.append((1, name, path, ln, 'untagged neutral name (D10)'))
    return tags


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--print', action='store_true', help='print instead of writing the file')
    ap.add_argument('--tree', help='scan this worktree instead of this one')
    args = ap.parse_args()
    tree = os.path.abspath(args.tree) if args.tree else None

    entries = []
    for path in tracked_files(tree):
        entries += scan(path, tree)
    # scan() lists an untagged Unknown name once per file (the same placeholder in two classes is
    # two names to settle).
    rows = sorted(entries, key=lambda e: (e[0], e[2], e[3]))
    counts = collections.Counter(e[0] for e in rows)
    review = [e for e in rows if e[0] <= THRESHOLD]

    out = ['# Names to review (generated by tools/re/names.py)', '',
           'Names chosen without the client\'s own evidence, scored as decision D56 says',
           '([conventions.md](../conventions.md), "Name certainty"). Scores 4-5 are settled; 3 and below',
           'are listed here until better evidence raises their tag. Regenerate with',
           '`python tools/re/names.py`; edit the tags in the code, not this file.', '',
           '| Score | Meaning | Names |', '|---|---|---|']
    meaning = {5: 'the client names it', 4: 'EA source, or a structure-confirmed Net-7 / C# name',
               3: 'by convention from a client-named file or sibling', 2: 'describes what the code does',
               1: 'neutral placeholder'}
    for s in (5, 4, 3, 2, 1):
        out.append('| %d | %s | %d |' % (s, meaning[s], counts.get(s, 0)))
    out += ['', '## For review (score %d and below)' % THRESHOLD, '',
            '| Score | Name | Where | Evidence |', '|---|---|---|---|']
    for score, name, path, line, evidence in review:
        out.append('| %d | `%s` | `%s:%d` | %s |' % (score, name, path, line, evidence.replace('|', '\\|')))
    if not review:
        out.append('| | (none) | | |')
    text = '\n'.join(out) + '\n'
    if args.print:
        print(text)
    else:
        open(OUT, 'w', encoding='utf-8', newline='\n').write(text)
        print('%s: %d names, %d for review' % (os.path.relpath(OUT, ROOT), len(rows), len(review)))
        import notexact
        notexact.write(tree)


if __name__ == '__main__':
    main()
