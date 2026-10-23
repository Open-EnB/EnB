# Prior work

What was already known about the client before this project, and where it lives. Read the
linked documents for the detail; this page says what each one gives the rebuild.

All of it is in **the reference decoder** (`C:\EnBDevelopment\the reference decoder`), the C# rebuild of the
Net-7 proxy. Its research used the same client.exe (SHA-256 `249f8524…51c4`), so its addresses
apply here unchanged.

## 1. Client reverse engineering

| Document | What it gives the rebuild | Original source it maps to |
|---|---|---|
| `docs/research/client-re-notes.md` | Address catalogue for AuxData in the client: the Delta object model (`DeltaBase` ctor 0x4A4D40, `DeltaCompound::Add` 0x4A5670), leaf types and their vtables, the AuxData packet path (0x7364C0 for opcode 0x1B), the CREATE factory 0x6F6000, the root constructors (Ship 0x6F66E0, Player 0x752510, ...). How schemas were extracted symbolically | `G\app\shared\auxdata\*` (`delta.cpp`, `delta_template.h`, `encode.cpp`, `policy.cpp`, ...), `G\app\packets` |
| `docs/research/auxdata.md` | What AuxData is, its wire format (bitsets, tree and record forms), the 11 root schemas, validation against 43,978 EA messages, Net-7's differences | `G\app\shared\auxdata` |
| `docs/research/login.md` | The login screen (vtable 0xAF2C30) and character select screen classes, their per-frame and event methods, `LoginTaskClass` | `G\app\client\ui_login.cpp`, `ui_avsel.cpp`, `tasks\tsk_login.cpp` |
| `docs/research/actions.md` | What the client sends to target, use a skill, use an item | `G\app\packets`, `G\app\client` |
| `docs/research/unknowns.md` | Every protocol and AuxData unknown, counted against all traces, with the likely way to settle each | everything that reads packets |
| `research/tools/auxschema/` | `extract_auxschema.py` (symbolic execution of the AuxData constructors with capstone), `clientre.py` (quick `dis`, `vtable`, `calls`, `str`, `movimm` over client.exe) | |
| `research/client.exe.rea-snapshot` | REA (Ghidra) analysis snapshot: open client.exe with it to skip initial analysis. It can't hold names (REA's Windows provider refuses edits) | |

## 2. Protocol and data

| Where | What it gives the rebuild |
|---|---|
| `src/Shared/the reference key table` | WWLib's crypto as E&B uses it: `RC4Class`, `PKey`, the XMP big-number routines, the client's session key exchange. The C# names keep WWLib's (`Prep_Key`, `RC4`). The C++ originals are in WWLib (`rc4.cpp`, `pk.cpp`, `xmp.cpp`), which we import |
| `src/Shared/the reference protocol table` | The game protocol: framing `{short size, short opcode, payload}`, every opcode, and declared message models for every opcode seen in the traces | 
| `src/Shared/EnB.Transports` | The Westwood TCP transport (unencrypted fast key exchange, then one RC4 stream per direction) and the EA UDP variant |
| `src/Shared/the reference game model` | AuxData schemas extracted from the client (JSON), the codec, and typed C# classes for all 11 roots (`ShipObject`, ...): a ready-made specification for the C++ `G\app\shared\auxdata` classes |
| `src/Shared/EnB.Trace`, `src/Harness` (`the reference decoder`) | Trace model and the EA-capture importer; decode, dump, compare, roundtrip and replay tools. The way to test a rebuilt client's packet code against real traffic |
| `docs/design.md` | The proxy's design. §4 (Westwood crypto), §8 (opcodes, packet models, AuxData), §16 (research findings: opcode census, command-line switches §16.3, the EA captures §16.4) are about the client and game |

## 3. Captures

| Where | What |
|---|---|
| `C:\EnBDevelopment\enbserver3\ENB-Packets-Live` | Original Westwood/EA server captures (EA-UDP; annotated and decrypted text dumps). About 190,000 game messages. The reference for what the real server sent |
| `C:\EnBDevelopment\the reference traces\captures` | Net-7 recordings (2026-10-19, 2026-10-21) |

## 4. Other code

| Where | What |
|---|---|
| `C:\EnBDevelopment\Net7\Net7Proxy` | The Net-7 team's C++ proxy (2.46, rebuilt). Has its own `rc4.cpp` and `SessionKeyExchange.cpp` |
| `C:\EnBDevelopment\Net7\Net7` | The Net-7 server (C++) |
| `C:\EnBDevelopment\Net7\ClientDetours` | Net-7's client patches (Detours) |

Order of trust, as in the reference decoder: the client itself and EA's captures first, then Net-7's
recordings, then Net-7's source.
