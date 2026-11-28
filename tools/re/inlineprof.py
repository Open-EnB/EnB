"""Per-site inline-decision profile of one function: which helpers VC6 inlined where, ours against the client's.

From FB10's prof.py / sites.py (ui_registration 0x5FBD30 / 0x600000, run 2026-11-26; docs/attempts/5FBD30/prof.py.txt,
docs/attempts/600000/sites.py.txt), made general. VC6 weighs each call site's inline decision on its own (a callee's
own source sets its threshold; a goto-reached block is weighed where written: workflow.md §3.1.2), so the residue
of a stuck function does not say which site went the other way; these strings do.

    inlineprof.py profile TARGET... --symbol S --addr ADDR --letters ADDR=L,... [site options] [--obj NAME] [--jobs N]
    inlineprof.py sites OBJ --symbol S --addr ADDR --letters ADDR=L,... [site options]

TARGET: a trybatch batch folder (each form's <form>/obj/<NAME>.obj; --obj NAME when a form has several objects) or
.obj files. Both sides' rows come from trybatch.measure (explain.py's alignment).

The call string: each call whose target --letters names, in layout order, as its letter. Give a helper's own address
one letter (called) and the address its inlined copy calls another (inlined): FB10's T = map::insert 0x6047D0 called,
t = its inner 0x606150 (inlined). A call trybatch keys by name (ours, no symbols.json address) is mapped by --names
PATTERN=ADDR (the first pattern the mangled name contains).

Decision sites (with --site REGEX): each row matching REGEX (its first group, else its first operand, is the decision
register) and followed by a conditional jump, kept when --near REGEX (optional) matches a row within two of it. From
each site, one path per --values value (default `other`): the rows walked with the register holding that value, a
`cmp R, imm` / `test R, R` deciding je / jne / jz / jnz (`other` equals no immediate and is not 0; a `cmp R, reg` counts
as not equal, FB10's rule), jmp followed (a cross-jumped copy still shows what it reaches), any other conditional jump
ending the path as `?`; the first call reached gives the site its letter (`?` when unlettered). The sites string:
the sites' letters in layout order, one string per value, joined by `/`.

profile prints the client's strings, then each form's with its distance to the client's (site by site, then the
calls), residue and raw rows, closest first. sites prints the sites one per line, ours beside the client's, each with
its address and the last three calls before it (to name the site).

FB10's 0x600000 profile, for example:
    inlineprof.py profile .work/trybatch/<batch> --symbol RegistrationInterface::Unknown_5F00C0 --addr 0x600000
        --letters 0x6047D0=T,0x606150=t,0x604BA0=C,0x606980=c,0x5F8A90=9,0x5F89F0=8,0x5F86F0=6,0x5F2990=2
        --site "^cmp (e[acd]x|e[sd]i|ebx), 1$" --near "^mov eax, dword ptr \\[e\\w\\w \\+ 0xd4\\]$" --values other,0x17
"""
import argparse
import difflib
import glob
import os
import re
import sys
from concurrent.futures import ProcessPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

COND = ('je', 'jne', 'jz', 'jnz')
OTHER = 'other'
REG32 = re.compile(r'^e(ax|bx|cx|dx|si|di|bp)$')
UNKNOWN = re.compile(r'Unknown_([0-9A-F]{6})@')


def parse_map(text, value=str):
    """'0x6047D0=T,0x606150=t' -> {0x6047D0: 'T', ...}; 'PAT=0x5F3770' with value=int the other way round."""
    out = {}
    for item in (text or '').split(','):
        if not item.strip():
            continue
        k, _, v = item.rpartition('=')
        if value is str:
            out[int(k, 16)] = v
        else:
            out[k] = int(v, 16)
    return out


def parse_values(text):
    return [OTHER if v.strip() == OTHER else int(v, 0) for v in (text or OTHER).split(',')]


