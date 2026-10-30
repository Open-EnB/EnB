"""M1.3's AuxData harness (milestones.md 2.1 gates (b) and (c)): tests/auxreplay.

    python tools/harness/auxreplay.py prepare TRACE.jsonl OUT.txt [--dump]
    ENB_VC6_HOSTS=<user>@<ip> python tools/harness/auxreplay.py build [--out DIR] [--app-sources]
    python tools/harness/auxreplay.py run NAME [--trace T] [--dump]     (prepare, run, census, values)
    python tools/harness/auxreplay.py values RESULT.txt AUXLOG.txt        (gate (c) only)

prepare: the messages the client received (ServerToClient), as the reference decoder's aux-census reads them:
CREATE 0x0004 (game id, type byte at 10) and AUX_DATA 0x001B. --dump (alex3, alex7) takes the
messages from the EA dump instead, rebuilt from its blocks by convert.py (the datagram files'
content, with the 28 / 12 messages the reference decoder's import lost: D52's 4,289 / 366 AUX_DATA).
build: links auxreplay against build/'s libraries (tools/vc6/build.py link, as auxdata_smoke.py)
into DIR/programs/auxreplay/. --app-sources compiles app/client/aux_roots.cpp and
app/shared/auxdata/unknown_root.cpp into the program (for an app.lib built before they existed).
run: NAME is a trace in TRACES (or --trace, or --dump); writes .work/harness/auxreplay/NAME.{in,out}.txt,
prints the census (gate (b)) and compares the field values with the reference decoder's aux-log --first
(roots.EA_REF's NAME.aux-log-first.txt, the frozen reference D154, gate (c)).

Values (gate c): the reference decoder's aux-log prints every leaf whose state or value changed, from its typed
model. This script replays the harness's U/D/X lines over a per-object state (unknown, default,
value) and prints the same kind of line; the two are compared in order per channel, ignoring the
time and the object's description.
"""
import argparse
import json
import os
import re
import shutil
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
WORK = os.path.join(REPO, '.work', 'harness', 'auxreplay')
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import roots  # noqa: E402
REF = roots.EA_REF            # the frozen reference readings (D154)
SCHEMA = os.path.join(REPO, 'docs', 'data', 'client-auxdata.json')   # auxroots.py schema, from client.exe
TRACES = {n: os.path.join(roots.EA_TRACES, n + '.trace.jsonl') for n in ('capture_1', 'capture_2', 'capture_3', 'alex3', 'alex7')}
LIVE = roots.LIVE_PACKETS
DUMPS = {'alex3': os.path.join(LIVE, 'Alex3-fixed.txt.txt'), 'alex7': os.path.join(LIVE, 'Alex7-fixed.txt.txt')}
OP_CREATE, OP_AUX = 0x0004, 0x001B


# ------------------------------------------------------------------------- prepare

def trace_messages(path):
    with open(path, encoding='utf-8') as f:
        for line in f:
            e = json.loads(line)
            if e.get('dir') not in ('ServerToClient', 'ProxyToClient') or 'op' not in e:
                continue        # (the capture_N traces also record the TCP handshake: 'hs', no opcode)
            yield e['channel'], int(e['op'], 16), bytes.fromhex(e.get('data') or '')


def dump_messages(path):
    """The messages convert.py rebuilds from an Alex dump's blocks (its datagram file's content)."""
    sys.path.insert(0, HERE)
    import convert as C
    os.makedirs(WORK, exist_ok=True)
    tmp = os.path.join(WORK, os.path.basename(path) + '.dgram')
    datagrams, _ = C.convert(path, tmp)
    msgs, _ = C.messages(datagrams)
    for session, direction, seq, op, data in msgs:
        if direction == 'S':
            yield 'ea-udp:' + session, op, data


def prepare(src, out, dgram=False):
    n = 0
    with open(out, 'w', newline='\n') as f:
        for chan, op, data in (dump_messages(src) if dgram else trace_messages(src)):
            chan = chan.replace(' ', '_')
            if op == OP_CREATE and len(data) >= 11:
                f.write(f'C {chan} {struct.unpack_from("<i", data)[0]} {data[10]}\n')
            elif op == OP_AUX:
                f.write(f'A {n} {chan} {data.hex()}\n')
                n += 1
    return n


# ------------------------------------------------------------------------- build

APP_SOURCES = (r'app\client\aux_roots.cpp', r'app\shared\auxdata\unknown_root.cpp',
               r'app\shared\auxdata\encode.cpp', r'app\shared\auxdata\policy.cpp',
               r'app\shared\auxdata\unknown_changes.cpp', r'app\shared\auxdata\delta_money.cpp')


