# Capture harness tools (M1)

Python side of the M1 capture harness ([docs/milestones.md](../../docs/milestones.md) §2) and
of the later harness programs (M1.2 to M2, listed at the end of this file). The C++ programs are
under `tests/capharness/`. Every external repository and data path is named once, in
`tools/re/roots.py` (listed in [docs/toolchain.md](../../docs/toolchain.md) §3). Inputs and
the reference decoder's reference outputs live outside git:

| Path | What |
|---|---|
| `C:\EnBDevelopment\Live-Era-Data\enbserver3\ENB-Packets-Live\Alex3-fixed.txt.txt`, `Alex7-…` | EA's raw EA-UDP dumps (decrypted datagrams per session) |
| `C:\EnBDevelopment\the reference decoder\captures\ea\*.trace.jsonl`, `session_*.trace.jsonl` (was `the reference traces\captures`) | the reference decoder's message-level traces |
| `.work/harness/alex3.dgram`, `alex7.dgram` | datagram files written by `convert.py` (regenerate, ~10 s) |
| `.work/harness/ref/` | the reference decoder outputs per trace: `dump --summary` (`*.summary.txt`), `aux-census`, `roundtrip`, `aux-log --first` (regenerate with `sh tools/harness/refs.sh`, ~2 min) |
| `.work/harness/bin/capharness.exe` | the harness, built by `build.py` |

## Tools

```
python tools/harness/convert.py DUMP OUT.dgram [--check TRACE.jsonl]
ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/build.py        # after pool.py lease
sh tools/harness/refs.sh                                        # the reference decoder reference outputs -> .work/harness/ref/
```

* **convert.py** parses a dump into a datagram file. `--check` rebuilds the game messages from
  the blocks (resends dropped, fragments joined) and compares them, in order, with the reference decoder's
  import of the same dump.
* **build.py** sends `tests/capharness` to a build VM (`vc6.sync`), runs one `cl /MT` that
  compiles and links, and copies the exe back. No `.lib` is linked yet (the M1.0 skeleton; the
  later programs link through `tools/vc6/build.py link`, see [../vc6/README.md](../vc6/README.md)).
* **refs.sh** saves the reference decoder's reference outputs per trace under `.work/harness/ref/` (~2 min).
* `compare.py`, the M1.0 count check of the skeleton against the reference decoder's summary, was retired
  (2026-10-25): `udpreplay.py` and `auxreplay.py` carry the gates now. `git log -- tools/harness/compare.py`
  has it.

## The EA-UDP datagram (as the dumps show it)

| Offset | Field |
|---|---|
| 0 | 4-byte checksum |
| 4 | flags (wire byte 9, `cordcon.h`): bit 0 = an ack list follows, bit 1 = closing; so 0 data, 1 acks then data, 2 close, 3 close with acks |
| 5 | with bit 0 set, the number of ack entries N |
| 6 | (bit 0 set) one byte: 0 from the client; from the server a varying value (0x00..0x51, falling over a session; meaning unknown) |
| 7 | (bit 0 set) N ack entries of 4 bytes: kind (0 ack, 1 resend request, 2 everything up to; `cordcon.h`), channel, 16-bit big-endian sequence number |
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

capharness (M1.0 skeleton) on every trace, checked by compare.py (retired): message counts and bytes per
direction and opcode equal the reference decoder's summary for all 8 traces (alex3, alex7, capture_1..3, the
three Net-7 sessions); its datagram counts equaled compare.py's for both datagram files.

## AuxData round trip and values (M1.3, gates (b) and (c))

```
python tools/vc6/build.py --lib app [--out DIR]                   # app.lib with app/client/aux_roots.cpp
ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/auxreplay.py build [--out DIR] [--app-sources]
python tools/harness/auxreplay.py run capture_1 [--out DIR]      # also capture_2, capture_3, alex3, alex7
python tools/harness/auxreplay.py run alex3 --dump [--out DIR]   # the messages rebuilt from the EA dump (D52)
```

* **tests/capharness/auxreplay** links the generated roots (`app/client/aux_roots.h`, tools/re/auxroots.py)
  and the auxdata library. Per channel it makes a root for each CREATE (0x0004: the type byte at
  offset 10, through the factory 0x6F6000's cases; a repeated id replaces the object, as the reference decoder's
  `aux-census`) and one Player root (game id 0). Each AUX_DATA (0x001B) goes through the packet
  entry 0x7364C0's body: `LinearBufferClass(payload + 6, len - 6, 0x400, false)`, then
  `root->Decode(buf, base)` (0x4A3FA0). The roots are wrapped (`Tracked<R>`) so that the
  notifications that reach the root (Unknown_3C updated, Unknown_40 defaulted, Unknown_44 cleared)
  give the three lists the message carried; these are re-encoded in the message's form
  (`RecordTableClass` for the tree form, `DeltaListSetClass` for the record form) and compared byte
  for byte. It prints a census like `aux-census` and writes every touched leaf (U index path value
  as Write gives it, D, X) to `.work/harness/auxreplay/NAME.out.txt`. Asserts go to
  `NAME.stubs.log` (the stub G_ASSERT) and are counted.
