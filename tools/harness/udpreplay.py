"""M1.4's EA-UDP harness (milestones.md 2.1 gate (a)): tests/udpreplay.

    ENB_VC6_HOSTS=<user>@<ip> python tools/harness/udpreplay.py build [--out DIR]
    python tools/harness/udpreplay.py run alex3|alex7 [--exe EXE] [--aux]
    python tools/harness/udpreplay.py handshake            (capture_1..3's 54 SYN1/ACK1/SYN2/ACK2)
    python tools/harness/udpreplay.py loop alex3|alex7 [--drop N,...] [--step MS]
    python tools/harness/udpreplay.py conn alex3|alex7 [--drop N,...] [--step MS]

build: links udpreplay with the app\\wwudp sources (compiled into the program, as auxreplay.py's
--app-sources) against DIR's libraries (wwlib's RC4Class) into DIR/programs/udpreplay/.

run: writes .work/harness/udpreplay/NAME.in.txt from the datagram file (.work/harness/NAME.dgram,
convert.py) and runs the harness. The input has, per datagram, the messages it sends for the
first time (W lines: the harness writes them on the sending side's CordCon first, so that its
sequence numbers and the acks it receives follow the capture) and the datagram (D). The three
datagrams per file that the dump cut short are rebuilt from their readable blocks with a new
checksum (reported). Checks:
  (1) the messages the receiving CordCons deliver: per channel equal to the messages rebuilt
      from the dump's blocks, in order; the whole sequence against convert.py's and the reference decoder's
      (the client delivers a reliable channel in sequence order, the dump's order is arrival);
  (2) every captured datagram accepted (checksum, RC4, examinePacket);
  (3) the ack lists our sending side generated (CordCon::pump: buildHeader) against the
      capture's, in order per session and direction; the acks the receiving side matched;
  (4) the message blocks (channel, seq) our sending side generated against the capture's
      first sendings.
--aux: the CREATE / AUX_DATA messages the client side delivered go through auxreplay
(build/programs/auxreplay, M1.3): gates (a) and (b) chained.

loop: the dump's messages and closes, but each side's own datagrams go to the other over a link
that drops every Nth datagram (default 0 = none, then 10 and 4), the clock advancing MS ms (default
20) per datagram of the dump: resend requests, the resend timer, the send window and the close,
which a replay of the capture can't test (the dumps have no times).

conn: the same replay and loop runs with --conn: what each receiving CordCon delivers is read through
app\\common's UDPConnection::Receive (CordCon::read, the packet factory: tests/udpreplay/
conn_replay.cpp supplies a test packet) and then through the stream connection's framing (Send_Packet
writes {size, opcode} and the payload to a memory transport; Receive reads them back through the
100,000-byte ring in chunks of 1 to 4,096 bytes). Passes when the {size, opcode, payload} messages
(M lines) equal, in order per session and direction, what CordCon::read delivers in the plain run
(R lines), every size field is 4 + the payload, the whole sequence equals the reference decoder's trace.jsonl as
`run` compares it, and the harness counted no error (a packet without its connection, a failed
send, bytes left in the ring).
"""
import argparse
import re
import difflib
import json
import os
import shutil
import struct
import subprocess
import sys
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
WORK = os.path.join(REPO, '.work', 'harness', 'udpreplay')
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import roots  # noqa: E402
sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
import rootguard  # noqa: E402
TRACES = roots.EA_TRACES      # the frozen reference traces (D154, fetch_reference.py)
WWUDP = [r'app\wwudp\cordcon.cpp', r'app\wwudp\cordmgr.cpp', r'app\wwudp\unknown_cordmsg.cpp']
# --conn (conn): the connection layer over them (docs/kernel-map.md 4), with what it needs
CONN = [r'app\common\circbuff.cpp', r'app\common\connect.cpp', r'app\common\udpconn.cpp',
        r'app\kernel\kernel.cpp', r'app\shared\auxdata\linearbu.cpp', r'app\packets\pk.cpp',
        r'app\packets\clientstub.cpp', r'app\shared\unknown_debug.cpp',
        r'app\common\unknown_navlist.cpp',   # WARP 155's navigation list
        r'app\common\lego_identifier.cpp',   # the object-to-object effects' bone path (opcodes 11-14)
        r'app\shared\avatar_desc.cpp',       # AVATAR_DESCRIPTION 97's member (avatar_desc_stubs.cpp)
        r'app\common\unknown_verb.cpp']      # Value_To_String(VerbEnum) (pkverbrequest.cpp and the other packets' logs)
# the packet classes rebuilt in their own files (pkclasses.py: pk_classes.h includes their headers;
# conn_replay.cpp sends what their Build makes)
for _sub in ('', 'gameedit'):
    _d = os.path.join(REPO, 'app', 'packets', _sub)
    if os.path.isdir(_d):
        CONN += sorted(os.path.join('app', 'packets', _sub, f) for f in os.listdir(_d)
                       if f.startswith('pk') and f.endswith('.cpp') and f != 'pk.cpp')
sys.path.insert(0, HERE)
import convert as C  # noqa: E402

