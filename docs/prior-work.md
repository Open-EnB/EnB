# Prior work

What was already known about the client before this project, and where it lives in this repository now.

Much of it comes from the maintainer's earlier, unpublished protocol research. That research used the same
client.exe (SHA-256 `249f8524…51c4`), so its addresses apply here unchanged. Everything the rebuild still needs
from it is in this repository: its findings in the docs and tools below, its readings as frozen data (the
reference traces and the reference decoder's readings, fetched by `tools/harness/fetch_reference.py`, D154), and
its opcode and class names, which are final (D153).

## 1. Client reverse engineering

| Finding | Where it is here | Original source it maps to |
|---|---|---|
| The AuxData object model: `DeltaBase` ctor 0x4A4D40, `DeltaCompound::Add` 0x4A5670, leaf types and their vtables, the packet path (0x7364C0 for opcode 0x1B), the CREATE factory 0x6F6000, the root constructors (Ship 0x6F66E0, Player 0x752510, ...) | [decode-path.md](decode-path.md), [packet-flow.md](packet-flow.md) §2.4, the rebuilt `app\shared\auxdata` | `G\app\shared\auxdata\*` (`delta.cpp`, `delta_template.h`, `encode.cpp`, `policy.cpp`, ...), `G\app\packets` |
| AuxData's wire format (bitsets, tree and record forms), the 11 root schemas, validation against 43,978 EA messages, Net-7's differences | `docs/data/client-auxdata.json`, `tests/auxreplay`, [library-matching.md](library-matching.md) §3.8 | `G\app\shared\auxdata` |
| The login and character select screens (vtable 0xAF2C30), `LoginTaskClass` | [kernel-map-step9.md](kernel-map-step9.md) | `G\app\client\ui_login.cpp`, `ui_avsel.cpp`, `tasks\tsk_login.cpp` |
| What the client sends to target, use a skill, use an item | [packet-flow.md](packet-flow.md) §3 | `G\app\packets`, `G\app\client` |
| Protocol and AuxData unknowns | [packet-flow.md](packet-flow.md) §5 | everything that reads packets |
| Schema extraction: symbolic execution of the AuxData constructors with capstone | `tools/re/auxextract.py` (same author, GPL-3 here) | |

## 2. Protocol and data

| Finding | Where it is here |
|---|---|
| Westwood crypto: `RC4Class`, `PKey`, the XMP big-number routines, the client's session key exchange | WWLib (`rc4.cpp`, `pk.cpp`, `xmp.cpp`), imported; the server key is `tools/harness/westwood_keys.py` |
| Game protocol: framing `{short size, short opcode, payload}`, every opcode (the Net-7 opcode table) | `tools/re/pkfactory.py`, [packet-flow.md](packet-flow.md) |
| Transport: the Westwood TCP transport (unencrypted fast key exchange, then one RC4 stream per direction), EA-UDP | `app\common\tcpconn.cpp`, `app\wwudp` (CordCon), `tests/udpreplay` |
| Traces: the EA captures as datagram files and the reference readings, to test rebuilt packet code against real traffic | `tools/harness/convert.py`, `fetch_reference.py`, `tests/udpreplay`, `tests/auxreplay` |

## 3. Captures

| Where | What |
|---|---|
| trunk/capturedPackets of [therealKyp/Earth-and-Beyond-server](https://github.com/therealKyp/Earth-and-Beyond-server); locally `roots.LIVE_ERA_DATA` (env `ENB_LIVE_ERA_DATA`) | Original Westwood/EA server captures (EA-UDP; annotated and decrypted text dumps). About 190,000 game messages. The reference for what the real server sent |
| the reference traces | Net-7 recordings (2026-10-19, 2026-10-21); not in the frozen archive, which holds only the EA captures (D154) |

## 4. Other code

Net-7's server, proxy and client patches (the emulator project's own source); its proxy has its own `rc4.cpp` and
`SessionKeyExchange.cpp`.

Order of trust: the client itself and EA's captures first, then Net-7's recordings, then Net-7's
source.
