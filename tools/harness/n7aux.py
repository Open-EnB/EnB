"""M1.5 gate (d): the Net-7 recordings' AUX_DATA through the M1.3 harness (tests/capharness/auxreplay).

    python tools/harness/n7aux.py [SESSION ...] [--out DIR]      (default: the three 2026-10 sessions)

The Net-7 server encodes some AUX_DATA differently from EA's (the reference decoder docs/research/auxdata.md
§7). the reference decoder's aux-census fails 2 / 10 / 5 of them; this script runs every AUX_DATA the client received
through the client's decode path (auxreplay.exe, built by `auxreplay.py build`), and counts the known
differences instead of failing on them. Each one is checked, not assumed:

  length   (§7.1) the length field differs from the bytes that follow; everything else re-encodes
           byte for byte (the harness decodes the bytes that follow, as the reference decoder does; the client
           sizes its buffer from the field, so where it is shorter the client stops early).
  members  (§7.2, §7.8) a full Player send ends where Player.GroupInfo.Members' header should be:
           the decode runs out exactly at the end. Checked by a second run with EA's `05`
           (Whole|Default) appended: it must then decode and re-encode byte for byte. For the values
           the Members fields that `05` defaults are dropped again (the client keeps what it read).
  short    (§7.3) a tree-form update that declares more bytes than it carries and runs out.
  sets     (not in §7) a field named in two sets (Value and Default, ...): the client's decoder
           applies one, so its encoder can't write the message back. Checked by a second run with
           our re-encoding in the message's place: it must touch the same fields with the same
           values and re-encode byte for byte.
  noroot   AUX_DATA for an object without a CREATE yet (§7.5), as aux-census's count.

Everything else must decode, re-encode and give the field values of the reference decoder's aux-log --first
(.work/harness/ref/NAME.aux-log-first.txt; auxreplay.py's comparison).
"""
import argparse
import collections
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import auxreplay as AR  # noqa: E402

the reference decoder = r'C:\EnBDevelopment\the reference decoder\src\Harness\the reference decoder\bin\Release\net10.0\the reference decoder.exe'
SESSIONS = ['session_2026-10-19_023139', 'session_2026-10-19_115352', 'session_2026-10-21_111654']
MEMBERS = 'Player.GroupInfo.Members'
EA_MEMBERS = b'\x05'        # EA's form ends with Members' header: Whole | Default


def read_input(path):
    """The message file's lines; AUX_DATA as [n, chan, payload]."""
    lines = []
    for line in open(path):
        if line[0] == 'A':
            p = line.split()
            lines.append(['A', int(p[1]), p[2], bytes.fromhex(p[3])])
        else:
            lines.append(line)
    return lines


def write_input(path, lines, patch):
    with open(path, 'w', newline='\n') as f:
        for e in lines:
            if isinstance(e, str):
                f.write(e)
            else:
                data = patch.get(e[1], e[3])
                f.write(f'A {e[1]} {e[2]} {data.hex()}\n')


def read_output(path):
    """n -> {'head': M line fields, 'status', 'extra', 'W': bytes or None, 'leaves': [lines]}"""
    msgs = {}
    cur = None
    for line in open(path):
        k = line[0]
        if k == 'M':
            p = line.split()
            cur = {'line': line, 'root': p[4], 'form': p[5], 'status': p[6], 'extra': p[7:], 'W': None, 'leaves': []}
            msgs[int(p[1])] = cur
        elif k == 'C':
            cur = None
        elif cur is not None and k == 'W':
            cur['W'] = bytes.fromhex(line[2:].strip())
        elif cur is not None and k in 'UDX':
            cur['leaves'].append(line)
    return msgs


def run_exe(exe, inp, res, log):
    if os.path.exists(log):
        os.remove(log)
    env = dict(os.environ, ENB_STUB_LOG=log)
    r = subprocess.run([os.path.abspath(exe), inp, res], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       text=True, timeout=3600, cwd=AR.WORK, env=env)
    asserts = sum('G_ASSERT failed' in line for line in open(log)) if os.path.exists(log) else 0
    return r.stdout, asserts


def declared(data):
    return int.from_bytes(data[4:6], 'little')