def build(out, app_sources):
    sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
    import build as B
    src = os.path.join(REPO, 'tests', 'auxreplay')
    stage = os.path.join(REPO, '.work', 'harness', 'auxreplay_src')
    shutil.rmtree(stage, ignore_errors=True)
    os.makedirs(stage)
    for fn in os.listdir(src):
        shutil.copy(os.path.join(src, fn), stage)
    if app_sources:
        # app files newer than build/app/app.lib, one stub file each (their statics stay apart)
        for k, p in enumerate(APP_SOURCES):
            with open(os.path.join(stage, 'app_sources_%d.cpp' % k), 'w', newline='\r\n') as f:
                f.write('// app file newer than build/app/app.lib (auxreplay.py --app-sources)\n')
                f.write('#include "' + os.path.relpath(os.path.join(REPO, p), stage) + '"\n')
    B.PROGRAMS['auxreplay'] = {'dir': os.path.relpath(stage, REPO), 'flags': B.I.LIB_FLAGS['ww3d2'] + ' /GX',
                               'subsystem': 'console', 'runtime': []}
    B.link_program('auxreplay', out)
    exe = os.path.join(out, 'programs', 'auxreplay', 'auxreplay.exe')
    if not os.path.exists(exe):
        raise SystemExit('no exe: see ' + os.path.join(out, 'programs', 'auxreplay'))
    print('built', exe)


# ------------------------------------------------------------------------- values (gate c)

def schema_types():
    doc = json.load(open(SCHEMA))
    types = {}

    def walk(node, path):
        t = node['type']
        if t == 'struct':
            for c in node['fields']:
                walk(c, path + '.' + c['name'])
        elif t == 'array':
            walk(node['element'], path + '.#')
        else:
            types[path] = t
    for name, root in doc['roots'].items():
        walk(root, name)
    return types


NUM = re.compile(r'\.\d+(?=\.|$)')


def fmt_g6(x):
    """C#'s float.ToString("G6", InvariantCulture)."""
    if x != x:
        return 'NaN'
    if x in (float('inf'), float('-inf')):
        return 'Infinity' if x > 0 else '-Infinity'
    if x == 0:
        return '-0' if str(x).startswith('-') else '0'
    s = f'{x:.6g}'
    if 'e' in s:
        m, e = s.split('e')
        ex = int(e)
        s = f'{m}E{"+" if ex >= 0 else "-"}{abs(ex):02d}'
    return s


def fmt_r(x):
    """C#'s float.ToString() (the shortest text that reads back as the same float)."""
    if x != x or x in (float('inf'), float('-inf')) or x == 0:
        return fmt_g6(x)
    for p in range(1, 10):
        s = f'{x:.{p - 1}e}'
        if struct.unpack('<f', struct.pack('<f', float(s)))[0] == x:
            break
    m, e = s.split('e')
    ex = int(e)
    if ex < -5 or ex >= 15:     # .NET's switch to E notation
        return f'{m}E{"+" if ex >= 0 else "-"}{abs(ex):02d}'
    from decimal import Decimal
    return format(Decimal(s).normalize(), 'f')


def show(t, hexv):
    b = bytes.fromhex(hexv)
    if t == 'string':
        n = struct.unpack_from('<H', b)[0] if len(b) >= 2 else 0
        s = b[2:2 + n].decode('latin-1').rstrip('\0')
        # aux-log's text shows other than ASCII as '?'
        return "'" + ''.join(c if ' ' <= c < '\x7f' else '?' for c in s) + "'"
    if t == 'float':
        return fmt_g6(struct.unpack('<f', b)[0])
    if t == 'int32':
        i = struct.unpack('<i', b)[0]
        return f'{i} (0x{i & 0xffffffff:x})' if i >= 0 else f'{i} (0x{i & 0xffffffff:x})'
    if t == 'bool':
        return 'True' if b[0] else 'False'
    if t == 'money':
        return str(struct.unpack('<q', b)[0])
    if t in ('enum', 'time'):
        i = struct.unpack('<i', b)[0]
        return f'{i} (0x{i & 0xffffffff:x})'
    if t == 'color':
        r, g, bl = (fmt_r(v) for v in struct.unpack('<3f', b))
        return f'AuxColor {{ R = {r}, G = {g}, B = {bl} }}'
    return b.hex()


def our_changes(result):
    """(chan, path, old, new) per leaf change, from the harness's output (as aux-log --first)."""
    types = schema_types()
    state = {}          # (chan, gameid) -> {path: shown}
    out = []
    cur = None
    pending = []

    def flush():
        # aux-log lists a message's changes in tree order
        if cur is None:
            return
        st = state.setdefault(cur, {})
        for idx, path, new in sorted(pending):
            old = st.get(path, 'unknown')
            if old != new:
                out.append((cur[0], path, old, new))
                st[path] = new

    with open(result) as f:
        for line in f:
            k = line[0]
            if k == 'M':
                flush()
                pending = []
                p = line.split()
                chan, gid, status = p[2], int(p[3]), p[6]
                cur = None if status in ('noroot', 'decode') else (chan, gid)
                continue
            if k == 'C':
                flush()
                pending = []
                p = line.split()
                state.pop((p[1], int(p[2])), None)
                cur = None
                continue
            if cur is None or k not in 'UDX':
                continue
            p = line.rstrip('\n').split(' ')
            idx, path = int(p[1]), p[2]
            if k == 'U':
                new = show(types.get(NUM.sub('.#', path), '?'), p[3] if len(p) > 3 else '')
            else:
                new = 'default' if k == 'D' else 'unknown'
            pending.append((idx, path, new))
    flush()
    return out



