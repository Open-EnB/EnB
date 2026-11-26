# Capture harness tools (M1)

Python side of the M1 capture harness ([docs/milestones.md](../../docs/milestones.md) §2) and
of the later harness programs (M1.2 to M2, listed at the end of this file). The C++ programs are
under `tests/`, one folder each (D155). Every external repository and data path is named once, in
`tools/re/roots.py` (listed in [docs/toolchain.md](../../docs/toolchain.md) §3). Inputs and
the reference decoder's readings live outside git:

| Path | What |
|---|---|
| `C:\EnBDevelopment\Live-Era-Data\enbserver3\ENB-Packets-Live\Alex3-fixed.txt.txt`, `Alex7-…` | EA's raw EA-UDP dumps (decrypted datagrams per session; public at trunk/capturedPackets of github.com/therealKyp/Earth-and-Beyond-server, D154) |
| `roots.EA_REFERENCE` (`C:\EnBDevelopment\Tools\Media\ea-reference`, unpacked by `fetch_reference.py`, D154) | the reference traces `ea\NAME.trace.jsonl` (message level) and the reference decoder's readings `ref\NAME.*.txt`: `dump --summary` (`*.summary.txt`), `aux-census`, `roundtrip`, `aux-log --first`; fixed, never regenerated |
| `.work/harness/alex3.dgram`, `alex7.dgram` | datagram files written by `convert.py` (regenerate, ~10 s) |

## Tools

```
python tools/harness/convert.py DUMP OUT.dgram [--check TRACE.jsonl]
python tools/harness/fetch_reference.py                        # the frozen reference (D154) -> roots.EA_REFERENCE
```

* **convert.py** parses a dump into a datagram file. `--check` rebuilds the game messages from
  the blocks (resends dropped, fragments joined) and compares them, in order, with the reference decoder's
  import of the same dump.
* **fetch_reference.py** downloads the EA captures' frozen reference (a release asset, SHA-256 checked;
  `--archive F` unpacks a copy) into `roots.EA_REFERENCE`: the reference traces and the reference
  decoder's readings (D154). The data is fixed; `refs.sh`, which made the readings, was retired.
* Retired: `compare.py`, the M1.0 count check against the reference decoder's summary (2026-10-25),
  and the skeleton it checked, `tests/capharness/capharness.cpp` with its `tools/harness/build.py`
  (D155, 2026-10-30). `udpreplay.py` and `auxreplay.py` carry the gates now; `git log` on those
  paths has them.

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

The reference importer does not decode the ack list: it takes the first offset from 6 at
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
| the reference decoder's messages | 26,078: all present, in the same order | 2,779: all present, in the same order |
| messages only in the datagram file | 28 | 12 |
| gaps in per-channel block sequence numbers | 2 (the reference decoder's reading: 64) | 2 (5) |

The extra messages are blocks that the reference decoder's start-offset guess swallowed; with them the
per-channel block sequence numbers are continuous except for 2 gaps per file, which makes them
real messages. Among them are 3 + 1 AUX_DATA (0x1B), including a 7,043-byte one in alex7, so the
AUX_DATA gates should count 4,289 / 366 for these files from the datagram file, against
the reference decoder's 4,286 / 365.

The M1.0 skeleton capharness (retired, D155) on every trace, checked by compare.py (retired): message counts and bytes per
direction and opcode equal the reference decoder's summary for all 8 traces (alex3, alex7, capture_1..3, the
three Net-7 sessions); its datagram counts equaled compare.py's for both datagram files.

## AuxData round trip and values (M1.3, gates (b) and (c))

```
python tools/vc6/build.py --lib app [--out DIR]                   # app.lib with app/client/co.cpp's aux constructors
ENB_VC6_HOSTS=<user>@<ip> python tools/harness/auxreplay.py build [--out DIR] [--app-sources]
python tools/harness/auxreplay.py run capture_1 [--out DIR]      # also capture_2, capture_3, alex3, alex7
python tools/harness/auxreplay.py run alex3 --dump [--out DIR]   # the messages rebuilt from the EA dump (D52)
```

* **tests/auxreplay** links the generated roots (`app/client/aux_roots.h`, tools/re/auxroots.py)
  and the auxdata library. Since Player's tree compiles in sclient.cpp (4292c395e, 2026-11-08) the aux objects'
  callees take most of app.lib into the link (co.obj -> item_effect.obj -> pk.obj -> cpacket.obj ->
  sclient.obj), so it links as the client does: wlib, wnet, WinInet, shell32 and the old iostreams, then a second
  pass with a logging stand-in for each symbol still unresolved (build.py `gen_stubs`; the rest at address 0,
  `force_unresolved`); `run` copies build/art's MSS32.DLL into the working directory, as clienttest.py. Per channel it makes a root for each CREATE (0x0004: the type byte at
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
  (`ref\NAME.aux-log-first.txt` in `roots.EA_REFERENCE`): it replays the U/D/X lines over a per-object state
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
`0c 08 04 09 05`. The reference decoder keeps the header byte as read, so its census passes it.

**The client's own encoder (M1.3, 2026-10-24).** After each decode the harness also has the root
write an update through the client's path, `UnknownRootClass::Encode` 0x4A3D50 (the collector
`DeltaCollectorClass`, a policy, the change table `ChangeTableClass`), and compares it with the
message: if the server's encoder is Westwood's same auxdata code, the message comes back. The
server's fields count their changes up (`DeltaClass::Unknown_34`) where the client's decode sets 2,
so the harness keeps a server-side version per field (an updated field and its parents get new
ones, a defaulted one keeps its own) and puts it in `Unknown70` before the encode. Player uses
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
ENB_VC6_HOSTS=<user>@<ip> python tools/harness/udpreplay.py build [--out DIR]
python tools/harness/udpreplay.py run alex3 [--aux]                 # also alex7
python tools/harness/udpreplay.py loop alex3 [--drop 0,10,4] [--step 20]
python tools/harness/udpreplay.py conn alex3 [--drop 0,10,4]          # through UDPConnection and the stream framing
```

udpreplay.py and clienttest.py (every command) refuse to run when the current directory is not inside the tree
holding the script (D112(4), tools/vc6/rootguard.py; `--root DIR` names the tree instead): a failed `cd` into a
worktree can no longer build or test the main tree's build/ in its place.

* **The wire datagram** (app/wwudp/cordcon.h): the dumps start at byte 5 of it. Bytes 0-4 are
  the type (5 from the client, 4 from the server), the session id and the sender's datagram
  counter (CordCon +0x8C), which is the dump's `seq`. Bytes 5 to the end are RC4-encrypted with a
  key per datagram: the session key, the encrypted length (4 bytes), the counter (2), 0 (2).
  The dump's first 4 bytes are the client's checksum (0x8B3EC0: reflected CRC-32, init
  0xFFFFFFFF, no final xor) of the whole wire datagram with that field zero. So the harness
  rebuilds the wire bytes and encrypts them, and the rebuilt CordCon checks the captured checksum:
  every datagram in both dumps passes, apart from the 3 per file the dump cut short.
* **tests/udpreplay** links app\wwudp (cordcon.cpp, cordmgr.cpp, unknown_cordmsg.cpp, compiled into the
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
server's private key (the reference key table, now `tools/harness/westwood_keys.py`) to the session key in its low 64 bits, as the
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
"written by a rebuilt class's Build"). Three Builds stamp a field with the aux-data clock
(Unknown_Get_Ticks, 0 in the harness): CLIENT_SET_TIME 52 (ServerSent, message byte 12), REQUEST_TIME 68
(TimeSent, byte 4) and LOGIN 2 (TimeSent, byte 0x44, after the 0x40-byte ticket). Those opcodes are compared with that field zeroed (`TICKS` in udpreplay.py; counted as
"time field masked"); every other byte must still be equal. D95(a): a rebuilt class whose client Build writes a
size field larger than the payload it adds can't give the received message back, so for the opcodes in
conn_replay.cpp's explicit list `Recorded_Opcodes` (AVATAR_DESCRIPTION 97: Get_Data_Size 0x110 for 0x108 bytes;
183: an entry sized 0x4C for 0x49 bytes) the harness sends the message as received instead: on the UDP side
from UDPConnection::Receive's buffer (`Conn_Receive_Buffer`, which `udpreplay.py build` defines after
udpconn.cpp), on the stream side the bytes its transport read (a size field that isn't the length is a harness
error). The rebuilt raw constructor still parses the message; its Build is not compared. Counted as "sent as
received (D95)", once per side (alex3 38, alex7 22 for 97). avatar_desc.cpp is compiled in for 97's member,
its unbuilt callees in `avatar_desc_stubs.cpp`.

## Net-7 recordings (M1.5, gate (d); retired)

`n7aux.py` (retired with D154; git history has it) ran every AUX_DATA the client received in four
Net-7 recordings (2022-09-25 and three of 2026-10) through auxreplay.exe, as `auxreplay.py run`
does, and counted Net-7's known encoding differences instead of failing on them:

* `length`: the length field differs from the bytes that follow; everything else re-encodes byte
  for byte (declared minus carried: +6 for Ship deltas, the whole payload's length; -8 Ship, -4
  Harvestable, -2 Hulk).
* `members`: a full Player send that runs out exactly where `Player.GroupInfo.Members`' header
  should be; it decodes and re-encodes byte for byte with EA's `05` (Whole | Default) appended.
* `short`: a tree-form update that declares more than it carries and runs out.
* `sets`: a field or a whole container named in more than one of Value / Default / Clear (Ship's
  Rank and EnergyPercent in Value and Default; Player's Whole header `0f` where EA's has `07`).
  The client applies one of them, so its encoder can't give the message back; a second run with
  our re-encoding in the message's place touched the same fields with the same values.

Results (2026-10-24): every AUX_DATA of the four recordings (2,346) decoded or fell in one of the
classes above (2 named no known root), none failed, no asserts, and every value equalled the reference decoder's reading
(a field changes when its bytes change, not its printed value). The 2022-09-25 recording has no
client side of the sector (server-to-recorder only), so what the sector server sent stood for
what the client got.

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
  2026-10-28 (LD1, D101: VIDTEST on LandClass, trees from programs/vidtest/viddeco.cpp): ours 2,191 draws / 83,028 prims /
  173,408 verts, the prims and verts the original's; one group differs, the trees drawn per patch (1,344 draws of the
  original's 23,695 prims where it draws 340). `--art DIR` runs ours on a copy of the art (`build/art`, EBCONFIG copied
  with its dates, `cp -rp`): LandClass rewrites a .pld it can't load, so never the install folder (D69).
  Since D120 (2026-10-28) drawcheck, vidtest_sbs and run_vidtest pass `-art build/art` by themselves whenever that folder
  exists and no art folder was given (`--art DIR` for drawcheck, `-- -art DIR` for the other two); without it they keep
  vidtest's own default (its working directory when it holds `mu01a.w3d`, else the install's EBCONFIG) and print a
  warning. Make the copy once per clone or worktree, with the dates (`artdir.py`):
  `cp -rp "/c/Program Files (x86)/EA GAMES/Earth & Beyond/EBCONFIG" build/art`.
  2026-10-28 (LD2): the trees from LandClass's own Create_Decorations / Build_Decorations (0x479D00 / 0x47A2F0) into its
  patch records: ours 1,187 / 83,028 / 173,408 in 50 groups, every group the original's (0 known differences; the
  strict compare: 17 of 1,187 draws differ, the clock-driven matrices). The lightmap equals the original's since
  VIDTEST's sun angle is computed with acos (824ff99).
* **symbolize.py** names the addresses of vidtest's crash report (`CRASH:` lines in its log: the
  exception address and the stack words that point into its code) with the linker map.

## The task kernel (docs/kernel-map.md 6, step 1)

```
ENB_VC6_HOSTS=<user>@<ip> python tools/harness/kerneltest.py build [--out DIR]
python tools/harness/kerneltest.py run
```

tests/kerneltest links app\kernel\kernel.cpp with scripted tasks on a clock the program
sets (it stands in for the aux-data clock, the input queue, WW3D's device check and the two globals
kernel.cpp calls) and checks the trace: a cycle with nothing due sleeps until the earliest task is,
the focused task alone gets Pre_Run and Render, Set_Focus / Next_Focus respect a task that won't
yield, a task removing itself during its Run is shut down and deleted after it (Pending_Delete),
another removed from inside a Run is deleted at once, Shutdown asks the focused task to yield,
then shuts down and deletes every task, and the asserts fire on their lines (157: added twice; 74:
a second kernel). 2026-10-24: 35 of 35 expected lines in order; 2026-11-02 (56d6c356, after it had stopped
linking at 6a08fe5d): 81 of 81 expected lines in order (100 lines in all). 2026-11-04: 81 of 81 again, after kerneltest.cpp's
stand-in for the asset manager became `WW3DAssetManager::TheInstance` (kernel.cpp's since c759a57b).

## The client base, SClient and the packet handlers (D94(c), kernel-map step 9d)

```
ENB_VC6_HOSTS=<user>@<ip> python tools/harness/clienttest.py build [--out DIR]
python tools/harness/clienttest.py run [--name alex3] [--session ffff9b25] [--count 1000] [--update]
```

In a fresh worktree: `ENB_VC6_HOSTS` comes from `python tools/vc6/pool.py lease --agent ID` (release it after), and `run`
needs MSS32.DLL next to the exe (exit 0xC0000135 without it): copy `build/art/MSS32.DLL` into
`.work/harness/clienttest/` (`wt.py new` copies build/art and build/d3d8shot into a worktree; one made before
2026-11-10 has neither).

tests/clienttest links app\client's client.cpp, sclient.cpp and cpacket.cpp, connect.cpp, dbgconn.cpp,
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
SClient's chat line 0x74D990 is the real one (2026-11-08, CW4): `build` renames its call of the chat panel's 0x563760
in sclient.cpp's object only (`#define Unknown_563760 Clienttest_Unknown_563760`), and client_stubs.cpp traces that
call (`stub ChatPanelInterface::Unknown_563760 TYPE 'PREFIX: text'`); type 6's line display call traces as Add_Line_3.

