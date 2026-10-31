#!/usr/bin/env python3
"""The progress card: how much of client.exe is rebuilt, and how readable the names are.

    python tools/progress.py                      # measure the working tree, write the three SVGs
    python tools/progress.py --history --today    # also backfill / extend docs/data/progress-history.csv
    python tools/progress.py --print              # numbers only, no files written
    python tools/progress.py --history --rebuild  # recompute every day of the history

Reads committed data only (never client.exe, the VMs or the toolchain): docs/data/imports.json,
app-functions.json, symbols.json, bytematch-*.json, gaps-*.json, not-exact.md, docs/binary.md,
stubs/*.cpp, and the C++ sources for the name census (tools/re/declcensus.py).

BYTES REBUILT. The denominator is the whole `.text` of client.exe (docs/binary.md section 2). Every
byte falls in exactly one of six states, in this order of precedence (a byte claimed by two goes
to the first):

  linker      the incremental-link thunk table, the int3 padding, and the compiler's exception
              handling stubs (docs/library-matching.md section 3.7, docs/coverage.md): output of
              the toolchain, not source. The EH stubs come back when the code is rebuilt and linked.
  exact       a function proven byte for byte: a (file, address) pair in imports.json (D80 ties
              included), or one of D54's `multiple` exact functions (the identical copies)
  NOT EXACT   functions written functionally (docs/data/not-exact.md, D64); an entry's address comes
              from symbols.json by name, else from its neighbours in the same source file
              (functions of one object are contiguous and in source order); a whole-file entry takes
              the functions app-functions.json attributes to that file; the few entries that
              cannot be placed are charged the average size of those that could (reported)
  stubbed     functions replaced by a stub in stubs/ (a STUB_CALLED name or comment gives the
              client address) and not otherwise exact
  third-party the C and C++ runtime, COM support and D3DX (library-matching.md section 3.7): the
              proven functions of docs/data/bytematch-{libcmt,libcpmt,comsupp,d3dx8}.json and the
              unproven bytes between a library's first and last proven function (never ours)
  unattributed everything else

Function sizes come from the verified function starts (D27): a function runs to the next known start
(app-functions.json for the app region, the named and matched functions for the libraries), or to
its measured length rounded up to the 16-byte alignment when that is shorter. A function without a
length whose next start is unknown is charged at most 2 KB. The sizes always come from today's tables,
so the history differs from day to day only by what each day's commit had proven.

"Bytes rebuilt" (the card's first bar and the README line) is exact plus third-party plus linker: what is
proven from our source plus what comes back unchanged by linking the same libraries with the same toolchain.
The bars show the states in DISPLAY order (those three in greens first). "Written in C++" is exact plus NOT EXACT.

READABLE NAMES. tools/re/declcensus.py counts every declaration (types, functions, members, globals,
parameters, locals) in app/, stubs/, programs/ and tests/ and scores it on D56's scale by its
`// NAME n:` tag; an untagged name that is not a placeholder counts as 5 (reported separately as
"untagged"), an untagged `Unknown...` name as 1. The imported EA library code is counted apart.

HISTORY. docs/data/progress-history.csv has one row per day, measured on that day's last commit
(`git show REF:path`). The counting rule changed on 2026-10-25 (D80: identical functions that differ
only in a relocation target are now told apart), so earlier days under-count.

The look of the SVGs follows Open-BFME's progress card (a card, a daily history, a code map), written
from scratch for this project's ledger. Standard library only.
"""
import argparse
import bisect
import csv
import datetime
import html
import json
import os
import re
import subprocess
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
sys.path.insert(0, os.path.join(ROOT, 'tools', 're'))
import declcensus  # noqa: E402

DATA = 'docs/data'
HISTORY_CSV = os.path.join(ROOT, DATA, 'progress-history.csv')
SVG_DIR = os.path.join(ROOT, 'docs')
LIB_START = 0x8C1B50                  # E&B's own objects end here (docs/binary.md section 2)
TEXT_VA, TEXT_SIZE = 0x401000, 0x6E58DD      # fallback; read from docs/binary.md
THUNK_END = 0x419650
PAD2 = (0xA420AC, 0xAA1C00)           # incremental-link padding (library-matching.md section 3.7)
EH = (0xAA1C00, 0xAE2830)             # compiler-generated exception-handling stubs
FIRST_COMMIT_DAY = '2026-10-22'
RULE_CHANGE = '2026-10-25'            # D80
THIRD_PARTY = ('libcmt', 'libcpmt', 'comsupp', 'd3dx8')
LIBS = ('app', 'binkmovie', 'wlib', 'wnet', 'ww3d2', 'wwaudio', 'wwdebug', 'wwlib', 'wwmath', 'wwsaveload',
        'wwshade')
# the executable in address order (docs/binary.md section 2; the runs of proven functions in
# docs/data/bytematch-*.json and imports.json draw the boundaries between libraries)
REGIONS = [
    ('app (E&B)', 0x401000, LIB_START),
    ('wwmath', LIB_START, 0x8E3690),
    ('d3dx8', 0x8E3690, 0x908D00),
    ('ww3d2', 0x908D00, 0x9C7E80),
    ('wwlib', 0x9C7E80, 0x9E9F00),
    ('wwdebug', 0x9E9F00, 0x9EE980),
    ('wwaudio', 0x9EE980, 0xA03C00),
    ('wwsaveload', 0xA03C00, 0xA07D40),
    ('wwshade', 0xA07D40, 0xA159C0),
    ('wwmath', 0xA159C0, 0xA16200),
    ('binkmovie', 0xA16200, 0xA18300),
    ('libcpmt', 0xA18300, 0xA25980),
    ('libcmt', 0xA25980, PAD2[0]),
    ('padding', PAD2[0], PAD2[1]),
    ('EH stubs', EH[0], EH[1]),
    ('other', EH[1], None),
]
STATES = ('exact', 'notexact', 'stub', 'third', 'linker', 'unattr')      # the history CSV's column order
DISPLAY = ('exact', 'third', 'linker', 'notexact', 'stub', 'unattr')     # the bars' and legends' order
REBUILT = ('exact', 'third', 'linker')                                   # 'Bytes rebuilt'
SHORT = {'exact': 'byte-exact', 'notexact': 'NOT EXACT', 'stub': 'stubbed', 'third': 'CRT / third-party',
         'linker': 'linker output', 'unattr': 'unattributed'}
