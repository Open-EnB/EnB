# Prior work

What was already known about the client before this project, and what each piece gives the
rebuild.

Most of it comes from earlier protocol research by the maintainer, outside this repository. That
research used the same client.exe (SHA-256 `249f8524…51c4`), so its addresses apply here
unchanged. What the rebuild still needs from it is now in this repository as frozen data (the
reference traces and the reference decoder's readings, fetched by `tools/harness/fetch_reference.py`,
D154; the opcode and class names are final, D153).

## 1. Client reverse engineering

| Finding | What it gives the rebuild | Original source it maps to |
|---|---|---|
| Address catalogue for AuxData | The Delta object model (`DeltaBase` ctor 0x4A4D40, `DeltaCompound::Add` 0x4A5670), leaf types and their vtables, the AuxData packet path (0x7364C0 for opcode 0x1B), the CREATE factory 0x6F6000, the root constructors (Ship 0x6F66E0, Player 0x752510, ...). How schemas were extracted symbolically | `G\app\shared\auxdata\*` (`delta.cpp`, `delta_template.h`, `encode.cpp`, `policy.cpp`, ...), `G\app\packets` |
| AuxData | What AuxData is, its wire format (bitsets, tree and record forms), the 11 root schemas, validation against 43,978 EA messages, Net-7's differences | `G\app\shared\auxdata` |
| Login | The login screen (vtable 0xAF2C30) and character select screen classes, their per-frame and event methods, `LoginTaskClass` | `G\app\client\ui_login.cpp`, `ui_avsel.cpp`, `tasks\tsk_login.cpp` |
| Actions | What the client sends to target, use a skill, use an item | `G\app\packets`, `G\app\client` |
| Unknowns | Every protocol and AuxData unknown, counted against all traces, with the likely way to settle each | everything that reads packets |
| Schema extraction | Symbolic execution of the AuxData constructors with capstone; this repository's copy is `tools/re/auxextract.py` | |

## 2. Protocol and data

| Finding | What it gives the rebuild |
|---|---|
| Westwood crypto | WWLib's crypto as E&B uses it: `RC4Class`, `PKey`, the XMP big-number routines, the client's session key exchange. The C++ originals are in WWLib (`rc4.cpp`, `pk.cpp`, `xmp.cpp`), which we import; the server key is `tools/harness/westwood_keys.py` |
| Game protocol | Framing `{short size, short opcode, payload}`, every opcode (the Net-7 opcode table), and declared message models for every opcode seen in the traces |
| Transport | The Westwood TCP transport (unencrypted fast key exchange, then one RC4 stream per direction); EA-UDP was only imported from the captures (the reference importer), never implemented |
| AuxData model | AuxData schemas extracted from the client (this repository's own extraction is `docs/data/client-auxdata.json`), a codec, and typed classes for all 11 roots: a ready-made specification for the C++ `G\app\shared\auxdata` classes |
| Traces | A trace model and the EA-capture importer; decode, dump, compare, roundtrip and replay readings (the reference decoder). The way to test a rebuilt client's packet code against real traffic |
| Research findings | Opcode census, command-line switches, the EA captures |

## 3. Captures

| Where | What |
|---|---|
| `C:\EnBDevelopment\Live-Era-Data\enbserver3\ENB-Packets-Live` | Original Westwood/EA server captures (EA-UDP; annotated and decrypted text dumps). About 190,000 game messages. The reference for what the real server sent |
| the reference traces | Net-7 recordings (2026-10-19, 2026-10-21); not in the frozen archive, which holds only the EA captures (D154) |

## 4. Other code

| Where | What |
|---|---|
| `C:\EnBDevelopment\Net7\Net7Proxy` | The Net-7 team's C++ proxy (2.46, rebuilt). Has its own `rc4.cpp` and `SessionKeyExchange.cpp` |
| `C:\EnBDevelopment\Net7\Net7` | The Net-7 server (C++) |
| `C:\EnBDevelopment\Net7\ClientDetours` | Net-7's client patches (Detours) |

Order of trust: the client itself and EA's captures first, then Net-7's recordings, then Net-7's
source.