* **auxreplay.py** prepares the message file from the trace (or, `--dump`, from convert.py's
  reading of an Alex dump), builds (`--app-sources` compiles the two new app files into the program
  for an older app.lib), runs, and compares the values with the reference decoder's `aux-log --first`
  (`.work/harness/ref/NAME.aux-log-first.txt`): it replays the U/D/X lines over a per-object state
  (unknown, default, value; reset by a CREATE), prints each change in tree order as aux-log does
  (ints `N (0xN)`, floats G6, colours `AuxColor { R = .., G = .., B = .. }`, text other than ASCII
  as `?`) and compares the sequences per channel, ignoring time and object description.

Results (2026-10-24):

| Trace | AUX_DATA | decoded, no assert | re-encoded byte for byte | values equal aux-log |
|---|---|---|---|---|
| capture_1 | 34,070 | 34,070 | 34,069 (1: below) | 681,376 of 681,376 |
| capture_2 | 4,741 | 4,741 | 4,741 | 147,771 of 147,771 |
| capture_3 | 5,167 | 5,167 | 5,167 | 337,019 of 337,019 |
| alex3 (trace / dump) | 4,286 / 4,289 (+1 before its CREATE) | all | all | 37,601 of 37,601 (trace) |
| alex7 (trace / dump) | 365 / 366 (+6 before their CREATE) | all | all | 34,626 of 34,626 (trace) |

The one message the client's encoder can't give back: capture_1's AUX_DATA #24128 (Hulk
3837051, `7b8c3a00 0500 01 080c0955`). The root marks Equipped and Cargo cleared, but Cargo's own
header byte is 0x55: whole + defaulted, with bits 4 and 6 set besides (the client's BitSetClass(4)
writes them 0). The client decodes it (Cargo defaulted) without an assert; our encoder writes
`0c 08 04 09 05`. the reference decoder keeps the header byte as read, so its census passes it.

**The client's own encoder (M1.3, 2026-10-24).** After each decode the harness also has the root
write an update through the client's path, `UnknownRootClass::Encode` 0x4A3D50 (the collector
`DeltaCollectorClass`, a policy, the change table `ChangeTableClass`), and compares it with the
message: if the server's encoder is Westwood's same auxdata code, the message comes back. The
server's fields count their changes up (`DeltaClass::Unknown_34`) where the client's decode sets 2,
so the harness keeps a server-side version per field (an updated field and its parents get new
ones, a defaulted one keeps its own) and puts it in `m_Unknown70` before the encode. Player uses
its own state (`UnknownSendRootClass::Unknown_48`, `Encode_Changes` 0x4A4660); the factory's roots
get a policy made by the harness through policy.cpp: "*.*" INCLUDE 1 before "*" INCLUDE, so the
compile gives the root DESCEND (Player's own "*" alone gives a small root INCLUDE and whole-root
updates, which the captures never show for the factory's roots). `E` lines in `NAME.out.txt` (equal, form, differ, none), and for a message
not given back `e` (our collector's lists) and `m` (the message's).

| Trace | AUX_DATA | encode equals the message | differ | nothing sent (the message has something) |
|---|---|---|---|---|
| capture_1 | 34,070 | 31,450 | 1,854 | 766 |
| capture_2 | 4,741 | 4,117 | 500 | 124 |
| capture_3 | 5,167 | 4,149 | 663 | 355 |
| alex3 (trace) | 4,286 | 4,121 | 135 | 30 |
| alex7 (trace) | 365 | 357 | 8 | 0 |

The tree or record form is the message's in every case, and no message that differs differs in an
updated field: all of them differ in defaulted or cleared fields only, the server's policy (fields it
excludes for other players' ships, Harvestable's resources and TechLevel, a Hulk's Cargo and
Equipped until something includes them; then the never-sent fields go out as defaulted, which our
everything-INCLUDE policy sends at once). Player: 4,371 of 4,382 equal (the 11: ClientSendUITriggers
and GroupInfo defaulted later).

## EA-UDP transport (M1.4, gate (a))

```
ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/udpreplay.py build [--out DIR]
python tools/harness/udpreplay.py run alex3 [--aux]                 # also alex7
python tools/harness/udpreplay.py loop alex3 [--drop 0,10,4] [--step 20]
python tools/harness/udpreplay.py conn alex3 [--drop 0,10,4]          # through UDPConnection and the stream framing
```

* **The wire datagram** (app/wwudp/cordcon.h): the dumps start at byte 5 of it. Bytes 0-4 are
  the type (5 from the client, 4 from the server), the session id and the sender's datagram
  counter (CordCon +0x8C), which is the dump's `seq`. Bytes 5 to the end are RC4-encrypted with a
  key per datagram: the session key, the encrypted length (4 bytes), the counter (2), 0 (2).
  The dump's first 4 bytes are the client's checksum (0x8B3EC0: reflected CRC-32, init
  0xFFFFFFFF, no final xor) of the whole wire datagram with that field zero. So the harness
  rebuilds the wire bytes and encrypts them, and the rebuilt CordCon checks the captured checksum:
  every datagram in both dumps passes, apart from the 3 per file the dump cut short.
