"""Convert the raw EA-UDP dumps (enbserver3\\ENB-Packets-Live\\Alex*-fixed.txt.txt) into a
datagram-level text file for the capture harness (format: tools/harness/README.md).

    python tools/harness/convert.py DUMP OUT.dgram [--check TRACE.jsonl]

--check rebuilds the game messages from the blocks (resends dropped, fragments joined, the
same rules as the reference importer) and compares them with the reference decoder's import of the same
dump, message by message.
"""
import argparse
import json
import re
import sys
from collections import Counter

DATAGRAM = re.compile(r'^session (\S+) (C->S|S->C)\s+seq (\S+)\s+len ([0-9a-fA-F]+)')
HEXROW = re.compile(r'^[0-9a-f]{4}: ((?:[0-9a-f]{2} ){1,16})')
KEYLINE = re.compile(r'^session: (\S+) rc4 key:\s*((?:[0-9A-Fa-f]{2}\s*){8})')
FRAGMENT_SIZE = 200


def read_dump(path):
    """Yields ('K', session, key bytes) and ('D', session, dir, seq, declared len, bytes)."""
    cur = None
    for line in open(path, encoding='latin1'):
        m = KEYLINE.match(line)
        if m:
            if cur:
                yield cur
                cur = None
            yield ('K', m.group(1), bytes.fromhex(m.group(2).replace(' ', '')))
            continue
        m = DATAGRAM.match(line)
        if m:
            if cur:
                yield cur
            cur = ('D', m.group(1), 'C' if m.group(2) == 'C->S' else 'S',
                   int(m.group(3), 16), int(m.group(4), 16), bytearray())
            continue
        m = HEXROW.match(line)
        if m and cur:
            cur[5].extend(bytes.fromhex(m.group(1)))
    if cur:
        yield cur


def read_blocks(d, pos, partial=False):
    """The blocks from pos to the end of d, or None if they don't fill it exactly.
    Each block: (size, channel, seq, type, fragment or -1, data offset, data length)."""
    out = []
    while pos < len(d):
        size = d[pos]
        pos += 1
        if size & 0x80:
            if pos >= len(d):
                return None
            size = ((size & 0x7F) << 8) | d[pos]
            pos += 1
        if pos + 4 > len(d):
            return None
        channel, btype = d[pos], d[pos + 3]
        seq = (d[pos + 1] << 8) | d[pos + 2]
        pos += 4
        frag, length = -1, size
        if btype & 0x10:
            if pos >= len(d):
                return None
            frag = d[pos]
            pos += 1
            length = min(FRAGMENT_SIZE, size - FRAGMENT_SIZE * frag)
        if length < 0 or pos + length > len(d):
            return out if partial else None
        out.append((size, channel, seq, btype, frag, pos, length))
        pos += length
    return out


def parse_datagram(d):
    """(type, flags, ack header, acks, blocks, problem). Layout after the 4-byte checksum:
    type (0 data, 1 acks then data, 2 close, 3 close with acks: bit 0 = an ack list), flags;
    with bit 0 set the flags byte is the ack
    count, then one byte (0 from the client, varies from the server) and count entries of
    4 bytes (flag, channel, 16-bit big-endian seq); then blocks filling the datagram."""
    if len(d) < 6:
        return None, None, None, [], [], 'short'
    dtype, flags = d[4], d[5]
    acks, hdr, pos = [], -1, 6
    if dtype & 1:
        hdr = d[6]
        pos = 7
        for i in range(flags):
            if pos + 4 > len(d):
                return dtype, flags, hdr, acks, [], 'ack list runs past the end'
            acks.append((d[pos], d[pos + 1], (d[pos + 2] << 8) | d[pos + 3]))
            pos += 4
    blocks = read_blocks(d, pos)
    if blocks is None:
        # The dump lost bytes in the middle (3 datagrams per file are shorter than their len).
        # Keep the blocks read forwards up to the gap, and the blocks that fill the datagram
        # from the first offset after them (as the reference importer finds blocks); the block whose data
        # spans the gap is lost, and a forward block that overlaps the tail is dropped.
        head = read_blocks(d, pos, partial=True)
        after = head[-1][5] if head else pos
        tail = []
        for start in range(after, len(d)):
            t = read_blocks(d, start)
            if t:
                tail = t
                head = [b for b in head if b[5] + b[6] <= start]
                break
        gap = (head[-1][5] + head[-1][6]) if head else pos
        return dtype, flags, hdr, acks, head + tail, \
            'bytes missing from the dump between offsets %d and %d' % (
                gap, tail[0][5] if tail else len(d))
    return dtype, flags, hdr, acks, blocks, None