# The messages whose rebuilt Build stamps a field with the aux-data clock (Unknown_Get_Ticks 0x4A3B40,
# 0 in the harness: conn_replay.cpp), so it can't give the received bytes back: opcode -> the field's
# offset in the message (header included). CLIENT_SET_TIME 52: ServerSent; REQUEST_TIME 68: TimeSent;
# LOGIN 2: TimeSent after the login ticket (TicketClass::Get_Size, 0x40 bytes).
TICKS = {2: 0x44, 52: 12, 68: 4}


def masked(m):
    """The message with its clock field zeroed (TICKS), else unchanged."""
    if len(m) >= 4:
        off = TICKS.get(struct.unpack_from('<h', m, 2)[0])
        if off is not None and len(m) >= off + 4:
            return m[:off] + bytes(4) + m[off + 4:]
    return m


def build(out):
    sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
    import build as B
    src = os.path.join(REPO, 'tests', 'udpreplay')
    stage = os.path.join(REPO, '.work', 'harness', 'udpreplay_src')
    shutil.rmtree(stage, ignore_errors=True)
    os.makedirs(stage)
    for fn in os.listdir(src):
        shutil.copy(os.path.join(src, fn), stage)
    with open(os.path.join(stage, 'app_sources.cpp'), 'w', newline='\r\n') as f:
        f.write('// app\\wwudp, compiled into the program (udpreplay.py build)\n')
        # avatar_desc.cpp's typed adds (kv_encoder_base.h's, D104) are declared only under this macro, and
        # an earlier file of this one translation unit includes kv_encoder_base.h first
        f.write('#define UNKNOWN_PACKETDATA_BODIES\n')
        # D176: CordCon's harness counters live in udpreplay's side table (cord_counters.h defines
        # CORD_HARNESS_COUNT, which cordcon.cpp leaves empty otherwise)
        f.write('#include "cord_counters.h"\n')
        for p in WWUDP + CONN:
            f.write('#include "' + os.path.relpath(os.path.join(REPO, p), stage) + '"\n')
            if p.endswith('udpconn.cpp'):
                # D95(a): conn_replay.cpp sends RECORDED's messages as received, from Receive's buffer
                f.write('char *Conn_Receive_Buffer() { return(Buffer); }\n')
    B.PROGRAMS['udpreplay'] = {'dir': os.path.relpath(stage, REPO), 'flags': B.I.LIB_FLAGS['ww3d2'] + ' /GX',
                               'subsystem': 'console', 'runtime': []}
    # E&B's INFMSG (wlib's wdebug.h, in cordcon.cpp) writes to the old iostream's ostream, as the client
    # does (0xA40DAB...): its library, LIBCIMT, after the new one
    if 'LIBCIMT.LIB' not in B.LINK_SYS:
        B.LINK_SYS.insert(B.LINK_SYS.index('LIBCPMT.LIB') + 1, 'LIBCIMT.LIB')
    B.link_program('udpreplay', out)
    exe = os.path.join(out, 'programs', 'udpreplay', 'udpreplay.exe')
    if not os.path.exists(exe):
        raise SystemExit('no exe: see ' + os.path.join(out, 'programs', 'udpreplay'))
    print('built', exe)


# ------------------------------------------------------------------------- input

T = []
for _i in range(256):
    _c = _i
    for _k in range(8):
        _c = (_c >> 1) ^ 0xEDB88320 if _c & 1 else _c >> 1
    T.append(_c)


def crc(buf, c=0xFFFFFFFF):
    """The client's checksum 0x8B3EC0 (CordCon's Set_Header_CRC): reflected CRC-32, no final xor."""
    for b in buf:
        c = T[(b ^ c) & 0xFF] ^ (c >> 8)
    return c


def small(v):
    return bytes([v]) if v < 0x80 else struct.pack('>H', v | 0x8000)


def seqb(v):
    return struct.pack('>H', v) if v < 0x8000 else struct.pack('>I', v | 0x80000000)


def read_dgram(path):
    """keys [(session, hex)], datagrams [[n, session, dir, seq, hex, acks, blocks, problem]]"""
    out, keys = [], []
    cur = None
    for line in open(path):
        p = line.split()
        if not p or p[0] == '#':
            continue
        if p[0] == 'K':
            keys.append((p[1], p[2]))
        elif p[0] == 'D':
            cur = [int(p[1]), p[2], p[3], int(p[4]), p[9] if len(p) > 9 else '', [], [], None]
            out.append(cur)
        elif p[0] == 'A':
            v = list(map(int, p[3:]))
            cur[5] = [tuple(v[i:i + 3]) for i in range(0, len(v), 3)]
        elif p[0] == 'B':
            cur[6].append(tuple(map(int, p[1:6])))
        elif p[0] == 'X':
            cur[7] = line[2:].strip()
    return keys, out


