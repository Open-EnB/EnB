"""Exact siblings of a stuck function (the user's rule, 2026-11-21: the original programmers did not
invent a new way for every INI loader or effect; a parked function's best evidence is its exact
siblings by the same authors).

    python tools/re/siblings.py ADDR [--top N] [--shape]     rank exact functions as siblings of ADDR
    python tools/re/siblings.py --all-parked [--status S...] [--out FILE.md]
                                                              every parked (and --status blocked) function:
                                                              best sibling, shape differences, untried forms

Sibling evidence, all from data the project already has (nothing is built, no VM):
  name    the same method name on another class (Load_INI, Update, Unknown_1C ...), +3; class names that
          share a stem (SectorSoundClass / BridgeSoundManagerClass: "Sound") +1
  file    the same source file +2; a neighbour (within 0x400 bytes) of the target +1
  calls   idf-weighted Jaccard of the call targets (resolved through the incremental thunks), 0..4;
          calls that all of the client shares (operator new, memcpy ...) weigh nearly nothing
  strs    the same for the string literals the code pushes (INI sections / keys), 0..3
  size    similar byte size (ratio >= 0.7 +1, >= 0.9 +1)
Exact = an address in a ledger shard's in_tree (docs/data/imports/*/*.json). Names come from
symbols.json (demangled by a small parser) and the attempts log; the call/string sets are scanned from the
client's bytes (E8 targets that are verified function starts or thunks; push / mov imm32 string addresses)
and cached in .work/sib/features.pkl (delete it after symbols.json or the ledger changes a lot).

--shape (always on with --all-parked) reads the target's source and its best sibling's source (a
`// @0xADDR` / `// 0xADDR` comment above the definition, else `Class::Method(`) and lists the differences
in control structure as candidate forms: early return vs nested if, loop kind, locals up front vs in place,
helper calls (the client's callee sets differ) vs inline code, accessor vs member read. A candidate is
marked untried when no attempts.csv row of the target mentions its keywords: a keyword heuristic, not proof;
read `attempts.py show ADDR` before trusting a row.
"""
import argparse
import bisect
import csv
import glob
import json
import math
import os
import pickle
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from clientimage import DEFAULT_CLIENT, ClientImage  # noqa: E402

ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
DATA = os.path.join(ROOT, 'docs', 'data')
CACHE = os.path.join(ROOT, '.work', 'sib', 'features.pkl')
if not os.path.isdir(os.path.dirname(CACHE)):
    # a worktree's .work/sib may not exist: use the main tree's when this is a worktree
    main = ROOT.split(os.sep + '.work' + os.sep)[0]
    CACHE = os.path.join(main, '.work', 'sib', 'features.pkl')

HEX = lambda a: f'0x{a:X}'  # noqa: E731


# ---------------------------------------------------------------- names
def method_of(sym):
    """(class, method) from 'Class::Method' or an MSVC decorated name; (None, None) when unknown."""
    if not sym:
        return None, None
    if sym.startswith('j_'):
        sym = sym[2:]
    if sym.startswith('?'):
        m = re.match(r'\?(\?\w|\w+)@(\?\$\w+|\w+)@@', sym)
        if not m:
            return None, None
        meth, cls = m.group(1), m.group(2)
        if meth.startswith('??0') or meth == '?0':
            meth = '<ctor>'
        elif meth.startswith('?1'):
            meth = '<dtor>'
        elif meth.startswith('?'):
            meth = '<op>'
        return cls.lstrip('?$'), meth
    m = re.match(r'^(?:[\w:<>]+::)?(\w+)::(~?\w+)\s*$', sym.split('(')[0].strip())
    if m:
        return m.group(1), m.group(2)
    return None, None


def stem(cls):
    """Words of a class name (CamelCase split), minus the noise words."""
    words = re.findall(r'[A-Z][a-z0-9]+|[A-Z]+(?![a-z])', cls or '')
    return {w.lower() for w in words} - {'class', 'manager', 'struct', 'base', 'effect', 'list', 'type'}


