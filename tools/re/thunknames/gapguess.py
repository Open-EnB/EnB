"""Gap guesser: test whole decorated names against each thunk slot's window, with the cascade (exploratory;
docs/binary.md 5.1-5.2, NOTES.md "informed guesses per gap").

The state is the full thunk table (every slot, unnamed ones included; the incremental update's last --tail
slots excluded) with the current name per slot. Anchors = the longest non-decreasing bucket run over the named
slots; every other slot's window = [bucket of the anchor before, bucket of the anchor after], and its near part
= within R_NEAR buckets of the bucket interpolated by slot position (thunkname.anchor_windows).

A guess is (slot, name, tier, note). It hits when its bucket lies in the slot's window ('window') or near part
('near'). Accepted hits become names, the anchors are recomputed (each hit extends the run by one and narrows its
neighbours' windows), and the remaining guesses are re-tested against the narrower windows, until nothing changes.
Every hit records its evidence: the expected false hits for its slot (guesses tested there x width / buckets),
whether the name's identifiers occur in client.exe, and how many other slots of the same class qualifier hit in
the same run (group).

Usage:
  python tools/re/thunknames/gapguess.py state [--state F]                 # summary of the current state
  python tools/re/thunknames/gapguess.py test GUESSES.tsv [--state F] [--out DIR] [--accept near|window] [--max-chance X]
GUESSES.tsv: slot, name, tier, note (header row). Writes DIR/hits.tsv and DIR/state.tsv (the new state).
The default state is docs/data/thunk-names.tsv (paths.STATE).
"""
import argparse
import bisect
import collections
import csv
import math
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (also puts tools/re on sys.path)
from linklayout import lhash, bucket, lnds_members, thunk_table, pe_text  # noqa: E402

MAXP, P = 8192, 6856
NB = MAXP + P
R_NEAR = 3
TAIL = 7
CLIENT_EXE = paths.CLIENT_EXE
DEFAULT_STATE = paths.STATE
IDENT = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')


def bk(name):
    return bucket(lhash(name), MAXP, P)


class State:
    def __init__(self, path=DEFAULT_STATE, revert_construct=True):
        body, base = pe_text(CLIENT_EXE)
        self.table = thunk_table(body, base)
        self.n = len(self.table)
        self.index = {s: i for i, (s, _) in enumerate(self.table)}
        self.names = [None] * self.n
        self.how = [''] * self.n
        self.orig = [None] * self.n
        for r in csv.DictReader(open(path), delimiter='\t'):
            i = self.index[int(r['slot'], 16)]
            nm = r['name']
            how = r.get('how', '') or ''
            if revert_construct and '_Construct->' in how and r.get('original'):
                nm, how = r['original'], ''
            self.names[i] = nm
            self.how[i] = how
            self.orig[i] = r.get('original') or None
        self.recompute()

    def recompute(self):
        lim = self.n - TAIL
        named = [(i, bk(self.names[i])) for i in range(lim) if self.names[i]]
        self.b = dict(named)
        on = lnds_members([b for _, b in named])
        self.anchors = [named[k] for k in on]
        self.onset = {i for i, _ in self.anchors}
        self.ai = [i for i, _ in self.anchors]

    def window(self, i):
        """(lo, hi, near_lo, near_hi) or None (an anchor, outside the anchors, or the appended tail)."""
        if i >= self.n - TAIL or i in self.onset:
            return None
        j = bisect.bisect_left(self.ai, i)
        if j == 0 or j == len(self.ai):
            return None
        (i0, b0), (i1, b1) = self.anchors[j - 1], self.anchors[j]
        x = b0 + (i - i0) * (b1 - b0) / (i1 - i0)
        return b0, b1, max(b0, math.ceil(x - R_NEAR)), min(b1, math.floor(x + R_NEAR))

    def summary(self):
        lim = self.n - TAIL
        named = sum(1 for i in range(lim) if self.names[i])
        unk = sum(1 for i in range(lim) if self.names[i] and 'Unknown' in self.names[i] and i not in self.onset)
        offn = sum(1 for i in range(lim) if self.names[i] and i not in self.onset and self.window(i))
        unn = sum(1 for i in range(lim) if not self.names[i] and self.window(i))
        ws = sorted(w[1] - w[0] + 1 for w in (self.window(i) for i in range(lim)) if w)
        return (f'slots {self.n} (tail {TAIL}), named {named}, on {len(self.onset)}, off named with window {offn} '
                f'(Unknown {unk}), unnamed with window {unn}, window median {ws[len(ws) // 2] if ws else 0}')

    def write(self, path):
        with open(path, 'w', newline='') as f:
            w = csv.writer(f, delimiter='\t', lineterminator='\n')
            w.writerow(['slot', 'target', 'name', 'original', 'bucket', 'on_order', 'how'])
            for i, (s, t) in enumerate(self.table):
                if self.names[i] is None and not self.how[i]:
                    continue
                w.writerow([f'0x{s:X}', f'0x{t:X}', self.names[i] or '', self.orig[i] or '',
                            self.b.get(i, ''), 'on' if i in self.onset else 'off', self.how[i]])


