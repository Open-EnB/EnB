"""Thunk-order name check: is symbols.json's decorated name of a thunked function retail's exact name?

    python tools/re/thunkname.py [--out .work/im3] [--objs 'C:/EnBDevelopment/EnB/build/*/obj/*.obj'] [--near 3]

VC6 LINK /INCREMENTAL writes the thunk table at the start of .text in the order of its external-symbol hash table
(docs/binary.md §5.1; linklayout.lhash / bucket, maxp 8192, split p fitted: 6,856 for retail, 15,048 buckets).
The named slots whose buckets lie on the longest non-decreasing run are the anchors (a name there is retail's exact
decorated name: linklayout.name_check). Every other slot lies between two anchors, so its real name hashes into the
bucket window between them, and, since thunks fill the buckets about evenly, near the bucket interpolated by its
slot position (`near`: within --near buckets of it; 94.7 % of retail's anchors, each left out in turn, lie within 3):

    on            the slot's name lies on the order (an anchor, or an alias ??_G / ??_E that fits)
    off           it does not; window = [bucket of the anchor before, bucket of the anchor after]
    off-conflict  off, but our bucket fits once one of the two bounding anchors is dropped (one of the two is wrong)
    appended      the incremental update's last --tail slots (not in hash order)

For an off name without "Unknown" the usual corrections of a decorated name are tested (variants()): ??_G / ??_E,
access (Q / A / I, U / E / M), virtual, static, const (this cv A <-> B), calling convention (a member's __cdecl only
with an ellipsis), a parameter's or the return type's integer / char / float kind (H / I / J / K / _N, D / E / C,
F / G, M / N), a pointer's or reference's cv (PAV <-> PBV), pointer <-> reference, class <-> struct (V <-> U), the
case of the first identifiers. A deleting destructor's signature is the compiler's: only access, virtual, ??_G / ??_E
and case. A variant in the near part is a hit. A wrong variant lands there by chance with about near / 15,048 per
variant, and the hash is not mixed at the name's tail (an int edit of the last parameter moves the bucket by a
median 53), so each kind's count comes with a null: the same variants against the window moved to a random place
(--null times, averaged; this keeps each variant's distance from our bucket). Kinds whose hits reach twice their
null are `proposed` (retail 2026-11-25: access, const, int, pointer <-> reference, pointer cv); the others are at
chance (case, ret, callconv, virtual, static, ??_G / ??_E, class key).

Output in --out: namecheck.tsv (every named slot: slot, target, our name, symbols.json source, state, bucket,
window, width, near part, the near and the window hits), proposals.tsv (each off name with exactly one near hit:
kind, proposed, name_only (no code change expected), the kind's precision 1 - null / hits, class_consistent (the
same change confirmed at another slot of the class), ours (--objs: the corrected name already in our objects),
the buckets and both names); summary on stdout. It never edits symbols.json or the source: a proposal is applied by
a builder, in the source declaration (symbols.json's `exact` names follow the objects), with a compile.
"""
import argparse
import bisect
import collections
import json
import math
import os
import random
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import linklayout  # noqa: E402

REPO = linklayout.REPO

# ---- a decorated-name parser: enough of MSVC's grammar to find the pieces the corrections change ----------------


class Bad(Exception):
    pass


PRIM1 = set('CDEFGHIJKMNOXZ')
FUNC_CODES = {c: k for k, cs in (('member', 'AEIMQU'), ('static', 'CKS'), ('global', 'Y')) for c in cs}


