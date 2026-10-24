"""Check capharness's counts against the reference decoder's (M1 gates; M1.0: counts only).

    python tools/harness/compare.py --exe .work/harness/bin/capharness.exe \\
        --trace TRACE.jsonl --summary REF.summary.txt [--dgram FILE.dgram]
    python tools/harness/compare.py --harness-out OUT.txt --summary ... [--dgram ...]

--summary is `the reference decoder dump TRACE --summary`'s output (saved under .work/harness/ref/).
Checks:
  1. the harness's message counts and payload bytes per direction and opcode (from the trace)
     equal the reference decoder's summary, summed over channel kinds;
  2. with --dgram: the harness's datagram, ack, block and rebuilt-message counts equal this
     script's own count of the same datagram file (convert.py's rules);
  3. with --dgram: the messages rebuilt from the datagrams against the trace's, per direction
     and opcode (reported; the datagram file has a few messages the reference decoder's import lost).
Exit code 0 when 1 and 2 pass.
"""
import argparse
import os
import subprocess
import sys
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))


def harness_tables(lines):
    tables = defaultdict(dict)   # tag -> {(dir, op): (count, bytes)}
    other = {}
    for line in lines:
        f = line.split()
        if len(f) == 6 and f[1] == 'op':
            tables[f[0]][(f[2], int(f[3], 16))] = (int(f[4]), int(f[5]))
        elif f and f[0] in ('trace', 'dgram'):
            other[' '.join(f[:-1])] = f[-1]
    return tables, other


def summary_table(path):
    t = Counter()
    b = Counter()
    total = None
    in_table = False
    for line in open(path, encoding='utf-8', errors='replace'):
        f = line.split()
        if not f:
            continue
        if total is None and len(f) == 2 and f[1] == 'messages':
            total = int(f[0])
        if f[:3] == ['kind', 'dir', 'opcode']:
            in_table = True
            continue
        if in_table and len(f) >= 5 and f[2].startswith('0x'):
            key = (f[1], int(f[2], 16))
            t[key] += int(f[-2])
            b[key] += int(f[-1])
    return {k: (t[k], b[k]) for k in t}, total


def dgram_counts(path):
    """This script's own count of a datagram file, as capharness prints it."""
    out = []
    dgtype, btype = Counter(), Counter()
    n = cs = sc = short = problems = keys = acks = blocks = control = resent = frags = notmsg = 0
    seen = set()
    pending = {}
    msgs = Counter()
    mbytes = Counter()
    cur = None
    for line in open(path, encoding='ascii'):
        k = line[:1]
        f = line.split()
        if k == 'K':
            keys += 1
        elif k == 'D':
            _, idx, session, d, seq, declared, ln, typ, flags, hexs = f
            cur = (session, d, bytes.fromhex(hexs))
            n += 1
            cs += d == 'C'
            sc += d == 'S'
            short += declared != ln
            dgtype[int(typ)] += 1
        elif k == 'A':
            acks += int(f[2])
        elif k == 'X':
            problems += 1
        elif k == 'B':
            size, ch, seq, typ, frag, off, ln = map(int, f[1:])
            blocks += 1
            btype[typ] += 1
            if size == 0 or (typ & 15) not in (3, 4):
                control += 1
                continue
            key = (cur[0], cur[1], ch, seq)
            if key in seen:
                resent += 1
                continue
            seen.add(key)
            data = cur[2][off:off + ln]
            if frag >= 0:
                frags += 1
                pk = (cur[0], cur[1], ch, (seq - frag) & 0xFFFF)
                p = pending.setdefault(pk, {})
                p[frag] = data
                if sum(map(len, p.values())) < size:
                    continue
                data = b''.join(p[i] for i in sorted(p))
                del pending[pk]
            if len(data) < 4 or int.from_bytes(data[:2], 'little') != len(data):
                notmsg += 1
                continue
            key = ('c>s' if cur[1] == 'C' else 's>c', int.from_bytes(data[2:4], 'little'))
            msgs[key] += 1
            mbytes[key] += len(data) - 4
    other = {
        'dgram keys': str(keys),
        'dgram datagrams %d c>s %d s>c %d short %d problems' % (n, cs, sc, short): str(problems),
        'dgram acks': str(acks),
        'dgram blocks %d control %d resent %d fragments %d not-a-message' % (blocks, control, resent, frags): str(notmsg),
        'dgram incomplete': str(len(pending)),
        'dgram messages': str(sum(msgs.values())),
    }
    for t, c in dgtype.items():
        other['dgram datagram-type %d' % t] = str(c)
    for t, c in btype.items():
        other['dgram block-type 0x%02x' % t] = str(c)
    return {k: (msgs[k], mbytes[k]) for k in msgs}, other