# ---------------------------------------------------------------- data
def load_ledger():
    """{addr: (lib, file)} for every in_tree (exact) function."""
    out = {}
    for f in glob.glob(os.path.join(DATA, 'imports', '*', '*.json')):
        lib = os.path.basename(os.path.dirname(f))
        d = json.load(open(f))
        for a in d.get('in_tree', []):
            out[int(a, 16)] = (lib, d.get('name') or os.path.basename(f))
    return out


def load_names():
    out = {}
    for s in json.load(open(os.path.join(DATA, 'symbols.json'))):
        if s['kind'] == 'function':
            out[int(s['address'], 16)] = s['name']
    return out


def load_attempts():
    rows = {}
    path = os.path.join(DATA, 'attempts.csv')
    with open(path, newline='', encoding='utf-8') as fh:
        for r in csv.DictReader(fh):
            rows.setdefault(int(r['address'], 16), []).append(r)
    return rows


def features():
    """Per function start: size, call-target set, string set. Cached."""
    if os.path.exists(CACHE):
        try:
            return pickle.load(open(CACHE, 'rb'))
        except Exception:
            pass
    img = ClientImage(DEFAULT_CLIENT)
    starts = sorted(img.function_starts()[0])
    thunks = img.incremental_thunks()
    startset = set(starts) | set(thunks)
    tstart, tend, traw = img.section('.text')
    strs = {va: t for va, t in img.strings(4).items()}
    data = img.data
    fe = {}
    call_re = re.compile(rb'\xE8|[\x68\xB8-\xBF]')
    for i, s in enumerate(starts):
        e = starts[i + 1] if i + 1 < len(starts) else min(s + 0x2000, tend)
        e = min(e, s + 0x20000)
        body = data[traw + (s - tstart):traw + (e - tstart)]
        n = len(body)
        while n > 0 and body[n - 1] in (0xCC, 0x90):
            n -= 1
        body = body[:n]
        calls, ss = set(), set()
        for m in call_re.finditer(body):
            o = m.start()
            if o + 5 > len(body):
                continue
            if body[o] == 0xE8:
                dst = (s + o + 5 + struct.unpack_from('<i', body, o + 1)[0]) & 0xFFFFFFFF
                if dst in startset:
                    calls.add(thunks.get(dst, dst))
            else:
                v = struct.unpack_from('<I', body, o + 1)[0]
                if v in strs:
                    ss.add(strs[v])
        fe[s] = {'size': n, 'calls': frozenset(calls), 'strs': frozenset(ss)}
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    pickle.dump(fe, open(CACHE, 'wb'))
    return fe


class Index:
    def __init__(self):
        self.fe = features()
        self.led = load_ledger()
        self.names = load_names()
        self.att = load_attempts()
        n = max(1, len(self.fe))
        self.cdf, self.sdf = {}, {}
        for f in self.fe.values():
            for c in f['calls']:
                self.cdf[c] = self.cdf.get(c, 0) + 1
            for t in f['strs']:
                self.sdf[t] = self.sdf.get(t, 0) + 1
        self.n = n
        self.exact = [a for a in self.led if a in self.fe]
        self.byfile = {}
        for a in self.exact:
            self.byfile.setdefault(self.led[a], []).append(a)
        self.byname = {}
        for a in self.exact:
            c, m = method_of(self.names.get(a) or (source_index().get(a) or (0, 0, 0, None))[3])
            if m:
                self.byname.setdefault(m, []).append((a, c))

    def idf(self, df, k):
        return math.log(self.n / (1 + df.get(k, 0)))

    def wj(self, a, b, df):
        if not a or not b:
            return 0.0
        inter = sum(self.idf(df, k) for k in a & b)
        union = sum(self.idf(df, k) for k in a | b)
        return inter / union if union > 0 else 0.0

    def info(self, a, fallback_name=None):
        f = self.fe.get(a, {'size': 0, 'calls': frozenset(), 'strs': frozenset()})
        name = self.names.get(a) or fallback_name
        return f, name