* **tests/capharness/udpreplay** links app\wwudp (cordcon.cpp, cordmgr.cpp, unknown_cordmsg.cpp, compiled into the
  program) and WWLib's RC4Class. Its `UDP` class (app/wwudp/udp.h) is a replay socket. Each
  session gets a client-side and a server-side CordCon, each behind its own CordMgr. A captured
  datagram goes into the receiving side's socket, then CordMgr::Pump, Process_CORD_Packet,
  CordCon::handlePacket and CordCon::read until nothing is left (`R` lines). Before each
  datagram, the sending side writes the messages the datagram sends for the first time
  (CordCon::openc for a new channel, then CordCon::write) and pumps (CordCon::pump: buildHeader's
  ack list, buildMsgHeader's blocks; `G` lines). Its sequence numbers and the acks it gets then
  follow the capture. Three things stand in for what the dump can't give:
  * the receive is CordMgr::Pump's data path (Deliver: Process_CORD_Packet) without Pump's tail,
    which pumps every connection: each side pumps when the capture has it send. The handshake and
    the loop test use the whole CordMgr::Pump (its tail sends SYN2 after ACK1);
  * the 3 datagrams per file that the dump cut short are rebuilt from their readable blocks, with
    a new checksum;
  * a sequence number the dump never shows (lost on the way, or in a cut) is written as a 1-byte
    filler, and messages are written in sequence order (the dump's order is arrival order);
  * channel 0's counter is set from the capture (`Z`): the keep-alive timer that advances it isn't
    replayed. The close is CordCon::close (`Q`) before the first "CLOSE".
* **udpreplay.py** prepares the input, runs the harness and compares. It checks (1) the delivered
  messages per channel against the messages rebuilt from the dump's blocks, in sequence order: a
  reliable channel (mode 3) delivers in sequence order, so the dump's arrival order differs
  where blocks came out of order; it also checks the whole sequence against the reference decoder's. It checks
  (2) which datagrams were accepted, (3) the ack lists against the capture's, in order, and (4)
  the message blocks' (channel, seq). `--aux` sends the CREATE / AUX_DATA messages the client
  side delivered through auxreplay (M1.3), which chains gates (a) and (b).

Results (2026-10-24):

| | alex3 | alex7 |
|---|---|---|
| datagrams accepted | 30,757 of 30,758 (1: a cut datagram with no readable block) | 3,429 of 3,430 (1: below) |
| messages delivered | 26,106: all 26,078 of the reference decoder's and the 28 extra, every channel equal in sequence order | 2,782: 2,770 of the reference decoder's 2,779 and the 12 extra; 43 of 44 channels equal |
| ack lists equal, in order | 14,933 of 14,973 | 940 of 954 |
| message blocks: our (channel, seq) | 24,772 of 26,465 in the capture's order; all sessions but one the same set | 4,596 of 4,645; 13 of 16 the same set |
| our datagrams that the peer's examinePacket accepts | 30,452 of 30,452 | 3,087 of 3,088 |
| AUX_DATA through auxreplay (`--aux`) | 4,290 delivered, all decode and re-encode | 370 delivered, 228 checked and passed, 142 for objects whose CREATE is held (below) |

**The handshake** (`python tools/harness/udpreplay.py handshake`): the 54 SYN1 / ACK1 / SYN2 / ACK2
exchanges in capture_1..3 (the annotated captures print every handshake byte and the session key).
A client-side CordMgr connects with the capture's session id and key (CordMgr::Connect,
Pump_Connections), and the capture's ACK1 and ACK2 go through CordMgr::Pump. All 54 pass: SYN1 equals
the capture's, SYN2's header (bytes 0-15) equals it, our SYN2's encrypted key decrypts with the
server's private key (the reference decoder's the reference key table) to the session key in its low 64 bits, as the
capture's does, and ACK2 makes a CordCon with the capture's data port, key and session. This runs
WWLib's XMP (RSA) and SecureRandomClass for the first time.

**Time.** CordCon's send side is the client's (pumpInternal 0x853A30's priority order, resend timer
and send window, CordStatsClass at +0x68), on wlib's `Wtime` (app/wwudp/wtime.h). The harness
supplies Wtime with its own clock (`wtime_replay.cpp`): `udpreplay IN OUT [MS [WINDOW]]` advances it
MS ms per datagram (default 0: the dumps have no times, so a replay fires no resend timer) and
gives each CordCon the send window limit WINDOW (ACK2's value, which the dumps lack; default
65,535). With the clock frozen the replay's results are the ones above; the only change from the
send side is the keep-alive: an empty channel-0 message before the first datagram arrives (+3
datagrams, all accepted by the peer).

**The loop test** (`udpreplay.py loop NAME`, `udpreplay --loop N`): the dump's messages (W) and closes
(Q) as in the replay, but each side's own datagrams go to the other side over a link that drops
every Nth datagram in each direction, the clock advancing 20 ms per datagram of the dump, then on
until nothing is queued, unacked or due. It tests what a capture can't: resend requests, the
resend timer, the window and the close. Results (2026-10-24), every one passing:

| | alex3 | alex7 |
|---|---|---|
| no loss | 31,485 datagrams, 0 resends | 3,528 datagrams, 0 resends |
| every 10th dropped | 35,186 sent, 3,516 dropped, 5,041 resends (1,017 resend requests received, the rest on the timer) | 4,130 sent, 407 dropped, 823 resends (378 requests) |
| every 4th dropped | 37,248 sent, 9,307 dropped, 16,065 resends (1,873 requests), 1 dup ack | 5,379 sent, 1,334 dropped, 3,432 resends (782 requests), 24 dup acks |
| each case | the 21 reliable channels deliver every message in order; the 33 sequenced (or reopened) ones in order; nothing left; the 3 closing sessions closed on both sides | 52 of 52 reliable, 6 of 6 sequenced; nothing left; 7 of 7 sessions closed |

