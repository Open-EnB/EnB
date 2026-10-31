# Build-VM tools (tools/vc6)

Everything that compiles with the client's compiler (VC6 SP4) in a build VM, and the library
import machinery. Configuration, the VM pool and its rules: [docs/toolchain.md](../../docs/toolchain.md)
§1. Workflow, the fix loop and its traps: [docs/workflow.md](../../docs/workflow.md). The
reverse-engineering side (byte matching, gaps, vtables) is in [../re/README.md](../re/README.md).

Setting up a build VM and the pool's base from nothing: [docs/setup-vc6.md](../../docs/setup-vc6.md)
(`setup/New-BuildVM.ps1`). Local settings (VM account, key, pool folder, template VM, cap) live in
`.work/vc6_config.json`, read by `vc6config.py` (its docstring lists them and their variables).

Build VMs are addressed by IP only. After a VM restart run `python tools/vc6/vc6.py discover`.
A job takes its VMs from `ENB_VC6_HOSTS=<user>@<ip>[,...]` (printed by `pool.py lease`).

## The VM and the pool

| Script | Does |
|---|---|
| `vc6.py compile [--flags F] [-I dir]... [-D name]... [--sync dir]... --out DIR files...` | Compile sources in a VM (same paths as on the host), bring the objects back |
| `vc6.py run "command"` | Run a command in the VM after VCVARS32 |
| `vc6.py check` | Print compiler and linker versions of every VM |
| `vc6.py discover [--network CIDR]` | Find the build VMs (SSH with our key and VC6, at the addresses Hyper-V reports for running VMs, or every address of CIDR), save them in `.work/vc6_hosts.json` |
| `vc6config.py` | The local settings, `.work/vc6_config.json` with `ENB_VC6_*` overrides (user, key, pool_dir, template_vm, prefix, max_vms, reserve_gb, switch) |
| `setup/New-BuildVM.ps1` | Build the VM and the pool's base end to end, unattended ([docs/setup-vc6.md](../../docs/setup-vc6.md)) |
| `setup/media.py --media DIR [--download]` | The VC6 and SP4 discs from the Internet Archive: SHA-1 check, MDF -> ISO |
| `setup/guest/Setup-Guest.ps1`, `setup/guest/Install-VC6.ps1` | In the VM: OpenSSH with the key, VC6 SP4 by file copy from `setup/vc6-files.tsv` (SHA-1 per file), the base's settings |
| `setup/check_vm.py` | A VM compiles as VC6 SP4: cl 12.00.8804, link 6.00.8447, `@comp.id` build 8799 |
| `pool.py list` | Pool VMs, their state and leases |
| `pool.py lease --agent NAME [--count N] [--holder-pid P] [--timeout S]` | Start VMs and print `ENB_VC6_HOSTS=...`; the lease dies with the holder pid (`$ENB_HOLDER_PID`, `$CLAUDE_PID`) or after `--max-age` hours. A VM starts only while the host keeps `reserve_gb` free after its 2 GB start |
| `pool.py release --agent NAME` (or `release VM...`) | Give the VMs back |
| `pool.py reclaim [--max-age H] [--idle-minutes M] [--prune [--keep K]]` | Release orphaned leases, stop idle VMs, optionally destroy free ones beyond K |
| `pool.py stop-idle [--minutes M]` | Stop free VMs idle for M minutes |
| `pool.py create [N] [--max M]` | Make N differencing clones of the base disk |
| `pool.py destroy VM...` or `--all-free` | Delete pool clones (never other VMs) |
| `commit_mine.py MSGFILE [SPEC.json] [--base REV] [--file NAME ...] [--addr A ...] [--path F ...] [--refresh-totals] [--dry]` | Commit only your own work when the index is shared with other builders (D62, D112(2)): the `whole` paths as they are, plus only the hunks of shared files (`importer.py`, headers) whose +/- lines contain your keywords; built in a private index (one per run) and landed by compare-and-swap (`commit-tree`, then `update-ref <branch> NEW OLD`; one rebuild on a moved HEAD, then a loud failure). `docs/data/imports.json` by entry: `--file NAME` (or a block-name key) takes that `files` block and its stem's `headers` rows (a header key as NAME takes that row), other builders' adjacent entries stay out; `in_tree_total` hunks dropped unless `--refresh-totals` (all libraries' count lines from the working copy). `attempts.csv`, `not-exact.md`, `names-review.md` by row, always: HEAD's file plus/minus the rows whose address (`--addr`, SPEC `addrs`, 0x keys, the --file blocks' in_tree) or file (`--path`, SPEC `files`/`whole`, the blocks' sources and headers) is yours; their count lines follow. `--dry` lists the choices |
| `commit_rows.py` | commit_mine's row and entry merges (no git): csv records with multi-line fields, .md table rows by path, imports.json `files`/`headers` entries and count lines |
| `wt.py new NAME [--base REV]` / `list` / `bring-over NAME [--target B] [--lib X] [--dry]` / `remove NAME [--delete-branch]` | Builder worktrees (D113). `new`: `.work/wt/NAME` on branch `wt/NAME` at REV (HEAD), junctions `.work/locks`, `pool`, `pseudo`, copies of `vc6_hosts.json`, the `.dgram` captures and the non-app libraries, the base in `.work/wt/NAME.base`; importer.py there runs as agent NAME. `bring-over`: the branch's commits not yet on B (master) are fast-forwarded or cherry-picked in a scratch worktree `NAME.bo` (attempts.csv conflicts by row union, names-review.md / not-exact.md regenerated, any other conflict refused), the libraries they touch verified there (`importer.py verify --pool 1`, then `replay`), and B moves only when nothing is lost against B's imports.json and the commits record what the verify gives: under the importer lock, by compare-and-swap, the checked-out worktree's uncommitted copies 3-way merged. `remove`: junctions first (rmdir), then `git worktree remove`; refuses a dirty worktree without `--force`. `list`: every worktree, its base, commits ahead, dirty or clean |

## Building and linking

| Script | Does |
|---|---|
| `auxpad.py sweep [--from 0 --to 63] [--agent NAME] [--host IP]` / `set N` | `sweep`: compile aux_roots.cpp alone (~5 s a variant, one pool VM leased and released, or `--host`) with each SYMBOL_PAD value and print pad -> which of the six end-of-file constructors are exact, and the all-six windows; the importer lock is taken per variant, not for the sweep (workflow.md 3.3). `set N`: write `SYMBOL_PAD` in `tools/re/auxroots.py` and regenerate aux_roots.h/.cpp |
| `build.py [--pool K] [--lib X]... [--out DIR] [--allow-dirty] [--root DIR]` | Build each library of the tree into DIR/<lib>/<lib>.lib (L1); refuses a dirty library unless `--allow-dirty`; never assembles. Every build.py command refuses a cwd outside the script's own tree (rootguard.py) |
| `build.py unresolved [--out DIR] [--report FILE]` | External symbols no library defines, by library and cause |
| `build.py link [--program vidtest] [--pool K] [--out DIR]` | Compile `programs/<name>` and `stubs/` and link against the libraries (L2); the test programs of `tools/harness` use this step |
| `rootguard.py` (module) | D112(4): `check(__file__, a.root)` refuses (naming both) when the cwd's git tree (nearest `.git`: a worktree under .work/ is its own) is not the tree holding the script; `--root DIR` replaces the cwd test and must name that tree. Used by build.py, appfile.py draft/match, udpreplay.py, clienttest.py |
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
python tools/vc6/importer.py verify --lib X [--lib Y] --pool K [--holder-pid P]   compile the tree's library and byte-match it on K leased VMs (release afterwards; D166: a function found at several byte-identical client addresses is credited to the address whose every caller is exact in the same file and is the one our callers reach, `objmatch.py caller_tiebreak`; the pair is in `in_tree` and in the file's `tiebreak` list, and leaves `ambiguous`)
python tools/vc6/importer.py report                                              write docs/imports.md
python tools/vc6/importer.py replay [--out DIR]                                  rebuild the libraries from the EA copies and the recorded decisions; exit 1 if the tree differs
python tools/vc6/importer.py plan --evidence DIR                                 choose a base copy per library and a copy per file
python tools/vc6/importer.py refine --evidence DIR [--lib X] [--file F]          swap files / splice functions from other copies where that gains and loses nothing
python tools/vc6/importer.py add                                                 bring EXTRA_FILES into the tree (no assemble)
python tools/vc6/importer.py assemble --evidence DIR                             DANGER, see below
```

**Per-agent scratch (D112).** `importer.py --agent NAME` (or `ENB_AGENT=NAME`; `tryv2.py --agent NAME VARIANTS.py` the same) puts the compile output under `.work/NAME/tmp/enb_import/` instead of the shared `%TEMP%\enb_import`, names the importer lock's owner, and prefixes tryv2's tags (`tryv2-NAME-base`), so concurrent builders stop overwriting each other's objects. With no agent, a run from inside a git worktree other than the main tree takes the worktree's directory name; otherwise the scratch is `%TEMP%\enb_import` and the lock owner USERNAME@host, as before. vc6.py's per-host sync cache stays shared (keyed by host IP).

**`assemble` restarts every library from its base copy.** Never run it on a tree with
uncommitted work: the git tree is authoritative.

| Script | Does |
|---|---|
| `verify_pr.py PR [--pool K] [--approved] [--no-post] [--keep] [--lib X]` | Verify a pull request before it reaches master (docs/github-plan.md 4.5.5): GitHub's test merge `pull/PR/merge` in a detached worktree `.work/wt/prPR.vf`, the libraries it touches (wt.py's libs_touched) verified on the pool, replay, pairs against master's record and the PR's own, `// NOT EXACT` markers before and after; posts the commit status `vc6-verify` and one PR comment (.work/verify/prPR.json). Refuses a PR that changes anything but C/C++ sources, docs and docs/data until `--approved` (the maintainer has read it) |
| `verify_watch.py once [--dry]` / `launch [--ref R]` / `land PR [--dry]` / `automerge on\|off` / `install [--minutes N]` / `uninstall` / `status` | The polling service (docs/github-plan.md 4.5.5): runs verify_pr.py on each ready PR to master whose head or master is new, and lands contributors' PRs (one commit with the verified record, through a `verified/prN` landing PR it merges when `checks` pass): by itself when automerge is on and the PR is eligible, or on a maintainer's `/land SHA`. The scheduled task "EnB verify watch" runs `launch`: master's own `once` from `.work/wt/verifier`. A gated PR waits at `vc6-verify: pending` for `/verify-approved SHA`; `/hold` stops a landing. Log `.work/verify/watch.log` |
| `pairs_cmp.py A.json B.json` | Compare the sets of `(lib, file, address)` pairs of two saved `imports.json` copies and print `gained`, `lost`, `lost outright` (addresses no file holds afterwards; D66(v) needs it empty) and `moves` (lost pairs whose address another file holds): the fix loop keeps a change only if nothing is lost. Tie-break pairs (D166, a file's `tiebreak` list) are shown apart (`gained tie-break`, `lost tie-break`); one lost with its file's other lost pairs is `lost tie-break with its caller (D66 vii, not a loss)` and not in `lost` |
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