class Profiler:
    def __init__(self, letters, names=None, site=None, near=None, values=(OTHER,), known=None):
        self.letters, self.names = letters, names or {}
        self.site = re.compile(site) if site else None
        self.near = re.compile(near) if near else None
        self.values, self.known = list(values), known or {}

    def key(self, ins):
        import trybatch as TB
        k = TB.call_key(ins, self.known)
        if isinstance(k, str):
            for pat, a in self.names.items():
                if pat in k:
                    return a
            m = UNKNOWN.search(k)
            if m:
                return int(m.group(1), 16)          # an Unknown_XXXXXX name carries its address
        return k

    def label(self, ins):
        k = self.key(ins)
        if isinstance(k, int):
            return self.letters.get(k) or '%X' % k
        return str(k)[:18]

    def calls(self, rows):
        return ''.join(self.letters.get(self.key(ins), '') for ins, _ in rows if ins.mn == 'call')

    def follow(self, rows, index, i, reg, value, limit=80):
        """The first call's key on the path from row i with REG holding VALUE (see the module's doc), or a '?' code."""
        last = None
        for _ in range(limit):
            if i >= len(rows):
                return 'end?'
            ins = rows[i][0]
            ops = [o.strip() for o in ins.op.split(',')] if ins.op else []
            if ins.mn == 'call':
                return self.key(ins)
            if ins.mn == 'cmp' and len(ops) == 2 and ops[0] == reg:
                try:
                    imm = int(ops[1], 0)
                except ValueError:
                    imm = None
                if imm is None and REG32.match(ops[1]):
                    last = 'ne'                     # cmp R, reg: FB10's rule (the other register differs)
                elif imm is None:
                    last = None                     # a memory operand: the next conditional jump ends the path
                else:
                    last = 'eq' if value != OTHER and value == imm else 'ne'
            elif ins.mn == 'test' and ops == [reg, reg]:
                last = 'eq' if value == 0 else 'ne'
            elif ins.mn in COND:
                if last is None:
                    return 'j?'
                taken = (ins.mn in ('jne', 'jnz')) == (last == 'ne')
                if taken:
                    t = ins.target
                    if not t or t[1] not in index:
                        return 'jt?'
                    i = index[t[1]]
                    continue
            elif ins.mn == 'jmp':
                t = ins.target
                if not t or t[1] not in index:
                    return 'jm?'
                i = index[t[1]]
                continue
            elif ins.mn.startswith('j'):
                return 'jx?'
            i += 1
        return 'end?'

    def sites(self, rows):
        """[(address, letters per value, the last three call labels before it)] in layout order."""
        if not self.site:
            return []
        index = {ins.addr: n for n, (ins, _) in enumerate(rows)}
        out, before = [], []
        for n, (ins, _) in enumerate(rows):
            if ins.mn == 'call':
                before.append(self.label(ins))
            m = self.site.search(ins.raw)
            if not m or n + 1 >= len(rows) or rows[n + 1][0].mn not in COND:
                continue
            if self.near and not any(self.near.search(rows[k][0].raw)
                                     for k in range(max(0, n - 2), min(len(rows), n + 3)) if k != n):
                continue
            reg = m.group(1) if m.groups() else ins.op.split(',')[0].strip()
            got = ''
            for v in self.values:
                k = self.follow(rows, index, n, reg, v)
                got += self.letters.get(k, '?') if isinstance(k, int) else '?'
            out.append((ins.addr, got, ' '.join(before[-3:])))
        return out

    def site_string(self, rows):
        s = self.sites(rows)
        return '/'.join(''.join(x[1][k] for x in s) for k in range(len(self.values))) if s else ''


def dist(s, client):
    """Rows to change to make s the client's string (difflib opcodes, the longer side of each)."""
    return sum(max(a2 - a1, b2 - b1) for t, a1, a2, b1, b2 in
               difflib.SequenceMatcher(None, s, client, autojunk=False).get_opcodes() if t != 'equal')


def rows_of(obj, symbol, addr):
    import trybatch as TB
    import explain as X
    known = X.symbol_addresses()
    mangled, _ = TB.resolve_symbol(obj, symbol, addr, ())
    m = TB.measure(obj, mangled, addr, known)
    L, R, _, _ = m['_rows']
    return L, R, m, known