A channel the dump reopens with another mode is written on as first opened (the harness has no
channel close), so it is checked as a sequenced one. alex7's 3 writes after the close reached that
side are refused, as in the replay. Last, each session still open (one in each file) gets a
go-away (type 6, CordMgr::Process_Go_Away_Packet) on its client side, the server side silent: the
client sends nothing more, and the idle close (CordCon::Go_Away: 90 s since the last datagram, 10 s
on) asks for the close (state 2: "CLOSE" queued, never sent): passes in every case.

What doesn't match, and why:
* **alex7, session 625d, server channel 4.** The dump cut datagrams #109-#111 and lost sequence
  numbers 218-221 and 223-229 with them. The client acked them (#129 acks up to 263), so it got
  them; the replay can't. The channel is reliable, so CordCon holds the 9 messages after the gap
  (it asks for them back: kind-1 entries in its ack list), and with them the CREATEs that 142
  AUX_DATA messages need.
* **alex7 #1720.** The client wrote two messages after the server's CLOSE had reached it in the
  dump's order. CordCon::write refuses while closing, so the server's ack for them fails
  examinePacket. The client must have written them before the CLOSE arrived.
* **Ack lists** differ by timing. The dump records the server's datagrams when they arrive and
  the client's when they leave, so the server-side acks it shows aren't in the server's own
  time order. The client also splits acks between datagrams where we don't, after channels were
  closed and reopened (the client drops closed channels from its active list: not rebuilt).
* **Blocks**: our side writes each message just before the datagram that first carries it and
  then sends what the window allows at once (the clock is frozen); the client wrote and pumped at
  its own times, so the per-datagram order differs. The sequence numbers are the same apart from alex7's 625d gap and
  the writes refused after the close.
* openc picks the channel the capture shows, except for 11 of the server's reopened channels in
  alex3: the server reuses the channel written least recently (time(0)), which the replay doesn't have.

**The connection layer** (`udpreplay.py conn NAME`, `udpreplay --conn [--loop N] ...`;
docs/kernel-map.md 4, 5): what each receiving CordCon delivers is read through app\common's
UDPConnection::Receive (CordCon::read into the shared message buffer, the packet factory 0x8696C0:
`conn_replay.cpp` supplies a test packet holding the bytes, wlib's CritSec and the rate average),
then each packet goes through the stream connection (connect.cpp, vtable 0xB085F4): Send_Packet
writes the {size, opcode} header and the payload to a memory transport, Receive reads them back
through the 100,000-byte ring (circbuff.cpp) in chunks of 1 to 4,096 bytes, the factory again. The
{size, opcode, payload} messages (M lines) must equal, in order per session and direction, what
CordCon::read gives in the plain run, each size field 4 + the payload, the whole sequence compared
with the reference decoder's trace.jsonl as `run` does. Results (2026-10-24): alex3 26,106 of 26,106 (vs the reference decoder:
all 26,078 and the 28 extra, as `run`), alex7 2,781 + 1 (2,782): the last client message of session
ffffaf39 arrives with the close, and UDPConnection::Receive, as the client's, checks the CordCon
first and disconnects (CordMgr::Release) without reading what is left (counted, a Z line). The loop
runs (drop none / 10 / 4) equal the plain loop runs in every session and direction. Two harness
cases are counted, not framed: the replay's 1-byte fillers (no header: the client's factory would
read the previous message's from the shared buffer; alex3 1, alex7 10 in the loop runs), and one
alex7 message whose size field (7,690) isn't the 264 bytes CordCon delivers (the dump's cut
message: the factory takes the header's size), compared over the delivered bytes and, since its own
size field disagrees with its length, without the 2-byte size field (D91: a rebuilt Build, Server_Ready
59's, writes the true size; the rule applies only where the recorded size field is wrong). The UDPConnection
is made over each side's CordCon with the constructor for an accepted one (0x82CBF0); the default
channel it opens is taken back, since the replay opens the capture's channels.
A packet class rebuilt in the tree sends what its own Build writes (conn_replay.cpp; counted as
"written by a rebuilt class's Build"). Two Builds stamp a field with the aux-data clock
(Unknown_Get_Ticks, 0 in the harness): CLIENT_SET_TIME 52 (ServerSent, message byte 12) and REQUEST_TIME 68
(TimeSent, byte 4). Those two opcodes are compared with that field zeroed (`TICKS` in udpreplay.py; counted as
"time field masked"); every other byte must still be equal. D95(a): a rebuilt class whose client Build writes a
size field larger than the payload it adds can't give the received message back, so for the opcodes in
conn_replay.cpp's explicit list `Recorded_Opcodes` (AVATAR_DESCRIPTION 97: Get_Data_Size 0x110 for 0x108 bytes;
183: an entry sized 0x4C for 0x49 bytes) the harness sends the message as received instead: on the UDP side
from UDPConnection::Receive's buffer (`Conn_Receive_Buffer`, which `udpreplay.py build` defines after
udpconn.cpp), on the stream side the bytes its transport read (a size field that isn't the length is a harness
error). The rebuilt raw constructor still parses the message; its Build is not compared. Counted as "sent as
received (D95)", once per side (alex3 38, alex7 22 for 97). avatar_desc.cpp is compiled in for 97's member,
its unbuilt callees in `avatar_desc_stubs.cpp`.

