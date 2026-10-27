# Build-VM tools (tools/vc6)

Everything that compiles with the client's compiler (VC6 SP4) in a build VM, and the library
import machinery. Configuration, the VM pool and its rules: [docs/toolchain.md](../../docs/toolchain.md)
§1. Workflow, the fix loop and its traps: [docs/workflow.md](../../docs/workflow.md). The
reverse-engineering side (byte matching, gaps, vtables) is in [../re/README.md](../re/README.md).

Build VMs are addressed by IP only. After a VM restart run `python tools/vc6/vc6.py discover`.
A job takes its VMs from `ENB_VC6_HOSTS=Daniel@<ip>[,...]` (printed by `pool.py lease`).

## The VM and the pool

| Script | Does |
|---|---|
| `vc6.py compile [--flags F] [-I dir]... [-D name]... [--sync dir]... --out DIR files...` | Compile sources in a VM (same paths as on the host), bring the objects back |
| `vc6.py run "command"` | Run a command in the VM after VCVARS32 |
| `vc6.py check` | Print compiler and linker versions of every VM |
| `vc6.py discover` | Find the build VMs (SSH on the Default Switch), save their IPs in `.work/vc6_hosts.json` |
| `pool.py list` | Pool VMs, their state and leases |
| `pool.py lease --agent NAME [--count N] [--holder-pid P] [--timeout S]` | Start VMs and print `ENB_VC6_HOSTS=...`; the lease dies with the holder pid (`$ENB_HOLDER_PID`, `$CLAUDE_PID`) or after `--max-age` hours |
| `pool.py release --agent NAME` (or `release VM...`) | Give the VMs back |
| `pool.py reclaim [--max-age H] [--idle-minutes M] [--prune [--keep K]]` | Release orphaned leases, stop idle VMs, optionally destroy free ones beyond K |
| `pool.py stop-idle [--minutes M]` | Stop free VMs idle for M minutes |
| `pool.py create [N] [--max M]` | Make N differencing clones of the base disk |
| `pool.py destroy VM...` or `--all-free` | Delete pool clones (never other VMs) |
| `commit_mine.py MSGFILE SPEC.json [--base HEAD] [--file NAME ...] [--dry]` | Commit only your own work when the index is shared with other builders (D62): the `whole` paths as they are, plus only the hunks of shared files (`importer.py`, headers, `docs/data/*`) whose +/- lines contain your keywords; built in a private index, then the shared index is pointed at the new HEAD for those paths. For `docs/data/imports.json` a key that is a file-block name, or `--file NAME` (repeatable), takes the hunks inside that `"NAME": {` block's line span (an address key would also match identical lines in other files' blocks); `in_tree_total` hunks are dropped; `--dry` lists the chosen hunks |

## Building and linking

