# Capture harness tools (M1)

Python side of the M1 capture harness ([docs/milestones.md](../../docs/milestones.md) §2). The
C++ program is `tests/capharness/`. Inputs and the reference decoder's reference outputs live outside git:

| Path | What |
|---|---|
| `C:\EnBDevelopment\Live-Era-Data\enbserver3\ENB-Packets-Live\Alex3-fixed.txt.txt`, `Alex7-…` | EA's raw EA-UDP dumps (decrypted datagrams per session) |
| `C:\EnBDevelopment\the reference traces\captures\ea\*.trace.jsonl`, `session_*.trace.jsonl` | the reference decoder's message-level traces |
| `.work/harness/alex3.dgram`, `alex7.dgram` | datagram files written by `convert.py` (regenerate, ~10 s) |
| `.work/harness/ref/` | the reference decoder outputs per trace: `dump --summary` (`*.summary.txt`), `aux-census`, `roundtrip`, `aux-log --first` (regenerate with `sh tools/harness/refs.sh`, ~2 min) |
| `.work/harness/bin/capharness.exe` | the harness, built by `build.py` |

## Tools

```
python tools/harness/convert.py DUMP OUT.dgram [--check TRACE.jsonl]
ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/build.py        # after pool.py lease
python tools/harness/compare.py --exe .work/harness/bin/capharness.exe \
    --trace TRACE.jsonl --summary .work/harness/ref/NAME.summary.txt [--dgram OUT.dgram]
```

* **convert.py** parses a dump into a datagram file. `--check` rebuilds the game messages from
  the blocks (resends dropped, fragments joined) and compares them, in order, with the reference decoder's
  import of the same dump.
* **build.py** sends `tests/capharness` to a build VM (`vc6.sync`), runs one `cl /MT` that
  compiles and links, and copies the exe back. No `.lib` is linked yet.
* **compare.py** checks the harness's counts: (1) messages and payload bytes per direction and
  opcode from the trace against `the reference decoder dump --summary`; (2) the harness's datagram, ack,
  block and rebuilt-message counts against its own Python count of the same datagram file;
  (3) reports the messages rebuilt from datagrams against the trace's.

## The EA-UDP datagram (as the dumps show it)

| Offset | Field |
|---|---|
| 0 | 4-byte checksum |
| 4 | type: 0 data, 1 acks then data, 2 close, 3 close with acks (bit 0 = an ack list follows) |
| 5 | flags; with bit 0 of type set, the number of ack entries N |
| 6 | (bit 0 set) one byte: 0 from the client; from the server a varying value (0x00..0x51, falling over a session; meaning unknown) |
| 7 | (bit 0 set) N ack entries of 4 bytes: flag (0x02 on the first of a pair, else 0x00), channel, 16-bit big-endian sequence number |
| then | blocks to the end of the datagram |

A block: size (1 byte, or 2 with the top bit set: 15 bits), channel, 16-bit big-endian sequence
number, type. Types 0x03/0x04/0x23/0x24 carry one whole game message (`{u16 size, u16 opcode,
payload}`). Types with bit 0x10 (0x13, 0x33) carry a fragment: an index byte follows, the size is
the whole message's, 200 bytes per fragment (the last the rest), fragment k has sequence number
first + k. Type 0x05 (size 0) closes nearly every datagram; its sequence number counts the
sender's datagrams. Type 0x00 (2 seen) and the close datagram's block (`CLOSE`) carry no
message. Resent blocks repeat channel and sequence number.

the reference decoder's `the reference importer` does not decode the ack list: it takes the first offset from 6 at
which blocks fill the datagram. In 8,075 of alex3's 30,758 datagrams that offset lies inside the
ack list. This is where the reference decoder's 180 unread blocks come from (177 + 1 block numbers it reads
that are not ours), and it hides 28 + 12 real messages (below). With the ack list decoded, the
blocks fill every datagram exactly, except the 3 per file that the dump cut short.

## Datagram file format (`.dgram`, v1)

Text, one record per line, fields separated by one space, numbers decimal, `\n` line ends.
Lines starting with `#` are comments. Records belong to the last `D` line before them.

| Line | Fields |
|---|---|
| `K session key` | a session's RC4 key (8 bytes as 16 hex digits), where the dump prints it |
| `D n session dir seq len bytes type flags hex` | datagram n (from 0, file order); session as the dump writes it (e.g. `ffff9b25`, the 16-bit id sign-extended); dir `C` (client to server) or `S`; seq and len from the dump's header (both hex there); bytes: how many bytes the dump has (less than len for 3 datagrams per file); type and flags (bytes 4 and 5; -1 if absent); the whole decrypted datagram, checksum included, as hex digits |
| `A hdr n f ch seq ...` | the ack list: header byte, count, then n triples (flag, channel, sequence) |
| `B size ch seq type frag off len` | a block: size field, channel, sequence, type, fragment index (-1 if none), offset and length of its data in the datagram |
| `X text` | a problem with the datagram. Only `bytes missing from the dump between offsets a and b`: the blocks before the gap were read forwards, those after it are the ones that fill the datagram from the first offset after the gap; the block spanning the gap is lost |

## Results (2026-10-24)

| | alex3 | alex7 |
|---|---|---|
| datagrams (type 0 / 1 / 2 / 3) | 30,758 (15,782 / 14,973 / 3 / 0) | 3,430 (2,467 / 952 / 9 / 2) |
| ack entries, blocks | 20,722, 43,467 | 1,389, 6,537 |
| datagrams cut short in the dump | 3 | 3 |
| messages rebuilt from blocks | 26,106 | 2,791 (1 message lost to a gap) |
| the reference decoder messages | 26,078: all present, in the same order | 2,779: all present, in the same order |
| messages only in the datagram file | 28 | 12 |
| gaps in per-channel block sequence numbers | 2 (the reference decoder's reading: 64) | 2 (5) |

The extra messages are blocks that the reference decoder's start-offset guess swallowed; with them the
per-channel block sequence numbers are continuous except for 2 gaps per file, which makes them
real messages. Among them are 3 + 1 AUX_DATA (0x1B), including a 7,043-byte one in alex7, so the
AUX_DATA gates should count 4,289 / 366 for these files from the datagram file, against
the reference decoder's 4,286 / 365.

capharness (M1.0 skeleton) on every trace, checked by compare.py: message counts and bytes per
direction and opcode equal the reference decoder's summary for all 8 traces (alex3, alex7, capture_1..3, the
three Net-7 sessions); its datagram counts equal compare.py's for both datagram files.

## vidtest (L2, M2)

```
python tools/vc6/build.py link [--program vidtest]           # after build.py and mkmss32.py lib
python tools/harness/run_vidtest.py [--frames 120 | --mode close --seconds 3] [-- -depth 16 ...]
python tools/harness/symbolize.py build/programs/vidtest/vidtest.map build/programs/vidtest/vidtest.log
```

* **run_vidtest.py** runs `build/programs/vidtest/vidtest.exe` on the host (D48) and checks it:
  exit code 0, `WW3D::Init` and `Set_Render_Device` = 0 in `vidtest.log`, `WW3D::Shutdown` = 0,
  and the clear colour in vidtest's own screenshot (`shot.bmp`, its D3D back buffer: never a
  screen grab). `--mode close` posts `WM_CLOSE` as a user would. It prints the stub log
  (`stubs.log`, D44).
* **symbolize.py** names the addresses of vidtest's crash report (`CRASH:` lines in its log: the
  exception address and the stack words that point into its code) with the linker map.
