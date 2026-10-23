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

Generated data ([data/](data/); regenerate with the tools in `tools/re`):

| File | Contents | Tool |
|---|---|---|
| [data/source-paths.txt](data/source-paths.txt) | every source path in the client | `srctree.py` |
| [data/class-methods.txt](data/class-methods.txt) | every `Class::Method` named in a string | `srctree.py` |
| [data/rtti.txt](data/rtti.txt) | RTTI type names | `srctree.py` |
| [data/asserts.txt](data/asserts.txt) | assert-like expressions with their (approximate) source file | `srctree.py` |
| [data/library-versions.md](data/library-versions.md) | which EA copy each library file follows | `libversions.py` |
| [data/bytematch-*.md](data/) (`ww3d2`, `wwlib`, `wwmath`, `wwaudio`, `wwsaveload`, `wwdebug`, `wwshade`) and `.json` | client functions proven byte-identical to EA source, per file and per copy | `tools/vc6/vc6.py`, `objmatch.py`, `matchreport.py` |
| [data/funcmatch.md](data/funcmatch.md), [data/funcmatch.json](data/funcmatch.json) | client functions matched to EA functions | `funcmatch.py` |

Addresses are virtual addresses in the retail client.exe, SHA-256 `249f8524…51c4`
([binary.md](binary.md)).
