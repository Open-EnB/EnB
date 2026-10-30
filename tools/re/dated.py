"""dated.py [--md OUT.md] [--json OUT.json]: list every dated or author-tagged change comment in
EA's WW library copies (Z = Zero Hour/GeneralsMD, G = Generals, R = Renegade), plus "Earth and
Beyond" notes and the VSS $Modtime of every file.

Per comment: lib, file, line (in the first copy that has it, Z > G > R), copies having it, date
(normalised YYYY-MM-DD), author, band against D9 (pre = before 06/27/02, mid = 06/27/02..08/04/02,
post = 08/05/02 and later), enclosing function ("^Name" = the comment sits just above Name), whether
the tree's file has the comment text, whether the tree's function body equals that copy's, and the
function's state in docs/data/imports.json (exact = one of the file's in_tree addresses carries the
name) or gaps-LIB.json (gap@ADDR sim)."""
import datetime
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
sys.path.insert(0, HERE)
import importer  # noqa: E402
from easource import function_spans, strip_comments  # noqa: E402

NAMES_OBJ = {}
DATA = os.path.join(REPO, 'docs', 'data')
EXT = ('.cpp', '.h', '.inl', '.c', '.hpp')
CUT1 = datetime.date(2002, 6, 27)
CUT2 = datetime.date(2002, 8, 5)
MON = {m: i + 1 for i, m in enumerate(
    'jan feb mar apr may jun jul aug sep oct nov dec'.split())}
KNOWN = {'KM', 'KJM', 'DMS', 'MW', 'HY', 'GTH', 'TSS', 'SKB', 'PWG', 'IML', 'STB', 'MBL', 'BMJ',
         'NAK', 'JGA', 'EHC', 'DRM', 'MLL', 'SR', 'TR', 'ST', 'NH', 'DS', 'JM', 'BG', 'CNC', 'JBA',
         'SBS', 'MWB', 'JSB', 'KJS', 'RAD', 'DAL', 'MG', 'CLD', 'AJA', 'JMH', 'BGC', 'YY', 'jba'}
NAMES = {'hector yee': 'HY', 'greg hjelstrom': 'GTH', '(gth)': 'GTH', 'kenny mitchell': 'KM',
         'darren schueller': 'DS', 'steve tall': 'ST', 'mark wilczynski': 'MW', 'byon garrabrant': 'BG',
         'patrick smith': 'PDS', 'jani penttinen': 'JP', 'naty hoffman': 'NH', 'ian leslie': 'IML'}
STOP = {'TODO', 'NOTE', 'HACK', 'NULL', 'END', 'EOF', 'ID', 'DDS', 'LOD', 'CRC', 'THIS', 'NOT',
        'MUST', 'FVF', 'ANSI', 'RTTI', 'RSA', 'SHA', 'POV', 'OLD', 'XBOX', 'WE', 'VC', 'UV', 'USER',
        'UNIX', 'THE', 'STL', 'RGB', 'OK', 'AND', 'OR', 'IF', 'DX', 'API', 'CPU', 'GPU', 'TNL', 'HW',
        'SW', 'PC', 'AM', 'PM', 'FIXME', 'XXX', 'BUG', 'WW', 'EA', 'MS', 'DX8', 'D3D', 'LRU', 'BSP',
        'AABB', 'OBB', 'MAX', 'MIN', 'TGA', 'INI', 'IO', 'ALL', 'NEW', 'DLL', 'EXE', 'GDI', 'CD', 'TV'}

RX_NUM = re.compile(r'(?<![\d./$-])(\d{1,2})[/-](\d{1,2})[/-](\d{4}|\d{2})(?![\d/])')
RX_ISO = re.compile(r'(?<!\d)(19\d\d|20\d\d)-(\d\d)-(\d\d)(?!\d)')
RX_MON = re.compile(r'\b(jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)[a-z]*\.?\s+'
                    r'(?:(\d{1,2})(?:st|nd|rd|th)?,?\s+)?(\d{4}|\'\d\d)', re.I)