def target_name(ix, a):
    rows = ix.att.get(a, [])
    for r in reversed(rows):
        if r['symbol']:
            return r['symbol'], r['file']
    return ix.names.get(a), None


def rank(ix, a, top=10):
    tname, tfile = target_name(ix, a)
    tc, tm = method_of(tname)
    tf, _ = ix.info(a)
    tlib = None
    out = []
    cand = set()
    if tm:
        cand |= {x for x, _ in ix.byname.get(tm, [])}
    # the same file (by attempts.csv's file) and address neighbours
    nb = [x for x in ix.exact if abs(x - a) <= 0x400]
    cand |= set(nb)
    # callee-set candidates: exact functions sharing a rare callee or string
    rare = [c for c in tf['calls'] if ix.cdf.get(c, 0) <= 40] + [s for s in tf['strs'] if ix.sdf.get(s, 0) <= 40]
    if rare:
        rs = set(rare)
        for x in ix.exact:
            f = ix.fe[x]
            if (f['calls'] | f['strs']) & rs:
                cand.add(x)
    cand.discard(a)
    for x in cand:
        f = ix.fe[x]
        ev, sc = [], 0.0
        xn = ix.names.get(x) or (source_index().get(x) or (0, 0, 0, None))[3]
        xc, xm = method_of(xn)
        if tm and xm == tm and xc != tc:
            sc += 3
            ev.append(f'same method {tm} on {xc}')
            if stem(tc) & stem(xc):
                sc += 1
                ev.append('class stem ' + '/'.join(sorted(stem(tc) & stem(xc))))
        lib, fl = ix.led[x]
        if tfile and (fl.replace('\\', '/').split('/')[-1] == os.path.basename(tfile.replace('\\', '/'))):
            sc += 2
            ev.append('same file')
        if abs(x - a) <= 0x400:
            sc += 1
            ev.append(f'neighbour {HEX(x - a) if x > a else "-" + HEX(a - x)}')
        j = ix.wj(tf['calls'], f['calls'], ix.cdf)
        if j > 0.05:
            sc += 4 * j
            shared = sorted(tf['calls'] & f['calls'], key=lambda c: ix.cdf.get(c, 0))[:3]
            ev.append(f'calls J={j:.2f} (' + ', '.join(ix.names.get(c, HEX(c)).split('@')[0].lstrip('?') for c in shared) + ')')
        j = ix.wj(tf['strs'], f['strs'], ix.sdf)
        if j > 0.05:
            sc += 3 * j
            shared = sorted(tf['strs'] & f['strs'], key=lambda t: ix.sdf.get(t, 0))[:3]
            ev.append(f'strs J={j:.2f} (' + ', '.join(repr(t[:18]) for t in shared) + ')')
        if tf['size'] and f['size']:
            r = min(tf['size'], f['size']) / max(tf['size'], f['size'])
            if r >= 0.7:
                sc += 1
                ev.append(f'size {f["size"]}')
            if r >= 0.9:
                sc += 1
        # a sibling needs a real link (method/file/calls/strs), size and neighbour alone are noise
        if not any(e.startswith(('same method', 'same file', 'calls', 'strs')) for e in ev):
            continue
        out.append((sc, x, xn, f'{lib}:{fl}', f['size'], ev))
    out.sort(key=lambda r: (-r[0], r[1]))
    return out[:top]


# ---------------------------------------------------------------- shape
SRC_DIRS = ['app', 'libs', 'wwaudio', 'ww3d2', 'wwlib', 'wwmath', 'wwdebug', 'wwsaveload', 'wwshade', 'binkmovie']


_SRC = None
ADDR_COMMENT = re.compile(r'^[ \t]*//[ \t]*@?(0x[0-9A-Fa-f]{6})\b[^\n]*\n((?:[ \t]*//[^\n]*\n)*)([^\n]*)', re.M)