def raw_changes(result):
    """auxreplay.our_changes, but a change is a change of the bytes, not of how aux-log shows them
    (the reference decoder compares wire values: a float that moves below G6's precision still shows as a change)."""
    types = AR.schema_types()
    state = {}
    out = []
    cur = None
    pending = []

    def flush():
        if cur is None:
            return
        st = state.setdefault(cur, {})
        for idx, path, raw, new in sorted(pending):
            old = st.get(path, ('unknown', 'unknown'))
            if old[0] != raw:
                out.append((cur[0], path, old[1], new))
                st[path] = (raw, new)

    with open(result) as f:
        for line in f:
            k = line[0]
            if k in 'MC':
                flush()
                pending = []
                p = line.split()
                if k == 'M':
                    cur = None if p[6] in ('noroot', 'decode') else (p[2], int(p[3]))
                else:
                    state.pop((p[1], int(p[2])), None)
                    cur = None
                continue
            if cur is None or k not in 'UDX':
                continue
            p = line.rstrip('\n').split(' ')
            idx, path = int(p[1]), p[2]
            if k == 'U':
                raw = p[3] if len(p) > 3 else ''
                new = AR.show(types.get(AR.NUM.sub('.#', path), '?'), raw)
            else:
                raw = new = 'default' if k == 'D' else 'unknown'
            pending.append((idx, path, raw, new))
    flush()
    return out


def session(name, exe, trace=None, auxlog=None):
    os.makedirs(AR.WORK, exist_ok=True)
    trace = trace or os.path.join(AR.CAPS, name + '.trace.jsonl')
    base = os.path.join(AR.WORK, name)
    inp, res = base + '.in.txt', base + '.out.txt'
    total = AR.prepare(trace, inp)
    lines = read_input(inp)
    payload = {e[1]: e[3] for e in lines if not isinstance(e, str)}
    _, asserts1 = run_exe(exe, inp, res, base + '.stubs.log')
    out1 = read_output(res)

    kind = {}           # n -> known difference (candidates until the second run)
    patch = {}
    for n, m in out1.items():
        data = payload[n]
        st = m['status']
        if st == 'ok':
            kind[n] = 'ok'
        elif st == 'noroot':
            kind[n] = 'noroot'
        elif st == 'differ':
            w = m['W']
            if w[:4] == data[:4] and w[6:] == data[6:]:
                kind[n] = 'length'
            else:
                kind[n] = 'sets?'
                patch[n] = w
        elif st == 'decode':
            pos, size = (int(x) for x in m['extra'][0].split('/'))
            if pos != size:
                kind[n] = 'fail'
            elif m['root'] == 'Player' and m['form'] == 'tree' and declared(data) == len(data) - 6:
                kind[n] = 'members?'
                fixed = bytearray(data + EA_MEMBERS)
                fixed[4:6] = (declared(data) + 1).to_bytes(2, 'little')
                patch[n] = bytes(fixed)
            elif declared(data) > len(data) - 6:
                kind[n] = 'short'
            else:
                kind[n] = 'fail'
        else:
            kind[n] = 'fail'

    # the second run: the candidates patched
    inp2, res2 = base + '.n7.in.txt', base + '.n7.out.txt'
    write_input(inp2, lines, patch)
    _, asserts2 = run_exe(exe, inp2, res2, base + '.n7.stubs.log')
    out2 = read_output(res2)
    notes = []
    patch3 = {}
    for n, k in kind.items():
        if k == 'sets?':
            m1, m2 = out1[n], out2[n]
            if m2['status'] == 'ok' and m1['leaves'] == m2['leaves']:
                kind[n] = 'sets'
                if declared(payload[n]) != len(payload[n]) - 6:
                    kind[n] = 'sets+length'
            else:
                kind[n] = 'fail'
                notes.append(f'  #{n} {m1["root"]}: our re-encoding gives {m2["status"]}, '
                             f'{"same" if m1["leaves"] == m2["leaves"] else "other"} fields')
        elif k == 'members?':
            m2 = out2[n]
            if m2['status'] == 'differ' and m2['W'][6:] != patch[n][6:]:
                patch3[n] = m2['W']
            elif m2['status'] != 'ok':
                kind[n] = 'fail'
                notes.append(f'  #{n} Player + 05: {m2["status"]}')

    # the third run: Player + 05 that also names fields in two sets, with our re-encoding
    inp3, res3 = base + '.n7b.in.txt', base + '.n7b.out.txt'
    write_input(inp3, lines, {**patch, **patch3})
    _, asserts3 = run_exe(exe, inp3, res3, base + '.n7b.stubs.log')
    out3 = read_output(res3)
    for n, k in kind.items():
        if k != 'members?':
            continue
        m2 = out2[n]
        if n in patch3:
            if out3[n]['status'] != 'ok' or out3[n]['leaves'] != m2['leaves']:
                kind[n] = 'fail'
                notes.append(f'  #{n} Player + 05, re-encoded: {out3[n]["status"]}')
                continue
        extra = [x for x in m2['leaves'] if x.split(' ')[2].startswith(MEMBERS + '.') or x.split(' ')[2] == MEMBERS]
        if all(x[0] == 'D' for x in extra):
            kind[n] = 'members+sets' if n in patch3 else 'members'
            m2['leaves'] = [x for x in m2['leaves'] if x not in extra]
        else:
            kind[n] = 'fail'
            notes.append(f'  #{n} Player + 05: Members fields other than defaulted')

    # the values: the second run's output, with the patched Members fields dropped again
    res4 = base + '.n7.values.txt'
    with open(res4, 'w', newline='\n') as f:
        for line in open(res2):
            if line[0] == 'M':
                n = int(line.split()[1])
                m = out2[n]
                f.write(m['line'])
                for x in m['leaves']:
                    f.write(x)
            elif line[0] == 'C':
                f.write(line)

    counts = collections.Counter(kind.values())
    by_root = collections.Counter((out1[n]['root'], k) for n, k in kind.items())
    lens = collections.Counter(declared(payload[n]) - (len(payload[n]) - 6) for n, k in kind.items()
                               if k in ('length', 'sets+length'))
    print(f'== {name}: {total} AUX_DATA, asserts {asserts1} + {asserts2} + {asserts3}')
    for (root, k), v in sorted(by_root.items()):
        if k != 'ok':
            print(f'  {root:16s} {k:12s} {v:5d}')
    print('  declared - carried (length): ' + ', '.join(f'{d:+d} x{v}' for d, v in sorted(lens.items())))
    for x in notes[:10]:
        print(x)
    print(f'  ok {counts["ok"]}, length {counts["length"]}, members {counts["members"]}, short {counts["short"]}, '
          f'members+sets {counts["members+sets"]}, sets {counts["sets"]} (+{counts["sets+length"]} with a length difference), noroot {counts["noroot"]}, '
          f'FAILED {counts["fail"]}')
    auxlog = auxlog or os.path.join(AR.REF, name + '.aux-log-first.txt')
    AR.our_changes = raw_changes
    same = AR.values(res4, auxlog)
    return counts, same