class Parser:
    def __init__(self, s):
        self.s, self.i = s, 0

    def peek(self, n=1):
        return self.s[self.i:self.i + n]

    def take(self, n=1):
        if self.i + n > len(self.s):
            raise Bad('end')
        t = self.s[self.i:self.i + n]
        self.i += n
        return t

    def ident(self):
        j = self.s.find('@', self.i)
        if j < 0:
            raise Bad('ident')
        t = self.s[self.i:j]
        self.i = j + 1
        return t

    def number(self):
        if self.peek().isdigit():
            self.take()
            return
        if self.peek() == '?':
            self.take()
        while self.peek() and self.peek() in 'ABCDEFGHIJKLMNOP':
            self.take()
        if self.take() != '@':
            raise Bad('number')

    def template_name(self):
        # after '?$': name '@' then template arguments until '@'
        if self.peek() == '?':
            self.take()
            self.special()
        else:
            self.ident()
        while self.peek() != '@':
            self.template_arg()
        self.take()

    def template_arg(self):
        if self.peek(2) == '$0':
            self.take(2)
            self.number()
        elif self.peek(2) in ('$1', '$E'):
            self.take(2)
            self.symbol_ref()
        elif self.peek(3) == '$$$':
            raise Bad('template arg')
        else:
            self.type()

    def symbol_ref(self):
        if self.take() != '?':
            raise Bad('symbol ref')
        p = Parser(self.s)
        p.i = self.i - 1
        p.function(nested=True)
        self.i = p.i

    def special(self):
        c = self.take()
        if c == '_':
            self.take()
        elif c == '$':
            self.template_name()

    def qualname(self):
        """Fragments until the terminating '@'."""
        while self.peek() != '@':
            if not self.peek():
                raise Bad('qualname')
            if self.peek().isdigit():
                self.take()
            elif self.peek(2) == '?$':
                self.take(2)
                self.template_name()
            elif self.peek() == '?':
                raise Bad('nested ?')
            else:
                self.ident()
        self.take()

    def type(self):
        """One type; returns its (start, end) span."""
        a = self.i
        c = self.take()
        if c in PRIM1 or c.isdigit():
            pass
        elif c == '_':
            self.take()
        elif c in 'PQRS':
            if self.peek() == '6':
                self.take()
                self.functype()
            elif self.peek() == '8':
                self.take()
                self.qualname()
                self.take()  # cv
                self.functype()
            else:
                self.take()  # cv
                self.type()
        elif c in 'AB':
            self.take()
            self.type()
        elif c in 'TUV':
            self.qualname()
        elif c == 'W':
            self.take()
            self.qualname()
        elif c == '?':
            self.take()
            self.type()
        elif c == '$':
            if self.peek(2) in ('$C', '$B', '$A'):
                k = self.take(2)
                if k == '$C':
                    self.take()
                elif k == '$A':
                    if self.take() != '6':
                        raise Bad('$$A')
                    self.functype()
                    return a, self.i
                self.type()
            else:
                raise Bad('$ type')
        elif c == 'Y':
            raise Bad('array')
        else:
            raise Bad(f'type {c}')
        return a, self.i

    def functype(self):
        self.take()  # calling convention
        if self.peek() == '@':
            self.take()
        else:
            self.type()
        self.arglist()
        if self.take() != 'Z':
            raise Bad('throw')

    def arglist(self):
        spans = []
        if self.peek() == 'X':
            self.take()
            return spans, 'X'
        while True:
            if self.peek() == '@':
                self.take()
                return spans, '@'
            if self.peek() == 'Z':
                self.take()
                return spans, 'Z'
            spans.append(self.type())

    def function(self, nested=False):
        """The whole decorated function name: a dict of spans, or Bad (nested: a template argument's symbol,
        which may be data: storage class digit, type, cv)."""
        if self.take() != '?':
            raise Bad('not decorated')
        name_a = self.i
        if self.peek() == '?':
            self.take()
            self.special()
        elif self.peek(2) == '$':
            raise Bad('template function')
        else:
            self.ident()
        first_end = self.i
        self.qualname()
        name_b = self.i
        code_at = self.i
        code = self.take()
        if nested and code in '01234':
            self.type()
            self.take()
            return None
        kind = FUNC_CODES.get(code)
        if kind is None:
            raise Bad(f'function code {code}')
        cv_at = None
        if kind == 'member':
            cv_at = self.i
            self.take()
        conv_at = self.i
        self.take()
        ret = None
        if self.peek() == '@':
            self.take()
        else:
            ret = self.type()
        args, end = self.arglist()
        if self.take() != 'Z':
            raise Bad('throw')
        if self.i != len(self.s):
            raise Bad('trailing')
        return dict(name=(name_a, name_b), first=(name_a, first_end), code=code_at, kind=kind, cv=cv_at,
                    conv=conv_at, ret=ret, args=args, end=end)


