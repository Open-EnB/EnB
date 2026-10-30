# GitHub org, repos and collaboration: proposal (2026-10-29)

Proposal for the empty org [Open-EnB](https://github.com/Open-EnB), written to be reviewed with the user item
by item. Every item is **open** until the user decides it; decisions then go to [progress.md](progress.md) §2 as
D-numbers and this file records the outcome beside each item. Nothing here has been acted on: nothing is pushed.

Background: the branch `github-prep` (worktree `.work/gh-prep`) holds the publishing work: LICENSE (GPL-3 with
EA's additional terms), public README, CONTRIBUTING, `.gitattributes`, `tools/prepush_check.py`,
`tools/docs_check.py`, `tools/progress.py`, CI workflows. On 2026-10-29 master was merged into it (8fad711) and it
was refreshed (b51c0eb): prepush_check PASS over 7,649 blobs, unit tests OK, docs_check FAIL (item P-GH-1).

The user's goals: the project builds and ships several executables (vidtest.exe, client.exe, later the Character
and Starship Creator, perhaps a server); third-party dependencies (Miles, Bink, DirectX 8) complicate it; GitHub is
for publishing and for inviting collaboration (Open-BFME was the inspiration), including other people's agents,
in a way that cannot compromise the project's integrity.

## Items at a glance

| Item | Subject | Recommendation | Status |
|---|---|---|---|
| P-GH-1 | Line endings: `.gitattributes`, the working tree and the tools | LF everywhere: `* text=auto eol=lf`, tools write LF, `tools/eol.py` guard (VC6 tested equal) | **D147: accepted, no hook** |
| P-GH-2 | Repo layout | Monorepo for the reconstruction plus satellite repos (§1) | **D144: accepted as proposed** |
| P-GH-3 | Commit email in history | Rewrite to the GitHub noreply address before the first push | **D145: email stays, no rewrite** |
| P-GH-4 | Working model once public (amends D69's "never push") | One PR per run (§3.1) | **D148: option C, user merges; D149: three Open-BFME practices** |
| P-GH-5 | Release policy | Source only until the Miles / DirectX licensing question is answered (§3.2) | **D150: only our own binaries; third-party DLLs from other sources** |
| P-GH-6 | Contributor path without VC6 | Define contribution kinds that do not need VC6 (§3.4) | **D146 (VC6 setup), D151 (lanes)** |
| P-GH-7 | Integrity model and permissions | The three layers and settings of §2 | **org settings applied 2026-10-29 (D152); repository rules when it exists** |
| P-GH-8 | Pre-push checklist | §3.7 | **D152: Open-EnB/EnB, master, private first; in progress** |
| P-GH-9 | Community and continuity | §3.8, §3.9 | **closed 2026-10-30: Net-7 contacted; no new names; labels only, no issues yet; recovery codes saved; no second owner, no archive policy for now** |
| P-GH-10 | Coordinating outside contributors' agents: claims, expiry, PR size | §4 | **open (proposal)** |

## P-GH-1: line endings (revised 2026-10-29 after the LF test and the transcript audit)

**The problem is two conventions on disk at once.** Every blob is LF (1,552 of 1,556; the other two hold stray
lone CRs). Git for Windows' system-wide `core.autocrlf=true` writes CRLF at checkout (1,398 working files), but every
writer the agents use writes LF: the Write tool, Git Bash `sed -i` (even when its pattern matches nothing), heredocs,
our generators (names.py, notexact.py); Python text mode on Windows writes CRLF. Tools that match or compare raw
bytes then disagree: importer.py's patch anchors and replay (`compare_tree` compares bytes; its base copies come from
EA's CRLF sources), edit scripts' `
` anchors, commit diffs.

**What it costs (audit of 245 transcripts, 2026-10-23..30; `.work/crlf-audit/`):**

* ~25-35 visible incidents in 7 days, 2-4 extra tool calls each, one tangled case ~25 calls: patch / edit scripts
  whose anchors miss `

` (the most frequent: importer.py PATCHES insertions, progress.md rewrites), `sed -i`
  turning files LF (tcpconn.cpp, a draft with 0 CRLF / 407 LF lines), Python writes giving mixed files (D22's shader
  headers "modified with an empty diff"), a Write that turned progress.md LF (a 2,834-line staged diff, reset and
  rewritten).
* The larger cost is prevention paid every session: 184 transcripts carry CRLF-defensive code (~1,900 commands with
  `newline=''` / `

` handling) and ~1,075 line-ending probes (`file`, byte counts); 958 "LF will be replaced by
  CRLF" warnings across 212 transcripts; about a dozen CRLF traps in workflow.md §3 / §3.8 and builders.md.
* Not a cause: the Edit tool (it keeps a file's endings; no "string not found" was line-ending related).

**VC6 does not care (tested):** all 1,547 text files converted to LF in a worktree at b84a780, full
`importer.py verify`: imports.json byte-identical to HEAD's (12,541 pairs; `pairs_cmp` gained [], lost []).

**Recommendation: one convention, LF, enforced by Git instead of by agents.**

1. **`.gitattributes`:** `* text=auto eol=lf`. The `eol` attribute overrides `core.autocrlf`, so every checkout is
   LF: this machine, every wt.py worktree, every contributor's clone whatever their Git settings, GitHub's CI. No
   per-machine setup. Exceptions: `*.bat` / `*.cmd` `eol=crlf` (none tracked today; cmd.exe needs it);
   `docs/data/asserts.txt -text` (generated; its lone CRs are the client's own string bytes).
2. **`app/client/sclient.h`:** remove its one stray lone CR (a content change; pairs checked, expected 0 / 0).
3. **One-time conversion on a quiet tree:** commit the attributes, rewrite every working file of the main tree
   and of each live worktree as LF (the tested `lf_convert.py` step; blobs do not change, so `git status` stays
   clean), confirm `git add --renormalize .` stages nothing.
4. **Tools write and compare LF:** importer.py converts EA's CRLF base copies to LF as it writes tree files, and
   replay compares the same way; the ~30 tools with `'

'` literals write `'
'`, and writes use
   `newline='
'` (Python text mode on Windows would write CRLF). A builder round, proven by replay plus a full
   verify: 0 gained / 0 lost.
5. **A guard so it cannot drift:** `tools/eol.py check|fix` (tracked text files with a CR on disk; `fix` rewrites
   them LF), run by commit_mine.py and the handoff; docs_check / CI fail on a CR in any blob not marked `-text`.
   Optionally a Claude Code PostToolUse hook on Write / Edit / Bash running `eol.py fix --changed` (a fraction of a
   second), which catches any stray Windows tool that writes CRLF.
6. **Docs:** the dozen CRLF traps in workflow.md and builders.md collapse into one line: "the tree is LF
   everywhere; the Write tool, sed and Python need no line-ending handling; `tools/eol.py fix` if a Windows tool wrote
   CRLF".

This also answers the GitHub question: the same `.gitattributes` makes every clone identical, and CI checks it.
Risk: step 3 rewrites ~1,400 working files once, so it runs with no builder active. EA's source copies outside the
repository stay as they are (importer.py converts on import).

**Superseded:** the branch's hand-made LF list; then `* text=auto` alone (keeps CRLF checkouts on Windows, so the
tool problems stay).

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

**D148 (2026-10-29): option C.** Each run works on `run/<date>`; the coordinator pushes that branch and opens one
PR at handoff; the user reviews and merges (no auto-merge). Contributors open PRs from forks. Builders never push.
Every push passes `tools/prepush_check.py` (pre-push hook). The agents' credential: a token for this repository
only, allowed to push branches and open PRs. Open-BFME, for comparison (checked 2026-10-29): trusted agents push to
`master` directly (ruleset: no force-push, no deletion), contributors open PRs, and hosted CI re-runs the byte
matching because their toolchain and executables are committed; ours stay bring-your-own (D146), so a PR is our
checkpoint before `main` changes.

**D149: from Open-BFME:** a README welcome for AI-generated PRs (one verified function per commit); `AGENTS.md`,
reread after 24 hours or when a commit reports it changed; address claims for contributors' agents (built when they
arrive).

**Work items before the first push:** the pre-push hook; `run/<date>` at run start (fetch and merge `origin/main`,
verify) and push plus `gh pr create` at handoff (coordinator-pacing and handoff skills); the scoped token;
`AGENTS.md` and the README section; the claims design.

The original question:


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
* ~~progress.md §4's work items as labelled issues; a Projects board for the milestones.~~ **2026-10-30:** the labels
  stay (`needs-vc6`, `no-vc6`, `good first issue`) as preparation; no issues for now; a Projects board later.
* Early contact with the Net-7 / E&B emulator community. **Done 2026-10-30 (the user).**
* ~~Reserve related names (Discord, a domain) once the org name is settled.~~ **2026-10-30: none reserved.**

### 3.9 Continuity (P-GH-9)

A second owner, saved recovery codes, an archive policy.

* **Second owner: none for now (2026-10-30).** The org's continuity rests on the one owner account, so its
  recovery (two second factors, saved recovery codes) is the whole of it. **Recovery codes saved (2026-10-30).**
* **Archive policy: none for now (2026-10-30).**

## 4. Coordinating outside contributors' agents (P-GH-10, proposal 2026-10-30)

Making the repository public is mainly an invitation to other people's AI agents. Three things have to work
before that: agents must not work on the same code at once (claims), abandoned work must free itself (expiry), and
the pull requests must stay few and reviewable (size). AGENTS.md and the README promise "claims" (D149) and say "one
verified function per commit", which would fragment the history and flood the review queue.

### 4.1 Why not Open-BFME's design

Open-BFME claims a body by pushing a branch `claims/0xRVA` to its own repository (`tools/claims.py`: an atomic
ref create, a 4-hour TTL, compare-and-swap takeover and release). It works because their agents may push to the
repository. Ours work from forks and cannot, and D148 keeps it that way. Anything a contributor can do without
write access is an issue, a comment, or a pull request.

### 4.2 Proposal: claims are issues, kept by a GitHub Action

* **A claim is an issue** labelled `claim`, opened from a template or by `python tools/claims.py claim UNIT`
  (through `gh`). Its title names the unit, its body the agent, the model and the person accountable.
* **The unit is a client source file, not an address**: the client's `__FILE__` path, or a provisional file
  (D137), whose address window `tools/re/appfile.py show` already knows; or an explicit range `0xA-0xB` for code
  without a file. A file is what a PR changes and what imports.json verifies, so one claim gives one PR.
* **An Action keeps the claims** (`.github/workflows/claims.yml`, on `issues` and `issue_comment`, with one
  concurrency group so two claims cannot race). It resolves the unit to its window, refuses an overlap with a
  live claim or with the maintainer's current run (next point), refuses a fourth live claim by the same account,
  and answers with the window it granted (or why not) and the expiry. It only reads text: it never checks out or
  runs a contributor's code, and its token can only write issues.
* **The maintainer's runs claim too.** The coordinator lists live claims at run start and skips them, and
  claims what it assigns to builders (one issue per file, closed by the run's PR). This keeps the inside and
  outside work in one register.
* **Expiry.** A claim lives **72 hours** without a pull request. A PR that says `Claims #N` keeps it alive while
  the PR is open and has activity in the last **7 days**. `/renew` in a comment adds 72 hours, at most twice
  without a PR. A daily scheduled run of the same Action closes expired claims with a comment (label
  `claim-expired`). A merged PR closes its claim; a PR closed unmerged frees it after 24 hours. The maintainer can
  `/release` any claim.
* **What claims do not do:** they are courtesy, not a lock. A PR for unclaimed code is still welcome; on a
  conflict the claimant's PR is reviewed first.

### 4.3 Proposal: PR size

* **One PR per claim**, any number of commits inside it. The rule "one verified function per commit" goes; the
  rule becomes: every commit builds, and the PR description carries the `pairs_cmp.py` result for its files
  (gained, lost, NOT EXACT added).
* **Squash-merged**, with one evidence subject ("Rebuild app/client/co.cpp: +23 pairs, 0 lost, 4 NOT EXACT") and
  the PR's commit list in the body, so master gets one commit per PR. The maintainer's own run PRs keep a normal
  merge (their commits are already one per evidence step).
* **Limits:** a PR touches one claim's files plus what it must (headers, `docs/data/` regenerated by the tools);
  shared headers and docs are allowed, other source files are not.

### 4.4 Verifying a contributor's PR

Contributors without VC6 cannot prove a PR. The maintainer's side needs `tools/vc6/verify_pr.py PR`: fetch the
PR into a worktree under `.work/`, verify the touched libraries on the pool, `pairs_cmp.py` against master, and
post the result as a PR comment (gains, losses, NOT EXACT). That is the only per-PR cost to the maintainer
besides reading the diff. It runs the contributor's code only inside a build VM's compiler, never on the host.
(Python tool changes in a PR are read before they are run.)

### 4.5 Open questions for the user

1. Claims as issues kept by an Action (4.2)? Alternatives: draft PRs as claims (no Action, but an empty PR per
   claim), or a pinned issue the maintainer edits by hand (no automation, does not scale).
2. The unit: a source file (recommended) or single addresses?
3. The numbers: 72 h without a PR, 7 days of PR inactivity, 3 live claims per account.
4. Squash-merge outside PRs, with "one verified function per commit" replaced as in 4.3?
5. Do the maintainer's runs claim through the same issues (one register), or only read them?