def repair(session, direction, counter, d, last):
    """A datagram the dump cut short (X): its readable blocks again, with a new checksum. Blocks
    read after the gap are kept only if plausible (mode 3/4/5, sequence within 1000 of the
    channel's last, as examinePacket requires)."""
    dtype, flags, hdr, acks, blocks, problem = C.parse_datagram(bytearray(d))
    pos = 7 + 4 * flags if dtype & 1 else 6
    body = bytearray(d[4:pos])
    kept = dropped = 0
    for size, ch, seq, btype, frag, off, ln in blocks:
        if (btype & 0x0F) not in (3, 4, 5) or abs(seq - last.get((session, direction, ch), seq)) > 1000:
            dropped += 1
            continue
        body += small(size) + small(ch) + seqb(seq) + bytes([btype])
        if btype & 0x10:
            body.append(frag)
        body += d[off:off + ln]
        kept += 1
    sess = int(session, 16) & 0xFFFF
    head = bytes([5 if direction == 'C' else 4, sess >> 8, sess & 0xFF, counter >> 8 & 0xFF, counter & 0xFF])
    return struct.pack('>I', crc(head + bytes(4) + bytes(body))) + bytes(body), kept, dropped


def prepare(dgram, path):
    keys, dgs = read_dgram(dgram)
    last = {}
    for d in dgs:
        if d[7]:
            fixed, kept, dropped = repair(d[1], d[2], d[3], bytes.fromhex(d[4]), last)
            print('  #%d %s %s cut short in the dump: rebuilt from %d blocks (%d dropped), new checksum' % (
                d[0], d[1], d[2], kept, dropped))
            d[4] = fixed.hex()
        else:
            for size, ch, seq, btype, frag in d[6]:
                if (btype & 0x0F) in (3, 4, 5):
                    last[(d[1], d[2], ch)] = seq
    parsed = [C.parse_datagram(bytearray(bytes.fromhex(d[4]))) for d in dgs]
    for d, p in zip(dgs, parsed):
        d[6] = [b[:5] for b in p[4]]
    # The messages written before each datagram (W lines). A message is written when its first
    # block appears, after every lower sequence number of its channel: the sender wrote those
    # first (the dump's order is arrival order). A sequence number the dump never shows (lost
    # on the way: sequenced channels, or the dump's gaps) gets a 1-byte filler, so that our
    # sender numbers the rest as the capture's did.
    msgs = {}
    parts = defaultdict(dict)
    for d, p in zip(dgs, parsed):
        raw = bytes.fromhex(d[4])
        for size, ch, seq, btype, frag, off, ln in p[4]:
            if size and (btype & 0x0F) in (3, 4):
                first = seq - frag if frag >= 0 else seq
                msgs.setdefault((d[1], d[2], ch, first), (size, btype & 0x0F, frag >= 0))
                parts[(d[1], d[2], ch, first)].setdefault(max(frag, 0), raw[off:off + ln])
    writes = defaultdict(list)
    nextw = Counter()
    incomplete = fillers = 0
    for d, p in zip(dgs, parsed):
        for size, ch, seq, btype, frag, off, ln in p[4]:
            if not size or (btype & 0x0F) not in (3, 4):
                continue
            chan = (d[1], d[2], ch)
            first = seq - frag if frag >= 0 else seq
            if first < nextw[chan]:
                continue
            s = nextw[chan]
            while s <= first:
                m = msgs.get(chan + (s,))
                if m is None:
                    writes[d[0]].append((ch, btype & 0x0F, b'\x00'))
                    fillers += 1
                    s += 1
                    continue
                msize, mode, fragmented = m
                got = parts[chan + (s,)]
                if fragmented:
                    n = (msize + 199) // 200
                    incomplete += any(i not in got for i in range(n))
                    data = b''.join(got.get(i, bytes(min(200, msize - 200 * i))) for i in range(n))
                else:
                    n, data = 1, got[0]
                writes[d[0]].append((ch, mode, data))
                s += n
            nextw[chan] = s
    if fillers:
        print('  %d sequence numbers the dump never shows: written as 1-byte fillers' % fillers)
    # Channel 0: the sender's counter as the capture has it (Z: stands in for the keep-alive timer,
    # not replayed), and the close (Q: the first non-empty channel-0 message, "CLOSE").
    zlines = defaultdict(list)
    closed = set()
    for d, p in zip(dgs, parsed):
        for size, ch, seq, btype, frag, off, ln in p[4]:
            if ch == 0 and (btype & 0x0F) == 5:
                if size and (d[1], d[2]) not in closed:
                    closed.add((d[1], d[2]))
                    zlines[d[0]].append('Z %d %s %s %d\n' % (d[0], d[1], d[2], seq))
                    zlines[d[0]].append('Q %d %s %s\n' % (d[0], d[1], d[2]))
                elif not size:
                    zlines[d[0]].append('Z %d %s %s %d\n' % (d[0], d[1], d[2], seq))
                break
    if incomplete:
        print('  %d fragmented messages written with fragments the dump lacks (zeros)' % incomplete)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    firsts = {}
    for d in dgs:
        firsts.setdefault(d[1], d[0])
    with open(path, 'w', newline='\n') as f:
        for s, k in sorted(keys, key=lambda k: firsts.get(k[0], 1 << 30)):
            f.write('K %s %s\n' % (s, k))
        for d in dgs:
            for zline in zlines.get(d[0], []):
                f.write(zline)
            for ch, mode, data in writes.get(d[0], []):
                f.write('W %d %s %s %d %d %s\n' % (d[0], d[1], d[2], ch, mode, data.hex()))
            f.write('D %d %s %s %d %s\n' % (d[0], d[1], d[2], d[3], d[4]))
    return keys, dgs