def parse(name):
    """The pieces of a decorated function name, or None when the subset grammar does not cover it."""
    try:
        return Parser(name).function()
    except (Bad, IndexError):
        return None


# ---- the corrections -----------------------------------------------------------------------------------------

SWAPS = {
    'H': ['I', 'J', 'K', '_N'], 'I': ['H', 'K', 'J'], 'J': ['H', 'K', 'I'], 'K': ['I', 'J', 'H'],
    '_N': ['H', 'E'], 'D': ['E', 'C'], 'E': ['D', 'C', '_N'], 'C': ['D', 'E'], 'F': ['G', 'H'], 'G': ['F', 'K'],
    'M': ['N'], 'N': ['M'],
}
ACCESS = {'A': 'AIQ', 'I': 'AIQ', 'Q': 'AIQ', 'E': 'EMU', 'M': 'EMU', 'U': 'EMU', 'C': 'CKS', 'K': 'CKS', 'S': 'CKS'}
VIRTUAL = {'A': 'E', 'I': 'M', 'Q': 'U', 'E': 'A', 'M': 'I', 'U': 'Q'}
STATIC = {'A': 'C', 'I': 'K', 'Q': 'S', 'C': 'A', 'K': 'I', 'S': 'Q'}
# kinds whose change leaves the code bytes alone (a name-only fix); the others can change code (or callers')
NAME_ONLY = {'gE', 'access', 'const', 'ptrcv', 'ptrref', 'classkey', 'case'}


def type_edits(t):
    """(kind, new text) edits of one type's text (its outer level, or the pointee of a pointer / reference)."""
    out = []
    if t in SWAPS:
        out += [('int', x) for x in SWAPS[t]]
        return out
    if len(t) >= 3 and t[0] in 'PQRSAB' and t[1] in 'ABCD' and t[2] not in '68':
        head, cv, inner = t[0], t[1], t[2:]
        for x in 'AB':
            if x != cv:
                out.append(('ptrcv', head + x + inner))
        if head == 'P':
            out.append(('ptrref', 'A' + cv + inner))
        elif head == 'A':
            out.append(('ptrref', 'P' + cv + inner))
        if inner in SWAPS:
            out += [('int', head + cv + x) for x in SWAPS[inner]]
        if inner[0] in 'UV' and not inner[1:2].isdigit():
            out.append(('classkey', head + cv + ('U' if inner[0] == 'V' else 'V') + inner[1:]))
    if t[0] == '?' and len(t) > 2 and t[2] in 'UV':
        out.append(('classkey', t[:2] + ('U' if t[2] == 'V' else 'V') + t[3:]))
    return out


def arg_table_ok(args):
    """A changed argument list keeps MSVC's backreference rule: no multi-character argument spelled out twice
    (the second would have been a digit), every digit within the table."""
    table = []
    for a in args:
        if len(a) == 1 and a.isdigit():
            if int(a) >= len(table):
                return False
            continue
        if len(a) > 1:
            if a in table:
                return False
            if len(table) < 10:
                table.append(a)
    return True