STATE_NAMES = {'exact': 'byte-exact', 'notexact': 'written, NOT EXACT', 'stub': 'stubbed',
               'third': 'CRT and third-party', 'linker': 'linker padding, thunks, EH stubs',
               'unattr': 'unattributed'}
PRIO = {'unattr': 0, 'third': 1, 'stub': 2, 'notexact': 3, 'exact': 4, 'linker': 5}
CODE = {s: i for i, s in enumerate(sorted(PRIO, key=PRIO.get))}      # byte value in the painted map
STATE_OF_CODE = {v: k for k, v in CODE.items()}


# ------------------------------------------------------------------------------------ sources

def git(*args, check=True):
    r = subprocess.run(['git', *args], cwd=ROOT, capture_output=True)
    if check and r.returncode:
        raise RuntimeError('git %s: %s' % (' '.join(args), r.stderr.decode('utf-8', 'replace')[:200]))
    return r.stdout


class Src:
    """The files of the working tree (ref None) or of one commit."""

    def __init__(self, ref=None):
        self.ref = ref
        self._ls = None

    def read(self, path):
        if self.ref is None:
            try:
                with open(os.path.join(ROOT, path), 'rb') as f:
                    return f.read()
            except OSError:
                return None
        r = subprocess.run(['git', 'show', '%s:%s' % (self.ref, path)], cwd=ROOT, capture_output=True)
        return r.stdout if r.returncode == 0 else None

    def json(self, path, default=None):
        b = self.read(path)
        if b is None:
            return default
        try:
            return json.loads(b.decode('utf-8'))
        except ValueError:
            return default

    def ls(self, prefix):
        if self._ls is None:
            if self.ref is None:
                out = git('ls-files').decode('utf-8', 'replace')
            else:
                out = git('ls-tree', '-r', '--name-only', self.ref).decode('utf-8', 'replace')
            self._ls = out.splitlines()
        return [p for p in self._ls if p.startswith(prefix)]


def hexint(s):
    return int(s, 16) if isinstance(s, str) else int(s)


# ------------------------------------------------------------------------------------ tables

class Tables:
    """Today's view of client.exe: where every known function starts and how long it is."""

    def __init__(self, src):
        b = src.read('docs/binary.md')
        self.text_va, self.text_size = TEXT_VA, TEXT_SIZE
        if b:
            m = re.search(r'`\.text`\s*\|\s*(0x[0-9A-Fa-f]+)\s*\|\s*(0x[0-9A-Fa-f]+)', b.decode('utf-8', 'replace'))
            if m:
                self.text_va, self.text_size = int(m.group(1), 16), int(m.group(2), 16)
        self.text_end = self.text_va + self.text_size
        starts, self.lengths, self.names = set(), {}, {}
        self.app_funcs = {}
        app = src.json(DATA + '/app-functions.json', {}) or {}
        for f in app.get('functions', []):
            a = hexint(f['address'])
            starts.add(a)
            self.app_funcs[a] = f
        for s in src.json(DATA + '/symbols.json', []) or []:
            if s.get('kind') != 'function':
                continue
            a = hexint(s['address'])
            starts.add(a)
            if s.get('length'):
                self.lengths[a] = s['length']
            base = s['name'].split('(')[0]
            self.names.setdefault(base, []).append((a, s.get('source')))
        for n in THIRD_PARTY:
            for a, v in (src.json('%s/bytematch-%s.json' % (DATA, n), {}) or {}).items():
                e = next(iter(v.values()))
                starts.add(hexint(a))
                self.lengths.setdefault(hexint(a), e.get('length') or 0)
        for p in src.ls(DATA + '/gaps-'):
            if p.endswith('.json'):
                for e in src.json(p, []) or []:
                    if isinstance(e, dict) and e.get('address') and e.get('status') != 'none':
                        starts.add(hexint(e['address']))
        for e in src.json(DATA + '/funcmatch.json', []) or []:
            if isinstance(e, dict) and e.get('client'):
                starts.add(hexint(e['client']))
        imp = src.json(DATA + '/imports.json', {}) or {}
        for lib in imp.values():
            for v in lib.get('files', {}).values():
                for a in v.get('in_tree', []) + v.get('ambiguous', []):
                    starts.add(hexint(a))
        self.starts = sorted(a for a in starts if self.text_va <= a < self.text_end)
        first_app = min((a for a in self.app_funcs), default=0x429630)
        self.padding1 = (THUNK_END, first_app)
        self.thunks = (self.text_va, THUNK_END)
        self.estimated_sizes = 0

    def size(self, addr, length=None):
        i = bisect.bisect_right(self.starts, addr)
        nxt = self.starts[i] if i < len(self.starts) else self.text_end
        gap = nxt - addr
        n = length or self.lengths.get(addr)
        if n:
            return max(1, min(gap, ((addr + n + 15) & ~15) - addr))
        self.estimated_sizes += 1
        return max(1, min(gap, 0x800))


# ------------------------------------------------------------------------------------ sets