from mangle import class_of  # noqa: E402,F401  (re-exported: the other tools import it from here)


def twins(name):
    """A deleting destructor is named both ??_G and ??_E; test both."""
    if name.startswith(('??_G', '??_E')):
        return [name, ('??_E' if name.startswith('??_G') else '??_G') + name[4:]]
    return [name]


_IDS = None


def client_ids():
    global _IDS
    if _IDS is None:
        data = open(CLIENT_EXE, 'rb').read()
        _IDS = {m.group().decode() for m in re.finditer(rb'[A-Za-z_][A-Za-z0-9_]{3,}', data)}
        _IDS |= {m.group().decode('utf-16-le') for m in re.finditer(rb'(?:[A-Za-z_]\x00)(?:[A-Za-z0-9_]\x00){3,}', data)}
    return _IDS


def name_ids(name):
    """The source identifiers of a decorated name: method and qualifiers (and class names in parameter types)."""
    m = re.match(r'\?\?(?:0|1|_G|_E|_7|4)(.*)', name)
    head = m.group(1) if m else name.lstrip('?')
    out = []
    quals, _, args = head.partition('@@')
    out += [q.lstrip('?$') for q in quals.split('@') if q]
    out += re.findall(r'(?<![A-Z])[VUW]4?([A-Za-z_][A-Za-z0-9_]*)@@', args)
    return [x for x in out if IDENT.fullmatch(x)]


def in_client(name):
    """Every identifier of the name longer than 3 characters, except std parts, occurs in client.exe as a whole
    identifier (ASCII or UTF-16)."""
    ids = [x for x in name_ids(name) if len(x) > 3 and x not in ('std', 'allocator', 'char_traits')]
    return bool(ids) and all(x in client_ids() for x in ids)