def source_index():
    """{addr: (path, text, end of the address comment line, 'Class::Method' from the signature)}, from the // @0xADDR comments."""
    global _SRC
    if _SRC is not None:
        return _SRC
    _SRC = {}
    for d in SRC_DIRS:
        for ext in ('cpp', 'h', 'inl'):
            for p in glob.glob(os.path.join(ROOT, d, '**', '*.' + ext), recursive=True):
                try:
                    t = open(p, encoding='latin1').read()
                except OSError:
                    continue
                for m in ADDR_COMMENT.finditer(t):
                    sig = m.group(3)
                    mm = re.search(r'(\w+)::(~?\w+)\s*\(', sig)
                    nm = f'{mm.group(1)}::{mm.group(2)}' if mm else None
                    _SRC.setdefault(int(m.group(1), 16), (p, t, m.start(3), nm))
    return _SRC


def find_source(root, addr, name, hint=None):
    hit = source_index().get(addr)
    if hit:
        return hit[0], cut_body(hit[1], hit[2])
    return find_source_old(root, addr, name, hint)


def find_source_old(root, addr, name, hint=None):
    """(path, body text) of a function: by its address comment, else by Class::Method( ."""
    pat = re.compile(r'^\s*//\s*@?0x%X\b' % addr, re.I | re.M)
    files = []
    if hint:
        p = os.path.join(root, hint.replace('\\', '/'))
        if os.path.exists(p):
            files.append(p)
    for d in SRC_DIRS:
        for ext in ('cpp', 'h', 'inl'):
            files += glob.glob(os.path.join(root, d, '**', '*.' + ext), recursive=True)
    seen = set()
    for p in files:
        if p in seen:
            continue
        seen.add(p)
        try:
            t = open(p, encoding='latin1').read()
        except OSError:
            continue
        m = pat.search(t)
        if m:
            return p, cut_body(t, m.end())
    c, meth = method_of(name)
    if c and meth:
        rx = re.compile(r'^[\w:<>\*&\s]*\b%s::%s\s*\(' % (re.escape(c), re.escape(meth)), re.M)
        for p in files:
            if p in seen and hint and not p.endswith(hint.replace('\\', '/')):
                continue
            try:
                t = open(p, encoding='latin1').read()
            except OSError:
                continue
            m = rx.search(t)
            if m:
                return p, cut_body(t, m.start())
    return None, None


def cut_body(t, pos):
    i = t.find('{', pos)
    if i < 0:
        return None
    d, j = 0, i
    while j < len(t):
        ch = t[j]
        if ch == '{':
            d += 1
        elif ch == '}':
            d -= 1
            if d == 0:
                return t[i:j + 1]
        j += 1
    return t[i:]


DECL = re.compile(r'^\s*(?:const\s+|unsigned\s+|static\s+)*[A-Za-z_][\w:<>]*\s*[\*&]*\s+\**\w+\s*(?:\[[^\]]*\])?\s*(?:=[^;]*|\([^;]*\))?;\s*$')
NOTDECL = re.compile(r'^\s*(return|else|delete|goto|break|continue|case|throw)\b')


def shape(body):
    """Control-structure features of a function body (text heuristics)."""
    if not body:
        return None
    lines = [re.sub(r'//.*', '', l) for l in body.splitlines()]
    text = '\n'.join(lines)
    depth = maxd = 0
    ifdepth_returns = 0
    for ch in text:
        if ch == '{':
            depth += 1
            maxd = max(maxd, depth)
        elif ch == '}':
            depth -= 1
    inner = [l for l in lines[1:-1] if l.strip()]
    first_stmt = None
    up = 0
    for k, l in enumerate(inner):
        if DECL.match(l) and not NOTDECL.match(l):
            up += 1
        else:
            first_stmt = k
            break
    total_decls = sum(1 for l in inner if DECL.match(l) and not NOTDECL.match(l))
    early = len(re.findall(r'if\s*\([^{;]*\)\s*\{?\s*return\b', text))
    returns = len(re.findall(r'\breturn\b', text))
    return {
        'lines': len(inner),
        'maxdepth': maxd - 1,
        'early_returns': early,
        'returns': returns,
        'continue': len(re.findall(r'\bcontinue\b', text)),
        'break': len(re.findall(r'\bbreak\b', text)),
        'for': len(re.findall(r'\bfor\s*\(', text)),
        'while': len(re.findall(r'\bwhile\s*\(', text)),
        'do': len(re.findall(r'\bdo\s*\{', text)),
        'else': len(re.findall(r'\belse\b', text)),
        'decl_up': up,
        'decl_total': total_decls,
        'accessor': len(re.findall(r'(?:->|\.)\s*(?:Get|Is|Set)_\w+\s*\(', text)),
        'calls': len(re.findall(r'\b\w+\s*\(', text)),
        'asm': bool(re.search(r'\b__asm\b', text)),
        'this': len(re.findall(r'\bthis\b', text)),
        'cast_c': len(re.findall(r'\(\s*(?:int|float|unsigned|char|short|bool|long)\s*\*?\s*\)', text)),
        'ifs': len(re.findall(r'\bif\s*\(', text)),
        'ternary': text.count('?'),
    }


