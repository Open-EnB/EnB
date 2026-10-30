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
| P-GH-10 | Coordinating outside contributors' agents: claims, expiry, PR size | §4 | **decided 2026-10-30: 4.2A, plan in §4.5; the claim unit (4.5.2) awaits confirmation** |

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
| 2 | Claim unit | asked for more exposition (4.5.2); the refined unit there awaits confirmation |
| 3 | Numbers | 72 h without a PR, 7 days of PR inactivity, 3 live claims per account: accepted |
| 4 | PR size | squash-merge accepted; the file limit questioned (4.5.4: dropped) |
| 5 | Maintainer's runs | claim through the same issues, one register |
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

* **A claim is an address range of not-yet-exact functions, at most 30 of them**, inside one source file (by
  `__FILE__` path or provisional file, D137). A small file is claimed whole (`claims.py claim app/x/y.cpp`); a
  big one by range (`claims.py claim app/client/cpacket.cpp:0x5A1000-0x5A3F00`, or by class name, which
  `claims.py` resolves to its functions' range). The Action counts the functions with `appfile.py`'s data and
  refuses a claim over the cap.
* **A claim covers which functions you rebuild, not which files you may edit.** Anyone may change a claimed
  file for cross-cutting work: a rename, a header retype, a D66 move. The claim holder rebases.
* **Lifetime.** 72 h without a PR (`/renew` adds 72 h, twice at most); while its PR is open, until 7 days pass
  with no activity; **never more than 14 days in all**, after which the Action closes it and the PR is reviewed
  as it stands. A merged PR closes its claim; a PR closed unmerged frees it after 24 hours.
* **Per account:** 3 live claims. The maintainer's coordinator account has no count limit but the same 30-function
  cap and lifetime per claim.
* **Not a lock:** a PR for unclaimed or expired code is welcome; on a conflict the claimant's PR is verified
  first.

#### 4.5.3 The maintainer's runs

* At run start the coordinator lists live claims and assigns builders only unclaimed ranges.
* It claims each builder's range before launching it (one issue per range, opened through `claims.py` from the
  run), and the run's PR closes those claims (`Claims #N` lines in its description).
* A builder that strays into a claimed range stops and reports, as it does today for another builder's file.

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
* **A self-hosted runner** on the maintainer's host (labels `enb-pool`) runs `verify.yml` on
  `pull_request_target`, which uses master's workflow and scripts; it calls `verify_pr.py` and nothing from the
  PR runs on the host. The PR's C++ reaches only a build VM's compiler. While the host is off, PRs wait.
* **Guards:** a first-time contributor's PR runs only after the maintainer approves it (GitHub's setting); a PR
  that changes Python, workflows or `importer.py`'s tables gets `vc6-verify` only after the maintainer has read
  it and adds the label `verify-approved` (a contributor's patch in `importer.py` is Python run on the host).
* **Merge rule (D148 amended):**
  * **Bot-merged:** a PR whose `vc6-verify` is gain-only, whose CI `checks` pass, and that touches only C++
    sources, headers and the files the tools regenerate. Before merging, the bot re-verifies if master moved.
  * **Maintainer-merged:** everything else (tools, docs, `.github/`, any loss, any D66 case).
  * The master ruleset requires `checks` and `vc6-verify`.

#### 4.5.6 Build order (before the repository goes public)

1. `tools/claims.py`, the claim issue template, `claims.yml` (claim, renew, release, expiry).
2. `tools/vc6/verify_pr.py`, run by hand on the maintainer's PRs first.
3. The self-hosted runner, `verify.yml`, the `verify-approved` label, the ruleset's `vc6-verify` check.
4. The merge bot (step 3's workflow, merging under 4.5.5's rule).
5. The coordinator-pacing and handoff skills: claim at launch, list at run start, `Claims #N` at handoff.
6. AGENTS.md, README and CONTRIBUTING rewritten for 4.5.1-4.5.5; progress.md's D-numbers.
7. A rehearsal: a second GitHub account claims a range, opens a PR from a fork, is verified and merged; a claim
   left alone expires.