def variants(name):
    """[(kind, variant)] of a decorated function name (no duplicates, the name itself excluded)."""
    out = []
    if name.startswith(('??_G', '??_E')):
        out.append(('gE', ('??_E' if name.startswith('??_G') else '??_G') + name[4:]))
    p = parse(name)
    if p is None:
        return dedup(name, out)
    s = name

    def put(at, old_len, new):
        return s[:at] + new + s[at + old_len:]
    code = s[p['code']]
    for c in ACCESS.get(code, ''):
        if c != code:
            out.append(('access', put(p['code'], 1, c)))
    ctor = s.startswith('??0')
    if code in VIRTUAL and not ctor:
        out.append(('virtual', put(p['code'], 1, VIRTUAL[code])))
    if p['kind'] == 'member' and code in STATIC and not s.startswith(('??0', '??1', '??_')):
        # member -> static: the this cv goes, the convention becomes cdecl
        out.append(('static', s[:p['code']] + STATIC[code] + 'A' + s[p['conv'] + 1:]))
    if p['kind'] == 'static':
        for cv in 'AB':
            out.append(('static', s[:p['code']] + STATIC[code] + cv + 'E' + s[p['conv'] + 1:]))
    if p['cv'] is not None and not s.startswith(('??0', '??1', '??_G', '??_E')):
        cv = s[p['cv']]
        for x in 'AB':
            if x != cv:
                out.append(('const', put(p['cv'], 1, x)))
    # a deleting destructor's signature is the compiler's (PAXI); a member is __cdecl only with an ellipsis
    fixed = s.startswith(('??_G', '??_E'))
    conv = s[p['conv']]
    convs = '' if fixed else ('EG' + ('A' if p['end'] == 'Z' else '')) if p['kind'] == 'member' else 'AGI'
    for x in convs:
        if x != conv:
            out.append(('callconv', put(p['conv'], 1, x)))
    if p['ret'] is not None and not fixed:
        a, b = p['ret']
        for k, t in type_edits(s[a:b]):
            out.append(('ret', put(a, b - a, t)))
    args = [s[a:b] for a, b in p['args']]
    for k, (a, b) in enumerate([] if fixed else p['args']):
        t = s[a:b]
        later = args[k + 1:]
        for kind, t2 in type_edits(t):
            new_args = args[:k] + [t2] + later
            if len(t) > 1 and any(x.isdigit() and len(x) == 1 for x in later):
                continue  # a later backreference may point at this argument: its spelling would change too
            if not arg_table_ok(new_args):
                continue
            out.append((kind, put(a, b - a, t2)))
    # the case of the first identifier (the method's or the class's name) and of the class name
    fa, fb = p['first']
    ident = s[fa:fb - 1] if not s.startswith('??') else None
    idents = []
    if ident and ident[:1].isalpha():
        idents.append((fa, ident))
    m = re.match(r'\?\?(?:_[A-Z]|[0-9A-Z])([A-Za-z_][A-Za-z0-9_]*)@', s) if s.startswith('??') else None
    if m:
        idents.append((m.start(1), m.group(1)))
    elif ident is not None and fb < len(s) and re.match(r'[A-Za-z_][A-Za-z0-9_]*@', s[fb:]):
        cls = s[fb:s.index('@', fb)]
        idents.append((fb, cls))
    for at, word in idents:
        for w in {word[0].swapcase() + word[1:], word.lower(), word.upper()}:
            if w != word:
                # a class's name changes everywhere it is spelled out
                out.append(('case', s[:at] + w + s[at + len(word):].replace(word + '@', w + '@')
                            if at != fa or s.startswith('??') else s[:at] + w + s[at + len(word):]))
    return dedup(name, out)


def dedup(name, out):
    seen, res = {name}, []
    for k, v in out:
        if v not in seen:
            seen.add(v)
            res.append((k, v))
    return res


# ---- the windows ---------------------------------------------------------------------------------------------

R_NEAR = 3  # buckets around the slot's interpolated bucket: 94.7 % of retail's anchors, left out one at a time


def anchor_windows(slots, anchors, r_near=R_NEAR):
    """slots: slot indexes to place; anchors: sorted [(slot index, bucket)]. -> {slot index: None (no anchor on
    one side) or (lo, hi, near_lo, near_hi)}: the buckets between the nearest anchors before and after, and the
    part within r_near of the bucket interpolated by slot position (thunks fill the buckets about evenly)."""
    ai = [i for i, _ in anchors]
    out = {}
    for i in slots:
        j = bisect.bisect_left(ai, i)
        if j < len(ai) and ai[j] == i:
            b = anchors[j][1]
            out[i] = (b, b, b, b)
        elif j == 0 or j == len(ai):
            out[i] = None
        else:
            (i0, b0), (i1, b1) = anchors[j - 1], anchors[j]
            x = b0 + (i - i0) * (b1 - b0) / (i1 - i0)
            out[i] = (b0, b1, max(b0, math.ceil(x - r_near)), min(b1, math.floor(x + r_near)))
    return out


