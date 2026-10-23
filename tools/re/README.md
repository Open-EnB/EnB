# Reverse-engineering tools

Python 3.12 with `capstone` (`pip install capstone`; only `ClientImage.disasm` needs it). Run from
this directory. Each tool checks client.exe's SHA-256 and defaults to the retail install path;
pass `--client PATH` otherwise. EA's repositories are expected at `C:\EnBDevelopment\EA`
(`easource.EA_ROOT`).

| Tool | Does | Writes |
|---|---|---|
| `clientimage.py` | Loads client.exe: sections, strings, the incremental-link thunk table, function starts, references to a set of dwords, disassembly | (module) |
| `client.py dis\|name\|refs\|callers ADDR...` | Quick lookups with every address named (symbols.json, then gaps reports as `~Name?`, thunks followed): one function's disassembly, what an address is, every dword reference in any section (vtables, data), every direct call or jump (also through thunks) with the calling function | stdout |
| `easource.py` | Indexes EA's Westwood sources: string literals, `#define` strings and assert expressions, by enclosing function. `LIBRARIES` lists the library copies used | (module) |
| `srctree.py` | Source paths, `Class::Method` names, RTTI names, assert expressions from the client | `docs/data/source-paths.txt`, `class-methods.txt`, `rtti.txt`, `asserts.txt` |
| `libversions.py` | Per library file: which EA copy (Renegade, Generals, Zero Hour) the client follows | `docs/data/library-versions.md` |
| `funcmatch.py` | Matches client functions to EA functions by the strings they share | `docs/data/funcmatch.md`, `funcmatch.json` |
| `objmatch.py file.obj...` | Finds each function of compiled objects in client.exe byte for byte (relocated fields masked): `exact`, `multiple`, `none`, `short` | stdout, `--json` |
| `objdiff.py file.obj SYMBOL ADDRESS` | Instruction-level diff of one compiled function against the client's at ADDRESS (`--list` lists an object's functions) | stdout |
| `objrel.py file.obj SYMBOL` | One compiled function's relocations, with the symbols they point to: tells an import call (`__imp__...`) from a direct one where objdiff shows both as `call REL` | stdout |
| `matchreport.py TAG=run.json...` | Combines objmatch runs of several EA copies into a per-file report | `docs/data/bytematch-*.md`, `.json` |
| `libextract.py LIB OUTDIR` | Extracts the object files from a static library (.lib) for byte matching (`tools/vc6/runtime_match.sh`) | OUTDIR |
| `gaps.py --title T --out OUT file.obj...` | Locates the client's version of each function that isn't byte-exact (between its matched neighbours) and scores the similarity: the reconstruction worklist | OUT.md, OUT.json |
| `layoutdiff.py --gaps G.json --objs DIR` | For located gaps, tallies member-offset differences per class (`[reg+X]` here, `[reg+Y]` in the client): finds headers whose class layout differs from E&B's | stdout |
| `vtables.py --objs DIR [--class C] [--seed C=ADDR] [--list]` | Finds the client's copy of each compiled vtable and aligns the slots: E&B's added or removed virtuals. Anchors on proven functions and symbols.json, weighting the class's own and rare slots; places nothing on slots most tables share. `--seed` gives the table (or any method in it) for a class whose own code doesn't match yet; `--list` prints both sides slot by slot, named | stdout, `--json` |
| `classify_gaps.py --gaps G.json --objs DIR` | Splits a library's gaps into linked-but-different and probably-unlinked (by references from code proven to be in the client) | `gaps-*-classified.md` |
| `symbols.py --objs DIR...` | Symbol map of client.exe from every proof (exact, vtable, strings, thunks), a Ghidra labelling script and a coverage report | `docs/data/symbols.json`, `ghidra_symbols.py`, `docs/coverage.md` |
| `app_inventory.py` | E&B's own functions: source file (by `__FILE__` references), `Class::Method` names, RTTI vtables | `docs/app-inventory.md`, `docs/data/app-functions.json` |
| `libdiff.py <dirA> <dirB>` | How much two EA copies of a library differ, file by file, ignoring comments and whitespace | stdout |

```
python srctree.py
python libversions.py
python funcmatch.py
python libdiff.py C:\EnBDevelopment\EA\CnC_Renegade\Code\wwlib C:\EnBDevelopment\EA\CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WWLib
```

Compiling needs the build VM: `tools/vc6/vc6.py` ([docs/toolchain.md](../../docs/toolchain.md) §1.1).
All libraries from all three EA copies, reports included: `bash ../vc6/bytematch_all.sh` (about
4 minutes). A byte-matching run for one library:

```
python ../vc6/vc6.py compile --out obj -I <wwlib> -I <wwdebug> <library>/*.cpp
python objmatch.py -q --json run.json obj/*.obj
python matchreport.py --title WWLib --out ../../docs/data/bytematch-wwlib R=renegade.json Z=zerohour.json
```

Build-VM tools in `tools/vc6`: `vc6.py` (compile in the VM), `bytematch_all.sh` (all EA copies),
`runtime_match.sh` (VC6 runtime and D3DX), `importer.py` (import and refine the libraries;
`docs/imports.md`), `annotations.py` (C-equivalent comments for assembly in imported files),
`layout.py CLASS HEADER` (a class's member offsets as VC6 lays it out, read from an unlinked
object), `trial.py COPY LIB/FILE...` (swap files to another EA copy, keep only if better;
`--max-loss`; `--errors FILE` writes every compile error, not just the first 15), `trial_all.py LIB COPY...` (trial every differing header/source pair),
`addpatch.py --block NAME SPEC.py` (apply patches to the tree and record them in one of `importer.py`'s
patch blocks; creates the block with `--title`), `tryvariants.py LIB FILE VARIANTS.py` (compile
variants of one file alone and report the client functions each gains or loses), `trydiff.py LIB FILE VARIANTS.py NAME` (compile one of those variants and keep its object, to
`objdiff.py` it when no variant gains),
`sdk_patch.py` (the Miles header as E&B's build used it, outside the repository). Several VMs:
`vc6.py discover`; `ENB_VC6_HOSTS` gives a job a subset of them (docs/toolchain.md §1.1).

`tools/stl/stl_corpus.cpp`: explicit VC6 standard-library instantiations, compiled and matched
to measure the template code in E&B's app region (docs/app-plan.md).

the reference decoder's `research/tools/auxschema/clientre.py` (`dis`, `vtable`, `calls`, `str`,
`movimm`) works on the same client and is handy alongside these.
