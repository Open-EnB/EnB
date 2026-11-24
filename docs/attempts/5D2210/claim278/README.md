# Claim 278: equipment slots 0x5D2210

Fresh HEAD d520b237; 3 valid candidates, 0 invalid. Original app flags, source basename and all physical assert lines preserved. Baseline 62 exact symbol/address pairs; final 64: target 0x5D2210 and emitted helper 0x47D010; none lost. Hosted maintainer verification remains required.

| Form | Source SHA-256 | Masked target SHA-256 | Pairs gained/lost |
|---|---|---|---|
| baseline | `60824b3441535763a766cb26866610371709f0c1fd5b5f9082055384e02d3db1` | `c1b4a782c711c256fbf96d749517cd2143c5365379269c6fbd4878c1bdf71b16` | 0 / 0 |
| pre | `276851b8a97e5cb2598f9bb4aabc3cd756f940d822d677f5cd19853e6bd878e4` | `c1b4a782c711c256fbf96d749517cd2143c5365379269c6fbd4878c1bdf71b16` | 0 / 0 |
| bottom | `9906bd4d75f9ba5e07c18f762c733fa34cad5bb2025c506b019f64c7fbe8b6c9` | `c1b4a782c711c256fbf96d749517cd2143c5365379269c6fbd4878c1bdf71b16` | 0 / 0 |
| bottomreverse | `230275888b11057e282c63859ab5db848cd878f34d931db44f6f787ba71ac087` | `d17dd3f0a7ba5e7c453e91b9287aa839f02663e8e30553542ea8d87ca20b45f9` | 2 / 0 |
| final | `e2557bc3e3df06da1342742dcaf6261f0925b79d1d9eaf6f504515cb23ae75d7` | `d17dd3f0a7ba5e7c453e91b9287aa839f02663e8e30553542ea8d87ca20b45f9` | 2 / 0 |

Prior history recorded 16 forms and 14 bytes of instruction-order residue; no replay was stashed. Here prefix increments and body-bottom count-first forms remained unmatched. The body-bottom slot-first form matches: FAIL_IF still exits before increments; the increments execute after the guarded frame calls even when the frame is null. The two distinct local counters cannot observe one another. Candidate patches are adjacent. Planned partial_i, partial_idx and compare were uncompiled after the audited exact result.

Full 1265-byte target including branch and switch tables and all 60 relocations and zero addends were audited against the retail image; paths compare by basename. One EH state and its handler, FuncInfo, predecessor and cleanup code were independently checked. The emitted 21-byte Create_Get<TextureClass> helper at 0x47D010, called by target at 0x5D24D6, has no relocations or EH. All 62 prior exact bodies and relocation identities stayed unchanged. Receiver registers and vtable offsets are covered by complete executable bytes. Local audit and baseline/final objects remain in .work/claim-278/5D2210; no retail bytes are committed.

Adjacent `*.replay.txt` files contain JSON with the exact unified diff in
the `patch` field. Decode that string before applying; whitespace is preserved.
