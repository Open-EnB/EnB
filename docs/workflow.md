# Working on the libraries: workflow and known traps

How the library work is done day to day, and what has gone wrong before. Written for whoever
picks the work up next. The tools themselves are listed in tools/re/README.md; the VMs are
described in docs/toolchain.md §1.

## 1. Working state outside git

| What | Where | How to recreate |
|---|---|---|
| Byte-match evidence for every EA copy (R/G/Z), needed by `importer.py refine` and `assemble` (`--evidence`) | `.work/bm2` (git-ignored) | `tools/vc6/bytematch_all.sh` (all three copies, each against its own libraries; `RAD` points at `SDK\radtools\include_enb`, written by `tools/vc6/sdk_patch.py`) |
| The tree's own compiled objects, read by `gaps.py`, `layoutdiff.py`, `vtables.py`, `symbols.py` | `%TEMP%\enb_import\verify-<lib>` | `python tools/vc6/importer.py verify` |
| The build VMs' addresses | `.work/vc6_hosts.json` | `python tools/vc6/vc6.py discover` (after the VMs restart: DHCP) |
| Scratch scripts, backups of `importer.py` and `imports.json`, logs | `.work/` | disposable |

`.work/` holds nothing that the repository depends on. Every kept change lives in the tree and
in `tools/vc6/importer.py` (`PATCHES`, `APPENDS`, `NEW_FILES`, `RECONSTRUCTED`).

## 2. The loop for one fix

1. **Find it.**
   * `python tools/re/gaps.py --title ww3d2 --out docs/data/gaps-ww3d2 %TEMP%\enb_import\verify-ww3d2\*.obj`
     locates the functions that aren't byte-exact.
   * `layoutdiff.py --gaps ... --objs ...` ranks classes by consistent member-offset shifts.
   * `vtables.py --objs ... [--class C]` aligns vtables.
   * `objdiff.py OBJ SYMBOL ADDRESS` shows one function's instruction-level difference.
   * `tools/vc6/layout.py CLASS HEADER` prints a class's offsets as VC6 lays it out.
2. **Find the evidence in the client.** Usually that's a constructor: it initialises members in
   declaration order, so it shows the true layout. Then look for the EA copy that has the member
   or virtual. Unknown E&B additions get neutral names (`m_UnknownXX`), with the evidence in a
   comment.
3. **Write it as a `PATCHES` entry** (old text, new text, reason) in `importer.py`, and apply it to
   the tree file. Use a Python script saved in `.work/`, not an inline heredoc: escapes such as
   `\t`, `\r\n` and `\x` get mangled by the shell. Match the file's own line endings and tabs.
   Every anchor must occur exactly once.
4. **Check.**
   * Save `docs/data/imports.json` first.
   * Run `importer.py verify --lib ...` for every library that includes the changed header.
     WWLib headers reach every library.
   * Compare the sets of `(file, address)` pairs before and after. Compare sets, not totals:
     a total can stay level while functions are traded.
5. **Keep or drop.**
   * Keep the change only if it gains and loses nothing. Any accepted loss must be written down,
     in the commit message and in docs/library-matching.md.
   * Then run `importer.py report`, commit, and add a row to docs/library-matching.md §3.6.1.
6. **Then refine.** `importer.py refine --evidence .work/bm2 --lib L --file F.cpp` often
   splices in the other copy's functions once the header fits.

Counting: the "in tree" totals that verify prints count each client function once. The
`(file, address)` pairs in `imports.json` also include app-region duplicates (inline functions
compiled into E&B's objects), so they run higher.

## 3. Known traps

* **No replay.** `importer.py assemble` restarts every library from its base copy. It is not a
  rebuild of the tree. Some decisions aren't replayed:
  * whole-file and header swaps made by `trial.py` and `trial_all.py`, which record `copy` but not
    `headers_from`;
  * refine's splices.

  The git tree is authoritative until a replay step exists. Don't run `assemble` on a tree with
  uncommitted work; if you do, `git checkout -- <lib>` restores it.
* **Refine checks only the file it refines.** A header swap is checked across every library, but
  a function splice is not. Run a verify of the affected libraries before committing.
* **VC6 lists overloaded virtuals in reverse declaration order.** Read the client's slots with
  that in mind. Not knowing it produced a wrong "unknown virtual" once, corrected the same night.
* **`vtables.py` artefacts.**
  * A client table showing exactly 16 more slots than ours, all at the end, means the reader ran
    into the next table; it isn't E&B virtuals.
  * A class reported against `RenderObjClass`'s own table (0xAE84F8) wasn't found at all.
* **`gaps.py` can misplace a whole object.** It anchors on matched neighbours. If the only match
  is a shared helper (`??_H` vector iterators), the object's gaps land in the wrong region. This
  happened with `rinfo.obj`, placed in the app region: find the real code by an instruction
  pattern instead.
* **`gaps.py` can pair overloads wrongly.** For example, the default constructor was paired with
  the client's copy constructor. Check the argument count (`ret N`, `[esp+N]`) before trusting a
  diff.
* **Inlining is fragile in some files.** `meshmatdesc.cpp` flips VC6's automatic inlining of an
  app-region helper (0x4E9CB0) when unrelated headers change: the documented loss in
  library-matching.md.
* **Patch anchors change when files change copy.** A `PATCHES` entry written for Zero Hour's file
  doesn't apply to Renegade's. `assemble` reports "patch ... no longer applies".
* **Clones share a computer name.** Always address the VMs by IP (`ENB_VC6_HOST` or
  `ENB_VC6_HOSTS`).