## Net-7 recordings (M1.5, gate (d))

```
python tools/harness/n7aux.py [SESSION ...]        # default: the three 2026-10 sessions; auxreplay.exe from auxreplay.py build
```

* **n7aux.py** runs every AUX_DATA the client received in a Net-7 recording
  (`the reference decoder\captures\session_*.trace.jsonl`, message level, the proxy's output)
  through auxreplay.exe, as `auxreplay.py run` does, and counts Net-7's known encoding differences
  (the reference decoder's `docs/research/auxdata.md` §7, items 1-3) instead of failing on them. Each is checked:
  `length` (item 1): the length field differs from the bytes that follow and everything else
  re-encodes byte for byte; `members` (item 2): a full Player send that runs out exactly where
  `Player.GroupInfo.Members`' header should be, which must decode and re-encode byte for byte
  with EA's `05` (Whole | Default) appended (the Members fields that `05` defaults are dropped
  again for the values: the client keeps what it read); `short` (item 3): a tree-form update that
  declares more than it carries and runs out; `sets` (not in §7): a field or a whole container
  named in more than one of Value / Default / Clear (Ship's Rank and EnergyPercent in Value and
  Default; Player's Whole header `0f` where EA's has `07`). The client's decoder applies one of
  them, so its encoder can't give the message back; a second run with our re-encoding in the
  message's place must touch the same fields with the same values and re-encode byte for byte.
  The values are compared with the reference decoder's `aux-log --first` with one change to auxreplay.py's
  comparison: a field changes when its bytes change, not its G6 text (aux-log lists
  `CombatExp: 0.255702 -> 0.255702`).

Results (2026-10-24), the reference decoder's aux-census in brackets:

| Session | AUX_DATA | ok | length | members | short | sets | noroot | failed | values equal aux-log |
|---|---|---|---|---|---|---|---|---|---|
| 2026-10-19 02:31 | 292 | 250 | 36 (36) | 2, all also `sets` | 0 | 4 | 0 | 0 (2) | 18,579 of 18,579 |
| 2026-10-19 11:53 | 1,358 | 976 | 340 (340) | 8, all also `sets` | 2 | 32 | 0 | 0 (10) | 81,377 of 81,377 |
| 2026-10-21 11:16 | 691 | 636 | 34 (34) | 5, all also `sets` | 0 | 14 | 2 (2) | 0 (5) | 38,171 of 38,171 |

the reference decoder's failures are exactly the `members` and `short` messages (2 + 0, 8 + 2, 5 + 0). The
length fields: declared minus carried is +6 for 319 Ship deltas (the whole payload's length), -8
for 76 Ship, -4 for 11 Harvestable and -2 for 4 Hulk updates. No asserts.

**2022-09-25 startup-charlogin** (`n7aux.py --server-side 2022-09-25_startup-charlogin`): this
recording has no client side of the sector. It holds the client's one TCP connection to the
Net-7 global server (3 messages each way, the login) and the proxy's UDP to the sector server
(Net-7's own protocol, not the client's). `--server-side` takes what the sector server sent the
proxy as what the client got, and makes the reference decoder's aux-census and aux-log of that copy in
`.work/harness/auxreplay/`. Its 5 AUX_DATA: 2 ok, 1 Player `members` (also `sets`), 2 `sets`
(Ship, ManufacturingLab), none failed (aux-census: 1, the Player); values 8,349 of 8,349.

## vidtest (L2, M2)

```
python tools/vc6/build.py link [--program vidtest]           # after build.py and mkmss32.py lib
python tools/harness/run_vidtest.py [--frames 120 | --mode close --seconds 3] [-- -depth 16 ...]
python tools/harness/symbolize.py build/programs/vidtest/vidtest.map build/programs/vidtest/vidtest.log
python tools/harness/vidtest_sbs.py run; python tools/harness/vidtest_sbs.py sbs [N ...]
```

* **run_vidtest.py** runs `build/programs/vidtest/vidtest.exe` on the host (D48) and checks it:
  exit code 0, `WW3D::Init` and `Set_Render_Device` = 0 in `vidtest.log`, `WW3D::Shutdown` = 0,
  and vidtest's own screenshot (`shot.bmp`, its D3D back buffer: never a screen grab).
  Since M2.2 vidtest runs VIDTEST's scenes and tests (art from EBCONFIG through its file factory,
  `-art DIR`; options in the header of `programs/vidtest/vidtest.cpp`). `--check model` (default):
  at least 2% of the pixels drawn with at least 200 colours; frame 120 is the polygon test, the
  second screenshot `shot2.bmp` (frame 60) the fill rate test; both are also written as `.png`.
  `--check clear` is L2's test (`-noscene`, the clear colour). `--mode close` posts `WM_CLOSE` as a
  user would and expects VIDTEST's exit code -3 (or 1 when the tests, about 2 s, ended first). It prints the stub log (`stubs.log`, D44). `vidtest.log` lists every file the
  libraries asked the file factory for (`file: NAME found|MISSING`).
* **bmp2png.py** converts a screenshot to `.png` (no Pillow needed).
* **vidtest_sbs.py** compares our vidtest with the original frame by frame (M2.2): `run` starts
  vidtest with the reference's arguments and `-shots` set to the frames in `ref/` (one number per
  Present, from 1, as run_reference.py counts them; vidtest runs its tests to the end), `sbs` writes
  ours (left) and the original's (right) side by side to `build/programs/vidtest/sbs/` and prints
  the share of pixels that differ. `run` passes the original's sun angle (`-sunangle`, from the
  GetTickCount stamps in `ref/d3d8shot.log`): VIDTEST turns its sun with the clock, and a random
  sun alone made 35-40% of the land frames differ.
