"""Row- and entry-level merges for commit_mine.py (D112(2)): pure text functions, no git.

All texts are git blob form (what `git show REV:path` prints); the caller converts the working copy
first, so line endings are whatever the repository stores and nothing here rewrites them.

* Row files (docs/data/attempts.csv, not-exact.md, names-review.md): `merge_rows(base, work, owned)`
  returns BASE plus the caller's added rows and minus the caller's removed ones; a row is the
  caller's when `owned(row)` says so. Other builders' rows, adjacent or not, never travel.
* docs/data/imports.json (indent=1, sort_keys, as importer.py writes it): `set_entries` replaces,
  inserts or removes single entries of a library's "files" or "headers" map, and `set_totals` copies
  a library's count lines (ambiguous_total, in_tree_total, totals).
"""
import csv
import json
import re

ROW_FILES = ('docs/data/attempts.csv', 'docs/data/not-exact.md', 'docs/data/names-review.md')
ADDR = re.compile(r'\b0x([0-9A-Fa-f]{6,8})\b')
PATH_TOKEN = re.compile(r'([\w.+-]+(?:/[\w.+-]+)+\.\w+)(?::\d+)?')
TOTAL_KEYS = ('ambiguous_total', 'in_tree_total', 'totals')


def norm_addr(a):
    m = ADDR.search(a or '')
    return '0x' + m[1].upper() if m else None


def norm_path(p):
    return p.replace('\\', '/').strip().lower()


# ---------------------------------------------------------------- row files

def csv_records(text):
    """(raw text, row dict or None for the header) per csv record; quoted multi-line fields stay whole."""
    lines = text.splitlines(keepends=True)
    taken = []

    def feed():
        for ln in lines:
            taken.append(ln)
            yield ln
    out, fields = [], None
    for row in csv.reader(feed()):
        raw = ''.join(taken)
        taken.clear()
        if fields is None:
            fields = row
            out.append((raw, None))
        else:
            out.append((raw, dict(zip(fields, row))))
    if taken:                                   # trailing text csv did not return as a row
        out.append((''.join(taken), None))
    return out


def md_key(line):
    """The repository path a table row is about (its first backticked `dir/file.ext[:line]`)."""
    if not line.startswith('| '):
        return None
    for tok in re.findall(r'`([^`]+)`', line):
        m = PATH_TOKEN.fullmatch(tok)
        if m:
            return m[1]
    return None


def records(path, text):
    """[(raw, key)], key = (address, file) of a row or None for lines that are no row."""
    if path.endswith('.csv'):
        return [(raw, None if row is None else (norm_addr(row.get('address')), row.get('file') or ''))
                for raw, row in csv_records(text)]
    out = []
    for ln in text.splitlines(keepends=True):
        k = md_key(ln)
        out.append((ln, None if k is None else (None, k)))
    return out


def owner(addrs, files):
    """owned(key): the row's address is one of addrs, or its file is one of files (paths compared lower-case)."""
    addrs = {norm_addr(a) for a in addrs if norm_addr(a)}
    files = {norm_path(f) for f in files if f}

    def owned(key):
        if key is None:
            return False
        a, f = key
        return (a is not None and a in addrs) or (bool(f) and norm_path(f) in files)
    return owned


def _eol(text):
    return '\r\n' if '\r\n' in text else '\n'


def merge_rows(path, base, work, owned):
    """(text, added rows, removed rows): BASE with only the owned rows changed to match WORK.

    An added row goes after the nearest earlier WORK record that the result already holds (so
    a generated table stays sorted and an appended csv row stays at the end)."""
    br, wr = records(path, base), records(path, work)
    eol = _eol(base or work)
    wcount = {}
    for raw, k in wr:
        if owned(k):
            wcount[raw] = wcount.get(raw, 0) + 1
    result, removed = [], []
    for raw, k in br:
        if owned(k):
            if wcount.get(raw, 0):
                wcount[raw] -= 1
            else:
                removed.append(raw)
                continue
        result.append(raw)
    bcount = {}
    for raw, k in br:
        if owned(k):
            bcount[raw] = bcount.get(raw, 0) + 1
    added = []
    for i, (raw, k) in enumerate(wr):
        if not owned(k):
            continue
        if bcount.get(raw, 0):
            bcount[raw] -= 1
            continue
        pos = None
        for j in range(i - 1, -1, -1):
            prev = wr[j][0]
            hits = [n for n, r in enumerate(result) if r == prev]
            if len(hits) == 1 or (hits and wr[j][1] is not None):
                pos = hits[-1] + 1
                break
        if pos is None:
            pos = 1 if result else 0               # after the header line
        if pos > 0 and not result[pos - 1].endswith('\n'):
            result[pos - 1] += eol
        if pos < len(result) and not raw.endswith('\n'):
            raw += eol
        result.insert(pos, raw)
        added.append(raw)
    text = ''.join(result)
    if path.endswith('not-exact.md'):
        text = _fix_notexact(text)
    elif path.endswith('names-review.md'):
        text = _fix_names(text, added, removed)
    return text, added, removed


def _fix_notexact(text):
    keys = [md_key(ln) for ln in text.splitlines()]
    keys = [k for k in keys if k]
    return re.sub(r'(?m)^\d+ entries in \d+ files\.', f'{len(keys)} entries in {len({k.lower() for k in keys})} files.',
                  text, count=1)