def profile_one(job):
    """(form, our sites, client sites, our calls, client calls, res, raw) of one object (a process pool's job)."""
    form, obj, symbol, addr, opts = job
    try:
        L, R, m, known = rows_of(obj, symbol, addr)
    except (Exception, SystemExit) as ex:  # noqa: B902 (explain raises SystemExit on a symbol it cannot pick)
        return form, 'ERR ' + str(ex)[:60], '', '', '', -1, -1
    p = Profiler(known=known, **opts)
    return form, p.site_string(L), p.site_string(R), p.calls(L), p.calls(R), m['res'], m['raw']


def targets(paths, obj_name=None):
    """[(form, object path)]: each .obj given, each form of a trybatch batch folder."""
    out = []
    for t in paths:
        if os.path.isfile(t):
            out.append((os.path.splitext(os.path.basename(t))[0], t))
            continue
        for form in sorted(os.listdir(t)):
            objs = sorted(glob.glob(os.path.join(t, form, 'obj', (obj_name or '*') + ('' if obj_name and
                                                                                         obj_name.endswith('.obj') else '.obj'))))
            if len(objs) == 1:
                out.append((form, objs[0]))
            elif len(objs) > 1:
                raise SystemExit(f'{form}: several objects ({len(objs)}): give --obj NAME')
    return out


def profile_lines(results):
    if not results:
        return ['no forms']
    cs = next((r[2] for r in results if r[2]), '')
    cc = next((r[4] for r in results if r[4]), '')
    lines = ['%-16s %s  %s' % ('CLIENT', cs, cc)]
    ranked = sorted(results, key=lambda r: (dist(r[1], cs) + dist(r[3], cc), r[5]))
    for form, s, _, c, _, res, raw in ranked:
        lines.append('%-16s %s  %s  d=%d+%d res=%d raw=%d' % (form[:16], s, c, dist(s, cs), dist(c, cc), res, raw))
    return lines


def sites_lines(p, L, R):
    so, sc = p.sites(L), p.sites(R)
    lines = ['ours %d sites, client %d (values %s)' % (len(so), len(sc), ','.join(map(str, p.values)))]
    blank = ('', '', '')
    for i in range(max(len(so), len(sc))):
        o = so[i] if i < len(so) else blank
        c = sc[i] if i < len(sc) else blank
        lines.append('%2d  %6s %s %-34s | %6s %s %s' % (i + 1, ('%X' % o[0]) if o[0] != '' else '', o[1], o[2][:34],
                                                         ('%X' % c[0]) if c[0] != '' else '', c[1], c[2][:34]))
    return lines


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('mode', choices=('profile', 'sites'))
    ap.add_argument('target', nargs='+', help='profile: batch folders or .obj files; sites: one .obj')
    ap.add_argument('--symbol', required=True)
    ap.add_argument('--addr', required=True)
    ap.add_argument('--letters', default='', help='ADDR=L,...: a call target\'s letter (client addresses)')
    ap.add_argument('--names', default='', help='PATTERN=ADDR,...: a call keyed by name, by the pattern it contains')
    ap.add_argument('--site', help='regex of a decision site\'s row (group 1: the register)')
    ap.add_argument('--near', help='regex one of the two rows before / after a site must match')
    ap.add_argument('--values', default=OTHER, help='the register\'s values, one path each (other,0x17)')
    ap.add_argument('--obj', help='the object name inside each form\'s obj/ folder')
    ap.add_argument('--jobs', type=int, default=6)
    a = ap.parse_args(argv)
    opts = dict(letters=parse_map(a.letters), names=parse_map(a.names, int), site=a.site, near=a.near,
                values=parse_values(a.values))
    addr = int(a.addr, 16)
    if a.mode == 'sites':
        L, R, _, known = rows_of(a.target[0], a.symbol, addr)
        print('\n'.join(sites_lines(Profiler(known=known, **opts), L, R)))
        return
    jobs = [(form, obj, a.symbol, addr, opts) for form, obj in targets(a.target, a.obj)]
    with ProcessPoolExecutor(max_workers=max(1, a.jobs)) as ex:
        results = list(ex.map(profile_one, jobs))
    print('\n'.join(profile_lines(results)))


if __name__ == '__main__':
    main()