def convert(dump, out):
    stats = Counter()
    datagrams = []
    keys = {}
    n = 0
    with open(out, 'w', newline='\n') as f:
        f.write('# capharness datagram file v1 (tools/harness/README.md); source %s\n' % dump)
        for rec in read_dump(dump):
            if rec[0] == 'K':
                keys[rec[1]] = rec[2]
                f.write('K %s %s\n' % (rec[1], rec[2].hex()))
                continue
            _, session, direction, seq, declared, d = rec
            dtype, flags, hdr, acks, blocks, problem = parse_datagram(d)
            f.write('D %d %s %s %d %d %d %d %d %s\n' % (
                n, session, direction, seq, declared, len(d),
                -1 if dtype is None else dtype, -1 if flags is None else flags, d.hex()))
            if dtype is not None and dtype & 1:
                f.write('A %d %d%s\n' % (hdr, len(acks), ''.join(' %d %d %d' % a for a in acks)))
            for b in blocks:
                f.write('B %d %d %d %d %d %d %d\n' % b)
            if problem:
                f.write('X %s\n' % problem)
                stats['problem: ' + problem.split(' between')[0]] += 1
            if declared != len(d):
                stats['datagrams shorter than their len'] += 1
            stats['datagrams'] += 1
            stats['datagram type %d' % (-1 if dtype is None else dtype)] += 1
            stats['blocks'] += len(blocks)
            stats['acks'] += len(acks)
            datagrams.append((session, direction, d, blocks, problem))
            n += 1
    return datagrams, stats


def messages(datagrams):
    """Game messages from the blocks, in order: (session, dir, block seq, opcode, payload)."""
    seen, frags, out, problems = set(), {}, [], Counter()
    for session, direction, d, blocks, problem in datagrams:
        for size, ch, seq, btype, frag, off, ln in blocks:
            if size == 0 or (btype & 0x0F) not in (3, 4):
                continue
            if (session, direction, ch, seq) in seen:
                continue
            seen.add((session, direction, ch, seq))
            data = bytes(d[off:off + ln])
            if frag >= 0:
                key = (session, direction, ch, (seq - frag) & 0xFFFF)
                parts = frags.setdefault(key, {})
                parts[frag] = data
                if sum(len(p) for p in parts.values()) < size:
                    continue
                data = b''.join(parts[i] for i in sorted(parts))
                del frags[key]
            if len(data) < 4 or int.from_bytes(data[:2], 'little') != len(data):
                problems['block that is not a game message'] += 1
                continue
            out.append((session, direction, seq, int.from_bytes(data[2:4], 'little'), data[4:]))
    problems['messages with missing fragments'] += len(frags)
    return out, problems


def check(datagrams, trace):
    ours, problems = messages(datagrams)
    ref = []
    for line in open(trace, encoding='utf-8'):
        e = json.loads(line)
        ref.append((e['channel'].split(':', 1)[1], 'C' if e['dir'] == 'ClientToServer' else 'S',
                    e.get('seq'), int(e['op'], 16), bytes.fromhex(e['data'])))
    print('messages: ours %d, reference %d' % (len(ours), len(ref)))
    for k, v in sorted(problems.items()):
        if v:
            print('  %s: %d' % (k, v))
    # Align: walk both lists; report the messages only one side has.
    import difflib
    sm = difflib.SequenceMatcher(None, ours, ref, autojunk=False)
    same = 0
    extra, missing = [], []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            same += i2 - i1
        else:
            extra.extend(ours[i1:i2])
            missing.extend(ref[j1:j2])
    print('identical and in the same order: %d; only ours: %d; only reference: %d' % (
        same, len(extra), len(missing)))
    for m in extra[:10]:
        print('  only ours: %s %s seq %d op 0x%04X len %d' % (m[0], m[1], m[2], m[3], len(m[4])))
    for m in missing[:10]:
        print('  only reference: %s %s seq %s op 0x%04X len %d' % (m[0], m[1], m[2], m[3], len(m[4])))
    return len(extra) == 0 and len(missing) == 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dump')
    ap.add_argument('out')
    ap.add_argument('--check')
    a = ap.parse_args()
    datagrams, stats = convert(a.dump, a.out)
    for k, v in sorted(stats.items()):
        print('%s: %d' % (k, v))
    if a.check:
        return 0 if check(datagrams, a.check) else 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