def server_side(name):
    """A copy of the trace with ServerToProxy as ServerToClient, and the reference decoder's aux-census and
    aux-log --first of it (both read only what reached the client)."""
    os.makedirs(AR.WORK, exist_ok=True)
    src = os.path.join(AR.CAPS, name + '.trace.jsonl')
    trace = os.path.join(AR.WORK, name + '.server-side.trace.jsonl')
    swap = {'ServerToProxy': 'ServerToClient', 'ProxyToServer': 'ClientToServer'}
    with open(src, encoding='utf-8') as f, open(trace, 'w', encoding='utf-8', newline='\n') as g:
        for line in f:
            e = json.loads(line)
            e['dir'] = swap.get(e['dir'], e['dir'])
            g.write(json.dumps(e, separators=(',', ':')) + '\n')
    census = subprocess.run([the reference decoder, 'aux-census', trace], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    print(f'the reference decoder aux-census ({name}, server side):\n' + census.stdout.rstrip())
    auxlog = os.path.join(AR.WORK, name + '.server-side.aux-log-first.txt')
    with open(auxlog, 'w', encoding='utf-8', newline='\n') as g:
        subprocess.run([the reference decoder, 'aux-log', trace, '--first'], stdout=g, stderr=subprocess.STDOUT, text=True)
    return trace, auxlog


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('sessions', nargs='*')
    ap.add_argument('--out', default=os.path.join(AR.REPO, 'build'))
    ap.add_argument('--server-side', action='store_true',
                    help="the recording has only the server's side of the sector (2022-09-25): take what "
                         "the server sent the proxy as what the client got, with the reference decoder's references made here")
    a = ap.parse_args()
    exe = os.path.join(a.out, 'programs', 'auxreplay', 'auxreplay.exe')
    bad = 0
    for name in a.sessions or SESSIONS:
        if a.server_side:
            trace, auxlog = server_side(name)
            counts, same = session(name, exe, trace, auxlog)
        else:
            counts, same = session(name, exe)
        bad += counts['fail'] + (0 if same else 1)
    raise SystemExit(1 if bad else 0)


if __name__ == '__main__':
    main()