# keywords whose presence in the attempts text says the form was probably tried
TRIED = {
    'early': r'early return|return;|nested|nest\b|if \(!|inverted|== 0\) return|!= 0\) \{',
    'loop': r'\bfor\b|while|do \{|do-while|loop',
    'decl': r'declared|declaration|up front|at the top|hoist|in place|scope|local',
    'helper': r'helper|inline|static function|outline|0x[0-9a-f]{6}\(',
    'accessor': r'accessor|get_|member|direct|->',
    'else': r'\belse\b|else-if|switch',
    'ternary': r'ternary|\? ',
    'cast': r'cast|\(int\)|\(float\)',
}


def candidates(ts, ss, tcalls, scalls, ix, att_text):
    """Shape differences target (ts) vs sibling (ss) as (kind, text) candidate forms."""
    out = []
    t = att_text.lower()

    def tried(kind):
        return bool(re.search(TRIED[kind], t))

    if ss['early_returns'] > ts['early_returns'] or ss['maxdepth'] > ts['maxdepth'] + 0:
        if ss['maxdepth'] > ts['maxdepth']:
            out.append(('early', f'sibling nests (depth {ss["maxdepth"]}, {ss["early_returns"]} early returns), ours depth {ts["maxdepth"]} / {ts["early_returns"]} early returns: nest the body in the guard'))
    if ts['early_returns'] > ss['early_returns']:
        out.append(('early', f'ours returns early ({ts["early_returns"]}x), the sibling {ss["early_returns"]}x (it nests the work in the guard or runs on): try the nested / single-exit form'))
    for kw, lab in (('for', 'for'), ('while', 'while'), ('do', 'do-while')):
        if ts[kw] != ss[kw]:
            out.append(('loop', f'{lab} loops: ours {ts[kw]}, sibling {ss[kw]}'))
    if ts['decl_up'] != ss['decl_up'] or abs(ts['decl_total'] - ss['decl_total']) >= 2:
        out.append(('decl', f'locals: ours {ts["decl_up"]} up front / {ts["decl_total"]} total, sibling {ss["decl_up"]} / {ss["decl_total"]}'))
    only_s = [c for c in scalls - tcalls if ix.cdf.get(c, 0) <= 200]
    only_t = [c for c in tcalls - scalls if ix.cdf.get(c, 0) <= 200]
    if only_s and abs(ts['calls'] - ss['calls']) <= max(6, ts['calls']):
        out.append(('helper', 'the sibling calls ' + ', '.join(ix.names.get(c, HEX(c)).split('@')[0].lstrip('?') for c in only_s[:3]) + ' which the client\'s target does not: inline it / the target holds its work as inline code'))
    if only_t:
        out.append(('helper', 'the target calls ' + ', '.join(ix.names.get(c, HEX(c)).split('@')[0].lstrip('?') for c in only_t[:3]) + ' which the sibling lacks (a helper there, inline in the sibling?)'))
    if abs(ts['accessor'] - ss['accessor']) >= 2:
        out.append(('accessor', f'accessor calls: ours {ts["accessor"]}, sibling {ss["accessor"]}'))
    if ts['else'] != ss['else']:
        out.append(('else', f'else branches: ours {ts["else"]}, sibling {ss["else"]}'))
    if ts['ternary'] != ss['ternary']:
        out.append(('ternary', f'?: operators: ours {ts["ternary"]}, sibling {ss["ternary"]}'))
    if ts['cast_c'] != ss['cast_c']:
        out.append(('cast', f'C casts: ours {ts["cast_c"]}, sibling {ss["cast_c"]}'))
    return [(k, s, not tried(k)) for k, s in out]


