# Agents working on this repository

Rules for AI coding agents (and the people running them) who contribute from outside. The
maintainer's own runs follow [CLAUDE.md](CLAUDE.md), which this file summarizes for contributors.

## Instruction freshness

Reread this file when 24 hours have passed since you last read it, and at once whenever a commit
message or a user message says it changed.

## Start here

1. [README.md](README.md): what the project is and what it leaves out.
2. [CONTRIBUTING.md](CONTRIBUTING.md): the contribution lanes (with or without Visual C++ 6.0),
   how a change is kept, how to open a pull request.
3. [docs/workflow.md](docs/workflow.md): section 2 is the fix loop; section 3 lists the known
   traps. Most failed attempts are already in it.
4. [docs/builders.md](docs/builders.md): the startup set per kind of work and the report format.

Do not read whole documents to familiarize yourself: `grep -n '^#'` and open the section you need.

## Hard rules

* **Proof decides.** A change to library or app code is kept only if the set of
  `(file, address)` pairs gains and loses nothing (`tools/vc6/pairs_cmp.py`). Without Visual C++ 6.0
  SP4 you cannot prove it yourself: say so in the pull request, and a maintainer verifies it.
* **Client wins.** Where client.exe names something, use its name. A name chosen without its
  evidence carries `// NAME n: evidence` (D56); code written from pseudocode carries
  `// NOT EXACT:` (D64).
* **Never commit binaries,** captures, anything from the game's install folder, or code from a
  source you cannot name. `tools/prepush_check.py` must pass.
* **Never push to `master`.** Work on a branch in your fork and open a pull request; only
  maintainers merge (D148).
* **Line endings need no care:** the tree is LF everywhere (D147).
* **Decisions are the maintainer's.** Propose with a recommendation in the pull request or an
  issue; never add or renumber a D-number.

## Claim before you rebuild (D156)

Agents from different contributors must not rebuild the same functions at once. Before you start:

1. `python tools/claims.py resolve UNIT` shows what a unit covers: a source file (the client's
   `__FILE__` path, `app\client\eff_o.cpp`), a range in one (`app\client\cpacket.cpp 0x5A1000-0x5A3F00`),
   or a bare range. A claim holds at most 30 functions that are not yet exact.
2. `python tools/claims.py claim UNIT --agent "<tool>" --model "<model>" --accountable "<person>"` opens
   the claim issue and prints the bot's answer (granted, or why not: an overlap, more than 30
   functions, or a fourth live claim of your account). `python tools/claims.py list` shows what is held.
3. Open a pull request within **8 hours** (a draft counts) with `Claims #N` in its description, and
   keep it moving: **3 days** without your activity while it is your turn, or **7 days** in all,
   and the claim expires. Time spent waiting for the maintainer (verification, review) does not
   count. `/renew` (twice, 8 hours each, before a PR) and `/release` work as comments on the issue.

A claim says which functions you rebuild, not which files you may touch: others may edit the
same file for renames or header changes, and you rebase. Work outside your claim that runs into
someone else's: stop and leave it to them.

## Pull requests from agents are welcome (D157)

* **One pull request per claim**, with as many commits as you like; it is squashed into one commit
  on `master`. Its title states the evidence ("Rebuild cpacket.cpp's ACK handlers: +12 pairs, 0
  lost, 2 NOT EXACT"). Say in the description that an agent wrote it, which model, and who is
  accountable; list any files outside the claim and why.
* **Every pull request is verified before it reaches `master`:** the maintainer's verify service
  compiles it with Visual C++ 6.0 SP4 on its build VMs and posts the result as the `vc6-verify`
  check within minutes (while the maintainer's machine is on). Nothing of your pull request runs
  on that machine: a pull request that changes Python, scripts or workflows waits until a
  maintainer has read it.
* **You need not commit `docs/data/imports.json`** if you have no VC6: the verified record goes
  into the landing commit for you. If you do have VC6, commit what `importer.py verify` writes.
* **Landing:** a pull request that gains pairs and loses none, passes the hosted `checks` and
  changes only C/C++ sources, headers and `docs/data` lands by itself; anything else lands when a
  maintainer has read it. Either way it lands as one commit with you as its author.
* Before opening it, run `python tools/docs_check.py`, `python tools/prepush_check.py` and, if you
  touched names, `python tools/re/names.py` (CI checks the regenerated files).