def class_of(name):
    """The qualifying part of a decorated name after its first fragment (Class@@, Class@Outer@@), or ''."""
    p = parse(name)
    return name[p['first'][1]:p['name'][1]] if p else ''


def edit_key(kind, variant):
    """What a correction changes, for the per-class consistency count: the function type code, this cv and
    convention for those kinds, else the kind alone."""
    p = parse(variant)
    if p and kind in ('access', 'virtual', 'static', 'const', 'callconv'):
        return kind, variant[p['code']:p['conv'] + 1]
    return kind, ''


def std_member(name):
    """A function of a std:: class (template): its decorated signature is the library header's, not ours."""
    p = parse(name)
    return bool(p) and name[p['first'][1]:p['name'][1]].endswith('@std@@')


def wrap_in(b, lo, hi):
    return lo <= b <= hi if lo <= hi else (b >= lo or b <= hi)


def check(table, names_of, sources, maxp=8192, p=None, tail=0, null=20, seed=1, r_near=R_NEAR):
    """The name check over the thunk table. Returns (p, nbuckets, rows, null counts {(window|near, kind)}).
    rows: dicts slot, target, name, source, on, window (lo, hi, near_lo, near_hi) or None, width, hits
    [(kind, variant)] in the window, near [(kind, variant)] in its near part, appended, bucket, class_consistent."""
    p, rows_ = linklayout.name_check(table, names_of, maxp, p)
    nb = maxp + p
    idx = {s: i for i, (s, _) in enumerate(table)}
    n = len(table)
    # anchors: on-order slots with one name (name_check's LNDS), the appended tail excluded
    single = {s for s, t in table if len(names_of.get(t, [])) == 1}
    anchors = sorted((idx[s], b) for s, t, nm, b, on in rows_ if on and s in single and idx[s] < n - tail)
    named = {s: (t, nm, b, on) for s, t, nm, b, on in rows_}
    win = anchor_windows(range(n), anchors, r_near)
    aset = {i for i, _ in anchors}
    ai = [i for i, _ in anchors]
    out = []
    for i, (s, t) in enumerate(table):
        nm = named.get(s)
        w = win[i]
        out.append(dict(slot=s, target=t, name=nm[1] if nm else '', source=sources.get(t, ''), on=bool(nm and nm[3]),
                        window=w, width=(w[1] - w[0] + 1) if w else None, hits=[], near=[], appended=i >= n - tail,
                        bucket=nm[2] if nm else None, class_consistent=False,
                        tight=i - 1 in aset and i + 1 in aset, conflict=False))
        if nm and not nm[3] and w:
            # our bucket fits between the second anchor before and the first after, or the first before and the
            # second after: one anchor disagrees with our name
            j = bisect.bisect_left(ai, i)
            lo2 = anchors[j - 2][1] if j >= 2 else 0
            hi2 = anchors[j + 1][1] if j + 1 < len(anchors) else nb
            out[-1]['conflict'] = (lo2 <= nm[2] <= w[1]) or (w[0] <= nm[2] <= hi2)
    rng = random.Random(seed)
    nulls = collections.Counter()
    for r in out:
        if not r['name'] or r['on'] or r['appended'] or r['window'] is None:
            continue
        if std_member(r['name']):
            r['std'] = True
            continue
        vs = variants(r['name'])
        r['nvariants'] = len(vs)
        lo, hi, nlo, nhi = r['window']
        bs = [(k, v, linklayout.bucket(linklayout.lhash(v), maxp, p)) for k, v in vs]
        r['hits'] = [(k, v) for k, v, b in bs if lo <= b <= hi]
        r['near'] = [(k, v) for k, v, b in bs if nlo <= b <= nhi]
        for _ in range(null):
            sh = rng.randrange(nb)
            for k, v, b in bs:
                if wrap_in(b, (lo + sh) % nb, (hi + sh) % nb):
                    nulls['window', k] += 1.0 / null
                if wrap_in(b, (nlo + sh) % nb, (nhi + sh) % nb):
                    nulls['near', k] += 1.0 / null
    # per-class consistency: one class, one change, confirmed (near, the only one) at two or more of its slots
    keyed = collections.Counter()
    for r in out:
        if len(r['near']) == 1:
            r['cls'] = class_of(r['name'])
            if r['cls']:
                keyed[(r['cls'],) + edit_key(*r['near'][0])] += 1
    for r in out:
        if r.get('cls'):
            r['class_consistent'] = keyed[(r['cls'],) + edit_key(*r['near'][0])] >= 2
    return p, nb, out, nulls


