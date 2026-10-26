# Contributing

This project rebuilds Westwood's Earth & Beyond `client.exe` as C++ that compiles back to the
same bytes. Contributions are welcome, but the working method is strict because the only judge is
a byte comparison against a binary this repository does not contain.

## Before you start

1. Read [README.md](README.md) for what the project is and what it leaves out.
2. Read [CLAUDE.md](CLAUDE.md). It is written for AI coding agents, but it is also the shortest
   statement of the hard rules for people: the git tree is authoritative, "client wins" in
   naming disputes, patches go through the importer, generated files are never hand-edited.
3. Read [docs/workflow.md](docs/workflow.md): the fix loop (section 2) and the known traps
   (section 3). Most failed attempts are in the trap list already.
4. Read [docs/builders.md](docs/builders.md) if you will work in a focused session on one item: the
   rules, the report format, who runs what.
5. Index of everything else: [docs/README.md](docs/README.md).

You need the retail client (SHA-256 in [docs/binary.md](docs/binary.md)), EA's published
Westwood source, and Visual C++ 6.0 SP4 to verify byte for byte. None of them is in this
repository (see "What is not here" in the README). Without them you can still improve the
documentation, the Python tools and the progress report, and review code against the notes.

## How a change is kept

* A change to library or app code is kept only if the set of `(file, address)` pairs gains
  something and loses nothing (`tools/vc6/pairs_cmp.py`, decision D66 names the exceptions). A
  higher total is not enough, because totals hide swaps.
* Names chosen without the client's own evidence carry a tag comment, `// NAME n: evidence`, on
  the declaring line (D56, [docs/conventions.md](docs/conventions.md) section 1). Functional
  code written from pseudocode instead of bytes carries `// NOT EXACT:` (D64).
* Inline assembly stays wherever the client has it, with a C-equivalent comment (D70).
* Run `python tools/re/names.py` and commit the regenerated `docs/data/names-review.md` and
  `docs/data/not-exact.md` with the change.
* Run `python tools/docs_check.py` and `python tools/prepush_check.py` before opening a pull
  request. CI runs the same two.

## Decisions

Open questions are written down as proposals with a recommendation; the maintainer decides. The
decisions are numbered (D1, D2, ...) in the table in [docs/progress.md](docs/progress.md)
section 2, and code, comments and commit messages cite them. Do not renumber, reword or add a
D-number yourself: put the proposal in the pull request or an issue, and the maintainer records
the outcome.

## Commits

* One imperative sentence as the subject that states the evidence or the gain, with counts
  before and after where there are any ("Rebuild the composite listener byte-exact: app 1121 ->
  1128 client functions, +8 pairs, none lost"). `git log` has many examples.
* One finished piece per commit; commit by file name, never `git add -A`.
* Keep each file's line endings. The tree has a CRLF/LF split that `.gitattributes` documents:
  most C++ and Markdown sources are CRLF; the files listed there as LF are LF.
* Docs stay UTF-8.

## Never commit binaries

No executables, DLLs, libraries, object files, VM disks, packet captures or recordings, and
nothing from the game's install folder or EA's data files. `tools/prepush_check.py` scans the
tree and the history for them (and for secrets and files over 5 MB) and CI fails on a hit. Large
scratch output goes in `.work/`, which is git-ignored.

## Licence

By contributing you agree that your work is released under the repository's licence
([LICENSE](LICENSE)): GPL v3 with Electronic Arts' additional terms. Do not paste code from
anywhere else unless its licence is compatible and you say where it came from.
