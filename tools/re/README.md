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
| `objmatch.py file.obj...` | Finds each function of compiled objects in client.exe byte for byte (relocated fields masked): `exact`, `multiple`, `none`, `short`. A `multiple` carries `count` (all copies) and `identical` (every copy the same function, relocated fields included: REL32 targets equal, absolute references equal or C strings equal minus the directory, for headers' `__FILE__`); the importer counts those as "exact, ambiguous address" (D54). A `multiple` is first broken by the relocation targets (D80, `break_tie`: string text, known symbols through the thunk table, vtable slots, addresses the library's exact functions show): one survivor with a confirmation makes it `exact` with `tiebreak: true`. What D80 leaves `multiple` then gets the self name (D88): the one survivor that symbols.json names with the function's own symbol from a non-exact proof (a vtable slot, a string) is `exact` with `tiebreak: true` and `selfname: true`; it runs only when D80 resolves nothing more, and its credits feed D80's next round. What both leave `multiple` gets the object's client range (D100): the one survivor inside the object's own first..last exact address (functions that are the object's alone: not a COMDAT, or a COMDAT with selection NODUPLICATES; inline, template and compiler-generated copies don't set the range) is `exact` with `tiebreak: true` and `range: true`, after D80 and the self name, feeding D80's next round. An address an exact symbol shares with another symbol of a different name is not learnt as that symbol's when it also has an address of its own (a COMDAT copy of an inline function byte-identical to another function, P1's `~ForwardingConnectionClass` / `~StatsConnectionClass` 0x8251A0), so it confirms no wrong twin. `match_objects(img, objs, region=(lo, hi))` searches only that range (the importer's `app`: 0x401000-0x8C1B50) | stdout, `--json` |
| `objdiff.py file.obj SYMBOL ADDRESS` | Instruction-level diff of one compiled function against the client's at ADDRESS (`--list` lists an object's functions) | stdout |
| `explain.py file.obj SYMBOL ADDRESS [--json]` | Classifies a near miss in one line, then the first mismatch in a short window and the masked residue count: relocation, misplaced-start, frame, field-offset, literal, register-only, order-only, inline-budget, shape (triage order: fix the first; workflow.md §3.1 has the lever per class). The client window ends at the nearest verified or proven (symbols.json) start. Own normalisation of both sides (relocations and image addresses G, targets T, stack offsets S, as `.work/c/mask.py`); `test_explain.py` holds the known cases | stdout |
| `attempts.py record\|show\|list\|check` | The attempt log (builders.md §2): `record ADDR --symbol --file --status exact\|partial\|parked\|blocked --score --forms --blocker CLASS --evidence --stash F` appends a row in the csv's own line ending (CRLF; detected from the header); `show ADDR\|SYMBOL` before any retry (prints `LIMIT REACHED` under D89: a register/order residue after two rounds, or two Opus rounds plus a Fable round); `list [--status] [--lib] [--blocker]`; `check FILES [--since T] [--ack ADDR] [--lever TEXT]` fails when a changed source touches a parked/blocked function logged after T, and always for a LIMIT REACHED one unless `--lever` names the new §3.1 row. Stashes (small source text only) go to `docs/attempts/<ADDR>/` | `docs/data/attempts.csv`, `docs/attempts/` |
| `objrel.py file.obj SYMBOL` | One compiled function's relocations, with the symbols they point to: tells an import call (`__imp__...`) from a direct one where objdiff shows both as `call REL` | stdout |
| `matchreport.py TAG=run.json...` | Combines objmatch runs of several EA copies into a per-file report | `docs/data/bytematch-*.md`, `.json` |
| `libextract.py LIB OUTDIR` | Extracts the object files from a static library (.lib) for byte matching (`tools/vc6/runtime_match.sh`) | OUTDIR |
| `gaps.py --title T --out OUT file.obj...` | Locates the client's version of each function that isn't byte-exact (between its matched neighbours) and scores the similarity: the reconstruction worklist | OUT.md, OUT.json |
| `layoutdiff.py --gaps G.json --objs DIR` | For located gaps, tallies member-offset differences per class (`[reg+X]` here, `[reg+Y]` in the client): finds headers whose class layout differs from E&B's | stdout |
| `vtables.py --objs DIR [--class C] [--seed C=ADDR] [--list]` | Finds the client's copy of each compiled vtable and aligns the slots: E&B's added or removed virtuals. Anchors on proven functions and symbols.json, weighting the class's own and rare slots; places nothing on slots most tables share. `--seed` gives the table (or any method in it) for a class whose own code doesn't match yet; `--list` prints both sides slot by slot, named | stdout, `--json` |
| `classify_gaps.py --gaps G.json --objs DIR` | Splits a library's gaps into linked-but-different and probably-unlinked (by references from code proven to be in the client) | `gaps-*-classified.md` |
| `symbols.py --objs DIR...` | Symbol map of client.exe from every proof (exact, vtable, strings, thunks), a Ghidra labelling script and a coverage report | `docs/data/symbols.json`, `ghidra_symbols.py`, `docs/coverage.md` |
| `app_inventory.py` | E&B's own functions: source file (by `__FILE__` references), `Class::Method` names, RTTI vtables | `docs/app-inventory.md`, `docs/data/app-functions.json` |
| `pkclasses.py [--check] [--marker] [--pk F]` | Generates app/packets/pk_classes.h (the 263 packet classes: the client's sizes, declared constructors, names from the client's profile labels, Print texts or the reference decoder's opcode names, D56 tags), the switch 0x869790 between pk.cpp's `// BEGIN/END pkclasses.py` markers (cases in the client's source order, the default's assert on line 1078) the harness's constructors, and app/packets/clientstub.cpp (the 149 stubs of the packets the client doesn't process, each on its client line, the classes declaring them from their vtable slots) | app/packets/pk_classes.h, app/packets/pk.cpp, app/packets/clientstub.cpp, tests/capharness/udpreplay/pk_ctor_stubs.cpp |
| `pkdecode.py [--report]`, `pkemit.py [--write] [--skip C] [--files F] [--markers DIR]` | Decode the simple packet classes from the client (raw constructor's Get calls and zero stores, Build's Add calls, a constant Get_Data_Size, a sprintf / _snprintf Print of members, trivial destructor and slots; members typed and named from Print's reads and 'Name=%d' text) per client object (each ends with <string>'s guarded static 0xC416FC); `--report` lists why the others don't decode. pkemit.py drafts the files: the drafts overwrite the tree files (a file keeps its classes' current name), `--markers` writes appfile.py match copies, .work/pkdecode/notexact.txt adds `// NOT EXACT:` lines | .work/pkdecode/decoded.pkl, app/packets/pk*.cpp / .h |
| `pkfactory.py [--out F]` | Maps the packet factory's switch 0x869790 (app\packets\pk.cpp): per opcode the bytes it news, the constructor, the vtable (the constructor's store to the object itself, not a member's; the case's own store when the class's constructor is inlined over a base's) and the reference decoder's opcode name | docs/data/packet-factory.txt |
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