NOT_EXACT_ROW = re.compile(r'^\| `([^`]+?):(\d+)` \| `([^`]*)` \|')
STUB_CALL = re.compile(r'STUB_CALLED\(\s*"([^"]*)"')
ADDR = re.compile(r'0x([0-9A-Fa-f]{6})\b')


class Sets:
    """What one commit (or the working tree) says is proven, written or stubbed."""

    def __init__(self, src):
        self.src = src
        imp = src.json(DATA + '/imports.json', {}) or {}
        self.exact, self.amb, self.pairs = set(), set(), 0
        self.fn_by_lib = {}
        self.file_exact = {}                  # source path -> sorted exact addresses
        for lib, e in imp.items():
            if not isinstance(e, dict):
                continue
            libset = set()
            for fname, v in (e.get('files') or {}).items():
                if not isinstance(v, dict):
                    continue
                addrs = [hexint(a) for a in v.get('in_tree', [])]
                self.pairs += len(addrs)
                libset |= set(addrs)
                self.exact |= set(addrs)
                self.amb |= {hexint(a) for a in v.get('ambiguous', [])}
                if lib == 'app':
                    self.file_exact['app/' + (v.get('name') or fname + '.cpp')] = sorted(addrs)
            self.fn_by_lib[lib] = len(libset)
        self.third = {}
        for n in THIRD_PARTY:
            d = src.json('%s/bytematch-%s.json' % (DATA, n), {}) or {}
            self.third[n] = {hexint(a): (next(iter(v.values())).get('length') or 0) for a, v in d.items()}
        self.notexact = []
        t = src.read(DATA + '/not-exact.md')
        if t:
            for line in t.decode('utf-8', 'replace').splitlines():
                m = NOT_EXACT_ROW.match(line)
                if m:
                    self.notexact.append((m.group(1), int(m.group(2)), m.group(3)))
        self.stub_addrs = self._stubs()

    def _stubs(self):
        out = set()
        for p in self.src.ls('stubs/'):
            if not p.endswith('.cpp'):
                continue
            text = (self.src.read(p) or b'').decode('utf-8', 'replace')
            lines = text.split('\n')
            for i, line in enumerate(lines):
                m = STUB_CALL.search(line)
                if not m:
                    continue
                a = ADDR.findall(m.group(1))
                if not a:           # the comment block above the function
                    j = i
                    while j > 0 and not lines[j].startswith(('void ', 'bool ', 'int ', 'char ', 'const ', 'unsigned ')) \
                            and i - j < 12:
                        j -= 1
                    k = j - 1
                    while k >= 0 and lines[k].lstrip().startswith('//'):
                        a += ADDR.findall(lines[k])
                        k -= 1
                out |= {int(x, 16) for x in a[:1]}
        return out


# ------------------------------------------------------------------------------------ accounting

def locate_notexact(tab, sets):
    """Addresses of the NOT EXACT functions: ({address}, unresolved entry count)."""
    found, unresolved = set(), 0
    by_file = {}
    for path, line, func in sets.notexact:
        by_file.setdefault(path, []).append((line, func))
    appfile = {}
    for a, f in tab.app_funcs.items():
        if f.get('file'):
            appfile.setdefault(f['file'].replace('\\', '/'), []).append(a)
    for path, entries in by_file.items():
        entries.sort()
        anchors = None
        for line, func in entries:
            if func == '(whole file)':
                got = [a for a in appfile.get(path, []) if a not in sets.exact]
                if got:
                    found |= set(got)
                else:
                    unresolved += 1
                continue
            cand = [a for a, src in tab.names.get(func, []) if src != 'exact' and a not in sets.exact]
            if cand:
                found |= set(cand)
                continue
            if anchors is None:
                anchors = file_anchors(tab, sets, path)
            a = place_by_anchor(tab, sets, anchors, line, found)
            if a:
                found.add(a)
            else:
                unresolved += 1
    return found, unresolved


def file_anchors(tab, sets, path):
    """[(source line, address)] of the file's exact functions whose definition is found in the source."""
    text = (sets.src.read(path) or b'').decode('utf-8', 'replace').split('\n')
    pos = {}
    for i, line in enumerate(text, 1):
        for m in re.finditer(r'([A-Za-z_~][\w]*::~?[A-Za-z_]\w*)\s*\(', line):
            pos.setdefault(m.group(1), i)
    by_addr = {}
    for base, lst in tab.names.items():
        for a, src in lst:
            by_addr.setdefault(a, base)
    out = []
    for a in sets.file_exact.get(path, []):
        nm = by_addr.get(a)
        if nm and nm in pos:
            out.append((pos[nm], a))
    return sorted(out)


def place_by_anchor(tab, sets, anchors, line, taken):
    if not anchors:
        return None
    before = [a for ln, a in anchors if ln < line]
    after = [a for ln, a in anchors if ln > line]
    lo = before[-1] if before else None
    hi = after[0] if after else None
    if lo is None and hi is None:
        return None
    app = sorted(tab.app_funcs)
    i = bisect.bisect_right(app, lo) if lo is not None else None
    if i is not None:
        for a in app[i:]:
            if hi is not None and a >= hi:
                return None
            if a not in sets.exact and a not in taken:
                return a
        return None
    j = bisect.bisect_left(app, hi)
    for a in reversed(app[:j]):
        if a not in sets.exact and a not in taken:
            return a
    return None


def paint(buf, tab, start, end, code):
    s = max(start, tab.text_va) - tab.text_va
    e = min(end, tab.text_end) - tab.text_va
    if e > s:
        buf[s:e] = bytes([code]) * (e - s)


class Result:
    pass


def dense_span(addrs, gap=0x2000):
    """(first, last) address of the biggest cluster of a library's proven functions (no hole over `gap`):
    a few small functions can match by chance far from the library's run, and must not stretch it."""
    best, cur = [], []
    for a in addrs:
        if cur and a - cur[-1] > gap:
            best = max(best, cur, key=len)
            cur = []
        cur.append(a)
    best = max(best, cur, key=len)
    return (best[0], best[-1]) if len(best) >= 8 else None