RX_DMON = re.compile(r'\b(\d{1,2})\s+(jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)[a-z]*\.?,?\s+'
                     r'(\d{4})', re.I)
RX_EB = re.compile(r'earth\s*(and|&)\s*beyond|\bE&B\b|\bEnB\b', re.I)
RX_TAG = re.compile(r'(?:^|[\s(\[])\(?([A-Za-z]{2,4})\)?(?=\s*[:\-]|\s+\d{1,2}/|\s*$)')
RX_TAG_START = re.compile(r'^\W*\(?([A-Z]{2,4}|gth)\)?(?:[\s:\-,]|$)')


def mkdate(y, m, d):
    y = int(y)
    y = y + 2000 if y < 50 else (y + 1900 if y < 100 else y)
    try:
        return datetime.date(y, int(m), int(d) if d else 1)
    except ValueError:
        return None


def find_date(c):
    for rx, f in ((RX_ISO, lambda g: mkdate(g[0], g[1], g[2])),
                  (RX_NUM, lambda g: mkdate(g[2], g[0], g[1])),
                  (RX_MON, lambda g: mkdate(g[2].lstrip("'"), MON[g[0][:3].lower()], g[1])),
                  (RX_DMON, lambda g: mkdate(g[2], MON[g[1][:3].lower()], g[0]))):
        m = rx.search(c)
        if m:
            d = f(m.groups())
            if d and 1990 <= d.year <= 2004:
                return d, m
    return None, None


def find_author(c, dm):
    low = c.lower()
    for n, a in NAMES.items():
        if n in low:
            return a
    m = RX_TAG_START.match(c)
    if m and (m.group(1).upper() in KNOWN) and m.group(1).upper() not in STOP:
        return m.group(1).upper()
    if dm:  # a capital token next to the date
        around = c[max(0, dm.start() - 12):dm.end() + 12]
        for t in re.findall(r'\b([A-Z]{2,4})\b', around):
            if t not in STOP:
                return t
    for t in re.findall(r'(?:^|[\s(])([A-Z]{2,4})(?=\s*[:\-]\s)', c):
        if t in KNOWN:
            return t
    return None


def band(d):
    if d is None:
        return ''
    return 'pre' if d < CUT1 else ('mid' if d < CUT2 else 'post')


def comment_lines(text):
    """[(lineno, comment text, offset)] for every line carrying comment characters."""
    clean = strip_comments(text)
    out, pos = [], 0
    for no, (tl, cl) in enumerate(zip(text.split('\n'), clean.split('\n')), 1):
        chars = ''.join(a if b == ' ' and a != b else ' ' for a, b in zip(tl, cl)).strip()
        chars = re.sub(r'^(//+|/\*+|\*+)', '', chars).strip()
        chars = re.sub(r'\*+/$', '', chars).strip()
        if chars:
            out.append((no, chars, pos))
        pos += len(tl) + 1
    return out


def norm(s):
    return re.sub(r'\s+', ' ', s).strip()


def demangle(sym):
    if not sym or not sym.startswith('?'):
        return sym
    m = re.match(r'\?\?([0-9A-Z_])([A-Za-z_0-9@]*?)@@', sym)
    if m:
        parts = m.group(2).split('@')
        cls = '::'.join(reversed(parts))
        last = parts[0]
        op = {'0': last, '1': '~' + last, '4': 'operator=', '8': 'operator==', '9': 'operator!=',
              'A': 'operator[]', '_G': 'scalar deleting dtor', '_E': 'vector deleting dtor'}.get(m.group(1), '?op' + m.group(1))
        return cls + '::' + op
    m = re.match(r'\?([A-Za-z_0-9]+)@([A-Za-z_0-9@]*?)@@', sym)
    if m:
        parts = [m.group(1)] + [p for p in m.group(2).split('@') if p]
        return '::'.join(reversed(parts))
    m = re.match(r'\?([A-Za-z_0-9]+)@@', sym)
    return m.group(1) if m else sym