| Script | Does |
|---|---|
| `build.py [--pool K] [--lib X]... [--out DIR] [--allow-dirty]` | Build each library of the tree into DIR/<lib>/<lib>.lib (L1); refuses a dirty library unless `--allow-dirty`; never assembles |
| `build.py unresolved [--out DIR] [--report FILE]` | External symbols no library defines, by library and cause |
| `build.py link [--program vidtest] [--pool K] [--out DIR]` | Compile `programs/<name>` and `stubs/` and link against the libraries (L2); the test programs of `tools/harness` use this step |
| `mkmss32.py def` / `lib [--out DIR]` | Make `mss32.def` from the game's Mss32.dll exports, then the import library `mss32.lib` (D51) |
| `mkauthlogin.py [--out DIR]` | Make `DIR/authlogin/authlogin.lib`, the import library of EA.Com's login component, from `authlogin.def` with VC6's `lib /def` (D90 9a; `build.py link` adds it when present) |
| `mss32.def`, `authlogin.def`, `checked/enb_checked.h` | Export lists (Mss32.dll's generated, authlogin.dll's two names by hand); the forced header of the checked build (`ENB_CHECKED=1`) |
| `layout.py CLASS HEADER [-I dir]...` | A class's member offsets as VC6 lays them out |
| `sdk_patch.py` | Derive `radtools\include_enb` (the Miles headers as E&B's build had them) beside the SDK |
| `runtime_match.sh` | Byte-match the VC6 runtime libraries and D3DX (`docs/data/bytematch-*`) |
| `bytematch_all.sh [--pool K] [WORKDIR]` | Compile all libraries from each EA copy (R, G, Z) and byte-match them (~4 min) |

## Importing and refining the libraries (importer.py)

`importer.py` keeps `docs/data/imports.json` (what came from where, what matches) and writes
`docs/imports.md`. Its `PATCHES`, `APPENDS`, `NEW_FILES`, `RECONSTRUCTED` tables hold every change to EA's code.

```
python tools/vc6/importer.py verify --lib X [--lib Y] --pool K [--holder-pid P]   compile the tree's library and byte-match it on K leased VMs (release afterwards)
python tools/vc6/importer.py report                                              write docs/imports.md
python tools/vc6/importer.py replay [--out DIR]                                  rebuild the libraries from the EA copies and the recorded decisions; exit 1 if the tree differs
python tools/vc6/importer.py plan --evidence DIR                                 choose a base copy per library and a copy per file
python tools/vc6/importer.py refine --evidence DIR [--lib X] [--file F]          swap files / splice functions from other copies where that gains and loses nothing
python tools/vc6/importer.py add                                                 bring EXTRA_FILES into the tree (no assemble)
python tools/vc6/importer.py assemble --evidence DIR                             DANGER, see below
```

**`assemble` restarts every library from its base copy.** Never run it on a tree with
uncommitted work: the git tree is authoritative.

| Script | Does |
|---|---|
| `pairs_cmp.py A.json B.json` | Compare the sets of `(lib, file, address)` pairs of two saved `imports.json` copies and print `gained` and `lost`: the fix loop keeps a change only if nothing is lost |
| `refresh_expected.py EVDIR LIB...` | Refresh a library's plan-time totals and expected counts from new evidence, keeping the tree decisions |
| `annotations.py` | C-equivalent comments for inline assembly, inserted by the importer (module) |
| `trial.py COPY LIB/FILE... [--keep-if-better] [--max-loss N] [--errors FILE]` | Swap files to another EA copy, verify, keep only if better |
| `trial_all.py LIB COPY... [--log FILE]` | `trial.py` over every differing header/source pair |
| `tryvariants.py LIB FILE VARIANTS.py` | Compile text-edit variants of one file; report the client functions each gains or loses. One library file at its top level only (no app subdirectories): otherwise use `tryv2.py` |
| `tryv2.py VARIANTS.py` | Variants whose edits span several files (a header and its .cpp, two app files, app subdirectories): `V = {name: [(relpath, old, new)]}`, `COMPILE`, optional `WATCH` addresses, optional `EXTRA_INC` (extra include/sync directories, default `libs/sgistl`); gains/losses per variant, tree files restored. Prefer it for app code |
| `flagsweep.py FILE SYMBOL ADDR [--source COPY] [--jobs N]` | Compiles FILE (or an untracked COPY of a library file) with every `/O1 /O2` x `/Ob0 /Ob1 /Ob2` x `/G3 /G4 /G5 /G6 /GB` in place of the library's switches (30 compiles, ~1 min on one VM) and ranks exact / `explain.py` class / masked residue: rules switches out in one run. So far no switch has ever fixed a near miss (workflow.md §3.1) |
| `trydiff.py LIB FILE VARIANTS.py NAME` | Compile one variant and keep its object for `tools/re/objdiff.py` |
| `zhall.py LIB DONOR [FILE...]` | For each function whose donor copy (Z, G, R) differs, splice it in, compile the file, print gains and losses; restores the file after each try |
| `addpatch.py --block NAME [--title T] SPEC.py` | Apply patches to the tree and record them in one of importer.py's patch blocks |
| `mkspec.py OUT_SPEC.py REASONS.py` | Turn uncommitted edits into an addpatch spec, one patch per diff hunk |

Specs and variant files (`SPEC.py`, `VARIANTS.py`, `REASONS.py`) live in `.work/`; write them
with a file tool, not a shell heredoc (escapes get mangled).

## Quirks

* **`bytematch_all.sh` writes to `%TEMP%\enb_bytematch` unless given a directory.** Run it as
  `tools/vc6/bytematch_all.sh .work/bm2`, or the evidence `refine`/`assemble` read stays old
  (it did after the wwshade flag change: refine and the plan counts still saw 46, not 61).
  After new evidence, `tools/vc6/refresh_expected.py .work/bm2 LIB` refreshes a library's plan
  totals and per-file `expected` counts without touching its tree decisions.

* **The importer lock and child processes.** A script holding the lock that runs `importer.py`,
  `trial.py`, `tryvariants.py` or `trydiff.py` as a child deadlocks until `--lock-wait`: run
  children from an unlocked parent (as `trial_all.py`) or call in-process. `tools/vc6/zhall.py` holds the
  lock for its whole run (~15 min for ww3d2); read-only `objdiff.py` on verify objects still works.
  Killing it mid-run leaves a spliced tree file and the lock: `git status`, restore, delete the
  lock. A transient `PermissionError` on tree writes can do the same: retry, then `git status`.

* **Type libraries.** vc6.py syncs only `SOURCE_EXT` (`.tlh`/`.tli` added; D21 done). `#import` writes absolute `c:\enbdevelopment\_vc6\obj_*\` paths into the `.tlh`: strip them before committing. Generate from E&B's `BrowserEngine.dll`, not EA's (same GUIDs, extra methods).

* **Linking: LIBCPMT, not LIBCP.** With `/MT` the C++ library is `LIBCPMT.LIB`; `LIBCP.LIB` is the
  single-threaded one and lacks `std::_Lockit::_Lockit()`. The VMs have only VC98's libraries:
  `tools/vc6/build.py` copies the DX8 SDK libs over. Its unresolved list covers only what the
  libraries reference among themselves; a program's own code adds more, and LIB members are
  pulled only on demand.

* **Stubs and vtables.** A stub constructor makes the stub file emit that class's vtable, so every
  virtual then needs a stub too; check by linking every object, not only the .lib members.
  `build.py`'s default `--report` overwrites `.work/milestones/l1/unresolved.txt`.

* **mss32.lib.** VC6's `lib /def` drops the leading underscore of stdcall names; `tools/vc6/mkmss32.py` links a dummy DLL and keeps its import library (D51, done).

* **build.py builds committed files only.** A new uncommitted .cpp shows up as unresolved symbols. Without
  `--report`, `build.py` overwrites `.work/milestones/l1/unresolved.txt`.

* **Checked builds.** WWASSERT / WWDEBUG_* are compiled out in the release libraries as in the
  client; `ENB_CHECKED=1 build.py --out build/chk` compiles them in. A checked build changes some
  mangled names (`private=public`): build every library the same way. A full build also compiles
  another builder's uncommitted importer.py entries.