def attempts_text(ix, a):
    return ' '.join((r['forms'] + ' ' + r['evidence'] + ' ' + r['blocker']) for r in ix.att.get(a, []))


def analyse(ix, a, top, shape_on):
    ranked = rank(ix, a, top)
    tname, tfile = target_name(ix, a)
    res = {'addr': a, 'name': tname, 'file': tfile, 'size': ix.fe.get(a, {}).get('size', 0), 'sibs': ranked, 'cands': [], 'src': None}
    if not shape_on or not ranked:
        return res
    tp, tb = find_source(ROOT, a, tname, tfile)
    ts = shape(tb)
    if not ts:
        return res
    res['src'] = os.path.relpath(tp, ROOT) if tp else None
    tf = ix.fe[a]
    for sc, x, xn, fl, sz, ev in ranked[:3]:
        sp, sbody = find_source(ROOT, x, xn)
        ss = shape(sbody)
        if not ss:
            continue
        cands = candidates(ts, ss, tf['calls'], ix.fe[x]['calls'], ix, attempts_text(ix, a))
        res['cands'].append((x, xn, cands))
    return res


# ---------------------------------------------------------------- output
def short(n, k=34):
    n = n or '?'
    c, m = method_of(n)
    s = f'{c}::{m}' if m and not n.startswith('?') and False else (f'{c}::{m}' if c and m else n)
    return s if len(s) <= k else s[:k - 1] + '~'


def print_one(ix, a, top, shape_on):
    r = analyse(ix, a, top, shape_on)
    print(f'{HEX(a)}  {r["name"] or "?"}  [{r["file"] or "?"}]  {r["size"]} bytes  '
          f'attempts: {len(ix.att.get(a, []))} rows')
    if not r['sibs']:
        print('  no exact sibling found')
    for sc, x, xn, fl, sz, ev in r['sibs']:
        print(f'  {sc:5.2f}  {HEX(x):9s} {short(xn):34s} {fl[:34]:34s} {sz:5d}  ' + '; '.join(ev))
    for x, xn, cands in r['cands']:
        print(f'  shape vs {HEX(x)} {short(xn)}:')
        for k, s, untried in cands:
            print(f'    {"UNTRIED" if untried else "tried? "} [{k}] {s}')


BLOCKER_ORDER = {'inline-budget': 0}


def latest(ix, a):
    rows = ix.att.get(a, [])
    return rows[-1] if rows else {}