def run(st, guesses, accept='near', max_chance=1.0, log=print):
    """guesses: [(slot index, name, tier, note)]. Accepts hits (cascade), returns the hit rows."""
    tested = collections.Counter(i for i, *_ in guesses)
    pending = list(guesses)
    hits = []
    rnd = 0
    while True:
        rnd += 1
        found = {}
        for g in pending:
            i, name, tier, note = g
            if i in found:
                continue
            w = st.window(i)
            if not w:
                continue
            lo, hi, nlo, nhi = w
            for nm in twins(name):
                b = bk(nm)
                where = 'near' if nlo <= b <= nhi else ('window' if lo <= b <= hi else None)
                if where and (accept == 'window' or where == 'near'):
                    width = (nhi - nlo + 1) if where == 'near' else (hi - lo + 1)
                    chance = tested[i] * width / NB
                    if chance <= max_chance:
                        found[i] = (nm, tier, note, where, w, b, chance)
                    break
        if not found:
            break
        # conflicting hits (two hits that cannot both be on the order): keep the most likely, retest the rest
        before = len(st.onset)
        prev = {}
        for i, (nm, *_rest) in found.items():
            prev[i] = (st.names[i], st.orig[i])
            if st.orig[i] is None and st.names[i] is not None:
                st.orig[i] = st.names[i]
            st.names[i] = nm
        st.recompute()
        bad = [i for i in found if i not in st.onset]
        for i in sorted(bad, key=lambda i: -found[i][6]):
            st.names[i], st.orig[i] = prev[i]
        if bad:
            st.recompute()
        kept = [i for i in found if i in st.onset]
        for i in kept:
            nm, tier, note, where, w, b, chance = found[i]
            st.how[i] = (st.how[i] + '; ' if st.how[i] else '') + f'{tier} {note}'.strip()
            hits.append(dict(slot=f'0x{st.table[i][0]:X}', target=f'0x{st.table[i][1]:X}', name=nm,
                             before=st.orig[i] or '', tier=tier, note=note, where=where,
                             window=f'{w[0]}-{w[1]}', near=f'{w[2]}-{w[3]}', bucket=b,
                             tested=tested[i], chance=f'{chance:.2g}', round=rnd))
        log(f'  cascade round {rnd}: hits {len(found)}, conflicts {len(bad)}, on {before} -> {len(st.onset)}')
        done = set(kept)
        pending = [g for g in pending if g[0] not in done and g[0] not in st.onset]
        if not kept:
            break
    by_cls = collections.Counter(class_of(h['name']) for h in hits)
    for h in hits:
        h['group'] = by_cls[class_of(h['name'])] if class_of(h['name']) else 1
        h['in_client'] = int(in_client(h['name']))
    return hits


def read_guesses(st, path):
    out = []
    for r in csv.DictReader(open(path), delimiter='\t'):
        s = int(r['slot'], 16)
        if s in st.index:
            out.append((st.index[s], r['name'], r.get('tier', ''), r.get('note', '')))
    return out


HIT_COLS = ['slot', 'target', 'name', 'before', 'tier', 'note', 'where', 'window', 'near', 'bucket', 'tested',
            'chance', 'group', 'in_client', 'round']


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=('state', 'test'))
    ap.add_argument('guesses', nargs='?')
    ap.add_argument('--state', default=DEFAULT_STATE)
    ap.add_argument('--keep-construct', action='store_true', help="don't revert round 1's _Construct rename")
    ap.add_argument('--out', default=paths.workdir('gaps'))
    ap.add_argument('--accept', choices=('near', 'window'), default='near')
    ap.add_argument('--max-chance', type=float, default=1.0)
    a = ap.parse_args()
    st = State(a.state, revert_construct=not a.keep_construct)
    print(st.summary(), flush=True)
    if a.cmd == 'state':
        return
    g = read_guesses(st, a.guesses)
    print(f'guesses {len(g)} for {len(set(x[0] for x in g))} slots', flush=True)
    hits = run(st, g, a.accept, a.max_chance)
    os.makedirs(a.out, exist_ok=True)
    with open(os.path.join(a.out, 'hits.tsv'), 'w', newline='') as f:
        w = csv.DictWriter(f, HIT_COLS, delimiter='\t', lineterminator='\n')
        w.writeheader()
        w.writerows(hits)
    st.write(os.path.join(a.out, 'state.tsv'))
    exp = sum(float(h['chance']) for h in hits)
    print(f'hits {len(hits)} (near {sum(h["where"] == "near" for h in hits)}), expected false among them '
          f'<= {exp:.1f}; in client.exe {sum(h["in_client"] for h in hits)}; '
          f'groups >= 2: {sum(h["group"] >= 2 for h in hits)}')
    print(st.summary())


if __name__ == '__main__':
    main()
