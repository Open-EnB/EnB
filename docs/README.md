# Documentation

| Document | What it covers |
|---|---|
| [progress.md](progress.md) | Log, decisions made, decisions open, next steps |
| [prior-work.md](prior-work.md) | What earlier work (the reference decoder) already found, and where |
| [binary.md](binary.md) | client.exe: hash, toolchain, sections, code regions, imports |
| [source-tree.md](source-tree.md) | The original source tree, recovered from the client, and how this repository mirrors it |
| [conventions.md](conventions.md) | Coding conventions for the rebuild |
| [toolchain.md](toolchain.md) | Compilers (VC6 and modern MSVC) and the third-party SDKs, checked against the client |
| [library-matching.md](library-matching.md) | Matching EA's published Westwood code to the client: method, results, plan |
| [workflow.md](workflow.md) | How a library fix is found, checked and kept; working state outside git; known traps |
| [imports.md](imports.md) | The imported libraries, file by file: origin, merges, local changes (generated) |
| [coverage.md](coverage.md) | How much of client.exe is identified (generated) |
| [app-inventory.md](app-inventory.md) | E&B's own code: functions by source file, names, RTTI (generated) |
| [app-plan.md](app-plan.md) | Proposal for reconstructing E&B's own code (decisions D6-D8) |
| [kernel-map.md](kernel-map.md) | The client's task kernel (KernelClass, the tasks, WinMain's start-up) and the packet layer (connections, the opcode factory, the handlers): a map and a reconstruction order |
| [milestones.md](milestones.md) | Milestones M1 (capture harness) and M2 (working VIDTEST): plan and proposals |
| [vidtest.md](vidtest.md) | VIDTEST.EXE's functional spec: program flow, scenes, tests and results.ini, E&B scene classes, library API, function table (M2.0) |
| [overnight-report.md](overnight-report.md) | Report of the 2026-10-23 overnight session |
| [overnight-report-2.md](overnight-report-2.md) | Report of the 2026-10-24 overnight run |

Generated data ([data/](data/); regenerate with the tools in `tools/re`):

| File | Contents | Tool |
|---|---|---|
| [data/source-paths.txt](data/source-paths.txt) | every source path in the client | `srctree.py` |
| [data/class-methods.txt](data/class-methods.txt) | every `Class::Method` named in a string | `srctree.py` |
| [data/rtti.txt](data/rtti.txt) | RTTI type names | `srctree.py` |
| [data/asserts.txt](data/asserts.txt) | assert-like expressions with their (approximate) source file | `srctree.py` |
| [data/names-review.md](data/names-review.md) | names scoring 3 or below under D56 (tagged `// NAME n:` or neutral `Unknown*`), lowest first, with their evidence | `names.py` |
| [data/library-versions.md](data/library-versions.md) | which EA copy each library file follows | `libversions.py` |
| [data/bytematch-*.md](data/) (`ww3d2`, `wwlib`, `wwmath`, `wwaudio`, `wwsaveload`, `wwdebug`, `wwshade`) and `.json` | client functions proven byte-identical to EA source, per file and per copy | `tools/vc6/bytematch_all.sh` |
| [data/bytematch-*.md](data/) (`libcmt`, `libcpmt`, `comsupp`, `d3dx8`) and `.json` | client functions proven to be Microsoft's precompiled libraries | `tools/vc6/runtime_match.sh` |
| [data/symbols.json](data/symbols.json), [data/ghidra_symbols.py](data/ghidra_symbols.py) | address -> name for everything proven; a Ghidra labelling script | `symbols.py` |
| [data/gaps-*.md](data/) and `gaps-*-classified.md` | each library's functions that aren't byte-exact, located, and split into linked / unlinked | `gaps.py`, `classify_gaps.py` |
| [data/funcmatch.md](data/funcmatch.md), [data/funcmatch.json](data/funcmatch.json) | client functions matched to EA functions | `funcmatch.py` |

Addresses are virtual addresses in the retail client.exe, SHA-256 `249f8524…51c4`
([binary.md](binary.md)).
