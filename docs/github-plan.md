# GitHub org, repos and collaboration: proposal (2026-10-29)

Proposal for the empty org [Open-EnB](https://github.com/Open-EnB), written to be reviewed with the user item
by item. Every item is **open** until the user decides it; decisions then go to [progress.md](progress.md) §2 as
D-numbers and this file records the outcome beside each item. Nothing here has been acted on: nothing is pushed.

Background: the branch `github-prep` (worktree `.work/gh-prep`) holds the publishing work: LICENSE (GPL-3 with
EA's additional terms), public README, CONTRIBUTING, `.gitattributes`, `tools/prepush_check.py`,
`tools/docs_check.py`, `tools/progress.py`, CI workflows. On 2026-10-29 master was merged into it (55d8c06) and it
was refreshed (fa6dd9c): prepush_check PASS over 7,649 blobs, unit tests OK, docs_check FAIL (item P-GH-1).

The user's goals: the project builds and ships several executables (vidtest.exe, client.exe, later the Character
and Starship Creator, perhaps a server); third-party dependencies (Miles, Bink, DirectX 8) complicate it; GitHub is
for publishing and for inviting collaboration (Open-BFME was the inspiration), including other people's agents,
in a way that cannot compromise the project's integrity.

## Items at a glance

| Item | Subject | Recommendation | Status |
|---|---|---|---|
| P-GH-1 | Line endings in `.gitattributes` / docs_check | `* text=auto` (VC6 output tested equal with LF sources), `*.sh eol=lf`, docs_check checks blobs | open |
| P-GH-2 | Repo layout | Monorepo for the reconstruction plus satellite repos (§1) | **D144: accepted as proposed** |
| P-GH-3 | Commit email in history | Rewrite to the GitHub noreply address before the first push | **D145: email stays, no rewrite** |
| P-GH-4 | Working model once public (amends D69's "never push") | Decide PR flow vs maintainer bot with ruleset bypass (§3.1) | open |
| P-GH-5 | Release policy | Source only until the Miles / DirectX licensing question is answered (§3.2) | open |
| P-GH-6 | Contributor path without VC6 | Define contribution kinds that do not need VC6 (§3.4) | **D146: VC6 setup docs and scripts first (§3.4); the rest open** |
| P-GH-7 | Integrity model and permissions | The three layers and settings of §2 | open |
| P-GH-8 | Pre-push checklist | §3.7 | open |
| P-GH-9 | Community and continuity | §3.8, §3.9 | open |

## P-GH-1: line endings

Every blob in the repository is LF, and was before the branch existed (`git ls-files --eol`: 1,552 `i/lf`, 2
`-text`, 2 empty). The CRLF seen in the main checkout comes from `core.autocrlf=true` at checkout (1,393 files
`w/crlf`). The branch's `.gitattributes` assumes most files are stored CRLF and lists the LF ones by hand under
`* -text`, so docs_check reports 1,457 LF files "not listed". Landing that file on master as written would likely
show the main checkout's CRLF copies as modified (no conversion under `-text`) and disturb running builders.

**Why the working copies differ (2026-10-29):** Git for Windows sets `core.autocrlf=true` system-wide, so
commits store LF and checkouts write CRLF. Files that tools or agents create stay LF on disk until Git rewrites them
(147 today), a few are mixed (2), and two files with stray lone CRs (`app/client/sclient.h`,
`docs/data/asserts.txt`) are kept as stored, CRLF.

**Does VC6 care? No (tested 2026-10-29):** a worktree at b01e8ef with all 1,547 text files converted to LF, a full
`importer.py verify` on a pool clone: all 11 libraries, imports.json byte-identical to HEAD's (12,541 pairs;
`pairs_cmp`: gained [], lost []).

**Recommendation (revised after the test):** `* text=auto`: blobs stay LF, each checkout follows its machine's
usual ending (nothing changes for the existing checkout); `*.sh text eol=lf` so bash scripts work everywhere; the two
lone-CR files keep `-text`; docs_check fails on a CR inside any other blob. The earlier proposal follows.

**Earlier recommendation:** replace the list with `* text=auto eol=lf` (blobs stay LF; every clone gets LF; the main
checkout's CRLF copies still compare equal after normalisation); docs_check fails on any CR inside a blob instead
of checking a list; the two `-text` files (`app/client/sclient.h`, `docs/data/asserts.txt`) keep `-text`.

## 1. Repos (P-GH-2)

**Not one repo per executable.** The executables share most of their code (the `ww*` libraries, `binkmovie`,
`app/shared`, the packet layer); Westwood built them from one tree (`e:\build\beta\G\`); and the project's
integrity check (the `(file, address)` pair sets before / after, `pairs_cmp.py`) spans every library in one
commit. One repo per program turns a single `wwlib` fix into coordinated cross-repo changes and submodule pins,
and loses the one-commit check. Split on **different codebase or different trust**, not on executable.

| Repo | Contents | Why separate |
|---|---|---|
| **`Open-EnB/EnB`** (today's repo) | The reconstruction: libraries, `app/`, every executable from the original tree under `programs/` (client, vidtest, later the Character and Starship Creator and E&BConfig), tools, docs | One source tree, one-commit verification |
| **`Open-EnB/.github`** | Org profile README, default CONTRIBUTING / SECURITY / CODE_OF_CONDUCT, issue and PR templates, shared workflows | Org-wide defaults |
| **`Open-EnB/the reference decoder`** (if made public) | The C# proxy (the reference decoder) | Different language, build and release cycle |
| **`Open-EnB/server`** (later) | A future server | Different codebase; talk to Net-7 first, they may overlap |
| *(optional)* **`Open-EnB/sdk-shims`** | Only our own SDK work: `sdk_patch.py`, `.def` files, `mkauthlogin.py`, hash manifests | Only if the main repo gets crowded; for now `tools/` |

**No repos for third-party material.** RAD's Miles and Bink, DirectX 8, VC6, client.exe, the game art and the
packet captures are non-redistributable or sensitive. Keep "bring your own":

* `tools/re/roots.py` reads its paths from a config file or environment variables instead of the hard-coded
  `C:\EnBDevelopment` (today: `DEV_ROOT`, `RETAIL_INSTALL`).
* A manifest of SHA-256 hashes for each external input, so anyone can check they have the right versions (the
  client's `249f8524…51c4`, the SDK files, MSS32.DLL).
* D143 fits: playable builds find the user's own install for the original art.

## 2. Integrity: contributions that cannot compromise the project (P-GH-7)

The real check (byte-exact against client.exe, compiled with VC6 SP4) cannot run on GitHub-hosted runners: it
needs client.exe and VC6. So three layers:

1. **Hosted CI, every PR, free:** `docs_check`, `prepush_check`, unit tests, generated-files freshness (rejects
   hand-edited generated files), license / header checks. Needs no secrets, runs nothing trusted.
2. **Maintainer verification, the real gate:** the VM pool runs `importer.py verify` and `pairs_cmp`, and posts
   the result to the PR as a required status check through a GitHub App (not a personal token). It runs only
   after a maintainer applies a label (e.g. `verify`). **Never** attach a self-hosted runner to `pull_request`
   events from forks: that runs anyone's code on the VMs and the home network.
3. **Main-branch ruleset:** PR required, both layers as required checks, CODEOWNERS review, no force push, no
   deletion, protected tags. Contributors work from forks; no write access.

**Org settings and permissions:**

* Base member permission: read. Members cannot create repos.
* Teams: `maintainers` (admin: the user, ideally plus a second owner account for recovery), `reviewers`
  (triage and review, no merge); outside contributors through fork PRs.
* 2FA required for members.
* Actions: allow-listed actions only; default `GITHUB_TOKEN` read-only; approval required before workflows run on
  fork PRs from anyone outside the org.
* Secret scanning with push protection on; Dependabot for the Actions versions.

**Other people's agents:**

* Publish an `AGENTS.md` (a public version of CLAUDE.md and builders.md) so their agents follow the same rules.
* A PR written by an agent says so, with a human accountable for it (as D71's `Co-Authored-By` does for ours).
* Small PRs that cite their `pairs_cmp` result and their attempts rows.
* Our own agents treat PR and issue text as untrusted data; a coordinator never runs contributed code except
  through the label-gated VM verification.

**Provenance:** a DCO sign-off (`Signed-off-by`) on every commit: contributors certify the work is their own,
in particular that it is not leaked Westwood / EA source, the main legal risk to the project's integrity.

## 3. Things not yet considered

### 3.1 Working model after going public (P-GH-4)

Once GitHub is authoritative, the overnight coordinator's few hundred commits a day either go through PRs (heavy)
or a maintainer bot gets a ruleset bypass with the local verify as the gate. Either way it amends D69's "never
push".

### 3.2 Licensing of releases (P-GH-5)

The client is GPL-3 with EA's additional terms but loads proprietary Miles / Bink (`MSS32.DLL`) and DirectX 8.
Whether a GPL binary may do that needs checking against EA's additional terms, and an outside view, before any
binary ships. Source-only releases sidestep it for now.

### 3.3 Trademark

"Earth & Beyond" is EA's. A clear non-affiliation notice in the org profile and README; no EA logos or box art.

### 3.4 Contributor barrier (P-GH-6)

Without VC6 SP4 a contributor cannot byte-check their own work. Either document how to obtain it, or define
contribution kinds that do not need it: naming (D56), docs, reverse-engineering analysis, the playable modern-MSVC
build, tools.

**D146 (2026-10-29):** contributors build their own environment: Hyper-V, the Internet Archive's VC6
Professional disc 1 (X04-14444) and SP4 (X05-86594), with the project's user documents and scripts; the project
links to the media and checks hashes, never redistributes them. Which contributions need no VC6 is still open.
The route is [setup-vc6.md](setup-vc6.md): one unattended script, `tools/vc6/setup/New-BuildVM.ps1`
(Windows from Microsoft's Quick Create image download, VC6 by file copy checked against `vc6-files.tsv`).

### 3.5 Two build targets

The byte-exact VC6 build (the measure, D7) and a playable modern build. CI, docs and releases must say which is
which.

### 3.6 Reproducible releases

A byte-exact build lets anyone confirm a release binary came from the published source: a strong integrity story.
Plan for code signing and antivirus false positives.

### 3.7 Before the first push (P-GH-8)

* ~~Commit email and history rewrite (P-GH-3)~~: D145, the email stays.
* The line-ending fix (P-GH-1).
* A final read of what becomes public in progress.md and the docs (internal decisions, paths, names).
* `roots.py` paths configurable (§1).
* Replace `OWNER/REPO` in `.github/workflows/readme-progress.yml`.
* Only `master` (or `main`) goes up: the local `wt/*` and `eval/*` branches stay local.

### 3.8 Community (P-GH-9)

* Discussions on, wiki off.
* §4's work items as labelled issues (`good first issue`, `needs-vc6`, `re-analysis`); a Projects board for the
  milestones.
* Early contact with the Net-7 / E&B emulator community.
* Reserve related names (Discord, a domain) once the org name is settled.

### 3.9 Continuity (P-GH-9)

A second owner, saved recovery codes, an archive policy.