def obj_names(obj_glob):
    """Every external symbol name our objects define or reference (to tell whether a corrected name is ours)."""
    import glob
    import objmatch
    out = set()
    for f in glob.glob(obj_glob):
        try:
            o = objmatch.CoffObject(f)
        except Exception:
            continue
        out.update(s['name'] for s in o.symbols if s['class'] == 2)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--client', default=linklayout.clientimage.DEFAULT_CLIENT)
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'im3'))
    ap.add_argument('--maxp', type=int, default=8192)
    ap.add_argument('--split', type=int, help='the split pointer p (default: fitted)')
    ap.add_argument('--tail', type=int, default=7, help='slots the incremental update appended (retail: 7)')
    ap.add_argument('--null', type=int, default=20, help='random window placements for the null count')
    ap.add_argument('--near', type=int, default=R_NEAR, help='buckets around the interpolated bucket (3)')
    ap.add_argument('--objs', help='our objects (glob): mark corrected names our objects already define or use')
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    ci = linklayout.clientimage.ClientImage(a.client)
    ts, te, raw = ci.section('.text')
    body = ci.data[raw:raw + te - ts]
    table = linklayout.thunk_table(body, ts)
    names_of, sources = {}, {}
    for s_ in json.load(open(linklayout.SYMBOLS)):
        if s_.get('kind') == 'function':
            va = int(s_['address'], 16)
            if s_['symbol'] not in names_of.setdefault(va, []):
                names_of[va].append(s_['symbol'])
            sources.setdefault(va, s_.get('source', ''))
    for va, ns in names_of.items():
        for nm in list(ns):
            if nm.startswith(('??_G', '??_E')):
                twin = ('??_E' if nm.startswith('??_G') else '??_G') + nm[4:]
                if twin not in ns:
                    ns.append(twin)
    p, nb, rows, nulls = check(table, names_of, sources, a.maxp, a.split, a.tail, a.null, r_near=a.near)
    ours = obj_names(a.objs) if a.objs else None
    report(rows, nulls, p, nb, a, ours)
    return 0