## Quirks

Traps found while using these tools (each also has a one-line pointer in docs/workflow.md §3).

* **`vtables.py` and classes whose own code doesn't match yet.** Inherited slots alone don't
  identify a table: the tool now says "not placed" (or warns "only inherited slots") instead
  of reporting the base class's table (it used to put such classes on `RenderObjClass`'s
  0xAE84F8). Seed it with any method's client address: `--seed Class=ADDR` (`client.py refs`
  finds the table holding a known method). Tables now end at the next table the code stores,
  so the old "16 extra slots" over-read is gone. Run it after regenerating `symbols.json`
  (`symbols.py`): its anchors come from there too.

* **`gaps.py` can misplace a whole object.** It anchors on matched neighbours. If the only match
  is a shared helper (`??_H` vector iterators), the object's gaps land in the wrong region. This
  happened with `rinfo.obj`, placed in the app region: find the real code by an instruction
  pattern instead.

* **`gaps.py` can pair overloads wrongly.** For example, the default constructor was paired with
  the client's copy constructor. Check the argument count (`ret N`, `[esp+N]`) before trusting a
  diff. It also swaps neighbours (`Render`/`Special_Render`), pairs a function with an unrelated
  one (asset manager ctor 0x909E60 vs the real 0x91C2C0; `Set_D3D_Base_Texture` vs a deleting
  dtor 0x94D5D0; `Set_Shader` 0xA03C50) or with template dtors (PivotMap, PKStraw, FileStraw):
  confirm with `objdiff.py` and `vtables.py` (2026-10-24).

