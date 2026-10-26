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
| [milestones.md](milestones.md) | Milestones M1 (capture harness) and M2 (working VIDTEST): spec per work item, link procedure |
| [vidtest.md](vidtest.md) | VIDTEST.EXE's functional spec: program flow, scenes, tests and results.ini, E&B scene classes, library API, function table (M2.0) |
| [builders.md](builders.md) | Builder subagents: startup set per kind, rules, models (D72), report format |
| [decode-path.md](decode-path.md) | The client's AUX_DATA decode path, function by function (M1.1) |
| [archive/](archive/) | Dated one-off reports (overnight runs), superseded by progress.md's log |

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
| [data/app-functions.json](data/app-functions.json) | the app region's functions with addresses, sizes and names | `tools/re/app_inventory.py` |
| [data/imports.json](data/imports.json) | per library file: origin, in-tree state, exact addresses (feeds imports.md) | `importer.py` |
| [data/packet-factory.txt](data/packet-factory.txt) | the packet factory's cases (opcode, class, size, constructor) | `pkfactory.py` |
| [data/vtables-ww3d2.json](data/vtables-ww3d2.json) | WW3D2 class vtables in the client | `vtables.py` (unverified) |
| [data/not-exact.md](data/not-exact.md) | functions marked `// NOT EXACT:` in the tree (D64) | `names.py` |
| [data/progress-history.csv](data/progress-history.csv) | one row per day: bytes of `.text` by state, exact functions, pairs, functions per library (measured on each day's last commit) | `tools/progress.py --history` |
| [progress.svg](progress.svg), [progress_chart.svg](progress_chart.svg), [progress_map.svg](progress_map.svg) | the progress card, the history chart and the address map of `.text`, embedded in the README | `tools/progress.py` |

Tool documentation: [tools/re/README.md](../tools/re/README.md), [tools/vc6/README.md](../tools/vc6/README.md), [tools/harness/README.md](../tools/harness/README.md).

Publishing: [../README.md](../README.md) (what the project is), [../CONTRIBUTING.md](../CONTRIBUTING.md), [../LICENSE](../LICENSE) (GPL v3 with EA's additional terms). `tools/progress.py` builds the progress card, `tools/docs_check.py` and `tools/prepush_check.py` guard the documentation and what may be pushed; CI is in [../.github/workflows/](../.github/workflows/) ([tools/re/README.md](../tools/re/README.md) lists the tools).

Addresses are virtual addresses in the retail client.exe, SHA-256 `249f8524…51c4`
([binary.md](binary.md)).