* **run_reference.py** runs the ORIGINAL `VIDTEST.EXE` for reference frames: `build` compiles
  `d3d8shot.c` on a pool VM into `build/d3d8shot/d3d8.dll`; `run` copies EBCONFIG to
  `build/programs/vidtest/ref_ebconfig/` (once: the original writes `results.ini` into its own
  folder), puts the proxy beside the copy's `VIDTEST.EXE` and runs it there. The proxy passes
  everything to the system's d3d8.dll, saves the program's own back buffer at chosen `Present`
  calls (`--every 50`, `--frames 1,10,30`; never a screen grab) and logs the program's
  `CreateFileA` calls. Output: `build/programs/vidtest/ref/` (`d3d8shot.log`, `frame_NNNNN.bmp`
  and `.png`, `results.ini`, `run.txt`). Options go before the subcommand. `--dump 500,900` also
  writes every indexed draw call of those frames to `draws_NNNNN.bin` (vertices and indices read
  from shadow copies of the locked buffers, transforms, textures, fog/blend/cull render states and
  texture stage states, read back with the device's Get calls) and saves each 32-bit texture they
  use as `tex_NNNNN_PTR.bmp` (the land's lightmap); `--out DIR` keeps `ref/`; `--exe PATH` runs
  another D3D8 program (our vidtest: add `-nomsgbox`) through the same proxy, so both sides are
  dumped alike. Example: `run_reference.py --every 0 --frames 900 --dump 900 --out DIR --exe
  build/programs/vidtest/vidtest.exe run -- -nomsgbox -resx 640 -resy 480 -depth 32 -windowed 1`.
* **drawdump.py** reads those dumps: `groups FILE` (draws by FVF and textures), `list FILE [--fvf
  202]`, `show FILE N` (one draw's states and first vertices); `read()` for scripts (M2.4 settled
  the land's orientation, fog, culling, trees and lightmap with it).
* **drawcheck.py** is M2's standard regression check beside the frame comparison (D78):
  `python tools/harness/drawcheck.py run` dumps frame 900's draw calls of the original VIDTEST.EXE
  (once, reused) and of our vidtest.exe through run_reference.py (about 15 s, `-nomsgbox`), and
  compares the per-group tables (FVF, stride, textures: draws / prims / vertices) with
  `drawcheck_baseline.json`; exit 0 and PASS when both are unchanged, else the groups that moved
  (`--update-baseline` accepts a run; `compare ORIG.bin OURS.bin` is the strict draw-by-draw check,
  lighting excluded, which does not pass yet). 2026-10-25 (libraries rebuilt that day): original 1,187
  draws / 83,028 prims in 50 groups, ours 2,327 / 100,837 in 50; 3 groups differ, all known: our land
  draws 1,502 tree-pass draws of the 256x256 format-26 map where the original draws 340, and the
  lightmap is X8R8G8B8 in ours (format 22) where the original's is A8R8G8B8 (21). PASS twice in a row; our own two runs
  differ in 17 draws' matrices / positions by the clock, so the check is on the tables.
* **symbolize.py** names the addresses of vidtest's crash report (`CRASH:` lines in its log: the
  exception address and the stack words that point into its code) with the linker map.

## The task kernel (docs/kernel-map.md 6, step 1)

```
ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/kerneltest.py build [--out DIR]
python tools/harness/kerneltest.py run
```

tests/capharness/kerneltest links app\kernel\kernel.cpp with scripted tasks on a clock the program
sets (it stands in for the aux-data clock, the input queue, WW3D's device check and the two globals
kernel.cpp calls) and checks the trace: a cycle with nothing due sleeps until the earliest task is,
the focused task alone gets Pre_Run and Render, Set_Focus / Next_Focus respect a task that won't
yield, a task removing itself during its Run is shut down and deleted after it (Pending_Delete),
another removed from inside a Run is deleted at once, Shutdown asks the focused task to yield,
then shuts down and deletes every task, and the asserts fire on their lines (157: added twice; 74:
a second kernel). 2026-10-24: 35 of 35 expected lines in order.

## The client base, SClient and the packet handlers (D94(c), kernel-map step 9d)

```
ENB_VC6_HOSTS=Daniel@<ip> python tools/harness/clienttest.py build [--out DIR]
python tools/harness/clienttest.py run [--name alex3] [--session ffff9b25] [--count 1000] [--update]
```

tests/capharness/clienttest links app\client's client.cpp, sclient.cpp and cpacket.cpp, connect.cpp, dbgconn.cpp,
circbuff.cpp, pk.cpp and every rebuilt packet file (one object per app file, so two files' statics never meet)
with logging stand-ins: `client_stubs.cpp` by hand (SClient's members the handlers call, co.cpp's object,
eff_o.cpp's effect, rs_track, sound_manager, shiputils, verbmanager, every SOptionsClass member SClient reaches:
its constructor 0x75A280 reads the registry and never runs, D69), `transport_stubs.cpp` (TCP, UDP, the composite
listener: exit 3 if reached) and two files `gen_stubs.py` writes into the stage: `pk_stubs.cpp` (udpreplay's
pk_ctor_stubs.cpp without the Process_Packet lines cpacket.cpp defines, P-T8-1; pkclasses.py is not touched) and
`clienttest_tables.cpp` (the factory's opcodes, names and classes; a handler is driven when cpacket.cpp defines it
and the class's raw constructor is rebuilt). `build` links twice when the first link leaves symbols unresolved:
`gen_stubs.py --link` writes `link_stubs.cpp`, a stand-in per unresolved symbol from its demangled name (Init's,
Shutdown's and Load_INIs' callees: about 80; a call to one shows as `gen NAME` in the trace; what it can't write,
a class by value, goes in client_stubs.cpp by hand). The SClient is made with its own constructor (the base's
included); its views are fakes with the client's slot layout (main view +0x80 / +0x84, loading view +0x0C, +0x68).

`run` writes the first COUNT server-to-client messages of SESSION from the reference decoder's NAME.trace.jsonl (udpreplay's
reference) to `.work/harness/clienttest/`, runs the program there and compares its trace with
`tests/capharness/clienttest/expected.txt` line for line (the first run, or `--update`, writes it; only a run
whose checks all pass). The tests: (1) the object, effect and record lists keyed by game id (ids 257 apart share
wwlib's bucket; Add / Find / Remove through SClient's overrides and the main view's notifications, the name
lookup, the ship); (2) the DEBUG setters before and after the redirect (Open_Connection 0x727F00 through
connect.cpp's factory, MEMORY protocol, to the harness's memory connection), each DEBUG message parsed back by
the factory and printed with the class's Print; (3) SClient::Process_Packets over that connection: an opcode
cpacket.cpp handles goes through the factory and the real handler, the rest become one recording test packet
(counted per opcode); a CREATE also adds a client object with its game id. 2026-10-27, alex3 ffff9b25's first
1,000 messages: 36 of 36 checks, 21 messages through 9 handlers (ACTIVATE_RENDER_STATE, SUBPARTS, CLIENT_SHIP,
DECAL, MESSAGE_STRING, CLIENT_SOUND, REMOVE_EFFECT, SET_TARGET, REMOVE), 979 recorded, 366 trace lines.

## Other harness programs (index)

Each script has its usage in its docstring; `python tools/harness/X.py --help` where it has argparse.

| Script | What | Needs |
|---|---|---|
| `convert.py` | EA-UDP dump to `.dgram` (above) | the dumps |
| `build.py` | the M1.0 capharness skeleton (above) | a pool VM |
| `refs.sh` | the reference decoder reference outputs (above) | the reference decoder built |
| `drawcheck.py` | M2's draw-dump regression check (above): original vs ours at frame 900, group tables against `drawcheck_baseline.json` | the proxy (`run_reference.py build`), our vidtest.exe (`build.py link`) |
| `auxdata_smoke.py` | builds and runs `tests/capharness/auxdata_smoke` (M1.2): both aux-data forms decode on a hand-built root and re-encode byte for byte; exit code 0 = pass | libraries built (`tools/vc6/build.py`, `mkmss32.py lib`), a pool VM |
| `auxreplay.py` | M1.3 gates (b), (c): AUX_DATA round trip and values (above) | libraries, pool VM, `.work/harness/ref/` |
| `n7aux.py` | AUX_DATA of Net-7 recordings through auxreplay (M1.5, above) | the reference decoder, sessions |
| `udpreplay.py` | EA-UDP transport, handshake, loop, connection layer (M1.4, above) | `.dgram` files, pool VM |
| `kerneltest.py` | `tests/capharness/kerneltest` (task kernel, scripted tasks, 35 expected trace lines); `build` then `run`. `taskstest.cpp` beside it has no script of its own | pool VM |
| `clienttest.py` | `tests/capharness/clienttest` (ClientClass's lists, the DEBUG setters and the redirect, SClient::Process_Packets through cpacket.cpp's handlers on a capture's messages; trace against `expected.txt`); `build` then `run` | pool VM, the reference decoder's trace.jsonl |
| `movietest.py` | `tests/capharness/movietest`: the movie task plays a Bink movie (kernel-map step 7); `build` stages Renegade's BinkMovie source from `roots.EA_ROOT`, `run` copies `binkw32.dll` and the movie from the retail install (read only) and runs in `.work/harness/movietest` | retail install, pool VM |
| `run_vidtest.py`, `vidtest_sbs.py`, `run_reference.py`, `drawdump.py`, `symbolize.py`, `bmp2png.py`, `d3d8shot.c` | vidtest (above) | |
| `vidtest_e2e.py` | starts vidtest as E&BConfig does (its command line, `CreateProcessA` with a relative name, E&BConfig's working directory = a copy of EBCONFIG at `build/programs/vidtest/e2e_ebconfig`, never the install) and reads `results.ini` as E&BConfig does; `--original` runs the retail VIDTEST (which has no `-nomsgbox`; ours gets it appended) | built vidtest, retail EBCONFIG folder to copy |
| `vidtest_exitcodes.py` | the exit-code matrix (cases `finish close key mode res scenes`, `--case NAME`, repeatable) with `-nomsgbox` always; window and any stray message box are found by process id; results.ini compared with `--ref` | built vidtest |

`vidtest_e2e.py` and `vidtest_exitcodes.py` stay two scripts: one reproduces E&BConfig's launch
(ctypes `CreateProcessA`, the working directory trick, the results.ini reader) and one drives
many cases by posting window messages; they share no code worth extracting.

## Quirks

* **Running the tree's programs.** `WW3DAssetManager` must exist before `WW3D::Init`, or
  `Set_Render_Device` crashes in `ShatterSystem::Init`. d3d8.dll is loaded at run time (no import).
  Windowed `-depth 16` still gets the desktop's 32-bit back buffer. Screenshots: read the program's
  own D3D back buffer. A screen-DC grab captures whatever window is on top (the user's screen),
  and PrintWindow gives black for D3D8 windows.

* **Rendering W3D models.** An HLOD's LOD 0 can be empty: set every HLOD, recursively, to
  `Get_LOD_Count()-1`. Call `SortingRendererClass::Flush()` before a back-buffer screenshot taken
  before `End_Render`, or alpha geometry is missing.

* **Capturing an original program.** A d3d8.dll proxy in a copy of its folder (d3d8.dll is not a
  KnownDLL) reads its back buffer and logs its file opens (`tools/harness/run_reference.py`).
  Put its options before the subcommand: `run_reference.py --frames 1,30 run -- -resx 800`.

* **VIDTEST's look depends on library switches.** Without `Enable_Overbright_Modify_On_Load` the
  models are dim (62.5% of frame 300 differs; about 3% with it). Ships appear only with the
  PredictiveLODOptimizer running each frame. The overlay's texture animations start at a phase
  set by the machine's uptime: not a bug. `-sync` has no visible effect when windowed. A
  full-screen quad needs vertices (1,1), (1,-1), (-1,-1), (-1,1) and indices 0,2,1,0,3,2, or
  it is culled.

* **E&BConfig and VIDTEST.** E&BConfig starts VIDTEST with a command line that has no program name,
  so the CRT's argument parsing drops `-device` and both programs use device 0. Its registry writes
  (HKLM `Software\Westwood Studios\Earth and Beyond\Render`, 32-bit) go to HKCU VirtualStore
  unless it runs elevated. `PeakPolyRate` is about polygons per frame times frames presented, since
  the whole run takes about a second on a modern GPU. `build.py link` reports stub compile errors
  while another builder has uncommitted app/ headers.

* **Custom render objects.** Don't override `Get_Num_Polys` or `Get_Cost` unless the client's class
  does (the land vtable 0xAE8994 doesn't): an override takes polygon budget from the other scenes.

* **d3d8 proxies.** d3d8 keeps a writable copy of the device vtable per device and rewrites some
  slots after CreateDevice: Set* hooks there see nothing; read state with Get* at draw time and
  re-hook at each Present. The proxy must pin itself (LoadLibrary on its own path): WW3D::Shutdown
  frees d3d8.dll while patched vtables still point into it. Default-pool textures (the original's
  lightmap) can't be locked: GetSurfaceLevel + CopyRects into an image surface.

* **Comparing VIDTEST frames.** Its sun follows GetTickCount: compare at the original's angle (from
  d3d8shot.log's stamps, `-sunangle`). Pixel diffs can't tell geometry from texture problems: dump
  the draws (`run_reference.py --dump`, `drawdump.py`).

## Protocol notes

Results moved out of docs/workflow.md §3 (2026-10-25).

* **EA-UDP ack lists.** An ack list follows when bit 0 of the datagram's flags byte is set (value 3
  exists too, not only 1): byte 5 is the entry count, then one header byte, then 4-byte entries
  (kind, channel, 16-bit big-endian seq). the reference decoder's `the reference importer` guesses the block start instead
  and loses 28 / 12 messages in the Alex dumps; use `tools/harness/convert.py`'s datagram files.
  The Alex dumps also lose bytes in the middle (not the end) of 3 datagrams per file (`X` lines).

* **EA-UDP dumps.** The Alex dumps start at byte 5 of the wire datagram: bytes 0-4 are the type
  (5 client, 4 server), the session id and the sender's datagram counter (the dump's `seq`). The
  checksum (the client's CRC 0x8B3EC0) covers the whole datagram including those bytes.

* **Net-7 recordings.** The 2022-09-25 recording has no client side of the sector (proxy-to-server
  only): run it with `n7aux.py --server-side`. Net-7's encoder differs from EA's in a fourth way
  beyond the reference decoder's auxdata.md §7: fields named in two sets (Value and Default, a Whole header
  `0f` for EA's `07`); the client applies one, so such messages re-encode differently.

* **The client's Delta model has no "default" state.** Updated and defaulted both set
  `m_Unknown70 = 2`; a harness keeps its own unknown/default/value state. `DeltaCompoundClass::
  Unknown_18` recurses into children only when `m_Unknown70 == 0`, `Unknown_1C` only when it is
  non-zero: expand compounds to their leaves yourself. The Player root derives from the second
  root class (vtable 0xAEAC40, ctor 0x4A3FF0, with the update collector), not from 0xAEAB88.