def measure(src, tab, label=''):
    """Paint the .text of client.exe by state and total it."""
    sets = Sets(src)
    tab.estimated_sizes = 0
    buf = bytearray(tab.text_size)           # 0 = unattributed
    # third-party, lowest priority first: the spans of the four runtime libraries, then functions
    for n, funcs in sets.third.items():
        if not funcs:
            continue
        span = dense_span(sorted(a for a in funcs if a >= LIB_START)) if n != 'comsupp' else None
        if span:
            lo, hi = span
            paint(buf, tab, lo, hi + tab.size(hi, funcs[hi]), CODE['third'])
        for a, ln in funcs.items():
            paint(buf, tab, a, a + tab.size(a, ln), CODE['third'])
    for a in sets.stub_addrs:
        paint(buf, tab, a, a + tab.size(a), CODE['stub'])
    located, unresolved = locate_notexact(tab, sets)
    ne_bytes = 0
    for a in located:
        paint(buf, tab, a, a + tab.size(a), CODE['notexact'])
        ne_bytes += tab.size(a)
    resolved = max(1, len(located))
    avg = ne_bytes // resolved if located else 0
    for a in sets.exact | sets.amb:
        paint(buf, tab, a, a + tab.size(a), CODE['exact'])
    for lo, hi in (tab.thunks, tab.padding1, PAD2, EH):
        paint(buf, tab, lo, hi, CODE['linker'])
    counts = {s: buf.count(CODE[s]) for s in STATES}
    # entries that could not be placed are charged an average function, out of the unattributed bytes
    extra = min(unresolved * avg, counts['unattr'])
    counts['notexact'] += extra
    counts['unattr'] -= extra
    r = Result()
    r.buf, r.counts, r.sets, r.tab = buf, counts, sets, tab
    r.unresolved, r.avg_ne = unresolved, avg
    r.located = len(located)
    r.exact_functions = len(sets.exact | sets.amb)
    r.total = tab.text_size
    return r


def region_table(r):
    """Per library region: bytes by state."""
    tab = r.tab
    rows = []
    for name, lo, hi in REGIONS:
        hi = tab.text_end if hi is None else hi
        s, e = lo - tab.text_va, min(hi, tab.text_end) - tab.text_va
        seg = r.buf[s:e]
        rows.append({'name': name, 'lo': lo, 'hi': hi, 'size': e - s,
                     **{st: seg.count(CODE[st]) for st in STATES}})
    return rows


# ------------------------------------------------------------------------------------ history

HIST_FIELDS = (['date', 'commit', 'text_bytes'] + ['bytes_' + s for s in STATES] +
               ['exact_functions', 'pairs'] + ['fn_' + lib for lib in LIBS] + ['notexact_functions'])


def row_of(date, commit, r):
    row = {'date': date, 'commit': commit, 'text_bytes': r.total, 'exact_functions': r.exact_functions,
           'pairs': r.sets.pairs, 'notexact_functions': r.located + r.unresolved}
    for s in STATES:
        row['bytes_' + s] = r.counts[s]
    for lib in LIBS:
        row['fn_' + lib] = r.sets.fn_by_lib.get(lib, 0)
    return row


def read_history():
    try:
        with open(HISTORY_CSV, newline='', encoding='utf-8') as f:
            return [dict(r) for r in csv.DictReader(f)]
    except OSError:
        return []


def write_history(rows):
    rows = sorted(rows, key=lambda r: r['date'])
    with open(HISTORY_CSV, 'w', newline='', encoding='utf-8') as f:
        w = csv.DictWriter(f, HIST_FIELDS, lineterminator='\n')
        w.writeheader()
        for r in rows:
            w.writerow({k: r.get(k, 0) for k in HIST_FIELDS})


def commits_by_day():
    """{date: newest commit of that day} over the history of HEAD, by author date."""
    out = git('log', '--format=%H %ad', '--date=short').decode().splitlines()
    days = {}
    for line in out:
        h, d = line.split()
        days.setdefault(d, h)
    return days


def backfill(tab, rebuild, today):
    have = {r['date']: r for r in read_history()}
    if rebuild:
        have = {}
    for d, h in sorted(commits_by_day().items()):
        if d in have or d == today:
            continue
        try:
            r = measure(Src(h), tab)
        except Exception as e:              # an old commit with data in a format we do not read
            print('history %s (%s): skipped: %s' % (d, h[:8], e), file=sys.stderr)
            continue
        have[d] = row_of(d, h[:8], r)
        print('history %s %s: exact %.2f%%' % (d, h[:8], 100.0 * r.counts['exact'] / r.total))
    return have


# ------------------------------------------------------------------------------------ names

def names_summary():
    return declcensus.summarize(declcensus.read_worktree)


# ------------------------------------------------------------------------------------ SVG

FONT = "-apple-system,BlinkMacSystemFont,'Segoe UI','Noto Sans',Helvetica,Arial,sans-serif"
COLORS = {'exact': '#3fb950', 'notexact': '#d29922', 'stub': '#a371f7', 'third': '#238636',
          'linker': '#1b4d2b', 'unattr': '#30363d'}
COLORS_LIGHT = {'exact': '#1a7f37', 'third': '#4ac26b', 'linker': '#aceebb', 'unattr': '#d0d7de'}
NAME_COLORS = {5: '#2ea043', 4: '#56d364', 3: '#d29922', 2: '#f0883e', 1: '#f85149'}
NAME_LABELS = {5: '5 the client names it', 4: '4 EA / structure-confirmed', 3: '3 by convention',
               2: '2 descriptive', 1: '1 placeholder'}
