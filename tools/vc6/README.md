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

## Building and linking

| Script | Does |
|---|---|
| `build.py [--pool K] [--lib X]... [--out DIR] [--allow-dirty]` | Build each library of the tree into DIR/<lib>/<lib>.lib (L1); refuses a dirty library unless `--allow-dirty`; never assembles |
| `build.py unresolved [--out DIR] [--report FILE]` | External symbols no library defines, by library and cause |
| `build.py link [--program vidtest] [--pool K] [--out DIR]` | Compile `programs/<name>` and `stubs/` and link against the libraries (L2); the test programs of `tools/harness` use this step |
| `mkmss32.py def` / `lib [--out DIR]` | Make `mss32.def` from the game's Mss32.dll exports, then the import library `mss32.lib` (D51) |
| `mss32.def`, `checked/enb_checked.h` | Generated export list; the forced header of the checked build (`ENB_CHECKED=1`) |
| `layout.py CLASS HEADER [-I dir]...` | A class's member offsets as VC6 lays them out |
| `sdk_patch.py` | Derive `radtools\include_enb` (the Miles headers as E&B's build had them) beside the SDK |
| `runtime_match.sh` | Byte-match the VC6 runtime libraries and D3DX (`docs/data/bytematch-*`) |
| `bytematch_all.sh [WORKDIR]` | Compile all libraries from each EA copy (R, G, Z) and byte-match them (~4 min) |

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
| `tryvariants.py LIB FILE VARIANTS.py` | Compile text-edit variants of one file; report the client functions each gains or loses |
| `trydiff.py LIB FILE VARIANTS.py NAME` | Compile one variant and keep its object for `tools/re/objdiff.py` |
| `zhall.py LIB DONOR [FILE...]` | For each function whose donor copy (Z, G, R) differs, splice it in, compile the file, print gains and losses; restores the file after each try |
| `addpatch.py --block NAME [--title T] SPEC.py` | Apply patches to the tree and record them in one of importer.py's patch blocks |
| `mkspec.py OUT_SPEC.py REASONS.py` | Turn uncommitted edits into an addpatch spec, one patch per diff hunk |

Specs and variant files (`SPEC.py`, `VARIANTS.py`, `REASONS.py`) live in `.work/`; write them
with a file tool, not a shell heredoc (escapes get mangled).