def ref_messages(dgs):
    """The messages rebuilt from the blocks (convert.messages' rules), with channel and datagram:
    [(session, dir, channel, whole message, n)]."""
    seen, frags, out, epoch = set(), {}, [], Counter()
    for d in dgs:
        raw = bytes.fromhex(d[4])
        p = C.parse_datagram(bytearray(raw))
        for size, ch, seq, btype, frag, off, ln in p[4]:
            if size == 0 or (btype & 0x0F) not in (3, 4):
                continue
            if (d[1], d[2], ch, seq) in seen:
                continue
            seen.add((d[1], d[2], ch, seq))
            if btype & 0x20:
                epoch[(d[1], d[2], ch)] += 1
            first = seq - frag if frag >= 0 else seq
            order = (epoch[(d[1], d[2], ch)], first)   # mode 4 drops old ones instead: they show as differences
            data = raw[off:off + ln]
            if frag >= 0:
                key = (d[1], d[2], ch, (seq - frag) & 0xFFFF)
                parts = frags.setdefault(key, {})
                parts[frag] = data
                if sum(len(x) for x in parts.values()) < size:
                    continue
                data = b''.join(parts[i] for i in sorted(parts))
                del frags[key]
            out.append((d[1], d[2], ch, data, d[0], order))
    return out


# ------------------------------------------------------------------------- checks

def seq_compare(ours, ref):
    sm = difflib.SequenceMatcher(None, ours, ref, autojunk=False)
    same = sum(i2 - i1 for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag == 'equal')
    return same, sm


def op(m):
    return int.from_bytes(m[2:4], 'little') if len(m) >= 4 else -1