def all_parked(ix, statuses, top, out, also=()):
    attempts_py = os.path.join(HERE, 'attempts.py')
    addrs = []
    for st in statuses:
        txt = subprocess.run([sys.executable, attempts_py, 'list', '--status', st], capture_output=True, text=True).stdout
        for line in txt.splitlines():
            m = re.match(r'(0x[0-9A-Fa-f]+)\s+(\w+)\s+(\S+)\s+(\S+)', line)
            if m:
                addrs.append((int(m.group(1), 16), st, m.group(3), m.group(4)))
    seen = {x[0] for x in addrs}
    for x in also:
        if x not in seen:
            addrs.append((x, latest(ix, x).get('status', '?'), '-', '-'))
    rows = []
    for a, st, sc0, bl0 in addrs:
        lr = latest(ix, a)
        sc, bl = lr.get('score') or '-', lr.get('blocker') or '-'
        r = analyse(ix, a, top, True)
        rows.append((a, st, sc, bl, r))

    WEAK = ('decl', 'cast', 'ternary')

    def strong(r):
        if not r['sibs']:
            return False
        ev = r['sibs'][0][5]
        if any(e.startswith('same method') for e in ev):
            return True
        for e in ev:
            m = re.match(r'(?:calls|strs) J=([0-9.]+)', e)
            if m and float(m.group(1)) >= 0.4:
                return True
        return False

    def lead(row):
        r = row[4]
        if not r['sibs']:
            return (9, 0, 0)
        best = r['sibs'][0][0]
        cs = r['cands'][0][2] if r['cands'] else []
        hard = sum(1 for k, s, u in cs if u and k not in WEAK)
        soft = sum(1 for k, s, u in cs if u and k in WEAK)
        if strong(r):
            tier = 0 if hard else 1 if soft else 2
        else:
            tier = 3 if (hard or soft) else 4
        return (tier, -best, -hard)
    rows.sort(key=lead)
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with open(out, 'w', encoding='utf-8') as fh:
        w = fh.write
        w('# Exact siblings of the parked / blocked functions\n\n')
        w('Generated by `tools/re/siblings.py --all-parked` (builder SIB, run 2026-11-21). Ranked by how concrete the lead is: '
          'strong sibling = the same method name on another class, or call / string Jaccard >= 0.4. Tier 0: a strong exact sibling and a control-structure '
          'difference (early return / nesting, loops, else, helper calls, accessors) the attempts rows do not mention; 1: only a weak difference '
          '(locals, casts, ?:) untried; 2: strong sibling, every difference mentioned in the attempts; 3-4: weaker siblings (same file / neighbour only); 9: none. '
          '"Untried" is a keyword test of the attempts rows (`attempts.py show ADDR` to confirm), not proof.\n\n')
        w('| tier | address | function | file | size | status | blocker | score | best sibling (score) | evidence | untried forms |\n')
        w('|---|---|---|---|---|---|---|---|---|---|---|\n')
        for a, st, sc, bl, r in rows:
            t = lead((a, st, sc, bl, r))[0]
            if r['sibs']:
                s = r['sibs'][0]
                sib = f'{HEX(s[1])} {short(s[2], 40)} ({s[0]:.1f})'
                ev = '; '.join(s[5])
                alt = ', '.join(f'{HEX(x[1])} {short(x[2], 24)} ({x[0]:.1f})' for x in r['sibs'][1:3])
                if alt:
                    sib += '; also ' + alt
            else:
                sib, ev = 'none', ''
            un = []
            for x, xn, cs in r['cands'][:1]:
                un = [f'[{k}] {s}' for k, s, u in cs if u]
            f = (r['file'] or '?')
            w(f'| {t} | {HEX(a)} | {r["name"] or "?"} | {f} | {r["size"]} | {st} | {bl} | {sc} | {sib} | {ev} | {"<br>".join(un)} |\n')
    have = sum(1 for _, _, _, _, r in rows if r['sibs'] and r['sibs'][0][0] >= 4)
    anyone = sum(1 for _, _, _, _, r in rows if r['sibs'])
    print(f'{len(rows)} functions, {anyone} with any exact sibling, {have} with a strong one (score >= 4); wrote {out}')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[1], formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('addr', nargs='?')
    ap.add_argument('--top', type=int, default=8)
    ap.add_argument('--shape', action='store_true', help="compare source shapes with the best siblings'")
    ap.add_argument('--all-parked', action='store_true')
    ap.add_argument('--status', nargs='+', default=['parked', 'blocked'])
    ap.add_argument('--also', nargs='+', default=[], help='more addresses for --all-parked (e.g. a partial whose notes say parked)')
    ap.add_argument('--out', default=os.path.join(ROOT.split(os.sep + '.work' + os.sep)[0], '.work', 'run-2026-11-21', 'siblings.md'))
    a = ap.parse_args()
    ix = Index()
    if a.all_parked:
        all_parked(ix, a.status, a.top, a.out, [int(x, 16) for x in a.also])
    elif a.addr:
        print_one(ix, int(a.addr, 16), a.top, a.shape)
    else:
        ap.error('an ADDR or --all-parked')


if __name__ == '__main__':
    main()