def _fix_names(text, added, removed):
    """Scores 1-3 count exactly the listed rows: move the summary counts by the rows changed."""
    delta = {}
    for raws, sign in ((added, 1), (removed, -1)):
        for raw in raws:
            m = re.match(r'\| (\d) \|', raw)
            if m:
                delta[m[1]] = delta.get(m[1], 0) + sign

    head, sep, rest = text.partition('## For review')
    head = re.sub(r'(?m)^(\| (\d) \| [^|\n]* \| )(\d+)( \|)$',
                  lambda m: f'{m[1]}{int(m[3]) + delta.get(m[2], 0)}{m[4]}', head)
    return head + sep + rest


# ---------------------------------------------------------------- imports.json

LIB = re.compile(r'^ "([^"]+)": \{$')


def lib_spans(lines):
    """{lib: (first, last)} line indexes (0-based, inclusive) of each top-level library object."""
    spans = {}
    for i, ln in enumerate(lines):
        m = LIB.match(ln.rstrip('\r\n'))
        if m:
            for j in range(i + 1, len(lines)):
                if re.match(r'^ \},?$', lines[j].rstrip('\r\n')):
                    spans[m[1]] = (i, j)
                    break
    return spans


def member_span(lines, lib, key):
    """(first, last) of `  "key": ...` inside lib (a one-line value or a `{`...`}` block), or None."""
    sp = lib_spans(lines).get(lib)
    if not sp:
        return None
    pat = re.compile(r'^  ' + re.escape('"' + key + '"') + r': (.*)$')
    for i in range(sp[0] + 1, sp[1]):
        m = pat.match(lines[i].rstrip('\r\n'))
        if not m:
            continue
        if m[1] in ('{', '['):
            close = '}' if m[1] == '{' else ']'
            for j in range(i + 1, sp[1]):
                if re.match(r'^  ' + re.escape(close) + r',?$', lines[j].rstrip('\r\n')):
                    return (i, j)
        return (i, i)
    return None


def map_entries(lines, lib, mapname):
    """(span, [(key, entry lines)]) of the `mapname` object of lib; entries at indent 3."""
    sp = member_span(lines, lib, mapname)
    if sp is None:
        return None, []
    if sp[0] == sp[1]:
        raise ValueError(f'{lib}.{mapname} is written on one line; commit_mine cannot edit it')
    out, i = [], sp[0] + 1
    while i < sp[1]:
        ln = lines[i].rstrip('\r\n')
        m = re.match(r'^   ("(?:[^"\\]|\\.)*"): (.*)$', ln)
        if not m:
            raise ValueError(f'{lib}.{mapname}: unexpected line {i + 1}: {ln!r}')
        key = json.loads(m[1])
        j = i
        if m[2] in ('{', '['):
            while not re.match(r'^   [}\]],?$', lines[j].rstrip('\r\n')):
                j += 1
        out.append((key, lines[i:j + 1]))
        i = j + 1
    return sp, out


def _comma(entry, want):
    last = entry[-1]
    body = last.rstrip('\r\n')
    eol = last[len(body):]
    body = body[:-1] if body.endswith(',') else body
    return entry[:-1] + [body + (',' if want else '') + eol]


def find_entry(lines, key, mapname):
    """Libraries whose `mapname` map has `key`."""
    libs = []
    for lib in lib_spans(lines):
        _, ents = map_entries(lines, lib, mapname)
        if any(k == key for k, _ in ents):
            libs.append(lib)
    return libs


def set_entries(cur, work, lib, mapname, keys):
    """cur's text with lib.mapname[key] for each key made equal to work's (inserted, replaced or removed)."""
    cl, wl = cur.splitlines(keepends=True), work.splitlines(keepends=True)
    sp, ents = map_entries(cl, lib, mapname)
    _, wents = map_entries(wl, lib, mapname)
    if sp is None:
        raise ValueError(f'{lib} has no {mapname} map in the base')
    ents = [(k, list(e)) for k, e in ents]
    wmap = dict(wents)
    changes = []
    for key in keys:
        idx = next((n for n, (k, _) in enumerate(ents) if k == key), None)
        if key in wmap:
            new = list(wmap[key])
            if idx is None:
                pos = next((n for n, (k, _) in enumerate(ents) if k > key), len(ents))
                ents.insert(pos, (key, new))
                changes.append(f'+{key}')
            elif [x.rstrip(',\r\n') for x in ents[idx][1]] != [x.rstrip(',\r\n') for x in new]:
                ents[idx] = (key, new)
                changes.append(f'~{key}')
        elif idx is not None:
            del ents[idx]
            changes.append(f'-{key}')
    body = []
    for n, (k, e) in enumerate(ents):
        body += _comma(e, n < len(ents) - 1)
    return ''.join(cl[:sp[0] + 1] + body + cl[sp[1]:]), changes


def set_totals(cur, work):
    """cur's text with every library's count lines (TOTAL_KEYS) taken from work."""
    cl, wl = cur.splitlines(keepends=True), work.splitlines(keepends=True)
    changes = []
    for lib in lib_spans(wl):
        for key in TOTAL_KEYS:
            cs, ws = member_span(cl, lib, key), member_span(wl, lib, key)
            if cs is None or ws is None:
                continue
            old, new = cl[cs[0]:cs[1] + 1], wl[ws[0]:ws[1] + 1]
            if [x.rstrip(',\r\n') for x in old] == [x.rstrip(',\r\n') for x in new]:
                continue
            new = _comma(new, old[-1].rstrip('\r\n').endswith(','))
            cl[cs[0]:cs[1] + 1] = new
            changes.append(f'{lib}.{key}')
    return ''.join(cl), changes