* **`objdiff.py` shows import calls as `call REL`.** Our `call dword ptr [__imp__AIL_lock@0]`
  and a direct `call rel32` look the same in its left column; the client's shows as
  `call dword ptr [REL]`. A difference there is not real until `tools/re/objrel.py OBJ SYMBOL`
  shows a non-`__imp__` target. Lines with immediates like `0xffffffff` against the client's
  `REL` are display only too.

* **REA can't decompile vtable-only functions.** Ghidra's auto-analysis defines no function at an
  address only reached through a vtable (0x9961E0, the distortion draw, referenced only from
  `.rdata` 0xB13984), and REA has no tool to create one: `batch_decompile` fails with "No procedure
  exists at the requested address" (or "no loaded source mapping"), `resolve_containing_procedure`
  says `not_in_procedure`. Use `tools/re/pseudocode.py` instead: it runs Ghidra headless, creates a
  function at every start `clientimage.py` knows, and saves pseudocode to `.work/pseudo/`
  (`--show ADDR` prints it). A batch result over the token limit is saved to a file: read its
  `normalized_result.items[].pseudocode`, and check it isn't Ghidra's log (failed items carry it,
  ~85 KB each, so even an all-error batch overflows and hides the error message).

* **Ghidra refuses a project path with a `.`-prefixed element** ("Path element starting with '.'
  is not permitted"), so a headless project can't live in `.work/`; `pseudocode.py` keeps it in
  `%LOCALAPPDATA%\EnB\ghidra`. Passing the install path's `Earth & Beyond` to `analyzeHeadless.bat`
  is also unsafe (`&`): it works on a copy, `.work/ghidra/client.exe`.

* **`objdiff.py` marks relocated operands as differences.** Our object shows a relocated field as
  `0` / `[0]` (or `[esi + REL]`), the client as `REL`: those `*` lines are not differences. Real
  ones are other instructions, extra pushes, or code only one side has.

* **Tool quirks.** `tryvariants.py` edits one file only: test header changes with a
  backup/restore script around it (`.work/night/item1/hack_h.py`). `objdiff.py` with a plain name
  can pick a prefix-matching function: pass the mangled name. `verify --lib` takes one library per
  flag. `client.py callers` on a hot function crashes on cp1252 stdout: set
  `PYTHONIOENCODING=utf-8` and redirect. In Git Bash, `"app\shared\auxdata\$f.cpp"` swallows `$f`:
  use `/`.

* **`symbols.json` and bytematch lag.** They don't name recent matches: read names from the verify
  objects (`.work/night/item6i/names.py`).

* **appfile.py match.** It puts the draft's folder first on the include path (a scratch copy of a
  tree header hides the tree's), ignores `// @ADDR` markers on in-class inline functions (define
  them out of class to check), and reports explicit specialisations as "exact (no marker)".

* **appfile.py sizes and names.** Its client size can include a following static or initialiser,
  so an exact match shows PREFIX with a larger client size (`Pop_Prefix`, `Get_Rest`,
  `~KVPairClass`): check the bytes. It pairs markers with functions by name, so same-named
  overloads get mixed up: rename them while drafting.

* **appfile.py match and includes.** A header included by a relative path (`#include "..\x.h"`)
  is found next to the including file, so a copy in the draft folder doesn't shadow it. App files
  that include WW3D2 headers need `-I` for every library directory.

* **asserts.txt's file attribution** gives each string the nearest preceding path: tcpqueue.cpp gets
  about 60 asserts of which only 2 are its own. funcmatch.py itself checks `__FILE__` since D77: a
  client function that names a source file is matched only to EA functions of that file (61 of 299
  matches dropped, all E&B functions with a Westwood assert's strings: symbols.json's 0x829C80 is
  tcpconn.cpp's constructor, not `PacketManagerClass::Clear_Socket_Error`).