`run` writes the first COUNT server-to-client messages of SESSION from the reference decoder's NAME.trace.jsonl (udpreplay's
reference) to `.work/harness/clienttest/`, runs the program there and compares its trace with
`tests/clienttest/expected.txt` line for line (the first run, or `--update`, writes it; only a run
whose checks all pass). The tests: (1) the object, effect and record lists keyed by game id (ids 257 apart share
wwlib's bucket; Add / Find / Remove through SClient's overrides and the main view's notifications, the name
lookup, the ship); (2) the DEBUG setters before and after the redirect (Open_Connection 0x727F00 through
connect.cpp's factory, MEMORY protocol, to the harness's memory connection), each DEBUG message parsed back by
the factory and printed with the class's Print; (3) SClient::Process_Packets over that connection: an opcode
cpacket.cpp handles goes through the factory and the real handler, the rest become one recording test packet
(counted per opcode); before that (3a) a packet whose handler refuses twice stays on the peek connection and is
handed over again, and (4) a server's packets build an object and move it: CREATE 4, SIMPLE_POSITIONAL_UPDATE 8
through Do_Frame / Process_AI to the model's Set_Transform, CHANGE_BASE_ASSET 38, REMOVE 7. 2026-10-27, alex3
ffff9b25's first 1,000 messages: 38 of 38 checks, 21 messages through 9 handlers, 979 recorded, 373 trace lines.
A handler added to cpacket.cpp changes the trace: check the difference, then `run --update`.

