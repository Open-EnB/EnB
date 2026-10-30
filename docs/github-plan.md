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
| P-GH-10 | Coordinating outside contributors' agents: claims, expiry, PR size | §4 | **decided 2026-10-30: 4.2A, plan in §4.5; building (4.5.6)** |

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
   the result to the PR as the required status `vc6-verify` (4.5.5: `verify_pr.py`, polled by `verify_watch.py`,
   posting with the maintainer's `gh` login). No self-hosted runner: GitHub never sends the host a job, so no
   PR's workflow can run there.
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

## 4. Coordinating outside contributors' agents (P-GH-10)

Making the repository public is mainly an invitation to other people's AI agents. Three things have to work
before that: agents must not rebuild the same code at once (claims), abandoned work must free itself (expiry),
and pull requests must stay few, reviewable and proven (size, verification). AGENTS.md and the README promised
"claims" (D149) and asked for "one verified function per commit", which would fragment the history and flood the
review queue.

### 4.1 Starting point: Open-BFME's claims

Open-BFME claims a body by pushing a branch `claims/0xRVA` to its own repository (`tools/claims.py`: an atomic
ref create, a 4-hour TTL in the commit message, compare-and-swap takeover and release; branches rather than other
refs because Claude Code's cloud sessions may push only branches). It works because their agents may push to the
repository. Ours work from forks under D148. Without write access a contributor can open an issue, comment, or
open a pull request, nothing else.

### 4.2 Proposals

#### 4.2A Claims are issues, kept by a GitHub Action (D148 as is)

* A claim is an issue labelled `claim`, opened from a template or by `python tools/claims.py claim UNIT`
  (through `gh`). Its body names the agent, the model and the person accountable.
* An Action (`.github/workflows/claims.yml`, on `issues` and `issue_comment`, one concurrency group so claims
  cannot race) resolves the unit to addresses, refuses an overlap with a live claim, answers with what it
  granted and when it expires, and closes expired claims daily. It only reads text: it never checks out or runs
  a contributor's code, and its token can only write issues.
* Contributors work in forks and open pull requests; the maintainer's tooling verifies them (4.4).

#### 4.2B Trusted contributors push branches (D148 revised)

* Pushing needs the *Write* role, granted per person; there is no push-only role. Write also lets a person
  create and delete unprotected branches and tags, publish releases, re-run and dispatch workflows, and push a
  workflow file that runs on their branch with the token permissions it declares. Rulesets would fence master,
  tags, `.github/**`, and leave only `claims/**` and `contrib/<login>/**` to contributors; they cannot limit a
  writer to their own branches, so one writer could delete another's claim.
* In return claims could be Open-BFME's, atomic by `git push` with no Action and no issue noise, and no forks.
* Write cannot be given to strangers, so every newcomer would still start on 4.2A's path: B is a second path,
  not a replacement.

#### 4.2C Comparison

| | A: issues + forks | B: trusted writers push branches |
|---|---|---|
| Who can take part | anyone with a GitHub account | people the maintainer invites |
| Claim mechanism | issue, kept by an Action | `claims/<unit>` branch, atomic by git |
| To build | the Action, claims.py, an issue template | claims.py (Open-BFME's design), the rulesets, a prune job |
| Risk to the repository | issue spam | writers can damage branches and claims; workflow tokens |
| Maintainer's cost per PR | verify + review + merge | the same |

Neither changes the last row, the real bottleneck; 4.3 is where that is addressed.

### 4.3 Challenges to the existing decisions

* **D148, "the user merges every PR".** It caps the project at what one person reviews. For C++ the proof is
  mechanical, so a PR whose verification is gain-only and that touches no tools, workflows or docs can be merged
  by a bot; the rest stays with the maintainer.
* **D146, "bring your own VC6".** Every proof runs on the maintainer's host. A self-hosted runner on the pool
  could verify each PR automatically, if it runs only master's scripts and passes the PR's C++ only into a
  build VM's compiler.
* **D149(1), "one verified function per commit".** From Open-BFME, whose agents push master and whose CI
  re-verifies each commit. With PRs, the PR is the unit.
* **D148, "builders never push"** and **D151, the lanes:** no change needed.

### 4.4 Decisions (the user, 2026-10-30)

| # | Question | Decision |
|---|---|---|
| 1 | Claim mechanism | **4.2A**: claims are issues kept by an Action; 4.2B not adopted (D148's "never master", no Write role for contributors) |
| 2 | Claim unit | at most 30 not-yet-exact functions in one file's window; a claim never stops edits to the file (4.5.2) |
| 3 | Numbers | tightened: 8 h without a PR (a draft counts), 3 days of PR inactivity, 7 days in all, both counted only while it is the contributor's turn; 3 live claims per account (4.5.2) |
| 4 | PR size | squash-merge accepted; the file limit questioned (4.5.4: dropped) |
| 5 | Maintainer's runs | claim through the same issues, one register; the run's PR opens as a draft at run start (4.5.3) |
| 6 | A or B | A |
| 4.3 | D148 merge rule | split by what a PR touches: accepted |
| 4.3 | D146 | automated verification accepted: **all work is verified before it reaches master**, the maintainer's run PRs included |
| 4.3 | D149(1) | to be updated (4.5.4) |
| 4.3 | D148 builders, D151 | no change |

The D-numbers (D148 amended, D149 updated, D146 extended) are recorded in progress.md when the build starts.

### 4.5 The final plan

#### 4.5.1 Claims

* An issue labelled `claim` (template, or `tools/claims.py claim UNIT`), kept by `claims.yml` as in 4.2A.
* `tools/claims.py list` shows the live claims with their addresses; `claims.py check ADDR|FILE` says who holds
  them. The coordinator and every picker consult it.
* Abuse handling is the maintainer's `/release` and GitHub's usual blocking; nothing else.

#### 4.5.2 The unit, and how much a claim can block

A file-level claim is too coarse. In the three days 2026-10-27..30 the maintainer's runs changed 94 of the 241 app
files (452 file changes in 367 commits), and three files took 40 or more each (`sclient.cpp` 47, `cpacket.cpp` 46, `co.cpp` 43):
one contributor holding `cpacket.cpp` for 72 hours, or for weeks behind an active PR, would stop the work where
it is busiest. So:

* **A claim is at most 30 not-yet-exact functions** (neither in a file's `in_tree` nor `ambiguous` in
  imports.json) **inside one source file's window** (the client's `__FILE__` path, as `docs/data/app-functions.json`
  attributes functions). A small file is claimed whole (`claim: app\client\eff_o.cpp`); a big one by range
  (`claim: app\client\cpacket.cpp 0x5A1000-0x5A3F00`); code no file is attributed yet is claimed by a bare range
  (`claim: 0x7D3CC0-0x7D5000`). Two claims conflict when they share a function. The data is master's.
* **A claim covers which functions you rebuild, not which files you may edit.** Anyone may change a claimed
  file for cross-cutting work: a rename, a header retype, a D66 move. The claim holder rebases.
* **Lifetime, counted only while it is the contributor's turn:**

  | Phase | Limit |
  |---|---|
  | no PR yet (a draft PR counts as a PR) | 8 h; `/renew` adds 8 h, twice at most (24 h) |
  | a PR names the claim (`Claims #N` in its description) | 3 days without the contributor's activity (a push, or a comment by the PR's author) |
  | in all | 7 days |
  | the PR closed unmerged | 24 h (wall clock) for another PR to name it |
  | the PR merged | the claim closes, landed |

  **Whose turn it is:** the contributor's before a PR exists, while the PR is a draft, and after the maintainer
  answered (a failed `vc6-verify` on the head commit, or a review asking for changes) until the next push. Waiting
  for verification, approval, review or merge is the maintainer's turn and stops the clock, so a weekend with the
  maintainer's host off costs no one a claim. The clock advances on the hourly sweep, at most 2 h per sweep.
* **Per account:** 3 live claims. Maintainers (the repository's members and collaborators) have no count limit
  but the same 30-function cap and lifetime per claim.
* **Commands** (a comment on the claim issue): `/renew` (the claimant, before a PR), `/release` (the claimant or a
  maintainer). Closing the issue releases it too.
* **Not a lock:** a PR for unclaimed or expired code is welcome; on a conflict the claimant's PR is verified
  first.

#### 4.5.3 The maintainer's runs

* At run start the coordinator lists live claims (`claims.py list`) and assigns builders only unclaimed ranges.
* **The run's PR opens as a draft at run start**, and the coordinator pushes the run branch after each builder's
  work is merged into it (D148 already lets it push its run branch), so the run's claims stay alive and
  contributors see the work before master does. The PR leaves draft at handoff.
* It claims each builder's range before launching it (`claims.py claim`, one issue per range) and adds
  `Claims #N` to the run's PR; the merged PR closes them.
* A builder that strays into a claimed range stops and reports, as it does today for another builder's file.

#### 4.5.3a How the claims are kept

`tools/claims.py` is both the contributor's command and the Action's code. The Action
(`.github/workflows/claims.yml`) runs `claims.py reconcile` on every claim issue event, every comment, hourly and
by hand; each run rebuilds the whole state from the open issues, so a run that GitHub drops while another is
queued loses nothing. The state of each claim lives in a comment by `github-actions[bot]` (a JSON marker that only
the bot's own comments are trusted for). Requests are granted first come, first served, by issue number. The
workflow checks out master, reads only issue and PR text through the API, and has `issues: write` and read
permissions otherwise; no contributor code runs.

#### 4.5.4 Pull requests and commits

* **One PR per claim** for contributors. Inside it, commit as you like: the maintainer's builders commit once per
  verified step (`commit_mine.py`), usually the .cpp and its header together (181 of 367 commits in the three
  days touched two app files, 76 one, 110 three or more, renames and moves the widest), and that is fine.
* **No file limit.** The 4.2 draft allowed only the claim's files; that would forbid what the maintainer's own
  builders do daily. A PR may touch any file it needs; what is checked is the whole tree's pairs (gain-only, D66
  aside), and the description lists files outside the claim with the reason.
* **Squash-merged**, one commit on master per contributor PR, subject with the evidence ("Rebuild
  cpacket.cpp's ACK handlers 0x5A1000-0x5A3F00: +12 pairs, 0 lost, 2 NOT EXACT"), the PR's commit list in the
  body. The maintainer's run PRs keep a normal merge.
* **D149(1) becomes:** AI-generated PRs are welcome, one PR per claim, squash-merged, proven by the automated
  verification; "one verified function per commit" goes.

#### 4.5.5 Verification and merge

Every PR is verified before it reaches master, the maintainer's run PRs included (D146 extended):

* **`tools/vc6/verify_pr.py PR`**: fetch the PR's head as data into a worktree under `.work/`, verify the
  libraries it touches on the pool, `pairs_cmp.py` against master, and post the result (gained, lost, NOT EXACT)
  as a PR comment and a commit status `vc6-verify`. It runs master's tools, never the PR's.
* **A polling service, not a runner** (the user, 2026-10-30): `tools/vc6/verify_watch.py`, a Windows scheduled
  task ("EnB verify watch", every 5 minutes while the maintainer is logged on, windowless via
  `conhost --headless`). It fetches master, moves the worktree `.work/wt/verifier` there and runs master's own
  `verify_watch.py once`, which verifies each open, non-draft PR whose head has no result yet or whose result was
  made on an older master. GitHub never sends the host a job, so no PR's workflow or script can run there; the
  PR's C++ reaches only a build VM's compiler. A self-hosted runner was rejected: on a public repository a fork's
  PR can bring its own workflow file to it. A runner in a VM or a container was rejected too: the verify needs the
  host's Hyper-V pool, the shared lease records, the client and the SDKs. While the host is off, PRs wait.
* **Guards:** a PR that changes anything a verify would run on the host (Python, scripts, workflows,
  `importer.py`'s tables, `.def` files: verify_pr.py's gate) is held at `vc6-verify: pending` until a maintainer
  has read it and comments `/verify-approved SHA` naming its head commit; a new push needs a new approval. A
  maintainer's own PRs are approved by their authorship.
* **Merge rule (D148 amended):**
  * **Bot-merged:** a PR whose `vc6-verify` is gain-only, whose CI `checks` pass, and that touches only C++
    sources, headers and the files the tools regenerate. Before merging, the bot re-verifies if master moved.
  * **Maintainer-merged:** everything else (tools, docs, `.github/`, any loss, any D66 case).
  * The master ruleset requires `checks` and `vc6-verify`.

#### 4.5.6 Build order (before the repository goes public)

1. `tools/claims.py`, the claim issue template, `claims.yml` (claim, renew, release, expiry).
2. `tools/vc6/verify_pr.py`, run by hand on the maintainer's PRs first. **Done 2026-10-30:** PR #1 verified
   (app and ww3d2, 1 min 41 s on two VMs, +0 / 0 lost, replay clean), `vc6-verify` posted. Open for step 4: a
   contributor without VC6 cannot commit the verified imports.json, so their PR's record differs and the verdict
   is `failure`; the merge bot (or `verify_pr.py`, onto the PR branch when the contributor allows maintainer
   edits) must carry the verified record.
3. The polling service and the ruleset's `vc6-verify` check. **Done 2026-10-30:** `verify_watch.py` and the
   task installed (it idles until master has the script, i.e. until PR #1 merges); tried through the verifier
   worktree on PR #1 (re-verified and posted, 1 min 41 s); `vc6-verify` joins `checks` in the ruleset applied
   when the repository goes public.
4. The merge bot (in `verify_watch.py`, merging under 4.5.5's rule).
5. The coordinator-pacing and handoff skills: claim at launch, list at run start, `Claims #N` at handoff.
6. AGENTS.md, README and CONTRIBUTING rewritten for 4.5.1-4.5.5; progress.md's D-numbers.
7. A rehearsal: a second GitHub account claims a range, opens a PR from a fork, is verified and merged; a claim
   left alone expires.