def run(name, exe, aux):
    dgram = os.path.join(REPO, '.work', 'harness', name + '.dgram')
    inp = os.path.join(WORK, name + '.in.txt')
    res = os.path.join(WORK, name + '.out.txt')
    keys, dgs = prepare(dgram, inp)
    p = subprocess.run([os.path.abspath(exe), inp, res], capture_output=True, text=True)
    if p.returncode != 0:
        print(p.stdout[-2000:], p.stderr[-2000:])
        raise SystemExit('udpreplay failed: %d' % p.returncode)
    ours, status, gen, summary, opens = [], Counter(), defaultdict(list), [], []
    for line in open(res):
        q = line.split()
        if not q:
            continue
        if q[0] == 'R':
            ours.append((q[3], q[2], int(q[4]), bytes.fromhex(q[5]) if len(q) > 5 else b'', int(q[1])))
        elif q[0] == 'P':
            status[(q[2], q[4])] += 1
        elif q[0] == 'G':
            gen[(q[3], q[2])].append((int(q[1]), q[4], q[5]))
        elif q[0] == 'S':
            summary.append(line.strip())
        elif q[0] == 'O':
            opens.append(line.strip())
    print('%s: %d datagrams' % (name, len(dgs)))
    for k in sorted(status):
        print('  datagrams %s %s: %d' % (k[0], k[1], status[k]))
    for line in summary:
        print('  ' + line)
    if opens:
        print('  openc gave another channel than the capture: %d (first: %s)' % (len(opens), opens[0]))

    # (1) messages
    ref = ref_messages(dgs)
    ok = True
    for dr, label in (('S', 'server->client'), ('C', 'client->server')):
        o = [m for m in ours if m[1] == dr]
        r = [m for m in ref if m[1] == dr]
        per_o, per_r = defaultdict(list), defaultdict(list)
        for m in o:
            per_o[(m[0], m[2])].append(m[3])
        for m in r:
            per_r[(m[0], m[2])].append(m)
        # a reliable channel (mode 3) is delivered in sequence order, the dump has arrival order
        for k in per_r:
            per_r[k] = [m[3] for m in sorted(per_r[k], key=lambda m: m[5])]
        equal_ch = sum(1 for k in set(per_o) | set(per_r) if per_o.get(k) == per_r.get(k))
        same, sm = seq_compare([(m[0], m[2], m[3]) for m in o], [(m[0], m[2], m[3]) for m in r])
        print('  messages %s: ours %d, dump %d; channels equal in sequence order %d of %d; whole sequence in the dump order %d' % (
            label, len(o), len(r), equal_ch, len(set(per_o) | set(per_r)), same))
        bad = [k for k in sorted(set(per_o) | set(per_r)) if per_o.get(k) != per_r.get(k)]
        for k in bad[:6]:
            a, b = per_o.get(k, []), per_r.get(k, [])
            s2, sm2 = seq_compare(a, b)
            print('    channel %s %d: ours %d, dump %d, equal in order %d' % (k[0], k[1], len(a), len(b), s2))
            shown = 0
            for tag, i1, i2, j1, j2 in sm2.get_opcodes():
                if tag != 'equal' and shown < 3:
                    shown += 1
                    print('      %s ours[%d:%d] ops %s / dump[%d:%d] ops %s' % (
                        tag, i1, i2, [hex(op(x)) for x in a[i1:i2]][:4], j1, j2, [hex(op(x)) for x in b[j1:j2]][:4]))
        ok = ok and not bad
    trace = os.path.join(TRACES, name + '.trace.jsonl')
    if os.path.exists(trace):
        tr = []
        for line in open(trace, encoding='utf-8'):
            e = json.loads(line)
            data = bytes.fromhex(e.get('data') or '')
            tr.append((e['channel'].split(':', 1)[1], 'C' if e['dir'] == 'ClientToServer' else 'S',
                       int(e['op'], 16), data))
        mine = [(m[0], m[1], op(m[3]), m[3][4:]) for m in ours]
        same, sm = seq_compare(mine, tr)
        ca, cb = Counter(mine), Counter(tr)
        print('  all messages vs reference (%s): ours %d, reference %d; in order %d; reference\'s not delivered %d, '
              'ours not in reference %d' % (os.path.basename(trace), len(mine), len(tr), same,
                                         sum((cb - ca).values()), sum((ca - cb).values())))

    # (3) acks, (4) blocks, per session and direction
    acks_eq = acks_n = acks_ours = 0
    blk_eq = blk_n = blk_ours = 0
    blk_set_eq = 0
    examples = []
    groups = defaultdict(list)
    for d in dgs:
        groups[(d[1], d[2])].append(d)
    for key, ds in sorted(groups.items()):
        cap_acks = [tuple(map(tuple, d[5])) for d in ds if d[5]]
        my = gen.get(key, [])
        my_acks = []
        my_blocks = []
        for n, a, b in my:
            if a != '-':
                my_acks.append(tuple(tuple(map(int, x.split(','))) for x in a.split(';')))
            if b != '-':
                my_blocks += [tuple(map(int, x.split(','))) for x in b.split(';') if not x.startswith('0,')]
        same, sm = seq_compare(my_acks, cap_acks)
        acks_eq += same
        acks_n += len(cap_acks)
        acks_ours += len(my_acks)
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag != 'equal' and len(examples) < 6:
                examples.append((key, my_acks[i1:i2][:2], cap_acks[j1:j2][:2]))
        sent, cap_blocks = set(), []
        for d in ds:
            for size, ch, seq, btype, frag in d[6]:
                if ch and size and (btype & 0x0F) in (3, 4) and (ch, seq) not in sent:
                    sent.add((ch, seq))
                    cap_blocks.append((ch, seq))
        same, _ = seq_compare(my_blocks, cap_blocks)
        blk_eq += same
        blk_n += len(cap_blocks)
        blk_ours += len(my_blocks)
        blk_set_eq += set(my_blocks) == set(cap_blocks)
    print('  ack lists: capture %d, ours %d, equal in order %d' % (acks_n, acks_ours, acks_eq))
    for key, a, b in examples:
        print('    %s %s: ours %s capture %s' % (key[0], key[1], a, b))
    print('  message blocks (channel, seq), first sendings: capture %d, ours %d, equal in order %d; '
          'sessions/directions with the same set %d of %d' % (blk_n, blk_ours, blk_eq, blk_set_eq, len(groups)))

    if aux:
        auxin = os.path.join(WORK, name + '.aux.txt')
        k = 0
        with open(auxin, 'w', newline='\n') as f:
            for s, dr, ch, data, n in ours:
                if dr != 'S' or len(data) < 4:
                    continue
                chan = 'ea-udp:' + s
                o, body = op(data), data[4:]
                if o == 0x0004 and len(body) >= 11:
                    f.write('C %s %d %d\n' % (chan, struct.unpack_from('<i', body)[0], body[10]))
                elif o == 0x001B:
                    f.write('A %d %s %s\n' % (k, chan, body.hex()))
                    k += 1
        axe = os.path.join(REPO, 'build', 'programs', 'auxreplay', 'auxreplay.exe')
        axo = os.path.join(WORK, name + '.aux.out.txt')
        p = subprocess.run([axe, auxin, axo], capture_output=True, text=True, cwd=WORK)
        print('  AUX_DATA delivered by CordCon: %d; auxreplay exit %d' % (k, p.returncode))
        print('\n'.join('    ' + l for l in p.stdout.strip().splitlines()[-14:]))
    return ok


# ------------------------------------------------------------------------- the handshake

LIVE = roots.LIVE_PACKETS
PACKET = re.compile(r'^Packet #(\d+): (\d+) bytes, (Client->Server|Server->Client)\s+([\d.]+):(\d+)')
HEXLINE = re.compile(r'^ ((?:[0-9A-F]{2} )*[0-9A-F]{2})(?:\s{2,}|\s*$)')


