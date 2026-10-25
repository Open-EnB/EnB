# Reverse-engineering tools

Python 3.12 with `capstone` (`pip install capstone`; only `ClientImage.disasm` needs it). Run from
this directory. Each tool checks client.exe's SHA-256 and defaults to the retail install path;
pass `--client PATH` otherwise. EA's repositories, the SDK, Net-7 and the capture data are expected at
the paths in `roots.py` (`python tools/re/roots.py` prints them and whether they exist;
[docs/toolchain.md](../../docs/toolchain.md) §3).

| Tool | Does | Writes |
|---|---|---|
| `clientimage.py` | Loads client.exe: sections, strings, the incremental-link thunk table, function starts, references to a set of dwords, disassembly | (module) |
| `client.py dis\|name\|refs\|callers ADDR...` | Quick lookups with every address named (symbols.json, then gaps reports as `~Name?`, thunks followed): one function's disassembly, what an address is, every dword reference in any section (vtables, data), every direct call or jump (also through thunks) with the calling function | stdout |
| `pseudocode.py [ADDR...] [--show]` | Ghidra pseudocode ahead of need: runs Ghidra 12.1 headless (`ghidra/DecompileTargets.java`), creates a function at every known start (REA misses vtable-only ones), names them as `client.py` does, and decompiles every function not yet byte-exact, or the given ones. First run about 5 min of analysis; later runs reuse the project in `%LOCALAPPDATA%\EnB\ghidra` | `.work/pseudo/XXXXXXXX.c`, `index.tsv` |
| `easource.py` | Indexes EA's Westwood sources: string literals, `#define` strings and assert expressions, by enclosing function. `LIBRARIES` lists the library copies used | (module) |
| `srctree.py` | Source paths, `Class::Method` names, RTTI names, assert expressions from the client | `docs/data/source-paths.txt`, `class-methods.txt`, `rtti.txt`, `asserts.txt` |
| `libversions.py` | Per library file: which EA copy (Renegade, Generals, Zero Hour) the client follows | `docs/data/library-versions.md` |
| `funcmatch.py` | Matches client functions to EA functions by the strings they share | `docs/data/funcmatch.md`, `funcmatch.json` |
| `objmatch.py file.obj...` | Finds each function of compiled objects in client.exe byte for byte (relocated fields masked): `exact`, `multiple`, `none`, `short`. A `multiple` carries `count` (all copies) and `identical` (every copy the same function, relocated fields included: REL32 targets equal, absolute references equal or C strings equal minus the directory, for headers' `__FILE__`); the importer counts those as "exact, ambiguous address" (D54). `match_objects(img, objs, region=(lo, hi))` searches only that range (the importer's `app`: 0x401000-0x8C1B50) | stdout, `--json` |
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
| `pkclasses.py [--check] [--marker] [--pk F]` | Generates app/packets/pk_classes.h (the 263 packet classes: the client's sizes, declared constructors, names from the client's profile labels, Print texts or the reference decoder's opcode names, D56 tags), the switch 0x869790 between pk.cpp's `// BEGIN/END pkclasses.py` markers (cases in the client's source order, the default's assert on line 1078) and the harness's constructors | app/packets/pk_classes.h, app/packets/pk.cpp, tests/capharness/udpreplay/pk_ctor_stubs.cpp |
| `pkfactory.py [--out F]` | Maps the packet factory's switch 0x869790 (app\packets\pk.cpp): per opcode the bytes it news, the constructor, the vtable (the case's own store when the class's constructor is inlined over a base's) and the reference decoder's opcode name | docs/data/packet-factory.txt |
| `appfile.py show\|draft\|match` | The per-file loop for E&B's own code (docs/app-plan.md §2). `show FILE` (a `__FILE__` path, `app\shared\auxdata\bitset.cpp`): the file's window from app-functions.json plus `--neighbours N` unattributed ones each side (`?`), each function's size (to the next verified start), asserts with their `__LINE__`, strings, vtables stored, callees (client.py names, else app-functions.json name or `[file]`). `draft FILE [--out DIR]`: the same as a .cpp skeleton, one stub per function with Ghidra's saved pseudocode and a `// @0xADDR` marker. `match DRAFT.cpp [-I DIR] [--flags F] [--diff NAME]`: compiles with VC6 (vc6.py; `/MT /O2 /Ob2 /GX`, NDEBUG off since the client keeps its asserts) and checks each function defined under a marker byte for byte at that address (objmatch patterns, so `short` ones count too; `PREFIX` if the client's is longer), else prints the objdiff similarity. `__LINE__` is an immediate (`push 8`), so asserts must sit on the client's lines (`#line N` in a draft) | stdout, DIR/FILE.cpp, DRAFT's dir/obj/ |
| `auxroots.py generate\|check\|tree ROOT\|layout` | The aux-data root classes (M1.3, D43): runs `auxextract.py` (a copy of the reference decoder's extract_auxschema.py that also records each node's vtable, constructor calls, position in its object and string default; symbolic execution of the 11 root constructors, a few seconds) and writes one C++ class per compound vtable in the client's form: members in offset order, `Add` in wire order, masks, the CREATE factory. `check`: the extracted trees equal the reference game model's client-auxdata.json. Functional; exact for Component, Decoration, Light (the docstring lists what differs) | `app/client/aux_roots.h`, `.cpp` |
| `auxnames.py` | Names the generated aux-data classes (M1.3, D56): matches each client compound (by vtable) to Net-7's C++ AuxClasses and the reference decoder's C# classes by structure (child counts, field names and order), never by guess. Run after `auxroots.py` has written `.work/auxroots.json`; read-only on both repositories | `tools/re/auxroots_names.json` (read by `auxroots.py`; `auxroots_pads.json` beside it is hand-edited input) |
| `roots.py` | The one place for the paths of everything outside this repository (EA, SDK, Net7, the reference decoder, the reference traces, Live-Era-Data, the retail install); imported by the tools that need them | (module) |
| `names.py [--print]` | Name certainty review (D56): scans the committed app/, stubs/, programs/, tests/ code for `// NAME n: evidence` tags and untagged neutral `*Unknown*` names (score 1), and lists every name scoring 3 or below, lowest first, with its evidence | `docs/data/names-review.md` |
| `notexact.py [--print]` | The NOT EXACT tracker (D64): lists every `// NOT EXACT:` comment in app/, stubs/, programs/, tests/ with file:line, the function it belongs to (best effort) and its text | `docs/data/not-exact.md` |
| `dated.py [--md F] [--json F]` | Every dated or author-tagged change comment in EA's WW copies (Z, G, R), banded against D9's cutoff, with whether the tree has it and the function's state: the version evidence for Zero Hour's changes | stdout or the given files |
| `strscan.py [LIB]` | Client strings that occur in a Zero Hour / Generals / Renegade source file of LIB (default ww3d2) but nowhere in the tree's copy: code the tree lost | stdout |
| `mark.py SRC OUT NAME=ADDR...` | Copies SRC with `// @ADDR` on the blank line above each named function (lines kept, so no assert moves): for `appfile.py match` | OUT |
| `libdiff.py <dirA> <dirB>` | How much two EA copies of a library differ, file by file, ignoring comments and whitespace | stdout |

```
python srctree.py
python libversions.py
python funcmatch.py
python libdiff.py C:\EnBDevelopment\EA\CnC_Renegade\Code\wwlib C:\EnBDevelopment\EA\CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WWLib
```

`vtables.py` needs the objects of the library, `--objs DIR` (required): `tools/vc6/build.py` leaves
them in `build/<lib>/obj`, so for one class
`python tools/re/vtables.py --objs build/ww3d2/obj --class ClassName --list` (add `--seed ClassName=ADDR`
when the class's own code doesn't match yet).

Compiling needs the build VM: `tools/vc6/vc6.py` ([docs/toolchain.md](../../docs/toolchain.md) §1.1).
All libraries from all three EA copies, reports included: `bash ../vc6/bytematch_all.sh` (about
4 minutes). A byte-matching run for one library:

```
python ../vc6/vc6.py compile --out obj -I <wwlib> -I <wwdebug> <library>/*.cpp
python objmatch.py -q --json run.json obj/*.obj
python matchreport.py --title WWLib --out ../../docs/data/bytematch-wwlib R=renegade.json Z=zerohour.json
```

The build-VM tools (`vc6.py`, `pool.py`, `importer.py`, `build.py`, `trial*.py`, `pairs_cmp.py` and the
rest) are indexed in [../vc6/README.md](../vc6/README.md).

`tools/stl/stl_corpus.cpp`: explicit VC6 standard-library instantiations, compiled and matched
to measure the template code in E&B's app region (docs/app-plan.md).

the reference decoder's `research/tools/auxschema/clientre.py` (`dis`, `vtable`, `calls`, `str`,
`movimm`) works on the same client and is handy alongside these.