STYLE = '''  <style>
    .card { fill: #0d1117; stroke: #30363d; } .strong { fill: #f0f6fc; } .muted { fill: #8b949e; }
    .grid { stroke: #30363d; } .up { fill: #3fb950; } .down { fill: #f85149; }
    .s-exact { fill: #3fb950; } .s-notexact { fill: #d29922; } .s-stub { fill: #a371f7; }
    .s-third { fill: #238636; } .s-linker { fill: #1b4d2b; } .s-unattr { fill: #30363d; }
    .l-exact { stroke: #3fb950; } .l-written { stroke: #58a6ff; }
    @media (prefers-color-scheme: light) {
      .card { fill: #ffffff; stroke: #d0d7de; } .strong { fill: #1f2328; } .muted { fill: #59636e; }
      .grid { stroke: #d0d7de; } .up { fill: #1a7f37; } .down { fill: #cf222e; }
      .s-exact { fill: #1a7f37; } .s-third { fill: #4ac26b; } .s-linker { fill: #aceebb; } .s-unattr { fill: #d0d7de; }
      .l-exact { stroke: #1a7f37; } .l-written { stroke: #0969da; }
    }
  </style>
'''


def esc(s):
    return html.escape(str(s), quote=True)


def pct(n, d):
    return 100.0 * n / d if d else 0.0


def fmt(n):
    return '{:,}'.format(int(n))


def svg_open(w, h, title, desc):
    return ('<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %d %d" role="img" '
            'aria-labelledby="title desc">\n  <title id="title">%s</title>\n  <desc id="desc">%s</desc>\n'
            % (w, h, w, h, esc(title), esc(desc))) + STYLE + \
        '  <rect class="card" x="0.5" y="0.5" width="%d" height="%d" rx="12"/>\n' % (w - 1, h - 1) + \
        '  <g font-family="%s">\n' % FONT


def stacked(x, y, w, h, parts, cls_prefix='s-', colors=None):
    """Stacked horizontal bar. parts: [(key, value)]; returns svg text (value sum is the whole)."""
    total = sum(v for _, v in parts) or 1
    out, cx = [], x
    out.append('<clipPath id="clip%d"><rect x="%s" y="%s" width="%s" height="%s" rx="%s"/></clipPath>'
               % (int(y), x, y, w, h, h / 2))
    out.append('<g clip-path="url(#clip%d)">' % int(y))
    for key, v in parts:
        ww = w * v / total
        if ww <= 0:
            continue
        fill = ' class="%s%s"' % (cls_prefix, key) if colors is None else ' fill="%s"' % colors[key]
        out.append('<rect x="%.2f" y="%s" width="%.2f" height="%s"%s/>' % (cx, y, ww, h, fill))
        cx += ww
    out.append('</g>')
    return ''.join(out)