def capture_handshakes(path):
    """(server ip, port, session, SYN1, ACK1, SYN2, RC4 key, ACK2) per handshake in an annotated
    capture (capture_1..3.txt), from the hex columns of its packets."""
    packets, cur, key_next = [], None, False
    for line in open(path, encoding='latin1'):
        m = PACKET.match(line)
        if m:
            cur = [int(m.group(1)), m.group(3)[0], m.group(4), int(m.group(5)), bytearray(), None]
            packets.append(cur)
            continue
        if line.startswith('RC4 session key'):
            key_next = True
            continue
        h = HEXLINE.match(line)
        if h and cur is not None:
            b = bytes.fromhex(h.group(1).replace(' ', ''))
            if key_next:
                cur[5] = b
                key_next = False
            else:
                cur[4] += b
    out, open_ = [], {}
    for n, d, ip, port, data, key in packets:
        if len(data) < 4 or data[1] != 0x14 or data[0] > 3:
            continue
        session = (data[2] << 8) | data[3]
        k = (ip, port, session)
        if data[0] == 0 and d == 'C':
            open_[k] = {'syn1': bytes(data)}
        elif k in open_:
            h = open_[k]
            if data[0] == 1 and d == 'S':
                h['ack1'] = bytes(data)
            elif data[0] == 2 and d == 'C':
                h['syn2'] = bytes(data)
                h['key'] = key
            elif data[0] == 3 and d == 'S' and 'syn2' in h and h.get('key'):
                h['ack2'] = bytes(data)
                out.append((ip, port, session, h))
                del open_[k]
    return out


def rsa_keys():
    """Westwood's RSA modulus and the server's private exponent (westwood_keys.py)."""
    import westwood_keys
    return westwood_keys.N, westwood_keys.D


def ip_int(ip):
    a = list(map(int, ip.split('.')))
    return (a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3]


def handshake(exe):
    n_mod, d_priv = rsa_keys()
    hs = []
    for name in ('capture_1', 'capture_2', 'capture_3'):
        for h in capture_handshakes(os.path.join(LIVE, name + '.txt')):
            hs.append((name,) + h)
    os.makedirs(WORK, exist_ok=True)
    inp = os.path.join(WORK, 'handshake.in.txt')
    res = os.path.join(WORK, 'handshake.out.txt')
    with open(inp, 'w', newline='\n') as f:
        for i, (name, ip, port, session, h) in enumerate(hs):
            f.write('H %d %d %d %d %s %s %s\n' % (i, ip_int(ip), port, session, h['key'].hex(),
                                                  h['ack1'].hex(), h['ack2'].hex()))
    p = subprocess.run([os.path.abspath(exe), inp, res], capture_output=True, text=True)
    if p.returncode != 0:
        raise SystemExit('udpreplay failed: %d %s' % (p.returncode, p.stderr[-500:]))
    got = defaultdict(dict)
    for line in open(res):
        q = line.split()
        if q and q[0] in ('HS1', 'HS2'):
            got[int(q[1])][q[0]] = bytes.fromhex(q[2])
        elif q and q[0] == 'HC':
            got[int(q[1])]['HC'] = q[2:]
    c = Counter()
    for i, (name, ip, port, session, h) in enumerate(hs):
        g = got.get(i, {})
        key = h['key']
        c['handshakes'] += 1
        c['SYN1 equal'] += g.get('HS1') == h['syn1']
        syn2 = g.get('HS2')
        if syn2:
            c['SYN2 header equal (bytes 0-15)'] += syn2[:16] == h['syn2'][:16]
            ours = pow(int.from_bytes(syn2[20:], 'big'), d_priv, n_mod)
            theirs = pow(int.from_bytes(h['syn2'][20:], 'big'), d_priv, n_mod)
            c['our SYN2 decrypts to the key'] += (ours & (1 << 64) - 1).to_bytes(8, 'little') == key
            c["the capture's SYN2 decrypts to its key"] += (theirs & (1 << 64) - 1).to_bytes(8, 'little') == key
            c['plaintext bits: ours as long as the capture\'s'] += ours.bit_length() <= n_mod.bit_length() - 1
        hc = g.get('HC')
        if hc and hc[0] != 'none':
            ack2 = h['ack2']
            c['CordCon from ACK2: data port, key, session'] += (int(hc[0]) == ((ack2[6] << 8) | ack2[7])
                                                              and hc[1] == key.hex() and int(hc[2]) == session)
    print('handshakes in capture_1..3: %d' % c['handshakes'])
    for k, v in c.items():
        if k != 'handshakes':
            print('  %s: %d' % (k, v))
    return all(v == c['handshakes'] for v in c.values())


