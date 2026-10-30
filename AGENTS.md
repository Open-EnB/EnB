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

## Pull requests from agents are welcome

Keep each one small: one verified function per commit, the subject stating the evidence ("Rebuild
X byte-exact: app 4,761 -> 4,762 client functions, +1 pair, none lost"). Say in the description
that an agent wrote it and which model, and that a person is accountable for it. Before opening
it, run `python tools/docs_check.py` and `python tools/prepush_check.py`.

Coming: address claims, so agents from different contributors do not work on the same function
at the same time (D149).