def render_card(r, names, hist, today_label):
    c, tot = r.counts, r.total
    exact_p = pct(c['exact'], tot)
    rebuilt = sum(c[s] for s in REBUILT)
    rebuilt_p = pct(rebuilt, tot)
    written = c['exact'] + c['notexact']
    delta = ''
    prev = [h for h in hist if h['date'] < today_label]
    if prev:
        p = prev[-1]
        d = rebuilt_p - pct(sum(int(p['bytes_' + s]) for s in REBUILT), int(p['text_bytes']) or tot)
        if abs(d) >= 0.005:
            delta = '<tspan class="%s" dx="10" font-size="13" font-weight="600">%s %.2f</tspan>' % (
                'up' if d >= 0 else 'down', '▲' if d >= 0 else '▼', abs(d))
    eb = names['eb']
    ntot = sum(eb.values()) or 1
    W, H = 880, 520
    o = [svg_open(W, H, 'Earth & Beyond client rebuild: %.2f%% rebuilt (%.2f%% byte-exact), %.2f%% written in C++'
                  % (rebuilt_p, exact_p, pct(written, tot)),
                  '%s of %s bytes of client.exe .text are rebuilt: %s byte for byte from source, the rest runtime libraries '
                  'and linker output; %s are written in C++.' % (fmt(rebuilt), fmt(tot), fmt(c['exact']), fmt(written)))]
    o.append('    <text x="28" y="30" class="muted" font-size="13" font-weight="600" letter-spacing="1.3">'
             'EARTH &amp; BEYOND CLIENT · REBUILD PROGRESS</text>\n')
    # bar 1
    o.append('    <text x="28" y="64" class="strong" font-size="15" font-weight="600">Bytes rebuilt%s</text>\n' % delta)
    o.append('    <text x="852" y="64" class="strong" font-size="20" font-weight="700" text-anchor="end">%.2f%%</text>\n' % rebuilt_p)
    o.append('    ' + stacked(28, 74, 824, 16, [(s, c[s]) for s in DISPLAY]) + '\n')
    lx = 28
    for s in DISPLAY:
        label = '%s %.1f%%' % (SHORT[s], pct(c[s], tot))
        o.append('    <rect x="%d" y="100" width="9" height="9" rx="2" class="s-%s"/>' % (lx, s))
        o.append('<text x="%d" y="108.5" class="muted" font-size="11.5">%s</text>\n' % (lx + 14, esc(label)))
        lx += 14 + int(len(label) * 6.1) + 18
    o.append('    <text x="28" y="128" class="muted" font-size="12.5">%s of %s bytes of client.exe\'s .text: %s KB byte-exact '
             'from source, %s KB runtime and D3DX, %s KB the linker\'s thunks, padding and EH stubs.</text>\n'
             % (fmt(rebuilt), fmt(tot), fmt(c['exact'] // 1024), fmt(c['third'] // 1024), fmt(c['linker'] // 1024)))
    # bar 2
    o.append('    <text x="28" y="164" class="strong" font-size="15" font-weight="600">Written in C++</text>\n')
    o.append('    <text x="852" y="164" class="strong" font-size="20" font-weight="700" text-anchor="end">%.2f%%</text>\n' % pct(written, tot))
    o.append('    ' + stacked(28, 174, 824, 16, [('exact', c['exact']), ('notexact', c['notexact']),
                                                  ('unattr', tot - written)]) + '\n')
    o.append('    <text x="28" y="208" class="muted" font-size="12.5">%s bytes written as source: %s byte-exact + %s NOT EXACT '
             '(functional, D64). Stubs are not counted.</text>\n' % (fmt(written), fmt(c['exact']), fmt(c['notexact'])))
    # bar 3: names
    unt = names['untagged5']
    o.append('    <text x="28" y="248" class="strong" font-size="15" font-weight="600">Readable names (D56 scale)</text>\n')
    o.append('    <text x="852" y="248" class="strong" font-size="20" font-weight="700" text-anchor="end">%.1f%% at 4-5</text>\n'
             % pct(eb[5] + eb[4], ntot))
    o.append('    ' + stacked(28, 258, 824, 16, [(k, eb[k]) for k in (5, 4, 3, 2, 1)], cls_prefix='', colors=NAME_COLORS) + '\n')
    lx = 28
    for k in (5, 4, 3, 2, 1):
        label = '%s %.1f%%' % (NAME_LABELS[k], pct(eb[k], ntot))
        o.append('    <rect x="%d" y="284" width="9" height="9" rx="2" fill="%s"/>' % (lx, NAME_COLORS[k]))
        o.append('<text x="%d" y="292.5" class="muted" font-size="11.5">%s</text>\n' % (lx + 14, esc(label)))
        lx += 14 + int(len(label) * 5.9) + 12
    o.append('    <text x="28" y="312" class="muted" font-size="12.5">%s declarations in E&amp;B code (types, functions, members, '
             'globals, parameters, locals). Untagged names count as 5 per D56: %s of them (%.1f%%).</text>\n'
             % (fmt(ntot), fmt(unt), pct(unt, ntot)))
    o.append('    <text x="28" y="332" class="muted" font-size="12.5">Beside the scale: %s declarations in the imported EA '
             'library code (EA source, EA\'s names, not scored).</text>\n' % fmt(names['ea']))
    # per-library functions
    o.append('    <text x="28" y="372" class="strong" font-size="15" font-weight="600">Functions proven, by library</text>\n')
    fn = r.sets.fn_by_lib
    x = 28
    y = 394
    for lib in LIBS:
        n = fn.get(lib, 0)
        o.append('    <text x="%d" y="%d" class="strong" font-size="16" font-weight="700">%s</text>' % (x, y, fmt(n)))
        o.append('<text x="%d" y="%d" class="muted" font-size="11.5">%s</text>\n' % (x, y + 15, esc(lib)))
        x += 77
    o.append('    <text x="28" y="432" class="muted" font-size="12.5">%s functions byte-exact in all (%s file-and-address pairs; '
             'a function can be proven by two files, and D54 adds %s identical copies).</text>\n'
             % (fmt(r.exact_functions), fmt(r.sets.pairs), fmt(len(r.sets.amb - r.sets.exact))))
    # footnote
    foot = ['Counting rule changed on %s (D80): identical functions that differ only in a relocation target are told apart, so earlier '
            'days under-count.' % RULE_CHANGE,
            'Measured from committed data only (tools/progress.py); NOT EXACT placement: %d entries located, %d charged an average '
            'function (%s bytes).' % (r.located, r.unresolved, fmt(r.avg_ne)),
            'Updated %s.' % today_label]
    for i, t in enumerate(foot):
        o.append('    <text x="28" y="%d" class="muted" font-size="11">%s</text>\n' % (464 + i * 16, esc(t)))
    o.append('  </g>\n</svg>\n')
    return ''.join(o)


def render_chart(hist):
    W, H = 880, 300
    o = [svg_open(W, H, 'Earth & Beyond client rebuild over time',
                  'Share of client.exe .text that is byte-exact and that is written in C++, per day.')]
    o.append('    <text x="28" y="30" class="muted" font-size="13" font-weight="600" letter-spacing="1.3">'
             'EARTH &amp; BEYOND CLIENT · OVER TIME</text>\n')
    o.append('    <line x1="400" x2="424" y1="25" y2="25" class="l-exact" stroke-width="3"/>'
             '<text x="430" y="29" class="muted" font-size="12">byte-exact</text>'
             '<line x1="520" x2="544" y1="25" y2="25" class="l-written" stroke-width="3"/>'
             '<text x="550" y="29" class="muted" font-size="12">written in C++ (exact + NOT EXACT)</text>\n')
    px, py, pw, ph = 70, 52, 770, 190
    rows = [h for h in hist if int(h['text_bytes'] or 0)]
    if not rows:
        o.append('  </g>\n</svg>\n')
        return ''.join(o)
    ex = [pct(int(h['bytes_exact']), int(h['text_bytes'])) for h in rows]
    wr = [pct(int(h['bytes_exact']) + int(h['bytes_notexact']), int(h['text_bytes'])) for h in rows]
    top = max(wr + ex + [1.0])
    step = next(s for s in (1, 2, 5, 10, 20, 25, 50) if top / s <= 5)
    ymax = (int(top // step) + 1) * step
    d0 = datetime.date.fromisoformat(rows[0]['date'])
    d1 = datetime.date.fromisoformat(rows[-1]['date'])
    span = max(1, (d1 - d0).days)

    def X(d):
        return px + pw * ((datetime.date.fromisoformat(d) - d0).days / span if len(rows) > 1 else 0.5)

    def Y(v):
        return py + ph * (1 - v / ymax)
    for k in range(0, int(ymax // step) + 1):
        v = k * step
        o.append('    <line x1="%d" x2="%d" y1="%.1f" y2="%.1f" class="grid"/><text x="%d" y="%.1f" class="muted" '
                 'font-size="11.5" text-anchor="end">%g%%</text>\n' % (px, px + pw, Y(v), Y(v), px - 8, Y(v) + 4, v))
    for h in rows:
        o.append('    <text x="%.1f" y="%d" class="muted" font-size="11.5" text-anchor="middle">%s</text>\n'
                 % (X(h['date']), py + ph + 20, esc(h['date'][5:])))
    if d0 <= datetime.date.fromisoformat(RULE_CHANGE) <= d1:
        x = X(RULE_CHANGE)
        o.append('    <line x1="%.1f" x2="%.1f" y1="%d" y2="%d" class="grid" stroke-dasharray="4 3"/>'
                 '<text x="%.1f" y="%d" class="muted" font-size="11" text-anchor="end">counting rule D80</text>\n'
                 % (x, x, py, py + ph, x - 4, py + 12))
    for cls, vals in (('l-written', wr), ('l-exact', ex)):
        pts = ' '.join('%.1f,%.1f' % (X(h['date']), Y(v)) for h, v in zip(rows, vals))
        o.append('    <polyline points="%s" fill="none" class="%s" stroke-width="2.5" stroke-linejoin="round"/>\n' % (pts, cls))
        for h, v in zip(rows, vals):
            o.append('    <circle cx="%.1f" cy="%.1f" r="3.2" class="%s" fill="none" stroke-width="2"><title>%s: %.2f%%</title></circle>\n'
                     % (X(h['date']), Y(v), cls, h['date'], v))
    o.append('    <text x="28" y="%d" class="muted" font-size="11">One point per day, from that day\'s last commit; '
             'the D80 rule change (%s) makes earlier days under-count.</text>\n' % (H - 14, RULE_CHANGE))
    o.append('  </g>\n</svg>\n')
    return ''.join(o)


def bins_of(buf, lo, hi, tab, n):
    """Per bin: counts of each state for the bytes lo..hi of the executable."""
    s0, e0 = lo - tab.text_va, hi - tab.text_va
    size = e0 - s0
    out = []
    for b in range(n):
        a = s0 + size * b // n
        z = s0 + size * (b + 1) // n
        seg = buf[a:z]
        out.append([seg.count(CODE[s]) for s in STATES])
    return out


def render_map(r):
    tab = r.tab
    rows = region_table(r)
    W = 880
    body = []
    body.append('    <text x="28" y="30" class="muted" font-size="13" font-weight="600" letter-spacing="1.3">'
                'EARTH &amp; BEYOND CLIENT · MAP OF .TEXT</text>\n')
    lx = 28
    for s in DISPLAY:
        body.append('    <rect x="%d" y="44" width="9" height="9" rx="2" class="s-%s"/><text x="%d" y="52.5" class="muted" '
                    'font-size="11.5">%s</text>\n' % (lx, s, lx + 13, esc(SHORT[s])))
        lx += 13 + int(len(SHORT[s]) * 6.2) + 16
    strips = [('E&B\'s own code: 0x%X-0x%X (%s KB), reached through the thunk table'
               % (tab.text_va, LIB_START, fmt((LIB_START - tab.text_va) // 1024)), tab.text_va, LIB_START),
              ('Libraries and runtime: 0x%X-0x%X (%s KB)'
               % (LIB_START, tab.text_end, fmt((tab.text_end - LIB_START) // 1024)), LIB_START, tab.text_end)]
    y = 82
    NB = 412
    for title, lo, hi in strips:
        body.append('    <text x="28" y="%d" class="strong" font-size="12.5" font-weight="600">%s</text>\n' % (y, esc(title)))
        body.append('    <g>')
        bw = 824.0 / NB
        for bi, cnt in enumerate(bins_of(r.buf, lo, hi, tab, NB)):
            tot = sum(cnt) or 1
            yy = y + 8
            hh = 44
            for s, v in sorted(zip(STATES, cnt), key=lambda sv: DISPLAY.index(sv[0])):
                if v <= 0:
                    continue
                h = hh * v / tot
                body.append('<rect x="%.2f" y="%.2f" width="%.2f" height="%.2f" class="s-%s"/>' % (28 + bi * bw, yy, bw + 0.3, h, s))
                yy += h
        body.append('</g>\n')
        if lo == LIB_START:
            for name, a, b in REGIONS:
                if a >= LIB_START and b is not None and b - a > 40000:
                    x = 28 + 824.0 * (a - lo) / (hi - lo)
                    body.append('    <line x1="%.1f" x2="%.1f" y1="%d" y2="%d" class="grid"/>\n' % (x, x, y + 8, y + 58))
                    body.append('    <text x="%.1f" y="%d" class="muted" font-size="10" transform="rotate(35 %.1f %d)">%s</text>\n'
                                % (x + 2, y + 70, x + 2, y + 70, esc(name)))
            y += 104
        else:
            y += 84
    body.append('    <text x="28" y="%d" class="strong" font-size="12.5" font-weight="600">By region</text>\n' % y)
    y += 20
    # columns: (x, anchor, header)
    cols = [(28, 'start', 'Region'), (150, 'start', 'Address range'), (330, 'end', 'KB'), (400, 'end', 'exact'),
            (480, 'end', 'CRT/3rd'), (550, 'end', 'linker'), (630, 'end', 'NOT EX.'), (690, 'end', 'stub'),
            (770, 'end', 'unattr.')]
    for x, an, hd in cols:
        body.append('    <text x="%d" y="%d" text-anchor="%s" class="muted" font-size="11" font-weight="600">%s</text>\n'
                    % (x, y, an, hd))
    y += 4
    body.append('    <line x1="28" x2="852" y1="%d" y2="%d" class="grid"/>\n' % (y, y))
    for row in rows:
        y += 17
        size = row['size'] or 1
        vals = [row['name'], '0x%X-0x%X' % (row['lo'], row['hi']), fmt(round(row['size'] / 1024))] + \
            ['%.1f%%' % pct(row[k], size) for k in DISPLAY]
        for (x, an, _), v in zip(cols, vals):
            body.append('    <text x="%d" y="%d" text-anchor="%s" class="strong" font-size="11">%s</text>\n' % (x, y, an, esc(v)))
    y += 22
    body.append('    <text x="28" y="%d" class="muted" font-size="10.5">Region boundaries follow the runs of proven functions in '
                'docs/data (docs/binary.md section 2). Each column is the share of the region\'s bytes.</text>\n' % y)
    H = y + 22
    head = svg_open(W, H, 'Map of client.exe by address',
                    'The executable .text in address order, coloured by what is rebuilt, with a table per library region.')
    return head + ''.join(body) + '  </g>\n</svg>\n'


# ------------------------------------------------------------------------------------ driver

def write_text(path, text):
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write(text)


def update_readme(r, names):
    """The one-line status above the card, between the progress markers."""
    path = os.path.join(ROOT, 'README.md')
    try:
        t = open(path, encoding='utf-8', newline='').read()
    except OSError:
        return
    c, tot = r.counts, r.total
    eb = names['eb']
    ntot = sum(eb.values()) or 1
    line = ("As of %s, **%.2f%%** of client.exe's `.text` is rebuilt: **%.2f%%** byte for byte from source (%s functions), "
            "%.2f%% runtime libraries and linker output. **%.2f%%** is written in C++. "
            "Of %s declarations in E&B's code, **%.1f%%** score 4-5 on D56's name scale "
            "(untagged names count as 5, %.1f%% of all)."
            % (datetime.date.today().isoformat(), pct(sum(c[s] for s in REBUILT), tot), pct(c['exact'], tot),
               fmt(r.exact_functions), pct(c['third'] + c['linker'], tot),
               pct(c['exact'] + c['notexact'], tot), fmt(ntot), pct(eb[5] + eb[4], ntot), pct(names['untagged5'], ntot)))
    a, b = '<!-- progress:start -->', '<!-- progress:end -->'
    i, j = t.find(a), t.find(b)
    if i < 0 or j < 0:
        return
    body = t[i + len(a):j]
    imgs = '\n\n'.join(l for l in body.split('\n') if l.startswith(('[![', '![')))
    new = t[:i + len(a)] + '\n' + line + '\n\n' + imgs + '\n' + t[j:]
    if new != t:
        write_text(path, new)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--history', action='store_true', help='backfill the days missing from the history CSV')
    ap.add_argument('--rebuild', action='store_true', help='with --history: recompute every day')
    ap.add_argument('--today', action='store_true', help='add or replace today\'s row from the working tree')
    ap.add_argument('--print', action='store_true', dest='show', help='print the numbers, write no files')
    ap.add_argument('--no-names', action='store_true', help='skip the declaration census (slow)')
    ap.add_argument('--readme', action='store_true', help='rewrite the status line in README.md')
    a = ap.parse_args()

    src = Src(None)
    tab = Tables(src)
    r = measure(src, tab)
    today = datetime.date.today().isoformat()
    head = git('rev-parse', '--short=8', 'HEAD').decode().strip()
    hist = None
    if a.history or a.today:
        rows = backfill(tab, a.rebuild, today) if a.history else {x['date']: x for x in read_history()}
        if a.today or a.history:
            rows[today] = row_of(today, head, r)
        hist = list(rows.values())
        if not a.show:
            write_history(hist)
    if hist is None:
        hist = read_history()
    names = names_summary() if not a.no_names else {'eb': {5: 1, 4: 0, 3: 0, 2: 0, 1: 0}, 'untagged5': 0, 'ea': 0}
    c = r.counts
    print('.text %s bytes' % fmt(r.total))
    for s in DISPLAY:
        print('  %-30s %10s  %6.2f%%' % (STATE_NAMES[s], fmt(c[s]), pct(c[s], r.total)))
    print('  rebuilt (exact + third-party + linker): %.2f%%' % pct(sum(c[s] for s in REBUILT), r.total))
    print('  written in C++ (exact + NOT EXACT): %.2f%%' % pct(c['exact'] + c['notexact'], r.total))
    print('  exact functions %s, pairs %s; NOT EXACT located %d, unplaced %d (avg %d bytes); sizes estimated for %d functions'
          % (fmt(r.exact_functions), fmt(r.sets.pairs), r.located, r.unresolved, r.avg_ne, r.tab.estimated_sizes))
    eb = names['eb']
    nt = sum(eb.values())
    print('  names: ' + ', '.join('%d: %d (%.1f%%)' % (k, eb[k], pct(eb[k], nt)) for k in (5, 4, 3, 2, 1)) +
          '; untagged counted as 5: %d; EA source %d' % (names['untagged5'], names['ea']))
    if a.show:
        return
    write_text(os.path.join(SVG_DIR, 'progress.svg'), render_card(r, names, hist, today))
    write_text(os.path.join(SVG_DIR, 'progress_chart.svg'), render_chart(hist))
    write_text(os.path.join(SVG_DIR, 'progress_map.svg'), render_map(r))
    if a.readme:
        update_readme(r, names)
    print('wrote docs/progress.svg, docs/progress_chart.svg, docs/progress_map.svg')


if __name__ == '__main__':
    main()