def loop(name, exe, drops, step):
    """The loop test (udpreplay --loop): the dump's messages and closes, each side's own datagrams
    to the other over a link dropping every Nth datagram. Passes when every reliable (mode 3)
    channel delivers exactly what was written, in order, every sequenced (mode 4) channel a part of
    it in order, nothing is left queued or unacked, and every session that closed in the dump
    closes (state 4 on both sides)."""
    dgram = os.path.join(REPO, '.work', 'harness', name + '.dgram')
    inp = os.path.join(WORK, name + '.in.txt')
    keys, dgs = prepare(dgram, inp)
    written = defaultdict(list)
    modes = {}
    closing = set()
    for line in open(inp):
        q = line.split()
        if q and q[0] == 'W':
            key = (q[2], q[3], int(q[4]))
            written[key].append(bytes.fromhex(q[6]) if len(q) > 6 else b'')
            modes.setdefault(key, set()).add(int(q[5]))
        elif q and q[0] == 'Q':
            closing.add(q[2])
    ok = True
    for n in drops:
        res = os.path.join(WORK, '%s.loop%d.txt' % (name, n))
        p = subprocess.run([os.path.abspath(exe), '--loop', str(n), inp, res, str(step)],
                           capture_output=True, text=True)
        if p.returncode != 0:
            print(p.stdout[-2000:], p.stderr[-2000:])
            raise SystemExit('udpreplay --loop failed: %d' % p.returncode)
        got = defaultdict(list)
        refused = Counter()
        sides = []
        away = []
        for line in open(res):
            q = line.split()
            if q and q[0] == 'E':
                refused[(q[2], q[1], int(q[-1]))] += 1
            elif q and q[0] == 'R':
                got[(q[3], q[2], int(q[4]))].append(bytes.fromhex(q[5]) if len(q) > 5 else b'')
            elif q and q[0] == 'L':
                sides.append(dict(zip(q[3::2], map(int, q[4::2])), session=q[1], dir=q[2]))
            elif q and q[0] == 'A':
                away.append((q[1], int(q[3]), int(q[5])))
        equal = sub = bad = 0
        for key, msgs in written.items():
            g = got.get(key, [])
            msgs = msgs[:len(msgs) - refused[key]]
            if modes[key] == {3}:
                if g == msgs:
                    equal += 1
                else:
                    bad += 1
                    if bad <= 3:
                        print('    reliable channel %s: delivered %d of %d written' % (key, len(g), len(msgs)))
            else:
                it = iter(msgs)
                if all(m in it for m in g):
                    sub += 1
                else:
                    bad += 1
                    if bad <= 3:
                        print('    sequenced channel %s: delivered out of order' % (key,))
        left = sum(s['queued'] + s['unacked'] for s in sides)
        unclosed = sorted(s['session'] for s in sides if s['session'] in closing and s['close-state'] != 4)
        tot = Counter()
        for s in sides:
            for k in ('sent', 'dropped', 'resends', 'resend-requests', 'dup-acks'):
                tot[k] += s[k]
        if refused:
            print('    writes CordCon refused (written after the close reached that side): %d' % sum(refused.values()))
        print('  drop every %s: %d datagrams sent, %d dropped, %d resends (%d resend requests received, the '
              'rest on the timer), %d dup acks' % (n or 'none', tot['sent'], tot['dropped'], tot['resends'],
                                                   tot['resend-requests'], tot['dup-acks']))
        print('    reliable channels delivering every message in order: %d of %d; sequenced channels (or reopened '
              'with another mode: the harness writes on as the first) in order: %d of %d; left queued or unacked: '
              '%d; closed sessions: %d of %d' % (
                  equal, sum(1 for k in written if modes[k] == {3}), sub, sum(1 for k in written if modes[k] != {3}),
                  left, len(closing) - len(set(unclosed)), len(closing)))
        gone = sum(1 for s, sent, state in away if sent == 0 and state != 0)
        print('    go-away to the client of the %d sessions still open: %d sent nothing more and asked to close '
              '(the idle close, 10 s on)' % (len(away), gone))
        ok = ok and bad == 0 and left == 0 and not unclosed and gone == len(away)
    return ok