def load_names():
    names = {}
    for x in json.load(open(os.path.join(DATA, 'symbols.json'))):
        names.setdefault(x['address'], set()).add(demangle(x['symbol']))
    for f in os.listdir(DATA):
        if f.startswith('bytematch-') and f.endswith('.json'):
            for a, v in json.load(open(os.path.join(DATA, f))).items():
                for c in v.values():
                    if isinstance(c, dict) and c.get('symbol'):
                        names.setdefault(a, set()).add(demangle(c['symbol']))
    return names


def main():
    args = sys.argv[1:]
    md = args[args.index('--md') + 1] if '--md' in args else None
    js = args[args.index('--json') + 1] if '--json' in args else None
    imports = json.load(open(os.path.join(DATA, 'imports.json')))
    names = load_names()
    global NAMES_OBJ
    np_ = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'names.json')
    NAMES_OBJ = json.load(open(np_)) if os.path.exists(np_) else {}
    gaps = {}
    for lib in importer.LIB_DIR:
        p = os.path.join(DATA, 'gaps-%s.json' % lib)
        if os.path.exists(p):
            for g in json.load(open(p)):
                key = (lib, g['object'].lower().replace('.obj', ''), g['function'])
                gaps[key] = '%s@%s %s' % ('gap', g['address'], g['similarity'])
    rows, modtimes, seen = [], [], {}
    for lib, dirs in importer.LIB_DIR.items():
        tree_dir = importer.dest(lib)
        tree_files = {f.lower(): os.path.join(tree_dir, f) for f in os.listdir(tree_dir)} if os.path.isdir(tree_dir) else {}
        files_state = imports.get(lib, {}).get('files', {})
        for copy in ('Z', 'G', 'R'):
            if copy not in dirs:
                continue
            root = os.path.join(importer.COPY_ROOT[copy], dirs[copy])
            if not os.path.isdir(root):
                continue
            for dp, _, fs in os.walk(root):
                for f in sorted(fs):
                    if not f.lower().endswith(EXT):
                        continue
                    path = os.path.join(dp, f)
                    text = open(path, encoding='latin-1').read().replace('\r\n', '\n')
                    fl = f.lower()
                    tp = tree_files.get(fl)
                    ttext = open(tp, encoding='latin-1').read().replace('\r\n', '\n') if tp else None
                    tnorm = norm(ttext) if ttext else ''
                    try:
                        spans = function_spans(text)
                    except Exception:
                        spans = []
                    tspans = {}
                    if ttext:
                        try:
                            for s in function_spans(ttext):
                                tspans.setdefault(s[0], ttext[s[1]:s[3] + 1])
                        except Exception:
                            pass
                    stem = os.path.splitext(fl)[0]
                    fst = files_state.get(stem, {})
                    exact = set()
                    objnames = NAMES_OBJ.get(lib, {}).get(stem, {})
                    for a in fst.get('in_tree', []):
                        exact |= names.get(a, set())
                        if a in objnames:
                            exact.add(demangle(objnames[a]))
                    for no, c, off in comment_lines(text):
                        if '$Modtime::' in c:
                            d, _ = find_date(c)
                            modtimes.append((lib, f, copy, d))
                            continue
                        d, dm = find_date(c)
                        au = find_author(c, dm)
                        eb = bool(RX_EB.search(c))
                        if not (d or eb or (au and RX_TAG_START.match(c))):
                            continue
                        if not d and not eb and au not in KNOWN:
                            continue
                        if '$' in c and ('$Archive' in c or '$Revision' in c or '$Author' in c):
                            continue
                        k = (lib, fl, norm(c))
                        if k in seen:
                            seen[k]['copies'] += copy
                            continue
                        fn = ''
                        body = None
                        for s in spans:
                            if s[1] <= off <= s[3]:
                                fn, body = s[0], text[s[1]:s[3] + 1]
                        if not fn:
                            for s in spans:
                                if off < s[1] and text.count('\n', off, s[1]) <= 6:
                                    fn, body = '^' + (s[0] or ''), text[s[1]:s[3] + 1]
                                    break
                        bare = fn.lstrip('^')
                        same = ''
                        if ttext is not None and norm(ttext) == norm(text):
                            same = 'same'
                        elif body is not None and bare in tspans:
                            same = 'same' if norm(body) == norm(tspans[bare]) else 'differs'
                        elif body is not None and ttext is not None:
                            same = 'absent'
                        state = ''
                        if bare:
                            if bare in exact:
                                state = 'exact'
                            else:
                                state = gaps.get((lib, stem, bare), '')
                        r = dict(lib=lib, file=f, line=no, copies=copy, date=d.isoformat() if d else '',
                                 band=band(d), author=au or '', eb=eb, fn=fn,
                                 in_tree=('n/a' if ttext is None else ('yes' if norm(c) in tnorm else 'no')),
                                 body=same, state=state, text=c[:140])
                        seen[k] = r
                        rows.append(r)
    if js:
        json.dump(dict(rows=rows, modtimes=[(a, b, c, d.isoformat() if d else '') for a, b, c, d in modtimes]),
                  open(js, 'w', newline='\n'), indent=1)
    out = ['# Dated / tagged change comments in EA\'s WW library copies', '',
           'Generated by tools/re/dated.py. Bands against D9: pre < 2002-06-27 <= mid < 2002-08-05 <= post.',
           'copies: Z = Zero Hour, G = Generals, R = Renegade. in_tree: the tree file has the comment text. '
           'body: tree function body vs this copy (same/differs/absent). state: exact in tree, or gap@client-address similarity.', '',
           '| lib | file:line | copies | date | band | author | function | in_tree | body | state | comment |',
           '|---|---|---|---|---|---|---|---|---|---|---|']
    rows.sort(key=lambda r: (r['lib'], r['file'].lower(), r['line']))
    for r in rows:
        out.append('| %s | %s:%d | %s | %s | %s | %s%s | %s | %s | %s | %s | %s |' % (
            r['lib'], r['file'], r['line'], r['copies'], r['date'], r['band'], r['author'],
            ' E&B' if r['eb'] else '', r['fn'], r['in_tree'], r['body'], r['state'],
            r['text'].replace('|', '\\|')))
    import collections
    cnt = collections.Counter((r['band'] or 'undated', r['author'] or '?') for r in rows if 'R' not in r['copies'])
    out += ['', '## Rows not in Renegade, by band and author', '', '| band | author | rows |', '|---|---|---|']
    out += ['| %s | %s | %d |' % (b, a, n) for (b, a), n in sorted(cnt.items())]
    chk = collections.defaultdict(collections.Counter)
    for r in rows:
        if 'R' in r['copies'] or not r['fn'] or r['fn'].startswith('^') or r['state'] != 'exact':
            continue
        chk[(r['band'] or 'undated', r['author'] or '?')][r['in_tree']] += 1
    out += ['', '## Byte check: comments inside a function that is byte-exact in the tree (not in Renegade)', '',
            'yes = the exact (client) form carries the commented change; no = the client\'s form lacks it.', '',
            '| band | author | yes | no |', '|---|---|---|---|']
    out += ['| %s | %s | %d | %d |' % (b, a, v['yes'], v['no']) for (b, a), v in sorted(chk.items())]
    out += ['', '## VSS $Modtime per file (Z / G / R), files with a Z or G date on or after 2002-06-27', '',
            '| lib | file | Z | G | R |', '|---|---|---|---|---|']
    mt = {}
    for lib, f, copy, d in modtimes:
        mt.setdefault((lib, f.lower()), {})[copy] = d
    for (lib, f), v in sorted(mt.items()):
        if any(v.get(c) and v[c] >= CUT1 for c in 'ZG'):
            out.append('| %s | %s | %s | %s | %s |' % (lib, f, *(v.get(c).isoformat() if v.get(c) else '' for c in 'ZGR')))
    text = '\n'.join(out) + '\n'
    if md:
        open(md, 'w', newline='\n', encoding='utf-8').write(text)
    else:
        sys.stdout.write(text)
    print('rows', len(rows), 'modtimes', len(modtimes), file=sys.stderr)


if __name__ == '__main__':
    main()