def diff_tables(name_a, a, name_b, b):
    bad = []
    for key in sorted(set(a) | set(b)):
        if a.get(key) != b.get(key):
            bad.append('  %s 0x%04x: %s %s, %s %s' % (key[0], key[1], name_a, a.get(key), name_b, b.get(key)))
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe')
    ap.add_argument('--harness-out')
    ap.add_argument('--trace')
    ap.add_argument('--summary', required=True)
    ap.add_argument('--dgram')
    a = ap.parse_args()
    if a.exe:
        cmd = [os.path.abspath(a.exe), '--trace', a.trace] + (['--dgram', a.dgram] if a.dgram else [])
        p = subprocess.run(cmd, capture_output=True, text=True)
        if p.returncode:
            print(p.stdout[-2000:], p.stderr)
            raise SystemExit('capharness failed (%d)' % p.returncode)
        lines = p.stdout.splitlines()
    else:
        lines = open(a.harness_out, encoding='ascii').read().splitlines()
    tables, other = harness_tables(lines)
    ok = True

    ref, total = summary_table(a.summary)
    bad = diff_tables('capharness', tables['trace'], 'the reference decoder', ref)
    ours_total = int(other.get('trace messages', -1))
    print('1. trace: capharness %d messages, %d (dir, opcode) rows; the reference decoder %s messages, %d rows: %s' % (
        ours_total, len(tables['trace']), total, len(ref), 'OK' if not bad and ours_total == total else 'DIFFERENT'))
    if bad or ours_total != total:
        ok = False
        print('\n'.join(bad[:30]))

    if a.dgram:
        mine, mine_other = dgram_counts(a.dgram)
        bad = diff_tables('capharness', tables['dgram'], 'compare.py', mine)
        theirs = other
        for k in sorted(set(theirs) | set(mine_other)):
            if k.startswith('dgram ') and (k.split()[1] in ('keys', 'datagrams', 'acks', 'blocks', 'incomplete',
                                                            'messages', 'datagram-type', 'block-type')):
                if theirs.get(k) != mine_other.get(k):
                    bad.append('  %s: capharness %s, compare.py %s' % (k, theirs.get(k), mine_other.get(k)))
        print('2. datagrams: capharness and compare.py agree on %d counts and %d message rows: %s' % (
            len(mine_other), len(mine), 'OK' if not bad else 'DIFFERENT'))
        if bad:
            ok = False
            print('\n'.join(bad[:30]))
        extra = Counter()
        missing = Counter()
        for key in set(mine) | set(tables['trace']):
            d = mine.get(key, (0, 0))[0] - tables['trace'].get(key, (0, 0))[0]
            if d > 0:
                extra[key] = d
            elif d < 0:
                missing[key] = -d
        print('3. messages from datagrams %d, from the trace %d: %d more, %d fewer' % (
            sum(v[0] for v in mine.values()), ours_total, sum(extra.values()), sum(missing.values())))
        for key, c in sorted(extra.items()):
            print('   more %s 0x%04x: %d' % (key[0], key[1], c))
        for key, c in sorted(missing.items()):
            print('   fewer %s 0x%04x: %d' % (key[0], key[1], c))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