def report(rows, nulls, p, nb, a, ours):
    unk = lambda r: 'Unknown' in r['name']
    named = [r for r in rows if r['name'] and not r['appended']]
    print(f'thunk table: {len(rows):,} slots, {len(named):,} named (the {a.tail} appended slots apart); split p {p}, '
          f'{nb:,} buckets; near = within {a.near} buckets of the bucket interpolated by slot position')

    def widths(rs):
        w = sorted(r['width'] for r in rs)
        nw = sorted(r['window'][3] - r['window'][2] + 1 for r in rs)
        mid = nw[len(nw) // 2] if nw else 0
        return (f'width 1 {sum(x == 1 for x in w):,}, <= 4 {sum(x <= 4 for x in w):,}, <= 16 {sum(x <= 16 for x in w):,}, '
                f'<= 64 {sum(x <= 64 for x in w):,}, median {w[len(w) // 2] if w else 0}; near part median {mid} '
                f'(a random name lands in it {100.0 * mid / nb:.3f} %)')
    for label, sel in (('names without "Unknown"', lambda r: not unk(r)), ('"Unknown" names', unk)):
        rs = [r for r in named if sel(r)]
        off = [r for r in rs if not r['on']]
        tw = [r for r in off if r['window']]
        print(f'{label}: on {sum(r["on"] for r in rs):,}, off {len(off):,} (both adjacent slots anchors '
              f'{sum(r["tight"] for r in off):,}; our bucket fits without one bounding anchor {sum(r["conflict"] for r in off)}); '
              f'off with anchors on both sides {len(tw):,}: {widths(tw)}')
    unnamed = [r for r in rows if not r['name'] and not r['appended'] and r['window']]
    print(f'unnamed slots with anchors on both sides {len(unnamed):,}: {widths(unnamed)}')
    tested = [r for r in named if not r['on'] and r['window'] and not unk(r) and not r.get('std')]
    hits = collections.Counter(k for r in tested for k, _ in r['hits'])
    near = collections.Counter(k for r in tested for k, _ in r['near'])
    one = [r for r in tested if len(r['near']) == 1]
    print(f'corrections tested on {len(tested):,} off names without "Unknown" (std:: members apart: '
          f'{sum(1 for r in named if r.get("std") and not unk(r)):,}) '
          f'({sum(r.get("nvariants", 0) for r in tested):,} variants; names the grammar does not cover '
          f'{sum(1 for r in tested if parse(r["name"]) is None):,}); names with a window hit '
          f'{sum(1 for r in tested if r["hits"]):,}, with a near hit {sum(1 for r in tested if r["near"]):,}, '
          f'exactly one near hit {len(one):,} (class-consistent {sum(r["class_consistent"] for r in one)})')
    print('  by kind: window hits / null, near hits / null, excess (null: the count expected by chance)')
    prec = {}
    for k in sorted(set(hits) | {k for _, k in nulls}, key=lambda k: -near[k]):
        prec[k] = max(0.0, 1 - nulls['near', k] / near[k]) if near[k] else 0.0
        print(f'    {k:9s} {hits[k]:5d} / {nulls["window", k]:6.1f}   {near[k]:5d} / {nulls["near", k]:6.1f}'
              f'   {near[k] - nulls["near", k]:6.1f}')
    with open(os.path.join(a.out, 'namecheck.tsv'), 'w') as f:
        f.write('slot\ttarget\tname\tsource\ton_order\tbucket\twindow\twidth\tnear\tcorrections_near\tcorrections_window\n')
        for r in rows:
            if not r['name']:
                continue
            w = r['window']
            state = 'appended' if r['appended'] else 'on' if r['on'] else 'off-conflict' if r['conflict'] else 'off'
            f.write(f"0x{r['slot']:X}\t0x{r['target']:X}\t{r['name']}\t{r['source']}\t{state}\t{r['bucket']}\t"
                    f"{f'{w[0]}-{w[1]}' if w else ''}\t{r['width'] or ''}\t{f'{w[2]}-{w[3]}' if w else ''}\t"
                    f"{' '.join(f'{k}:{v}' for k, v in r['near'])}\t{' '.join(f'{k}:{v}' for k, v in r['hits'])}\n")
    with open(os.path.join(a.out, 'proposals.tsv'), 'w') as f:
        f.write('target\tslot\tkind\tproposed\tname_only\tkind_precision\tclass_consistent\tsource\tours\tbucket\twindow\tnear'
                '\tour_bucket\tour_name\tcorrected\n')
        for r in sorted(one, key=lambda r: (-prec.get(r['near'][0][0], 0), not r['class_consistent'], r['target'])):
            k, v = r['near'][0]
            b = linklayout.bucket(linklayout.lhash(v), a.maxp, p)
            o = '' if ours is None else ('yes' if v in ours else 'no')
            w = r['window']
            f.write(f"0x{r['target']:X}\t0x{r['slot']:X}\t{k}\t{'yes' if prec.get(k, 0) >= 0.5 else 'no'}\t"
                    f"{'yes' if k in NAME_ONLY else 'no'}\t{prec.get(k, 0):.2f}\t"
                    f"{'yes' if r['class_consistent'] else 'no'}\t{r['source']}\t{o}\t{b}\t{w[0]}-{w[1]}\t{w[2]}-{w[3]}\t"
                    f"{r['bucket']}\t{r['name']}\t{v}\n")
    print(f'tables: {os.path.join(a.out, "namecheck.tsv")}, {os.path.join(a.out, "proposals.tsv")}')


if __name__ == '__main__':
    sys.exit(main())