def conn(name, exe, drops, step):
    """udpreplay --conn against the plain runs (see the module's doc)."""
    dgram = os.path.join(REPO, '.work', 'harness', name + '.dgram')
    inp = os.path.join(WORK, name + '.in.txt')
    keys, dgs = prepare(dgram, inp)

    def run_one(args, res):
        p = subprocess.run([os.path.abspath(exe)] + args + [inp, res] + ([str(step)] if '--loop' in args else []),
                           capture_output=True, text=True)
        if p.returncode != 0:
            print(p.stdout[-2000:], p.stderr[-2000:])
            raise SystemExit('udpreplay %s failed: %d' % (' '.join(args), p.returncode))
        msgs, errors, sizes_bad, shorts, dropped = defaultdict(list), None, 0, 0, Counter()
        for line in open(res):
            q = line.split()
            if not q:
                continue
            if q[0] == 'R':
                data = bytes.fromhex(q[5]) if len(q) > 5 else b''
                if len(data) < 4:
                    shorts += 1     # the replay's 1-byte fillers: the factory refuses them
                    continue
                msgs[(q[3], q[2])].append(data)
            elif q[0] == 'M':
                payload = bytes.fromhex(q[6]) if len(q) > 6 else b''
                size, opcode = int(q[4]), int(q[5])
                sizes_bad += size != 4 + len(payload)
                msgs[(q[3], q[2])].append(struct.pack('<hH', size, opcode) + payload)
            elif q[0] == 'Z':
                dropped[(q[3], q[2])] += int(q[4])
            elif q[0] == 'S' and q[1:3] == ['conn', 'errors']:
                errors = int(q[3])
                shorts = int(q[5])
                BUILT[0] = int(q[7]) if len(q) > 7 else 0
                RECORDED[0] = int(q[9]) if len(q) > 9 else 0
        return msgs, errors, sizes_bad, shorts, dropped

    ok = True
    BUILT = [0]     # the messages the rebuilt packet classes' Build wrote (conn_replay.cpp)
    RECORDED = [0]  # D95(a): rebuilt classes whose client Build oversizes, sent as received (Recorded_Opcodes)
    for label, extra in [('capture replay', [])] + [('loop, drop every %d' % n, ['--loop', str(n)]) for n in drops]:
        tag = 'replay' if not extra else 'loop' + extra[1]
        plain, _, _, plain_short, _ = run_one(extra, os.path.join(WORK, '%s.%s.plain.txt' % (name, tag)))
        mine, errors, sizes_bad, shorts, dropped = run_one(['--conn'] + extra, os.path.join(WORK, '%s.%s.conn.txt' % (name, tag)))
        n_plain = sum(len(v) for v in plain.values())
        n_mine = sum(len(v) for v in mine.values())
        n_dropped = sum(dropped.values())
        keys_all = set(plain) | set(mine)
        # what a closed CordCon still held is the plain run's last messages of that session and direction;
        # a message whose size field isn't its length (the dump's cut messages, rebuilt with zeros or
        # short) comes out as long as its header says (the factory takes the header's size): compared
        # over the bytes CordCon delivered
        resized = 0
        timed = 0

        def same(a, b):
            if a == b:
                return True
            if masked(a) == masked(b):
                return True
            # D91: a recorded message whose own size field disagrees with its length (a message the dump
            # cut short: alex7 session 625d #89 says 0x1E0A) is compared over the common length, skipping
            # the 2-byte size field, which a rebuilt Build writes true
            if len(a) >= 4 and struct.unpack_from('<h', a)[0] != len(a) and b[2:len(a)] == a[2:len(b)]:
                return True
            return False

        equal = 0
        for k in keys_all:
            a = plain.get(k, [])
            a = a[:len(a) - dropped[k]]
            b = mine.get(k, [])
            if len(a) == len(b) and all(same(x, y) for x, y in zip(a, b)):
                equal += 1
                resized += sum(1 for x, y in zip(a, b) if x != y and masked(x) != masked(y))
                timed += sum(1 for x, y in zip(a, b) if x != y and masked(x) == masked(y))
        line = '  %s: CordCon::read %d, through UDPConnection and the stream %d (+ %d left in a closed CordCon: %s); ' \
               'sessions/directions equal in order %d of %d; size fields wrong %d; harness errors %s; under 4 bytes ' \
               '(no header: counted, not framed) %d of %d; size field not the length (compared over the length) %d; ' \
               'written by a rebuilt class\'s Build %d (time field masked: %d); sent as received (D95) %d' % (
                   label, n_plain, n_mine, n_dropped, ', '.join('%s %s' % k for k in sorted(dropped)) or '-', equal,
                   len(keys_all), sizes_bad, errors, shorts, plain_short, resized, BUILT[0], timed, RECORDED[0])
        good = equal == len(keys_all) and n_plain == n_mine + n_dropped and sizes_bad == 0 and errors == 0 \
            and shorts == plain_short
        if not extra:
            trace = os.path.join(TRACES, name + '.trace.jsonl')
            tr = []
            for tl in open(trace, encoding='utf-8'):
                e = json.loads(tl)
                tr.append((e['channel'].split(':', 1)[1], 'C' if e['dir'] == 'ClientToServer' else 'S',
                           int(e['op'], 16), bytes.fromhex(e.get('data') or '')))
            # the M lines in output order, as run's comparison takes the R lines
            seq = []
            for l2 in open(os.path.join(WORK, '%s.%s.conn.txt' % (name, tag))):
                q = l2.split()
                if q and q[0] == 'M':
                    seq.append((q[3], q[2], int(q[5]), bytes.fromhex(q[6]) if len(q) > 6 else b''))
            same, _ = seq_compare(seq, tr)
            ca, cb = Counter(seq), Counter(tr)
            line += '\n    vs reference (%s): ours %d, reference %d; in order %d; reference\'s not delivered %d, ours not in ' \
                    'reference %d' % (os.path.basename(trace), len(seq), len(tr), same, sum((cb - ca).values()),
                                    sum((ca - cb).values()))
        print(line)
        ok = ok and good
    print('%s conn: %s' % (name, 'PASS' if ok else 'FAIL'))
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['build', 'run', 'handshake', 'loop', 'conn'])
    ap.add_argument('name', nargs='?')
    ap.add_argument('--out', default=os.path.join(REPO, 'build'))
    ap.add_argument('--exe')
    ap.add_argument('--aux', action='store_true')
    ap.add_argument('--drop', default='0,10,4', help='loop: drop every Nth datagram, for each N')
    ap.add_argument('--step', type=int, default=20, help='loop: ms per datagram of the dump')
    ap.add_argument('--root', help=rootguard.HELP)
    a = ap.parse_args()
    rootguard.check(__file__, a.root)   # D112(4): build writes build/, the others .work/harness/ (and test that tree)
    if a.cmd == 'build':
        build(a.out)
        return 0
    exe = a.exe or os.path.join(a.out, 'programs', 'udpreplay', 'udpreplay.exe')
    if a.cmd == 'handshake':
        return 0 if handshake(exe) else 1
    if a.cmd == 'conn':
        return 0 if conn(a.name, exe, [int(x) for x in a.drop.split(',')], a.step) else 1
    if a.cmd == 'loop':
        return 0 if loop(a.name, exe, [int(x) for x in a.drop.split(',')], a.step) else 1
    return 0 if run(a.name, exe, a.aux) else 1


if __name__ == '__main__':
    sys.exit(main())