**Objects through the real CREATE (2026-10-28, HN1):** CREATE 4, CHANGE_BASE_ASSET 38 and SUBPARTS 180 are driven
(gen_stubs.py's `NOT_DRIVEN` is empty). clienttest.cpp puts a fake asset manager (a `WW3DAssetManager` subclass:
`WW3DAssetManager::Get_Instance()` and co.cpp's `*0xBDADC0`) behind co.cpp's loader 0x6F5140 / 0x6F51D0: its slot
+0x78 gives a model for a loaded file's NAME (FILE is NAME.w3d), `Create_Render_Obj` the "missing" model. The models
are `FakeModelClass`, a real RenderObjClass that logs Set_Transform, Set_LOD_Bias, Set_Hidden, Add_Sub_Object_To_Bone
and its deletion (`model N ...`); NAME ship... has the bones ~FL ~FR ~BL ~BR, NAME rs_... is an rs_track model
(Class_ID 0x10001). The INIs are clienttest's own, `tests/clienttest/data/basset.ini` (the loader) and
`cbasset.ini` (Set_Model's radius, the mount points): `run` copies them into `.work/harness/clienttest/`, never the
game's folder (D69). The aux root factory 0x6F6000's stand-in gives the types that have a root the harness's
(its slot +0x04, the deleting destructor, logs). 57 checks (12 new: (4)'s object in the table with the packet's
game id / type / base asset / scale, its model loaded, the radius, the six part models, the swap, the deletes;
(3)'s 202 CREATEs and 4 REMOVEs), 2,960 trace lines.
**CREATE 4's subclasses (2026-10-28, CS1):** (4b) sends synthetic CREATEs of types 24 and 28 (no alex3 message has
them): co_subtypes.cpp's constructors, type 24's proximity-object slots (+0x188), SET_TARGET 25 on type 28 (0x70E750) and
REMOVE through both destructors. 66 checks, 2,993 trace lines.
**dynamic_obj.cpp (2026-10-28, DO1):** type 24's DynObj is rebuilt (its slots no longer stand-ins); its PxWrapper
(px_wrapper.cpp, not rebuilt) has stand-ins in client_stubs.cpp: the constructors record the owner and clear the list,
so the destructor's FAIL_IF (line 70) stays quiet. 67 checks (the PxWrapper's owner), 2,994 trace lines.
**px.cpp and px_wrapper.cpp (2026-10-28, PX1):** ClientClass's PxList and type 24's PxWrapper are real code now; the
PxClass and PxWrapper stand-ins are gone, client_stubs.cpp has stand-ins for the other px_*.cpp classes the wrapper
builds (PxObject, PxBoxList, PxCollisionBox: the box classes construct their PxBox base). 67 checks, 2,999 trace lines.
**Real objects instead of stand-ins (2026-11-01, CT):** the link had stopped on LNK2005 duplicates (ui_group, rs_track,
gadget, gadget_list_manager, ui_cpit, unknown_root, name, lookup, delta, soundlib, WWAudio) and then on the objects they pull
in. The stand-ins for functions the tree now builds are gone (RsTrackClass, UIGroupClass, GadgetClass::Unknown_6607D0,
GadgetListManagerClass::Unknown_661940, UICpitClass::Unknown_584690, DeltaCompoundClass::Decode, NameClass / LookupClass /
DeltaInterpolatorClass, SoundLibraryClass::Play_2D / 3D_Sound, WWAudioClass's, pkupdate's 0x8ADB20), so those run for real on the
harness's fakes: the fake gadget is a zeroed 0x100-byte object with its name at +0x24 in a real manager's list; the cockpit's
+0xE0 object is a fake whose four calls are stand-ins in clienttest.cpp; the chat panel, PDA, hot keys, line display and
the main view's +0x34 object are zeroed fakes with a table that pops slot +0x7C's two arguments (SClient::Unknown_742530 is
real); `Unknown_Get_Ticks` (unknown_root.cpp) and `WWAudioClass::Flush_Cache` are real functions that main() jumps to the
harness's clock and to a trace line (a second definition would be a duplicate). ui_stubs.cpp / ui_avsel_stubs.cpp hold what
gen_stubs --link cannot write (test programs keep their own stand-ins; the client has none). A crash prints its address on stderr (a vectored handler:
look it up in the exe's .map). 103 checks, 3,178 trace lines; the trace lost the lines of the deleted stand-ins
(`stub DeltaCompoundClass::Decode` x213, the Play_2D_Sound, RsTrack and gadget 0x6607D0 / group 0x5981B0.. lines, 0x56BA80).

## Other harness programs (index)

Each script has its usage in its docstring; `python tools/harness/X.py --help` where it has argparse.

| Script | What | Needs |
|---|---|---|
| `convert.py` | EA-UDP dump to `.dgram` (above) | the dumps |
| `fetch_reference.py` | the frozen reference (above) | network, or `--archive F` |
| `drawcheck.py` | M2's draw-dump regression check (above): original vs ours at frame 900, group tables against `drawcheck_baseline.json` | the proxy (`run_reference.py build`), our vidtest.exe (`build.py link`) |
| `auxdata_smoke.py` | builds and runs `tests/auxdata_smoke` (M1.2): both aux-data forms decode on a hand-built root and re-encode byte for byte; exit code 0 = pass | libraries built (`tools/vc6/build.py`, `mkmss32.py lib`), a pool VM |
| `auxreplay.py` | M1.3 gates (b), (c): AUX_DATA round trip and values (above) | libraries, pool VM, the frozen reference |
| `n7aux.py` (retired, D154) | AUX_DATA of Net-7 recordings through auxreplay (M1.5, above) | the reference decoder, sessions |
| `udpreplay.py` | EA-UDP transport, handshake, loop, connection layer (M1.4, above) | `.dgram` files, pool VM |
| `kerneltest.py` | `tests/kerneltest` (task kernel, scripted tasks, 35 expected trace lines); `build` then `run`. `taskstest.cpp` beside it has no script of its own | pool VM |
| `clienttest.py` | `tests/clienttest` (ClientClass's lists, the DEBUG setters and the redirect, SClient::Process_Packets through cpacket.cpp's handlers on a capture's messages; trace against `expected.txt`); `build` then `run` | pool VM, the reference decoder's trace.jsonl |
| `movietest.py` | `tests/movietest`: the movie task plays a Bink movie (kernel-map step 7); `build` stages Renegade's BinkMovie source from `roots.EA_ROOT`, `run` copies `binkw32.dll` and the movie from the retail install (read only) and runs in `.work/harness/movietest` | retail install, pool VM |
| `clientrun.py` | runs our client, the one client build (`build/programs/client/client.exe`, `client.map` beside it, linked from every library with no stand-in: `tools/vc6/build.py link --program client`; a worktree's own `build/` in a worktree; `--exe PATH` another), or a copy of retail. The runtime data stays outside git in `CLIENT_DATA_ROOT`, `C:\EnBDevelopment\ClientPOC` (the user's folder, its name kept): `run` copies the exe and its map into its `release` folder and starts it there (windowed or full screen is the registry's; `--fullscreen` has no effect), kills it by PID after `--seconds`, and digests `_except.txt` (and a `stubs.log` when an older stand-in exe wrote one: repeats folded, the first `CRASH:` line named from `client.map`); `log` digests the last run. `run --shots N,M` saves the client's own back buffer at those Present calls through the d3d8shot proxy (`build/d3d8shot/d3d8.dll`, copied beside the exe for the run and deleted after it; never a screen grab, D69) as `.work/poc/shots/HHMMSS/frame_NNNNN.png` with the proxy's `d3d8shot.log`. The login screen presents thousands of frames a second (frame 120000 is about 25 s in), so pick large numbers. Each run's first failure goes in `.work/poc/runs.md`. **Several at once:** `--instance NAME` runs in `CLIENT_DATA_ROOT\inst\NAME\release` beside its own `inst\NAME\Data` (`client\ini` and `client\output` copied once, every other folder a junction to `CLIENT_DATA_ROOT\Data`'s: antprox `/REPLAY` rewrites `auth.ini`'s AAIUrl / LKeyUrl at each start, and with one shared `auth.ini` the first-started client sent its account login to the other replay server; own `_except.txt`, proxy, shots `.work/poc/shots/NAME-HHMMSS`; prints `client pid N`), `--exe PATH` runs another tree's link output, e.g. `<worktree>\build\programs\client\client.exe` (must exist), `replay --input FILE [--seconds N] [--shots N,M]` runs the same against antprox `/REPLAY` (antprox is machine.py's `antprox`, the Rust port `AntaresProxy-Rust\target\release\antprox-rs.exe` since 2026-11-24, the C# 1.0.0 release before; `ANTPROX_EXE` overrides it with another build; the client is the child of that antprox; its output goes in the shot folder; CLIENT ARGS after `--` become `/CLIENTOP:` switches); `--messages` (run, replay) adds `-ENABLE_MESSAGES` and `msglisten.py` (`<shots dir>\messages.log`); the instance's copy of the exe gets its own mutex name and the proxy's `D3D8SHOT_CURSOR=off` stops the cursor warp (below). `clean --instance NAME | --all | --older HOURS` deletes instances (junctions into `CLIENT_DATA_ROOT\Data` unlinked, never followed; a running client's instance kept): each builder its own before reporting, the coordinator `--all` at handoff. **`replay` also prints the client's exit** (D191: antprox.out's "Client exited" against the last input's time, or "still running when this run killed it"). **`--ea`** (with `--instance`, D200) runs the instance in `EA_DATA_ROOT\inst\NAME` (`C:\EnBDevelopment\ClientPOC_EA`) over that folder's `Data` and DLLs, EA's final data (P-EA1-1; local, never committed); the exe stays the tree's client, `--exe`'s or `--retail`'s, and `clean` covers both folders' instances. **`replay --retail`** (D190; needs `--instance` and `--shots`) runs a copy of `INSTALL\release\client.exe` in the instance folder under the d3d8shot proxy with `D3D8SHOT_INPUT=1` (`Hook_Input` in d3d8shot.c patches the client's user32 imports to read the posted messages, as it does for ours; rebuild the proxy with `run_reference.py build` for it) and posts BM_CLICK to the terms box's "I Agree"; the same `--input` file as ours; character select needs frames in the hundreds to few thousands (`--shots 300,1000,3000`), not the login screen's 100000s. `dumpcmp.py A B` compares two draw dumps (`D3D8SHOT_DUMP=N` in the environment) draw by draw. A dump on an event: with `D3D8SHOT_DUMP=0` in the environment (buffers shadowed from the start, no frame listed), an e2e `shot dump_NAME` step also dumps the frame after the shot (`draws_NNNNN.bin` and its `tex_*` in the run folder; e2e/sp_galaxymap.scn). It writes `*.th6` thumbnails beside the mixes in `CLIENT_DATA_ROOT\Data\client\mixfiles` (as the client does) | every library built, `mkauthlogin.py`, pool VM for the link |
| `e2e.py` | end-to-end scenarios for client builds (D237): `tools/harness/e2e/*.scn`, one client session for all of them, event-driven steps, tagged input through the proxy, shots on events, the gen stand-ins per scenario (section below) | the linked client (`build.py link --program client`, every library and binkmovie built at that tree's head), antprox, a current proxy |
| `run_vidtest.py`, `vidtest_sbs.py`, `run_reference.py`, `drawdump.py`, `symbolize.py`, `bmp2png.py`, `d3d8shot.c` | vidtest (above) | |
| `mss32log.c`, `audiolog.py`, `tools/test_audiolog.py` | the audio event log (test-plan C2): a logging MSS32.DLL proxy, its build and its log's reader (below, "Audio log") | a pool VM for the build; an instance (`e2e.py --audio`) |
| `machine.py`, `machine.example.json` | the per-machine config: every path outside the tree (below, "Machine config and the baseline store") | nothing |
| `baseline.py`, `tools/test_baseline.py` | the baseline store `Open-EnB/baselines`: capture retail runs with manifests and noise, verify them (no game needed), compare a run (below) | the store's checkout; capture: retail, the local Net-7 server |
| `msglisten.py` | listens to a running client's debug-message server (the client's `-ENABLE_MESSAGES`: app/shared/message/msgserv.cpp's MsgServer on the first free TCP port of 9999-10009, bound to every interface by the client's own code, so run it on this host only). `msglisten.py [--pid PID | --port P] [--out FILE] [--seconds N] [--levels MASK] [--modules A,B] [--files F,G] [--prefix TEXT] [--raw]`: `--pid` finds that client's port with netstat (several clients each get their own) and waits for it; it sends one filter record (`S` name `` ` `` levels `` ` `` modules `` ` `` files `` ` `` prefix, `\0`-ended, empty fields as ' ') and prints each `M` record (level `` ` `` [sender,]category `` ` `` file `` ` `` line `` ` `` x `` ` `` y `` ` `` sec.usec `` ` `` text, `\0`-ended) as `HH:MM:SS.uuuuuu LEVEL category: text  file(line)`. The client drops messages while no listener is connected and accepts one at most once a second in the server's run (the kernel task, every 200 ms, and after each error report), so start-up asserts before WinMain's 'adding message server to kernel' never arrive. `clientrun.py run|replay --messages` starts the client with `-ENABLE_MESSAGES` (replay: antprox `/CLIENTOP:`) and msglisten beside it into `<shots dir>\messages.log` | a client started with `-ENABLE_MESSAGES` |
| `artdir.py` | the art folder default of drawcheck, vidtest_sbs and run_vidtest (D120): `build/art` when it exists, else a warning | `build/art` (`cp -rp` of EBCONFIG) |
| `vidtest_e2e.py` | starts vidtest as E&BConfig does (its command line, `CreateProcessA` with a relative name, E&BConfig's working directory = a copy of EBCONFIG at `build/programs/vidtest/e2e_ebconfig`, never the install) and reads `results.ini` as E&BConfig does; `--original` runs the retail VIDTEST (which has no `-nomsgbox`; ours gets it appended) | built vidtest, retail EBCONFIG folder to copy |
| `vidtest_exitcodes.py` | the exit-code matrix (cases `finish close key mode res scenes`, `--case NAME`, repeatable) with `-nomsgbox` always; window and any stray message box are found by process id; results.ini compared with `--ref` | built vidtest |

## End-to-end scenarios (`e2e.py`, D237)

`python tools/harness/e2e.py run [NAME...] [--instance ID] [--exe PATH] [--messages] [--separate]` runs a client
build through scenarios against antprox `/REPLAY` (D181; world login included) and prints one line per scenario
(`PASS` / `FAIL` / `XFAIL` / `XPASS`, the generated stand-ins hit, the failure, its seconds); exit 0 only if every
one is as expected (`XFAIL`: a scenario marked `expected-fail: REASON` ran every step and failed only on its
`stand-in` step; `XPASS`, the stand-in no longer hit, fails the run so the stale note is removed; any other failure
is `FAIL`). `list` and `check` parse only. `--exe` defaults to this tree's `build/programs/client/client.exe`
(a worktree's own link; missing: the run stops and says so); the instance (default `e2e`) is clientrun.py's, removed at
the end (`clientrun.py clean`) unless `--keep`.

**One client session for the whole run (2026-11-14):** the first scenario runs its `include`s (the start-up movie,
login, character select, the world); every later scenario runs only its own steps in the same world, so a scenario
must leave the world as it found it (close what it opens). After a scenario that fails, the client is stopped and
the rest run in a fresh session (a crash or a window left open does not fail the others); after an `XFAIL` too
(2026-11-14: its stand-in left the world changed, e.g. the pop-up browser never destroyed). The run ends early, the
rest printed `FAIL ... not run`, when a second crash is at the same place as an earlier one (the same exception
code, instruction and first return addresses) or when two sessions in a row fail before the world. `--separate` gives every
scenario its own client. Logs in `.work/e2e/YYYYMMDD-HHMMSS-session/` (`YYYYMMDD-HHMMSS-NAME` for a single scenario; `--out DIR` or `ENB_E2E_OUT` moves the root): `e2e.log`
(each step, each match), `events.log` (every line of antprox.out, stubs.log, messages.log and d3d8shot.log with
its time, written as lines arrive with repeated identical `G_ASSERT` / `FAIL_IF` lines folded to counts), `result-NAME.json`, `stubs.log`, `antprox.out`, `d3d8shot.log`, the shots.

**Scenarios** are `tools/harness/e2e/NAME.scn`, in the order world_entry, starmap, options, pda_pages, cargo, chat,
then the rest by name (2026-11-14: the chat panel's windows chat_windows, group_windows, help_windows, petition,
option_pages, options_combo, option_control, quit_menu; group_looking; hot_keys; pda_faction; the pop-up browser's
items browser_credits, browser_selfhelp, browser_manual, browser_petition_faq / _bug / _billing). e2e.py's `LAST`
names scenarios that must run after everything else (none today). The browser items load live pages (the URLs are client.exe's
strings in the settings store's table, settings_store.cpp 0x7C97B0); a scenario asserts the window
(fc_jweb01.ini), never the page, so a dead URL passes too. `_wip_NAME.scn` crashes the client today (its header
names the fault) and runs only when named (none today). The recorded session has two scenarios in the default set: play1 (`/play 1` on
Broadcast, the loading screen, the recording's starbase and sector 4505, `/stop`, the hand-off home) and
play1_windows (inside sector 4505: the PDA's Skills and Faction tabs, missions, equipment, cargo and the item
details of a right-click). Data windows belong inside a playback: the home sector has no skills, missions or
items to show. The default set (and the per-landing smoke set) is these 25 scenarios. Four more, named-only
(the `_` prefix: not in the default set), play a recording to its end, no `/stop` (2026-11-14): `_play8`
(`/play 8`, two sector sessions, about 380 s), `_play9` (`/play 9`, three, about 340 s), `_play10` (`/play 10`,
six, about 260 s) and `_play7` (`/play 7`, eight, about 6400 s: its session 5 alone is 5738 s; not run to its end
yet: e2e.py keeps every event line in memory and the assert flood of that hour outgrew the host, its header has the
numbers); each waits on every session's playback line, then antreplay's 'end of the recording; back to' and the
home connection's START_ACK, and passes when the client is back in the home sector and alive. A session that
starts docked logs no 'Render: Frame Rate' on either client: its entry waits on ui_starbase.cpp's 'Changing room
from -1' instead (`_play7`). Run them by name, `e2e.py run _play8 _play9 _play10 [--retail --instance X]`, at a
handoff and after landings in starbase or packet code. Two runs at once (retail and ours, other instances) may
start in the same second: the second's folder is `YYYYMMDD-HHMMSS-NAME-2`.

**No timed waits where an event exists:** steps advance on events (the replay server's `VERSION_RESPONSE`,
`GLOBAL_AVATAR_LIST`, `GLOBAL_TICKET_REQUEST`, `START_ACK`). Where the client logs nothing at "ready", two steps
stand in for the old `alive N` waits: `wait-quiet proxy S` (the screen's loading is over when its file opens stop:
the login screen, character select's avatars, the world, a window's model) and `click ... until EVENT` (the click
is repeated until its effect shows: the movie's 3000 ms guard, character select's opening, during which the slot
and Enter are ignored, hence `redo 1`). `wait-since SOURCE PATTERN S` (2026-11-25) waits until S seconds after the
newest line matching PATTERN in the scenario (a wait-event's match counts, even after the line left the event
window): antreplay plays a sector session at the recorded pace from the client's LOGIN however long the client takes
to load (6-25 s after it on retail and ours), so a shot S seconds after the playback line is at one point of the
recording, where `wait-quiet` plus `alive 20` landed in its flight or its rest by load time; the step fails when it
starts more than 1 s past its point. `alive S` remains as an assertion (the client runs, no `expect-no` line
appears), not as a wait. Gadget names come from `e2e/gadgets.txt` (NAME X Y at 1280x1024, each with the replay or
the tooltip in a shot that found it): the client does not log gadgets' names or rectangles, so a name is a table
entry, not a lookup in the running client.

**Writing a scenario:** a `stand-in` step watches stubs.log only from its own line on: put it before the step that
hits the stand-in. A minimized client window stops taking the harness's posted input (the client does not come back
well from the tray): check for it before blaming the harness. Write each wait against a line a run has actually
logged (antreplay logs no START_ACK for a playback connection). Window behaviour the scenarios met: a window's
fa_*.ini opens again when it closes; chat-panel drop-down items act on mouse-down; chat-panel requests queue while a
window is open (an Esc during the options window opens the quit menu after it closes); the chat panel's browser items
do nothing while the settings store's +0x34 is set.

**Modifier keys, chords and held keys (test-plan C6, 2026-11-25):** `key KEYS` takes a VK as before (decimal or 0x
hex), a key's name (`enter`, `esc`, `tab`, `space`, `backspace`, the arrows, `home` ... `pgdn`, `f1`-`f24`,
`num0`-`num9`, control_keys.ini's punctuation) or a letter, and chords joined by `+`: `key shift+i`, `key ctrl+i`,
`key alt+f1` (each key down in order, up in reverse). `keydown KEYS` / `keyup KEYS` press or let go only, so a key
stays down across the steps between (`keydown shift`, `key i`, `keyup shift`); `hold KEY` takes the names too.
`clientrun.py`'s `--input` files take the same (`T key shift+i`, `T keydown shift`). They are posted as a keyboard
sends them (`clientrun.key_message`): the modifier's own key-down first, its scan code in lParam, Alt held without
Ctrl as WM_SYSKEYDOWN / WM_SYSKEYUP with lParam's context bit 29 (Alt's own key-up with it clear), a key-down of a
key already down as an auto-repeat (bit 30); a plain key's messages are exactly the old `key VK`'s. client.exe reads
the modifiers only through user32, no DirectInput or GetKeyboardState: its key handler 0x4307A0 (w32kbd.cpp, for
WM_KEYDOWN / WM_KEYUP / WM_SYSKEYDOWN / WM_SYSKEYUP) asks `GetKeyState(VK_SHIFT / VK_CONTROL / VK_MENU)` while
it handles the key, the key test 0x430CA0 `GetAsyncKeyState`, the mouse's 0x4314C0 `GetKeyState`, and ToAscii
(0x42EB80) gets a key state built from the event's flags. The proxy answers GetKeyState / GetAsyncKeyState from the
key messages it passed on (`In_Keys`; the left / right Shift, Ctrl and Alt too) and logs the modifiers down at
each key-down, `input: replayed key 0x49 +shift` (`+ctrl`, `+alt`, ` sys` for WM_SYSKEYDOWN, ` repeat`), and each
key-up, `input: replayed key-up 0x49`; a plain key's line is unchanged. A scenario must let go of every key it
holds (parse error otherwise; a failure between is let go at the scenario's end), and a lone Alt or Alt+Space is
refused (the window menu's keyboard mode, the system menu's modal loop: D69). The client drops an auto-repeat
key-down outside text entry (0x4307A0: lParam bit 30 without bit 31), so `hold`'s downs stay fresh downs without
bit 30, as before (a keyboard's held key would reach the client once). `sb_vault` (live) opens the Vault with
Shift+I (control_keys.ini `[Vault] PSW_1=$I`; plain I is `[Equipment]`) and closes it with the held form, then
toggles Ctrl+I (`[Toggle Ping]`) on and off. Unit tests: `python -m unittest tools/harness/test_keys.py`.

**The exe carries no test code (2026-11-14):** the stand-in build's input and multi-instance hooks are gone. The
d3d8shot proxy, copied beside the instance's exe for every run, gives the client its virtual cursor and keyboard
(`D3D8SHOT_INPUT=1`, d3d8shot.c `Hook_Input`: the exe's user32 imports), exactly as it does for the retail
client (`clientrun.py replay --retail`), so one scenario can drive both. e2e posts with `0x5043` in wParam's high
word and sets `D3D8SHOT_INPUT_TAG=1`: the proxy strips the tag, logs each button / key-down as `input: replayed
...`, and drops untagged mouse and key messages (`input: NOT REPLAYED (dropped) ...`): a person's click or the
real cursor crossing the test window does not reach the client, and the result line counts the dropped ones.
`clientrun.py replay` posts untagged (its `post_step(tag=False)`). Several instances at once: `Inst.prepare()`
gives the instance's COPY of the exe its own mutex name (the string `enb_mutex_lock` rewritten in the copy, same
length), and without virtual input the proxy's `D3D8SHOT_CURSOR=off` makes SetCursorPos / ClipCursor no-ops. The
one client build has no stand-in layer either (the stand-in build's read-only registry and its `CRASH:` line went with
it): a crash shows as the client's exit and its `_except.txt`, as on retail.

**The retail client as the expected result (D190, D248):** `e2e.py run [NAME...] --retail --instance NAME` drives a
COPY of the retail client (in the instance folder, never the install) through the same scenarios: the same proxy,
the same tagged input, its terms box's "I Agree" clicked for it. It writes no stubs.log, so steps on the `stubs`
source are ours only and are skipped there (e2e.log says so); a retail crash shows as 'the client exited'.
`e2e.py compare DIR_OURS DIR_RETAIL` (two run folders) prints per scenario whether the two clients SENT the same
packets (antprox.out's 'received NAME, N bytes' lines between the scenarios' marks; keep-alives, time requests
and DEBUG left out: they follow the wall clock) and the first differences. First use, 2026-11-14: world_entry,
options, cargo, chat, group_looking: retail 5 of 5 in 46 s, and all five SAME.

**The retail check (2026-11-19, E2):** `e2e.py retail [NAME...] [--exe PATH] [--instance BASE] [--retail-run
MANIFEST] [--detail] [--no-shots]` runs our build (instance BASE-ours, default `cmp-ours`) and the retail client
(BASE-retail) through the same scenarios at once, then compares them: one line per scenario, `SAME NAME: N packets,
shots K of K alike` or `DIFF NAME: what` (the verdicts when they differ, ours vs retail's; the packets' counts and
first difference; each differing shot with its cells and box), and a count line; exit 1 on any DIFF. `--detail`
adds the packet diff and each shot's path under a DIFF; `--retail-run` reuses a retail run's manifest (it varies
less than ours changes) instead of running retail again, but only one of the same scenario list: a scenario's
packets and shots depend on what ran before it in the session (play1 play1_windows alone against the 25-set's
retail run: a CLIENT_CHAT_REQUEST and five home cells differed that the paired run has equal). `_faction_rows`
(named only) hovers and clicks the Faction tab's first row: the probe that showed retail's rows are off too
(no tooltip, no selection), yet drawn. Both runs' logs and manifests are
`.work/e2e/retail-YYYYMMDD-HHMMSS-{ours,retail}.{log,json}`. `run --manifest FILE` writes any run's manifest (each scenario's
verdict and session folder: a run whose failure started a fresh session has several), and `compare` takes a
manifest, a folder, or folders joined by `,` on each side, shots included (`--no-shots`: packets and verdicts).
**Shots:** the same `shot` step's back buffer from both clients (D69), paired by name in scenario order (.bmp, or
.png after the session's close), cut into 16x16 cells, every 2nd pixel and row; a cell differs when its mean
|difference| per channel is over 16 of 255. `e2e/shot_mask.txt` (its header has the rules) leaves out regions
that follow the wall clock on either client (the home sector's turning planet, the pop-up browser's live page),
compares a playback's shots (play1, play1_windows, the `_play` sessions) by the frame's mean brightness only (ships
and effects move: retail differs from retail in up to 207 of 256 cells) and skips w_entered. Packets the replay
server logs in the same millisecond on two connections are put in name order (/stop's VERB_REQUEST and MASTER_JOIN
came in either order on both clients). Calibrated 2026-11-19 at c7a92fe7f: retail vs retail and ours vs ours 25 of
25 SAME; ours vs retail (two pairs, each ~5 minutes, 25 of 25 PASS on both) 22 of 25 SAME, the same three DIFF both
times: pda_faction (ours draws no panel behind the six faction rows) and play1 / play1_windows (sector 4505 about 10
of 255 darker on ours in every in-sector shot; the home sector equal).

**No registry write (D249):** the client calls BrowserEngine.dll's `DllRegisterServer` at every start (the DLL's own
COM keys, the server path being the registering copy's). Every harness run sets `D3D8SHOT_NOREGISTER=1`: the proxy
hands the client's `GetProcAddress(.., "DllRegisterServer")` a no-op (d3d8shot.log: `DllRegisterServer: not
called`), and the browser opens through the registration the install made.

**Stand-ins per scenario:** stubs.log lists a stand-in's first call only. At each scenario's first own step e2e
writes `mark NAME` through the proxy's trigger file; the proxy calls the exe's exported `Stub_Log_Mark`
(stubs/stub_log.cpp), which writes `MARK NAME` and clears every first-call flag, so the stand-ins a later
scenario hits are listed again for it. `gen` is every generated stand-in hit while the scenario ran, `gen_own`
those hit in its own steps that the session's login had not hit (the queue's score input, P-M2-6). A build
without the export still runs (`no stand-in marker` in e2e.log; later scenarios then list only new ones).

**Shots on events:** `shot NAME` writes the proxy's trigger file (`D3D8SHOT_TRIGGER`, d3d8shot.c: looked at every
8th Present); the next frame is saved as `shot_NAME_NNNNN.png` from the client's own back buffer (D69). The proxy
in `build/d3d8shot/` must be current: `python tools/harness/run_reference.py build` after any change to
d3d8shot.c (a proxy of 2026-11-04 ignored the trigger, and every shot step waited out its 15 s). A trigger file
the proxy cannot open yet, or reads empty, is read again at a later Present (2026-11-25: taken at once it was
saved as `shot_shot_N`, the shot's name lost, while e2e still logged it taken).
**Draw log per shot (test-plan C3):** e2e sets `D3D8SHOT_DRAWLOG=1` for every session, so each shot is taken one
frame after the trigger was seen (the trigger file stays until then) and that frame's draw calls go to
`shot_NAME_NNNNN.json` beside the image (written before it): every draw (call, primitive type and count, vertices,
stride, FVF, the textures of stages 0 and 1, the world matrix, 34 render states, 13 stage states of stages 0 and
1), the frame's distinct view and projection matrices with the order they were set in, and each texture drawn
(creation parameters and an FNV-1a 64 hash of level 0's contents at that frame). The schema is above
d3d8shot.c's `Log_Draw`. Without the variable nothing changes (the shot in the trigger's own frame, no JSON).
`expect-ink SHOT X,Y,W,H MIN` (2026-11-21) asserts on that shot: at least MIN pixels in the box whose brightest
channel reaches 200 (`INK_LEVEL`; the UI's text is near white on the dark blue panels), so a scenario checks that a list
was filled, not only that its packets went (sb_refinery's ore rows, sb_manufacture's second category level).

**Station operations on a live server (2026-11-15):** `e2e.py --live [SERVER] [--retail] --instance NAME run
sb_vendor sb_talk` (options before `run`: `--live` takes an optional SERVER, so `--live run` would read `run` as
the server) runs the `sb_*` scenarios (and only those; the default set leaves them out) against a real Net-7
server through antprox `/SERVER:SERVER /NET7BIN:DIR /OPCODES` instead of `/REPLAY`. SERVER defaults to
`localhost`, `--net7bin` to the user's local 2010 server (`C:\EnBDevelopment\Net7\bin_local`, account admin /
admin, safe for any scenario). `--antprox-arg ARG` (repeatable; `--antprox-arg=ARG` when ARG starts with `-`)
appends ARG to antprox's command line. The proxy is machine.py's `antprox`, the same one replay runs use (since
2026-11-25: the store's antprox-rs.exe; the C# dev publish's role here, while the frozen 1.0.0 could not bind
loopback (D253), has ended). Both clients run live: the retail copy (`--retail`) and ours (`--exe`, default
this tree's build), which the C# dev publish launched unmodified on EA-UDP since it cannot patch an unknown build
(not re-checked with antprox-rs). The login (`_login_live.scn`) clicks the username field twice (the login screen
swallows the first click) and clears each field (End, 14 Backspaces) before typing, since the client remembers
the last name. Live antprox logs opcode numbers; e2e appends the names (`... = sent TALK_TREE` is the server's,
`= received NAME` the client's), so steps match on names as in replay.
- **Putting the character back (`--db-snapshot [AVATAR]`, 2026-11-18, D270):** the `sb_*` scenarios leave state on
  the character (a warhead per sb_buysell, a Deploy Satellite job item per accepted job, components, credits).
  `e2e.py --live --db-snapshot run sb_...` first dumps AVATAR's (default 6) rows of the play tables (`DB_TABLES`:
  inventory, vault, trade, equipment, ammo, level info with the credits, missions, recipes, skills, factions,
  exploration, position, avatar_info, ship_data / ship_info) from the `net7-mysql` container into
  `.work/e2e/<YYYYMMDD-HHMMSS>-db-snapshot/avatar<N>.sql`, a ready restore script (one transaction: `DELETE ... WHERE
  avatar_id = N` per table, the dumped INSERTs; foreign key checks off in that session only, since avatar_data and
  ship_* reference avatar_info). After the run, also when it fails or is interrupted, it restores the rows in place (next bullet; before 2026-11-24 it stopped Net7.exe (and
  Net7SSL.exe), runs the script, and starts Net7.exe again in its own console, ready when UDP port 3693 is bound
  (~100 s; the server log cannot be read while Net7.exe runs) and started it again). The stop was needed: Net-7 writes each change through
  as it happens and reads the rows at login, but a killed client stays in the server until its link-dead drop,
  which queues hull, sector and ammo saves of its own (`PlayerManager::DropPlayerFromSector`). Nothing outside the
  avatar's rows is touched; to undo by hand, pipe the script into `docker exec -i net7-mysql sh -c 'mysql -uroot
  -p"$MYSQL_ROOT_PASSWORD" net7_user'` with the server stopped.
- **The account screens (`ui_*`, test-plan UI-01 / UI-02, 2026-11-26, CS6):** `ui_charcreate` creates a character
  (every class, every avatar tab, Random Appearance on the head and body tabs, the ship pages, the name, the server
  list, Play Game) and deletes it from character select; `ui_account` walks the login screen's Account Management
  menu (Edit Account's No, Register Account's notice and second page, Add Gametime Code's Back), every slot, every
  Previous out of creation, a relogin, the world and Quit to Login. Named only, live only:
  `e2e.py --live --retail --instance X --audio --db-snapshot schema --antprox-arg=/NOPATCHES run ui_charcreate`.
  `--db-snapshot schema` dumps the whole `net7_user` schema (DROP / CREATE with each AUTO_INCREMENT, one consistent
  read) and puts it back in place after the run (0.9 s): a created character takes a new avatar_id in every avatar
  table and moves faction_data's auto-increment, which an avatar's rows cannot undo. `/NOPATCHES`: antprox-rs
  patches the retail client for Net-7 (class names and flags, tattoo save) and never ours (an unknown build), and
  the patched retail stores another character for the same clicks (prof 1 against 2): retail is captured unpatched,
  as ours runs (`baseline.py capture --antprox-arg=/NOPATCHES`, recorded in the manifest's `server.antprox_args`).
  The header `tutorial: fresh` removes the instance's tutorial.dat: while it exists
  `TutorialManagerClass::Read_Completed_File` (0x6AD4B0) marks every creation step done and no creation voice line
  plays; without it retail plays 1492_90_001Se at login, 328se on the class page, 347se on the avatar page and the
  ship page's 300se..303se in a row (about 38 s; a ship-tab click before 303se starts clears the queue). The
  `audio` source (`--audio`) gives each named sound's `open` / `start` / `stop` / `end` / `close` as
  `start m28 FILE`, so a step can wait on a voice line. The step `db-avatars NAME` writes the rows of the characters
  the account gained since the session started to `db-NAME.sql`: avatar_data (parts, colours, the blend pulls
  height_weight_0..4), ship_data, faction_data, as the server stored the creation request. Random Appearance draws
  from a generator that repeats run to run: the stored character is the same on every unpatched retail run and on
  ours, column for column. The narrator's gesture on the login screen follows the returning-user line
  (`ReturningUser%d`, `time(NULL) % 3`, 0x5ADA50): login-screen shots compare only between runs that played the
  same 323Se / 324Se / 325Se. Character select's and the creation pages' buttons take a click only after their
  turn-in (about 3 s after a page change) and the login screen's Account Management only after the pointer
  rested on it.
- **Restoring in place (control port, 2026-11-24):** Net7.exe has a loopback UDP control port (`Net7/Net7/ControlPort.cpp`):
  `e2e.net7_control_port` takes machine.py `net7_control_port` (`ENB_NET7_CONTROL_PORT`) when set, else the last
  `control_port=` line of `<net7_bin>\Net7Config.cfg` (the server reads it at start), else 3810; 0 is off. Commands:
  `STATUS` (`READY players=N saves=idle|busy ssl=0|1 quiesced=0|1`, or `STARTING`), `QUIESCE` (logs every player out,
  drains the save queue, refuses logins; `QUIESCED`, or `ERROR timeout` after 30 s) and `RESUME`. `net7_ready` is
  `READY` with `ssl=1` and `quiesced=0` (a server left quiesced refuses logins). `e2e.db_restore` (--db-snapshot and
  baseline.py's database step) uses them when STATUS says READY: QUIESCE, the scripts, RESUME, 0.4-0.8 s, no restart (a
  client still logged in, or link-dead, is dropped by the QUIESCE, so no late save overwrites the rows: checked 70 s after
  a sb_buysell run). Without the control port, after a failed QUIESCE (undone with a RESUME first), or with
  `ENB_NET7_RESTART=1` / `restart=True`, it stops Net7.exe, restores and starts it (`e2e.net7_start`): 5-6 s since the
  start-up rework (was ~80-100 s); a server that answers is first given up to `NET7_SAVE_WAIT` (20 s) for STATUS to show
  `saves=idle`, so the kill does not cut a save. Each script is its own transaction, applied in order; a failure stops
  there, names the script (`restore FAILED at PATH (N of M scripts applied before it ...)`) and makes db_restore return
  False (the server is still resumed or started). Its `info` dict records the mode (`in-place` | `restart`), the seconds,
  the port, the STATUS line after and the failed script. Not reset by an in-place restore: the server's world state (MOBs
  killed or created, asteroids mined or `/create`d, the job board's generated jobs), which is why a
  space set (`sp_*`: `sp_combat` creates and kills a MOB) is captured with a restart before each run (baseline.py
  `--restart`, its default for a list with an `sp_` scenario); the `sb_*` station scenarios do not depend on it.
  The server also caches the guild roster at startup (`guild_members` joined to `avatar_data`; the Net-7 server's
  author, 2026-11-24): a scenario that changes guild membership or rank, or a restore that changes those rows, needs
  the restart mode. Accounts, avatar lists and player state are read from the database at each login, and no late
  save can land on avatars 6-8 after `QUIESCED`. `net7_start` starts Net7.exe outside the harness's process tree (a
  short-lived Python in between, a new console and process group, out of the caller's job when it allows): stopping
  a capture task or tree-killing its shell leaves the server running (2026-11-24: it killed it before).
- **Where the avatar is:** `D3D8SHOT_CAMERA=FILE` makes the proxy write the frame's first view matrices to FILE
  (every 2nd Present). The first is the room camera; the avatar is 6.4 units ahead of it. Each room has its own
  coordinates; the room changes show in messages.log (`Changing room from A to B`).
- **The camera's whole path (`--camera-track`, 2026-11-25):** a thread reads that file every 20 ms and appends each
  new frame's views to `camera-track.txt` (`TICK T FRAME N` and N times 16 floats; TICK is GetTickCount, the clock
  of d3d8shot.log, T the events.log clock): about every 2nd to 4th frame of the session. In space the frame's
  first view is an orthographic one (its projection is the draw log's first) and the second the ship camera; in a
  starbase the first is the room camera. For camera moves a shot per event only samples (a landing, a gate jump:
  `_play10`; test-plan SP-01 / SP-02). The camera controllers draw on one generator of a fixed seed (0xBDB1C0,
  0x1105), so where a camera holds still (the dock, warp and room cameras) is the same place on every run, to the
  tenth of a unit on retail: compare those plateaus, not single frames.
- **Walking steps:** `walk-to X,Y [tol T] [within S]`, `face YAW [tol D] [within S]`, `hold VK until SOURCE
  PATTERN [within S]` (walk through a door until the room changes), `pose` (logs it). They are closed loops on
  the pose, not timed input, since the avatar starts on a random pad of the hangar. Measured on retail: 'A' and a
  held right button both walk about 26 units a second with about 4.5 units of coast; the right button steers by
  the cursor's offset from where it went down (about 0.43 degrees a second per pixel); nothing turns in place
  (a 180 degree turn takes about 11 s and drifts about 9 units); turning is ignored during the hangar's entry
  camera move. A wall stops a walk: give a waypoint. `_sb_home.scn` logs in (`_login_live.scn`) and walks from
  any pad to room 1's middle; every `sb_*` scenario starts and ends there.
- **Recording a person's session:** `e2e.py record --live --retail --instance NAME` starts the same session with
  the input tag off, so the user plays. It writes `recorded.txt` (time, each key and every button message: the
  downs (`click`, `rclick`, `mclick`, each with a back-buffer shot), the double-clicks (`dblclick`, ...) and the
  ups (`up`, `rup`, `mup`: where a drag let go; before 2026-11-18 only the downs were kept), the pose, the
  packets both ways until the next input) and `track.txt` (the walk and the room
  changes). A scenario is written from it by hand: poses become `walk-to` / `face`, clicks keep their
  coordinates, the packets become the events to wait for.
- **State:** GM chat commands put back what a scenario spends (`/createcredits N`, `/createitem ID COUNT`).
- **Space (`sp_*`, 2026-11-20):** a second character, Elanor (avatar 7, slot 1: Terran Enforcer, 50/50/50, hull
  upgrade 6, every Enforcer skill at its cap; set in the DB by hand, `.work/elanor_max.sql`), who logs in in space in
  Jupiter more than 5,000 units from Jove's Fury, so `/createmob ID` (GM: `mob_base.mob_id`; the server uses the
  mob's own level and refuses within 5,000 of a station) works where she appears. `_login_space.scn` is
  `_login_live_account.scn` (the account part of `_login_live`, split out) and `select.slot1`, clicked until the
  client takes it (its selection sound's mixfile_sound_01.mix), then Enter alone until LOGIN and Elanor's
  `player117440576_options.ini` asserted (2026-11-25: `redo 1` re-clicked the slot while the screen still turned and
  logged Zanor in docked). The `sp_*` scenarios
  are in neither default set: name them, and pass the avatar to the snapshot, `e2e.py --live --retail
  --db-snapshot 7 --instance space run sp_combat`. `sp_combat` (from `.work/e2e/223904-record`): `/createmob 1993`
  (Renegade Progen Tribuni, level 45), E ([Target Near Enemy]; repeated with `key VK until ...`, since right after the
  CREATE the client may not know the mob yet), the hotbar's slots 4 and 5 clicked (SKILL_ABILITY each, then the
  server's answer; the number keys sent nothing: the recording's Rally and Repair Equipment, on the store's rows of
  2026-11-24 Biocessation and Enrage Group, which the server answers on the Progen mob with a PRIORITY_MESSAGE and no
  OBJECT_EFFECT: the scenario's wait for an effect failed on retail every time until 2026-11-25), F (ACTION) until POINT_EFFECT and the hulk's CREATE, N until the
  hulk is the target (a one-verb VERB_UPDATE, its Loot: Next Target's list is by sphere distance, our own ship
  always at 0, the hulk at 0 only when the mob died within the two radii of us, about 218 between the centres, and
  then first by the object list's order; farther out our ship comes first, a VERB_UPDATE without verbs: CS4, 37
  runs, `sp_nexttarget` cuts sp_combat there and shoots the target window after each of two presses; ITEM_BASE
  comes only for loot bases the client has not seen), its loot button (LOOT_HULK_PERMISSION) and the loot window's first row until OPEN_INTERFACE (the hulk empty;
  the loot is random, so the number of takes varies). Retail passes, 65 s (149 s with the restore). The recording
  dragged a skill from the PDA's list to the hotbar; a replayed drag found the list scrolled elsewhere (the list
  keeps no fixed start). **The effects (2026-11-25 evening, CS2):** the hotbar is not in the database: it is the
  client data's `Data\client\output\shortcut.ini` (`[117440576_Skills]`: `<3/><Biorepression/><Biocessation/>`,
  `<22/><Enrage/><Enrage Group/>`; ClientPOC's, 2026-11-21, which every instance copies afresh). Each skill answer
  now gets 1 s (`wait-since`) before its shot. Then the PDA (96,918), its Skills page (71,258), the list's down arrow
  (322,805) 30 times to its end (fixed: Elanor's 19 skills end Reactor Tech, Repair Equipment, Shield Tech, Terran
  Culture; entries without abilities are half height) and its up arrow (322,593) once: Rally on top, its rank-1
  ability at 50,632, Repair Equipment's at 50,785. A click there uses nothing (no SKILL_ABILITY); `down` / `up`
  drags them onto slots 4 and 5 (the client loads skill_rally.dds), the PDA's X (322,258) closes it, and each slot
  is clicked until SKILL_ABILITY and the server's OBJECT_EFFECT (Net-7 ARally: the buff's effect 333 on our ship;
  ARepairEquipment: 170 on our ship, a mob target defaulting to self, then repair ticks): shots `pda_skills`,
  `pda_dragged`, `skill_rally`, `skill_repair`. Retail passes, 93 s.
- **Flight (`sp_flight`, 2026-11-21):** every flight input, each a `pose-mark`, the input, its packet (MOVE: thrust;
  TURN: yaw; TILT: pitch) and `expect-pose` on the camera: the thrust gadget's upper half (640,878: forward, again
  stops) and lower half (640,968: reverse, again stops), A and Z held (`hold VK for S`), the four arrow keys, the
  right button held with the pointer left / right / above / below (`rdrag X,Y for S`: the nose follows it), then W
  (the nearest nav point, Jupiter Path the First, too close: the server answers Q with a MESSAGE_STRING), the
  target panel's >> twice (1248,987: Nav Group Shrine, 54.95k) and Q: WARP, WARP_INDEX about 19 s later, about
  50k on. Retail and ours pass (~250 s with the restore) with the SAME 201 packets. New steps for it (e2e.py's
  docstring): `hold VK for S` (a down every 50 ms, a fixed count: each thrust down sends a MOVE), `rdrag`,
  `pose-mark`, `expect-pose forward|back|left|right|up|down|moved MIN` (the best of the poses sampled during the
  input, since the chase camera lags and the ship coasts). Left raises the pose's yaw, up its pitch.
- **Private channels (`sp_chat_private`, 2026-11-21):** Chat -> Private Channel -> Join -> New sends
  CLIENT_CHAT_REQUEST type 4 and the local server answers nothing (its CCE_INSERT_CHANNEL branch is empty), so no
  window follows and the chat panel's request flag stays set; Join -> Existing's box takes digits only (edit flags
  0xC4, stredit's 4: 'ab12' shows '12'), and its Confirm then prints 'Another request is pending' and sends nothing.
  Retail and ours alike (the scenario's header has the coordinates and the client's functions).
- **Gate, landing, dock with the camera's path (`sp_gate`, `sp_land`, `sp_dock`; test-plan SP-01 to SP-03,
  2026-11-26, CS5):** Elanor from her login place: `sp_gate` warps to the Sector Gate to Saturn (W, >> four times,
  Q) and clicks its verb (1238,672): Net-7 opens the gate, sends CAMERA_CONTROL 4 5.8 s later, the client's
  stargate camera draws two or three frames, the client's own ACTION about 0.3 s later, the hand-off to Saturn;
  `sp_land` wormholes to Endriago (GM `/wormhole 2210`), warps to the planet (W, >> twice, Q) and clicks Land:
  the client's Land verb (ui_verb.cpp: ACTION 0x1D, camera request 8) runs the landing camera (state 5,
  camctrl_land.cpp 0x793CE0), which holds one place about 4 s while the ship flies its spline (shots `land_1s`,
  `land_2s5` hold the ship's place in their draw logs), then the surface and its Launchpad (W, the verb):
  CAMERA_CONTROL 4 there runs the stargate camera (state 3) as at a gate in space; `sp_dock` warps to Jove's Fury
  (W, >> three times, Q) and docks: CAMERA_CONTROL 5 (request 10) runs the docking camera (state 6,
  camctrl_dock.cpp 0x792750) about 5 s; then it walks to the hangar's ship as `sb_rooms` does and launches. Not
  reachable on the local Net-7: the planet-landing camera (state 7, camctrl_pland.cpp 0x794A90; CAMERA_CONTROL 1,
  which Net-7 never sends; GM `/ccamera 1` beside the planet changed nothing) and the take-off camera (state 9,
  camctrl_takeoff.cpp; the cockpit's warp button in view mode 1, ui_cpit.cpp 0x583250: clicks at 640,922, 640,912
  and 690,920 on Endriago's surface changed nothing, as R10 found on the replay). Run each with `--camera-track
  --db-snapshot 7`; the store's runs come from `baseline.py capture sp_gate sp_land sp_dock --live --db-snapshot 7
  --camera-track` (`--camera-track` stores camera-track.txt). **Restart Net-7 before every run of either client**
  (baseline.py capture does for `sp_*`; for ours `ENB_NET7_RESTART=1 baseline.py inputs --apply-db` first): the
  server's time since its start moves the planets (the space shots: Jupiter in front of the sun or beside it) and
  turns a planet surface's first view (a top-down one); on a server up for half an hour ours' shots differed from
  the store's by 55 cells where retail's own runs differ by 1-2. The gate warp's end has two kinds on both clients
  even so: the warp camera (0x797B50) cuts to its exit place either 4.1-4.2 s or 3.0-3.6 s before the server's
  WARP_INDEX, 1000-1400 units apart (retail 5 and 3 of 8 runs, ours 3 and 6 of 9; `sp_gate`'s later anchors follow
  it), so a run is compared with retail runs of its own kind (the store's three are all the first kind). `camtrack.py compare --retail
  DIR... --ours DIR... --scenario sp_land` compares each 3-D view of the track around the scenario's moments
  (SCENARIOS): the held places to 0.1 unit, the path by place (time-free) and the angle at the same time from the
  anchor give or take 0.1 s, the tolerance retail's own spread at that time plus each run's frame-to-frame shake;
  `%rel` anchors compare a camera that a cut puts beside the ship by its offset (the ship's own place at the
  server's CAMERA_CONTROL follows the server's timing). `camtrack.py ship ... --shots land_1s land_2s5 --ref
  surface` compares the ship's place under the held landing camera. Windows end before the scenario's next input
  (a camera move after it follows the harness's timing). Unit tests: `python -m unittest
  tools/harness/test_camtrack.py`.
- **Extended keys (2026-11-21):** the client's keyboard code (w32kbd.cpp) reads lParam's scan code and extended
  bit; an arrow key posted with lParam 1 is the numpad's and nothing happens. `clientrun.key_lparam` gives every
  posted key its scan code and, for the arrows, Insert / Delete / Home / End / PgUp / PgDn, the extended bit, as a
  keyboard does (the user's recording: Left 0x014B0001). The default set still passes 25 / 25 with it.
- First results: `sb_vendor` (room 2, a vendor's talk tree and trade window, back) and `sb_talk` (an NPC's
  two-page talk tree) pass on retail in one session, 224 s.
- 2026-11-18 (H1): `sb_buysell` (room 2's Miyoshi Basho: a vendor row and Buy, a cargo slot and Sell, each an
  INVENTORY_MOVE; selling below the price paid asks 'You will take significant losses on this sale' Yes / No first;
  a vendor refuses to buy back what she sells, Vila Mosj and Miyoshi alike: a warhead is bought, an Iron from
  sb_refinery sold) and `sb_jobs` (room 3's job terminal: JOB_LIST,
  the Combat / Trade / Explore tabs, the close box). The 2010 server listed no jobs (SectorManager::RefreshJobs
  found no mission with a job category: JOB_LIST 8 bytes); since 2026-11-18 the server and DB have them (below). The buy / sell clicks of the recordings miss their double-clicks: the
  recorder logs only button-downs (0x201 / 0x204 / 0x207), not 0x203 or the button-ups. Results: retail and ours
  (this tree's stand-in build at run/2026-11-18 1a45b1d78) pass all four `sb_*` in one session each (ours 417 s);
  `e2e.py compare` reads live runs too (packet names only: the live log has no sizes; the walk's
  STARBASE_AVATAR_CHANGE count follows the pilot, so it is left out), and sb_buysell, sb_jobs (4),
  sb_vendor (7) and sb_talk (5) send the SAME packets on both.
- 2026-11-18 (H1, phase 2): `sb_refinery` (room 1's refinery terminal at -29.7,-7.1: 'Choose resource to create'
  lists what the cargo's ores make; Iron, then Refine 1 takes 5 Iron Ore and 92 credits, MANUFACTURE_ACTION; the
  Iron reaches the cargo at the next login; `/createitem 2842 5` puts the ore back: a slash command goes out as
  CLIENT_CHAT, not CLIENT_CHAT_REQUEST, and the chat entry does not open while a terminal window is up) and
  `sb_manufacture` (room 1's manufacturing terminal at -43.4,40.6: the category tree, each node a
  MANUFACTURE_ACTION, to a leaf's item list, MANUFACTURE_ITEM_CATAGORY, and its item, MANUFACTURE_ITEM_ID; the
  server names the components '(missing)' and refuses; Manufacture stays disabled) and `sb_dismantle` (room 1's
  dismantling terminal at -52.9,34.4: a Vesta 4P dragged onto 'Place Item Here', INVENTORY_MOVE on the button-up,
  the parts listed, '<<Inv' takes it back, MANUFACTURE_ACTION; ammunition and ore do not dismantle and send
  nothing; the Vesta was put in cargo column 2 row 2 once with `/createitem 3246 1`). New steps for it: `down
  X,Y` and `up X,Y` (the left button pressed / moved there held and let go: a drag; the item windows move an item
  on the button-up). `compare` now counts each scenario's packets from its own first step (the proxy's `mark
  NAME`), so a session's first scenario no longer includes the login.
  Results: retail passes all seven `sb_*`; ours (wt/H1 after its ui_manu / sbaseview / ui_analyze fixes) passes
  all seven with no stand-in hit, and every scenario sends the SAME packets as retail (sb_buysell 9, sb_jobs 4,
  sb_manufacture 7, sb_refinery 8, sb_dismantle 5, sb_vendor 7, sb_talk 5). Before the fixes ours crashed
  opening the dismantling terminal (0x69F8F0 case 6 built a stand-in in place of UIAnalyzeClass 0x5435D0) and hit
  generated stand-ins for the manufacturing window's sort scope (0x541980 / 0x6E18B0).
- 2026-11-18 (H1, phase 3; the coordinator's Net7.exe and job missions 422 / 425-427, Joves Fury's terminal at
  level 10: the board fills on RefreshJobs' 5-minute refresh after the server starts, 5-9 random jobs): the room-3
  terminal is the one with the jobs. `sb_jobs_accept`: each tab and its first row (a tab alone sends nothing; a row
  sends STARBASE_REQUEST 7, the server's JOB_DESCRIPTION), Accept Job on the Explore tab's first job (action 8,
  JOB_ACCEPT_REPLY and the job item's ITEM_BASE), the close box (action 9; the server answers JOB_ACCEPT_REPLY too),
  then the PDA's missions (U): the new third mission, its Forfeit (603,356), Yes on 'Are you sure you want to
  forfeit this mission?' (500,405): MISSION_FORFEIT. A board without an Explore job fails it: rerun after the next
  refresh. Leftover: the Deploy Satellite job's item stays in the cargo (a stack, column 2 row 4) after the forfeit.
  JOB_DELETE is never sent (the server's notes). `sb_manufacture_make`: the Vesta 4P's three components by GM
  (`/createitem 1381 1`, `1399 1`, `1435 1`: BlackBox CoreCage, Evolver PowR-Prison, Vortex Dielectric
  Micro-Plates; net7 item_base), the tree to the Vesta 4P, Manufacture (MANUFACTURE_ACTION; about 3 s later the
  server's PRIORITY_MESSAGE 'NAME manufactured a 200% Vesta 4P', 'Attempt Succeeded', the Vesta in cargo column 2
  row 3), then the Vesta sold at Miyoshi Basho (PRIORITY_MESSAGE 'You have gained N credits'), so the cargo comes
  back as it was (a spare set of components from failed tries stays in column 3). Retail and ours (wt/H1B) pass
  both with no stand-in hit and the SAME packets (sb_jobs_accept 9, sb_manufacture_make 23).
- **A character saved in space** (the local server sometimes keeps the undocked place: the login ends beside
  Jove's Fury and `_sb_home` fails at its first walk-to, pose ~116000,-224000): `e2e.py --live localhost
  --retail --instance NAME run _sb_dock` (named only) logs in, targets the station (641,1009, SET_TARGET) and
  docks (the target panel's right verb, 1238,672, until SERVER_HANDOFF); then run the `sb_*` set again.
- **The Vault and the rooms (test-plan UI-04 and UI-03, 2026-11-25):** `sb_vault_ops` opens the Vault with
  Shift+I, drags cargo column 1 row 1 onto the vault's row 2 column 1 and back (INVENTORY_MOVE each, the server's
  AUX_DATA), then the Sort button (821,300) three times with the left button (the sort key, 0x657DA0 false) and
  twice with the right (the reverse flag: 'BUTTON01' is two gadgets on one mesh, button masks 5 and 0x50), each an
  INVENTORY_SORT, and its exit box (866,331). Net-7 sorts at most once in 10 s (`Player::HandleInventorySort`, else
  'Cargo sort machine resetting'), so each later sort waits `alive 10.5`. The cargo ends as it began (the database's
  slots too); the vault ends sorted by the client's starting key (the server keeps that order in memory: after
  an `sb_*` run's database restore the first run shows the stored order again). `sb_rooms` walks
  Jove's Fury's four rooms (0 hangar, 1 main deck, 2 bazaar, 3 lounge: net7 `starbase_rooms` 41-44), a `pose` and a
  shot (with its draw log) per spot, opens and closes every terminal (`starbase_terminals`: room 1's analyze,
  manufacturing and refining, room 3's jobs and Intergalactic Net, whose JWEB00 window loads a live page from the
  internet; close box 1259,215), an `aim_` shot of each terminal's view before its click, the NPC name tags in the
  room shots, and leaves: the hangar's ship clicked (770,284 at 50,-5 facing -124: room 0 -> -1, STARBASE_REQUEST,
  the hand-off to space), docked again as `_sb_dock` and walked home (~470 s on retail). Two traps of the walk: a
  turn in place drifts the avatar a few units, so each terminal is approached on a heading close to its `face`
  (manufacturing from the dismantling spot: from -47,30 the turn left the terminal off its screen point); and a
  terminal is lit (its tooltip, the click taken) only with the avatar in reach, the refinery's edge at camera y
  about -5.0 (-4.8 refused on retail and ours alike), so its last leg aims past it (-5.5 tol 2.5) against the walk's
  2.5-4.2 units of coast. Retail's own spread over three runs: the terminal shots identical (0 cells, the same draw
  logs; the job board's rows are the server's random jobs), the vault and cargo windows identical, the room shots
  up to 95 of 256 cells (ambient avatars, animated models, the arrival pose). Ours at a9fa08ee3 (2026-11-25): both
  pass with the SAME packets and the same server answers (AUX_DATA lengths) to every move and sort, the vault and
  cargo windows identical to retail's after every step, every terminal shot identical, the room shots inside
  retail's spread. The live compare reads packet names only; the server's answers stand in for the payloads.

**Smoke set (D237):** a landing that changes the client's behaviour runs `e2e.py run --exe <its worktree's
build\programs\client\client.exe> --instance <name>` and lands only with every line as expected.

`vidtest_e2e.py` and `vidtest_exitcodes.py` stay two scripts: one reproduces E&BConfig's launch
(ctypes `CreateProcessA`, the working directory trick, the results.ini reader) and one drives
many cases by posting window messages; they share no code worth extracting.

## Machine config and the baseline store (`machine.py`, `baseline.py`; test-plan C1)

**`machine.py`** holds every path the harness reads outside the tree: `client_data` (ClientPOC), `ea_data`
(ClientPOC_EA), `retail_install`, `antprox_dir` (the proxy pair's folder: the store's `tools/antprox`, default
`<baselines>/tools/antprox`), `antprox` (live runs' proxy: `<antprox_dir>/antprox-rs.exe`, the
Rust port 1.2.0.0), `antprox_replay` (replay runs' proxy until antprox-rs can replay: `<antprox_dir>/antprox.exe`, C# 1.1.0), `antreplay` (the replay server antprox `/REPLAY` starts: `<antprox_dir>/antreplay.exe`, C#
1.1.0; antprox looks for it beside itself, so clientrun.py passes `/REPLAYEXE:` only when it is elsewhere; without
the folder both fall back to AntaresProxy-Rust's `target\release` and AntaresProxy's `releases\antprox-1.1.0-win-x64`,
and machine.py's printout says so), `net7_bin`,
`baselines` (the store's checkout), `captures` (AntaresProxy's), `recordings` (the folder antprox `/REPLAY` reads,
`/RECORDINGS:`; default `<baselines>/inputs/captures`, the store's copy of the replay set: clientrun.py passes it
when the folder exists, else antprox's own default `%LOCALAPPDATA%\AntaresProxy\recordings`), and `label` (this
machine's name in the store, default a slug of the GPU's name). Each key: its environment variable
(`ENB_CLIENT_DATA`, `ENB_EA_DATA`, `ENB_RETAIL_INSTALL`, `ENB_ANTPROX_DIR`, `ANTPROX_EXE` (it also overrides `antprox_replay`), `ENB_ANTPROX_REPLAY`, `ENB_ANTREPLAY`, `ENB_NET7_BIN`,
`ENB_BASELINES`, `ENB_CAPTURES`, `ENB_RECORDINGS`, `ENB_MACHINE_LABEL`), else the config file (`ENB_MACHINE`, else `~/.enb/machine.json`; `machine.example.json`;
relative paths are relative to the file), else a default beside the main checkout (a worktree's `.git` file is
followed to it). `python tools/harness/machine.py` prints each resolved path, its source and whether it exists.
clientrun.py, e2e.py and run_reference.py take their paths from it. (`tools/re/roots.py` still names the
build-side roots: EA, SDK, references.)

**`baseline.py`** keeps retail runs in the private store `Open-EnB/baselines` (README there: layout, LFS, the
rule: observations and run inputs only, never the game's files):

```
python tools/harness/baseline.py capture NAME... [--runs 3] [--live [SERVER]] [--restart | --no-restart] [--no-db-restore] [--no-audio] [--db-snapshot [AVATAR|schema]] [--antprox-arg=ARG] [--instance bl]
python tools/harness/baseline.py capture --set SET [NAME...] [--runs 3] [--live [SERVER]] [--restart | --no-restart]   # one session a run
python tools/harness/baseline.py verify [NAME...]            # every stored file against its manifest's SHA-256
python tools/harness/baseline.py verify --run RUN [--client C] [--env E]    # an e2e run against the baseline
python tools/harness/baseline.py list
python tools/harness/baseline.py inputs [SET...] [--write] [--save-db] [--apply-db]   # the run inputs
```

`capture` runs `e2e.py --retail --keep --out <store>/.staging` once per run and scenario (each scenario its own
client, scenario after scenario; until `--runs` PASS runs or more than `--retries` failed ones). With `--live` it
first puts the store's database state back (`inputs/db`, below; in place through Net7.exe's control port, under a second; restarted instead with `ENB_NET7_RESTART=1`, about 6 s)
before every run, unless `--no-db-restore`; `--restart` restarts Net7.exe for it before every run instead (also with
`--no-db-restore`: a restart without scripts), the default when a named scenario is a space one (`sp_*`: MOBs, asteroids
and spawned mobs stay in the server's memory across an in-place restore; `--no-restart` overrides). The manifest's
`server` records what was done: `mode` (live / replay), `restore` (`in-place` | `restart` | `none`), `seconds`,
`db_scripts` (one SHA-256 over the applied scripts' paths and hashes), `status` (the control port's STATUS line after
it), `control_port`, `failed` (a script that failed: the run then stops), with `db.restored_before` as before; a
set's manifest also has `session_seconds`. It stores each PASS as `runs/<client>/<env>/<scenario>/<UTC YYYYMMDD-HHMMSS>/`
(`<client>` `retail-<8 hex of the exe's SHA-256>`, `<env>` `<label>-<W>x<H>` from the shots) with e2e.log,
events.log, antprox.out, d3d8shot.log, camera.txt, messages.log, result-NAME.json, the scenario's shots, a
`packets.json` (messages.log as it is up to 1 MB, `MESSAGES_RAW_MAX`; a larger one, a space session's 49-58 MB of
effect_description.cpp's three asserts a frame and frameint.cpp's frame-rate line, is stored filtered by
`filter_messages`: the frame-rate lines dropped, an assert repeated since the last other line folded to one
`[repeated N x]` line, cut at 4 MB, `MESSAGES_MAX`: 48.8 MB -> 0.3 MB; the full log stays in the e2e session folder in
`.staging`, and the manifest's `messages_log` gives its size, SHA-256, lines and what was dropped, folded or cut;
packets.json is e2e.sent_packets) and `manifest.json` (client SHA-256, data id: a hash over (path, size) of the
data root plus the content of files up to 16 KB; the client's registry settings, read only: HKLM's 32-bit view
and UAC's VirtualStore copy that a run reads; the instance's `Data\client\ini` hash; resolution, GPU, driver, OS;
the d3d8 proxy's SHA-256; antprox's file name, `/DESCRIBE` version and SHA-256 and antreplay's name, version and
SHA-256 (with `used`: false for a live run, which does not start it); EnB commit and dirty flag;
the scenario's files' SHA-256; the server mode, with `server.db.restored_before` (each restored script's
SHA-256) for a live run and `server.recordings` (the folder, its set hash, whether it is the store's copy) for a
replay run; every kept file's SHA-256 and size), then rewrites the scenario's
`noise.json` (every stored run against every other, raw `shot_diff`, no shot mask: differing cells and mean per
pair, the max per shot; whether packets and verdicts were equal). A failed run is reported and stays in
`.staging`. `verify` needs neither the game nor Windows; a file still a Git LFS pointer is reported as "run git
lfs pull". `verify --run` (an e2e session folder, the e2e `--out` folder: the session folders of its newest run, a
fresh one after each failure; folders joined by `,`, an e2e `--manifest` file or a stored run): verdict equal,
packets equal to at least one stored run's, each shot's differing cells against its best-matching stored run at most
the noise max; one SAME / DIFF line per scenario, then every scenario not compared and why (left out by NAME
arguments; a stored set's the run has no result for: `SKIP` under `--set`, exit 1). **Stale builds:** for a run of
our build (e2e.log's session line names the exe; a retail run is exempt) it first prints the exe's link time (the PE
header's TimeDateStamp, else the file's mtime) and SHA-256; when a commit touching the client's sources is newer
(`CLIENT_SOURCES`: `app` and every other top-level folder the client links from: `binkmovie`, `libs`, `programs`,
`stubs`, `ww3d2`, `wwaudio`, `wwdebug`, `wwlib`, `wwmath`, `wwsaveload`, `wwshade`; `git log` in the tree baseline.py
runs from), the first line is `STALE BUILD: exe linked <time>, sources changed <time> (<n> commits since)` and verify
exits 2 unless `--allow-stale` (the line is printed either way). An exe named but missing counts as stale.

**Sets** (the user, 2026-11-25): `capture --set SET [NAME...]` runs the scenarios (none named: e2e's default set,
`e2e.scenario_names`) as ONE e2e session a run, the way e2e runs its default set: one client, the login once, each
later scenario its own steps; three runs are three client launches. Scenarios that need a session of their own
(`SESSION_OWN` in baseline.py: `sb_emote_dock`, which starts in the hangar the login leaves the character in and ends
docked there again, while every other station scenario's own steps start and end in the concourse: after it,
sb_buysell's first walk hits a wall; `sb_manufacture_make`, which after sb_buysell in one session never sees
`sent ITEM_BASE` at step 84 and passes alone) are taken out of a set and captured alone, each in its own scenario folder. A run is stored only when every scenario
passed in that one session (e2e starts a fresh session after a failure: such a run is reported and left in
`.staging`), under `runs/<client>/<env>/<SET>/<UTC run>/`: one session's logs, every scenario's result, shots and draw
logs, `packets.json` {scenario: packets}, and a manifest with `set`, `order` (the scenarios as run) and `scenarios`
([{name, verdict, failure, shots, drawlogs}]). Its `noise.json` is keyed by scenario, then shot (`scenarios`), with
`order_equal`. A scenario captured alone keeps its own folder (the earlier runs stay valid). `verify --run` finds each
scenario of the run inside the stored folders (`dir_scenarios`): a run of several scenarios prefers a set that ran
it, a run of one its own folder, `--set SET` names one; it says when the scenario ran after a different one than in
the stored run ("scenario order differs": state carried from the scenario before can explain a difference; "a
fresh session" when e2e restarted the client before it). **Such a scenario is `NOT COMPARABLE`, not DIFF, when only
its packets, shots, draw logs or audio differ** (2026-11-25: the scenario before changes texture address modes, the
draw order and matrices, retail against retail too: pda_faction alone against the default set read DIFF on retail
itself); the line lists what differed and says to compare a run in the stored order. A verdict that differs or a shot
missing on either side stays DIFF. **`RETAIL-UNSTABLE`** (2026-11-25 evening, coordinator call c2; never SAME, never
folded into a pass, no tolerance widened): a scenario whose only differences are where the stored retail runs
disagree among themselves, leave one out (as `.work/cs/loo.py` measured: station 3D audio identical in 0 of 9 retail
runs). Audio (`audio_stability`): per sound family (`audio_family`: the file name with each digit run as `#`, so the
client's random variants share one: the space music TerranWander07-2 / 01-5, sfx_foot_glass_N; a set the client
draws from by its own code is one sound whatever the variant (`AUDIO_RANDOM_SETS`, with its evidence: the login
greeting 1492_00_323Se / 324Se / 325Se, 0x5ADA50's `ReturningUser%d` by `time(NULL) % 3`), so a run that drew
another of them matches, in noise.json's strict sequence too; samples the log gives only as hash:size are named first from the client data's files and mixes,
`name_samples`, audiolog's cache), the `m` / `t` events of its streams or 3D samples (`KIND FAMILY`): a family whose
events in the run match no stored run's is weighed by its shape (events and loops): the same shape as a stored run,
only another variant played, RETAIL-UNSTABLE when the stored runs' variants fail leave one out, else DIFF (retail
always played that one); another shape, RETAIL-UNSTABLE when the stored runs' shapes fail leave one out, else DIFF (a
family no stored run has: DIFF); every family matching but the interleaving differing: RETAIL-UNSTABLE only when the
stored runs' whole sequences fail leave one out too. The line names the scenario, the families and the run's variants. Draw logs (`drawlog_stability`, per shot): each stored run's log against the
other stored runs' (their own noise); a run's DIFF is RETAIL-UNSTABLE only when every reason it failed for
(`drawlog_verdict`'s REASONS: `states:FIELD`, `states:new STATE`, `textures`, `draws`, `cams`, `worlds`) is one a
left-out stored run fails for too; a DIFF says which reasons retail's own runs show. With one stored run nothing is
measured: DIFF. Packets (`packet_verdict`, brief CS3, c9): a run's packets equal to no stored run's are weighed by
the kinds of difference (`packet_reasons`: `count:NAME` sent another number of times, `size:NAME` in sizes the other
lacks, `order` the rest the same in another order); RETAIL-UNSTABLE when against some stored run every kind is one a
stored run shows against its nearest other stored run, leave one out (`packet_stability`: sp_combat's random loot,
the number of INVENTORY_MOVE takes; the job board's list), else DIFF with the kinds against the nearest stored run
and those retail's runs show. Shots and 2D counts are not covered: a shot keeps its stored noise max (DIFF as
before). Exit codes: 0 every scenario SAME,
1 any DIFF (or a `--set` scenario missing), 2 a stale build, 4 no DIFF but some RETAIL-UNSTABLE, 3 neither but some
not comparable; the count line says how many of each. Tests: `test_baseline.py` Audio / DrawLogs `retail_unstable`. A live
set restores the database state once a run, before the session.

**Draw logs** (test-plan C3; d3d8shot `D3D8SHOT_DRAWLOG`, above): a stored run keeps each shot's
`shot_NAME_NNNNN.json` (manifest `drawlogs`: [key, file]). `noise.json` gives each shot a `drawlog` entry: the runs,
whether their logs are `identical` (all but the frame number), the draw count range, the `varying` fields, the
largest number of draws without a partner, `matrix_spread` (view, proj, world, sets: the largest element difference
between two runs) and the file sizes. The compare (`compare_drawlogs`): each draw flattened to fields (`call prim
prims verts stride fvf`, `tex0.*` / `tex1.*` the texture's parameters and hash, `rs.NAME`, `ts0.NAME`, `ts1.NAME`), the
two logs' draws aligned on (call, prim, fvf, stride) with difflib so one draw more does not shift the rest, then
field by field; matrices within `DRAW_MATRIX_SLACK` (1.5) times the stored spread plus `DRAW_MATRIX_FLOOR` (1e-4).
`verify --run` per shot: SAME when equal to a stored run's log or different only in fields and draw counts the stored
runs vary in themselves; else the multiset compare below decides, and a DIFF gives the counts, the first differing
draw and its fields (`N fields differ in M of P paired draws`), the matrices out of tolerance, then the multiset's
reasons. A run stored before the draw log (2026-11-24's) reads "no draw log stored", not a failure.
**The multiset compare** (2026-11-25, `multiset_verdict`): a moving scene (a playback's asteroids switching LOD,
ships reordering the draws) reads as DIFF in order although every state, texture and object is there, so a shot the
ordered compare fails is compared again as a multiset, against the stored logs themselves (not noise.json): each
draw's state is every flat field but `prims` / `verts` (the LOD); a state of the run no stored run has passes when
its nearest stored state (fewest fields apart; the same call, primitive, FVF and texture sizes first) differs only
in fields that differ so between two stored runs (the same the other way for a stored state the run lacks); each
state's draws are paired with the stored run's, the same LOD first, nearest by world translation, and a pair is out
when its world differs by more than that stored draw's own spread over the other stored runs (x `DRAW_MATRIX_SLACK`,
plus the floor); the draw counts per state (summed differences) and the set of textures may differ from the nearest
stored run as much as two stored runs differ (the counts x the slack); the number of draws out of their spread, of
those the draws still in every stored run, and the frame's views and projections may be as large as a stored run's
own against the other stored runs (leave one out: a playback's objects and second camera move by the shot's timing,
so retail has draws out of the spread two of its runs measure: 54-227 at play1's 50-55 s, against ours' 31-33); each
measure against the stored run nearest in it. SAME that way reads `draw logs K of N the same (M as a multiset)`.
With one stored run the multiset must be equal. Tests: `tools/test_baseline.py` DrawLogs.

**Audio** (test-plan C2; `capture` runs e2e with `--audio` unless `--no-audio`): the session's `audio.jsonl` is cut
per scenario by the results' `tick_start` / `tick_end` (e2e.py writes GetTickCount at a scenario's start and end; the
log's `tick` is the same clock): a scenario gets every event after the previous scenario's end up to its own (the first
one the client's start and the login too; what follows the last end is left out). **The cut** (2026-11-25 evening,
c3): e2e also writes `tick_input`, the tick of a scenario's first input step; with every later scenario's
`tick_input` in the results (`audio_cut_of`: `input`), a scenario's slice ends at the next scenario's first input
instead of its own end, so a sound its last input started that plays just after its end stays with it (sp_galaxymap's
message_1.wav was in sp_warp's slice, 1 ms in); the last scenario ends at its end; older results cut at the end
(`end`). Each summary records its `cut`, and `verify --run` cuts the run's log the way the stored runs were cut (all
`input`, else `end`). Each is stored as `audio-NAME.json`
(manifest `audio`): `sequence` (the open / start / stop / end / pause / resume / close events in order: event, handle
kind `m` stream / `t` 3D / `s` 2D, sound (a stream's file or a sample's `hash:size`), the loop count at a start, the frame
`f` and ms `t`), `names` (`hash:size` -> file, from the `load` events), `counts` (per sound, each event kind's count:
the per-frame volume / position events are counted, never stored), `events`, `ticks`, `frames`. The raw log is kept
as `audio.jsonl` only below 1 MB (`AUDIO_RAW_MAX`; a long session's is 5-10 MB); the manifest's `audio_log` gives its
size, SHA-256, event count and `kept`. `noise.json` per scenario: `audio` with `strict_identical` (the `m` / `t`
sequence of event, kind, sound and loop the same in every run), `strict_first_diff`, `loose` (each 2D `event kind
sound`'s count range over the runs), `loose_equal`, `events`. `verify --run` cuts the run's log the same way and
compares the `m` / `t` sequence strictly (equal to one stored run's; else the first difference against the stored run
it agrees with longest) and each 2D count within the stored range (else the first out of range); a run stored without
audio reads "no audio stored".

**The run inputs** (`<store>/inputs/`, test-plan section 7 item 2): `captures/` the replay set (the ten /REPLAY
entries of nine recordings, each `.pcapng` with its recorder `.json`, as in AntaresProxy's `captures/`; Git LFS),
`records/<YYYYMMDD-HHMMSS>/` the tracks of the e2e record sessions the scenarios came from (`recorded.txt`,
`track.txt`, `camera.txt`; local start time), `db/` the database state (`account1.sql`, `avatar6.sql` ...
`avatar8.sql`, `content/` and a README: how to bring a stock Net-7 database to it). `inputs/manifest.json`: each
file's SHA-256 and size; for a capture its replay ids (`REPLAY_IDS` in baseline.py: from the scenarios, else the
numbering rule) and the scenarios that `/play` it; for a track the scenarios that cite its record folder; one
hash per set (`sets.captures.sha256` is the recordings folder's identity in a replay run's manifest). `inputs`
checks every file against it (so does `verify`); `--write` rewrites it from the files; `--save-db` dumps account
1's row and every avatar of it (all 23 `net7_user` tables keyed by `avatar_id`: e2e.py `DB_AVATAR_TABLES`) as
delete-then-insert scripts that also create the rows on a stock database; `--apply-db` restores them.
`--db-snapshot` keeps its 17 tables.

**The proxies** (`<store>/tools/antprox/`): `antprox-rs.exe`, `antprox.exe` (C# 1.1.0, replay runs; below) and `antreplay.exe` (Git LFS) with a `manifest.json`
(version, SHA-256, size, the path each was copied from and that repository's HEAD then); `baseline.py tools --add
EXE...` copies and records one, `tools` and `verify` check them. Replay through the pair does not work yet
(2026-11-24): antprox-rs 1.2.0.0 logs "the original profile (P1) is not built yet in the Rust proxy. Nothing was
started" for `/REPLAY` (startup.rs: `/REPLAY` selects the original profile, which has no session code; the parsed
`/REPLAYEXE:` and `/RECORDINGS:` are read by nothing), so antreplay.exe is never started. Replay runs therefore start
the C# release from the store (`antprox_replay`, below) until then.
Tests: `tools/test_baseline.py` (CI's `python -m unittest discover -s tools -p "test_*.py"` runs it; under
tools/harness it was never collected), with the draw-log compare on synthetic logs.

**Two proxies for now (the user, 2026-11-25):** live runs start machine.py `antprox` (antprox-rs 1.2.0.0), replay
runs `antprox_replay` (`<antprox_dir>/antprox.exe`, the C# 1.1.0 release beside antreplay 1.1.0;
`ENB_ANTPROX_REPLAY`; clientrun.py `antprox_exe`). The split exists only because antprox-rs 1.2 cannot replay ("the
original profile (P1) is not built yet") and goes when it can. `ANTPROX_EXE` still overrides whichever a run uses.
A replay run's manifest names the proxy it started (`antprox`), antreplay (`used`: true) and the recordings set.

## Audio log (`mss32log.c`, `audiolog.py`; test-plan C2)

The audio counterpart of d3d8shot: `build/mss32log/mss32.dll`, an MSS32.DLL proxy that logs the client's Miles
calls as JSON lines and passes every call on unchanged. **Facts (2026-11-24):** retail, the install, ClientPOC and
ClientPOC_EA carry one Mss32.dll (SHA-256 `6f34ef0d…79cc`, Miles 6.1a, RAD 2001), 344 named exports at ordinals
1-344 (stdcall `_AIL_x@N`, among them `_DllMain@12`; fastcall `@stream_background@0`; 23 cdecl). Retail client.exe
imports 81 of them by name, our build 80 (no `_AIL_open_3D_listener@4`); mssfast.m3d (34) and mssds3dh.m3d (19)
and mp3dec.asi (6) import from `mss32.dll` too, so they bind to the proxy: only calls whose return address is
in the main exe's image are logged.

**Build:** `python tools/harness/audiolog.py build [--pool 1]` (pool VM, `cl /MT /O2`, as d3d8shot): 64 functions
written out with mss.h 6.1a's prototypes (`SDK/radtools/include_enb`), the other 280 naked `jmp [ptr]` thunks
generated by `audiolog.py gen` from the DLL's export table (argument bytes from the decorated names). Exports
are `__declspec(dllexport)`, no .def (toolchain.md, Miles fixes: a .def loses the decoration); the build checks
the built DLL's names and ordinals against the real one (344 of 344 the same). A wrapper with the wrong
argument bytes would export another name and fail that check.

**Use:** `e2e.py run ... --audio` (or `ENB_E2E_AUDIO=1`; default off) puts it in the instance for the session:
`clientrun.Inst.use_audio_log` copies the source's Mss32.dll to `mss32r.dll` and the proxy over `Mss32.dll`
(instances only, never the shared release folder), sets `MSS32LOG_FILE=<run>/audio.jsonl`, and
`unuse_audio_log` puts the real DLL back at the end. Without `MSS32LOG_FILE` the proxy logs nothing.

**Schema** (one object a line, keys in this order): `i` (0 the header `log`, then 1, 2, ...), `t` (ms since the
log opened, QueryPerformanceCounter), `tick` (GetTickCount, d3d8shot.log's clock), `f` (the D3D frame: d3d8shot's
`D3d8shot_Frame` export since 2026-11-24, the frame being drawn numbered as the shots are (Presented + 1); absent
with an older d3d8.dll), `ev`, then the fields. Handles are ids in creation order: `d` driver, `p` 3D provider, `f` filter, `l`
listener, `s` 2D sample, `t` 3D sample, `m` stream, `u` unseen. Events: `startup`, `shutdown`, `preference`
(n, v), `driver_open` (api, h, rate, ch, bits) / `driver_close`, `master_volume`, `provider_enum` (h, name),
`provider_open` (h, ret), `provider_close`, `listener_open` / `listener_close`, `speaker_type`, `room_type`,
`filter_enum`, `filter_pref`, `alloc` / `release` / `init`, `load` (a memory sample: type, size, hash =
FNV-1a 64 of the bytes; a 3D sample's size is its RIFF length), `open` (a stream: file, sample), `close`,
`start`, `stop`, `end`, `pause`, `resume` (with `pos_ms` / `len_ms`, a 3D sample's `pos` / `len` in bytes, read
just before), `volume`, `pan`, `rate`, `loop`, `loop_block`, `seek_ms`, `seek`, `processor`, `position` (x, y, z),
`orientation` (fx..uz), `velocity`, `distances` (max, min), `effects`, `cone`. Events on a sound carry `snd`: the
stream's file, or `hash:size`; `audiolog.py` names those from the data's mix files (MIX1 entries of the same
size, hashed; cached in `.work/audio-names.json`).

**AU-01's reading (2026-11-26):** `baseline.py verify --run RUN --audio-only [--set SET]` prints the audio verdict
alone, one line a scenario (`AUDIO SAME | RETAIL-UNSTABLE | DIFF | NONE`: streams and 3D sounds in order, 2D sounds
by count against the stored range), and a count line; packets, shots and draw logs are not read.

**Reader:** `audiolog.py summary RUN`, `sequence RUN [--kinds mt]`, `diff A B [--kinds mt] [--counts]
[--time-tol MS] [--all --float-tol X]` (module docstring). **Run-to-run noise (retail, replay, 2026-11-24):**
world_entry twice: streams and 3D samples (`--kinds mt`, 59 items) identical; the 2D UI sounds follow the
virtual cursor's hovers (one more `sfx_mouse_over_21.wav` in one run, the clicks interleaved differently).
Per-frame events (`volume` on streams, `position`, `orientation`) scale with the frame rate: 4897 against 1066
events in two 28 s runs. play1 twice differs in content: the station greeting voice is picked at random
(`1492_00_325Se.mp3` against `323Se`) and the flight differs (68 s against 86 s, other ship sounds), so AU-01
compares world_entry-like stretches by `--kinds mt` sequence and longer ones by `--counts` with a tolerance.
Retail against ours (world_entry): `--kinds mt` and `--counts` identical, `--all` identical for the first 157
events (to the first hover difference).

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

* **The stub log's own opens are not the game's (e2e `wait-quiet proxy`).** Our client tees each
  `G_ASSERT` / `FAIL_IF` into stubs.log, and `Stub_Log_Line` opens the file once a line, so d3d8shot.log
  logs a `CreateFileA("stubs.log", W, ...)` per assert; retail has no stub log. In space both clients
  assert effect_description.cpp 266 / 318 / 348 every frame on Net-7's item data (retail run 232121's
  messages.log: about 300 a second at 60 fps; ours about 900 at its higher frame rate), so these opens
  kept `wait-quiet proxy` from ever seeing quiet on ours (sp_combat, run 232902). e2e.py leaves them out
  of the `proxy` source (`_OWN_OPEN`); they stay in d3d8shot.log.

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
  (kind, channel, 16-bit big-endian seq). The reference importer guesses the block start instead
  and loses 28 / 12 messages in the Alex dumps; use `tools/harness/convert.py`'s datagram files.
  The Alex dumps also lose bytes in the middle (not the end) of 3 datagrams per file (`X` lines).

* **EA-UDP dumps.** The Alex dumps start at byte 5 of the wire datagram: bytes 0-4 are the type
  (5 client, 4 server), the session id and the sender's datagram counter (the dump's `seq`). The
  checksum (the client's CRC 0x8B3EC0) covers the whole datagram including those bytes.

* **The client's Delta model has no "default" state.** Updated and defaulted both set
  `Unknown70 = 2`; a harness keeps its own unknown/default/value state. `DeltaCompoundClass::
  Unknown_18` recurses into children only when `Unknown70 == 0`, `Unknown_1C` only when it is
  non-zero: expand compounds to their leaves yourself. The Player root derives from the second
  root class (vtable 0xAEAC40, ctor 0x4A3FF0, with the update collector), not from 0xAEAB88.