AUXLOG = re.compile(r'^\s*[\d.]+s (\S+) (.*)$')


def ref_changes(auxlog, roots):
    rx = re.compile(r'(?:^|\s)((?:' + '|'.join(roots) + r')(?:\.[^:\s]*)?): (.*?) -> (.*)$')
    out = []
    with open(auxlog, encoding='utf-8', errors='replace') as f:
        for line in f:
            m = AUXLOG.match(line)
            if not m:
                continue
            chan, rest = m.groups()
            mm = rx.search(rest)
            if not mm:
                continue
            out.append((chan, mm.group(1), mm.group(2), mm.group(3)))
    return out


def norm(v):
    # the typed model shows some fields its own way (enums by name, colours, times): compare
    # numbers loosely
    return v


def values(result, auxlog, limit=10):
    roots = list(json.load(open(SCHEMA))['roots'])
    ours = our_changes(result)
    ref = ref_changes(auxlog, roots)
    by = {}
    for c in ours:
        by.setdefault(c[0], []).append(c[1:])
    rb = {}
    for c in ref:
        rb.setdefault(c[0], []).append(c[1:])
    total = same = 0
    shown = 0
    kinds = {}
    for chan in sorted(set(by) | set(rb)):
        a, b = by.get(chan, []), rb.get(chan, [])
        n = max(len(a), len(b))
        total += n
        for i in range(min(len(a), len(b))):
            if a[i] == b[i]:
                same += 1
            else:
                key = NUM.sub('.#', a[i][0]) if a[i][0] == b[i][0] else 'path order'
                kinds[key] = kinds.get(key, 0) + 1
                if shown < limit:
                    print(f'  {chan} #{i}: ours {a[i]} | reference {b[i]}')
                    shown += 1
        if len(a) != len(b):
            print(f'  {chan}: {len(a)} changes ours, {len(b)} reference')
    print(f'values: {same} of {total} field changes equal ({len(ours)} ours, {len(ref)} reference)')
    for k, v in sorted(kinds.items(), key=lambda kv: -kv[1])[:15]:
        print(f'  {v:7d} differ at {k}')
    return same == total


# ------------------------------------------------------------------------- run

def run(name, trace, dgram, exe, auxlog):
    os.makedirs(WORK, exist_ok=True)
    tag = name + ('.dump' if dgram else '')
    inp = os.path.join(WORK, tag + '.in.txt')
    res = os.path.join(WORK, tag + '.out.txt')
    log = os.path.join(WORK, tag + '.stubs.log')
    if os.path.exists(log):
        os.remove(log)
    n = prepare(dgram or trace, inp, dgram=bool(dgram))
    print(f'{tag}: {n} AUX_DATA')
    env = dict(os.environ, ENB_STUB_LOG=log)
    r = subprocess.run([os.path.abspath(exe), inp, res], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       text=True, timeout=3600, cwd=WORK, env=env)
    print(r.stdout.strip())
    asserts = sum('G_ASSERT failed' in line for line in open(log)) if os.path.exists(log) else 0
    print(f'asserts: {asserts}' + (f' (see {log})' if asserts else ''))
    if auxlog and os.path.exists(auxlog):
        values(res, auxlog)
    return r.returncode


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['prepare', 'build', 'run', 'values'])
    ap.add_argument('args', nargs='*')
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    ap.add_argument('--app-sources', action='store_true')
    ap.add_argument('--trace')
    ap.add_argument('--dump', action='store_true')
    a = ap.parse_args()
    exe = os.path.join(a.out, 'programs', 'auxreplay', 'auxreplay.exe')
    if a.cmd == 'prepare':
        print(prepare(a.args[0], a.args[1], dgram=a.dump), 'AUX_DATA')
    elif a.cmd == 'build':
        build(a.out, a.app_sources)
    elif a.cmd == 'values':
        raise SystemExit(0 if values(a.args[0], a.args[1]) else 1)
    else:
        name = a.args[0]
        trace = a.trace or TRACES.get(name)
        dgram = DUMPS[name] if a.dump else None
        # aux-log is the reference decoder's reading of the trace, so the values are compared for traces only
        raise SystemExit(run(name, trace, dgram, exe, None if dgram else os.path.join(REF, name + '.aux-log-first.txt')))


if __name__ == '__main__':
    main()
