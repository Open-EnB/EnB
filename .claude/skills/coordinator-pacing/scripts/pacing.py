"""Coordinator pacing: usage, host memory, pool and tree health in one command, with the launch allowance.

    python .claude/skills/coordinator-pacing/scripts/pacing.py usage
    python .claude/skills/coordinator-pacing/scripts/pacing.py memory
    python .claude/skills/coordinator-pacing/scripts/pacing.py pool
    python .claude/skills/coordinator-pacing/scripts/pacing.py health [--baseline IMPORTS.json]
    python .claude/skills/coordinator-pacing/scripts/pacing.py heartbeat --plan PLAN.md --builders N
            [--baseline IMPORTS.json] [--handoff HH:MM] [--cap 92] [--floor-weekly 5]
            [--coordinator-rate 0.6] [--weekly-rate 0.1] [--session-ratio 8] [--floor-gb 3] [--gb-per-builder 2] [--no-log]

`usage` reads the account's windows with the local Claude Code login token (the same numbers as /usage).
`heartbeat` prints usage, free memory, the pool, HEAD's health, the unlanded wt/* branches (`wt.py list --unlanded`'s
first line) and the number of builders that may be
launched now, and appends one line to the plan's log (unless --no-log). The projection has two parts, as
burnrate.py's fit over the runs' heartbeat lines does: the coordinator's own burn per hour plus each builder's
(weekly: 0.6 % + 0.1 % per Opus builder; the session window: --session-ratio times that, an assumption, not a
measurement). When the plan holds a previous heartbeat line, both parts are scaled so that their sum for the
builders running equals the burn observed since then. (Before 2026-11-10 all burn was charged to the builders,
0.5 % each or the observed burn divided by their number: it said "allowance 0" in four of five runs.)
"""
import argparse
import ast
import io
import json
import os
import re
import subprocess
import sys
from datetime import datetime, timedelta

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..', '..'))
PAIRS_CMP = os.path.join(REPO, 'tools', 'vc6', 'pairs_cmp.py')
POOL = os.path.join(REPO, 'tools', 'vc6', 'pool.py')
LEDGER = os.path.join(REPO, 'tools', 'vc6', 'ledger.py')   # HEAD's ledger shards -> one snapshot file


# ---------------------------------------------------------------- usage
def usage():
    """Return [(kind, used%, remaining%, reset_local)] for the unscoped windows."""
    import urllib.request
    cred = json.load(open(os.path.expanduser('~/.claude/.credentials.json')))
    tok = cred['claudeAiOauth']['accessToken']
    req = urllib.request.Request(
        'https://api.anthropic.com/api/oauth/usage',
        headers={'Authorization': 'Bearer ' + tok, 'anthropic-beta': 'oauth-2025-04-20',
                 'User-Agent': 'enb-coordinator-pacing'})
    d = json.load(urllib.request.urlopen(req, timeout=20))
    rows = []
    for lim in d['limits']:
        if lim.get('scope'):
            continue
        reset = datetime.fromisoformat(lim['resets_at'].replace('Z', '+00:00')).astimezone().replace(tzinfo=None)
        rows.append((lim['kind'], int(lim['percent']), 100 - int(lim['percent']), reset))
    return rows


def usage_lines(rows):
    return ['%-11s used %3d%%  remaining %3d%%  resets %s' % (k, u, r, t.strftime('%Y-%m-%d %H:%M'))
            for k, u, r, t in rows]


# --------------------------------------------------------------- memory
def free_gb():
    out = subprocess.run(
        ['powershell', '-NoProfile', '-NonInteractive', '-Command',
         '$o = Get-CimInstance Win32_OperatingSystem; '
         '"{0} {1}" -f $o.FreePhysicalMemory, $o.TotalVisibleMemorySize'],
        capture_output=True, text=True, timeout=60).stdout.split()
    return int(out[0]) / 1048576.0, int(out[1]) / 1048576.0


# ----------------------------------------------------------------- pool
def pool():
    out = subprocess.run([sys.executable, POOL, 'list'], capture_output=True, text=True, timeout=120).stdout
    vms = []
    for line in out.splitlines():
        m = re.match(r'(enb-vc6-\d+)\s+(\w+)\s+(\S+)\s+(free|leased by (\S+))', line)
        if m:
            vms.append((m.group(1), m.group(2), m.group(5) or ''))
    return vms


# --------------------------------------------------------------- health
def health(baseline):
    """HEAD's ledger parses (every shard); the pairs lost outright against the baseline (rename moves excluded)."""
    tmp = os.path.join(REPO, '.work', 'imports.head.json')
    r = subprocess.run([sys.executable, LEDGER, 'export', '--rev', 'HEAD', '-o', tmp], cwd=REPO,
                       capture_output=True, text=True)
    head = io.open(tmp, encoding='utf-8').read() if r.returncode == 0 and os.path.exists(tmp) else ''
    try:
        json.loads(head)
        parses = True
    except ValueError:
        parses = False
    lost_outright, gained_n = None, None
    if baseline and parses:
        out = subprocess.run([sys.executable, PAIRS_CMP, baseline, tmp], cwd=REPO,
                             capture_output=True, text=True).stdout
        # one labelled list per line; D166 added `gained tie-break` and the `lost tie-break ...` lines
        rows = dict(re.findall(r'^([a-z][^\[\n]*?) (\[.*\])$', out, re.M))
        if 'gained' in rows and 'lost' in rows:
            gained = ast.literal_eval(rows['gained']) + ast.literal_eval(rows.get('gained tie-break', '[]'))
            lost = ast.literal_eval(rows['lost'])
            # an address still matched under any file at HEAD is a move (D66(i)/(v)), not a loss
            ga = {x[2] for x in gained}
            data = json.loads(head)
            for lib in data.values():
                if isinstance(lib, dict):
                    for f in (lib.get('files') or {}).values():
                        if isinstance(f, dict):
                            ga.update(a for a in f.get('in_tree') or [] if isinstance(a, str))
            lost_outright = sorted({x[2] for x in lost} - ga)
            gained_n = len(gained)
    return parses, gained_n, lost_outright


# ------------------------------------------------------------ heartbeat
HB_RE = re.compile(r'^- (\d\d):(\d\d) heartbeat.*?session\s+used\s+(\d+)%.*$', re.M)
HB_WEEK_RE = re.compile(r'^- (\d\d):(\d\d) heartbeat.*?weekly_all\s+used\s+(\d+)%.*$', re.M)


def last_heartbeat(plan_text):
    """(time, session%, weekly%) of the last heartbeat lines written by this script, or None."""
    s = HB_RE.findall(plan_text)
    w = HB_WEEK_RE.findall(plan_text)
    if not s or not w:
        return None
    hh, mm, sp = s[-1]
    _, _, wp = w[-1]
    t = datetime.now().replace(hour=int(hh), minute=int(mm), second=0, microsecond=0)
    if t > datetime.now():
        t -= timedelta(days=1)
    return t, int(sp), int(wp)


def parse_hhmm(s):
    hh, mm = s.split(':')
    t = datetime.now().replace(hour=int(hh), minute=int(mm), second=0, microsecond=0)
    if t < datetime.now():
        t += timedelta(days=1)
    return t


def unlanded():
    """`wt.py list --unlanded`'s first line: the wt/* branches with commits on neither master nor a run branch."""
    try:
        sys.path.insert(0, os.path.join(REPO, 'tools', 'vc6'))
        import wt
        return wt.unlanded_line(wt.unlanded_branches())
    except (Exception, SystemExit) as e:    # wt exits through sys.exit on a git error: never stop the heartbeat
        return 'unlanded    ? (%s)' % (str(e)[:120] or type(e).__name__)


def split_rates(coord, per_builder, builders, observed=None):
    """(coordinator %/h, %/h per builder): the fit's two rates; with an observed total %/h (> 0) both are scaled
    so that coord + per_builder * BUILDERS equals it (one reading cannot split the two: the fit's ratio is kept)."""
    if observed and observed > 0:
        k = observed / (coord + per_builder * builders)
        return coord * k, per_builder * k
    return coord, per_builder


def projected_use(used, rates, builders, hours):
    """The window's percentage after HOURS with BUILDERS running; None when the window is not reported."""
    return None if used is None or hours is None else used + (rates[0] + rates[1] * builders) * hours


def usage_allowance(week_used, sess_used, builders, wrates, srates, hw, hs, cap, most=12):
    """How many more builders keep both projections at or under CAP (0..MOST)."""
    n = 0
    while n < most:
        pw = projected_use(week_used, wrates, builders + n + 1, hw)
        ps_ = projected_use(sess_used, srates, builders + n + 1, hs)
        if pw > cap or (ps_ is not None and ps_ > cap):
            break
        n += 1
    return n


def heartbeat(a):
    now = datetime.now()
    rows = usage()
    by_kind = {k: (u, r, t) for k, u, r, t in rows}
    sess = by_kind.get('five_hour') or by_kind.get('session')
    week = by_kind.get('seven_day') or by_kind.get('weekly_all')
    if week is None:
        week = rows[-1][1:] if rows else (0, 100, now)    # no named weekly row: the last one the API gives
    if sess is not None and sess is week:
        sess = None
    fgb, tgb = free_gb()
    vms = pool()
    running = sum(1 for v in vms if v[1] == 'Running')
    leased = sum(1 for v in vms if v[2])
    parses, gained_n, lost = health(a.baseline)
    accepted = {x.strip().upper() for x in (a.accept or '').split(',') if x.strip()}
    if lost:
        lost = [x for x in lost if x.upper() not in accepted]

    plan_text = io.open(a.plan, encoding='utf-8', newline='').read() if a.plan and os.path.exists(a.plan) else ''
    prev = last_heartbeat(plan_text)
    fit_w = (a.coordinator_rate, a.weekly_rate)
    fit_s = (a.coordinator_rate * a.session_ratio, a.weekly_rate * a.session_ratio)
    wrates, srates, rate_src = fit_w, fit_s, 'the fit'
    if prev:
        hours = (now - prev[0]).total_seconds() / 3600.0
        if hours >= 0.25:
            wr = (week[0] - prev[2]) / hours
            sr = (sess[0] - prev[1]) / hours if sess else 0
            if wr > 0 or sr > 0:
                wrates = split_rates(*fit_w, a.builders, wr)
                srates = split_rates(*fit_s, a.builders, sr)
                rate_src = 'scaled to the burn observed since %s' % prev[0].strftime('%H:%M')

    handoff = parse_hhmm(a.handoff) if a.handoff else None
    horizon_w = min(handoff, week[2]) if handoff else week[2]
    hw = max(0.0, (horizon_w - now).total_seconds() / 3600.0)
    hs = None    # no session window reported: n/a, never constrains
    if sess:
        horizon_s = min(handoff, sess[2]) if handoff else sess[2]
        hs = max(0.0, (horizon_s - now).total_seconds() / 3600.0)

    def projected(n):
        return (projected_use(week[0], wrates, n, hw), projected_use(sess[0] if sess else None, srates, n, hs))

    allow_mem = max(0, int((fgb - a.floor_gb) // a.gb_per_builder))
    allow_use = usage_allowance(week[0], sess[0] if sess else None, a.builders, wrates, srates, hw, hs, a.cap)
    if week[1] < a.floor_weekly:
        allow_use = 0
    allow = min(allow_mem, allow_use)
    if not parses or lost:
        allow = 0

    lines = usage_lines(rows)
    lines.append('memory      free %.1f GB of %.1f GB; pool %d running, %d leased; builders running %d'
                 % (fgb, tgb, running, leased, a.builders))
    lines.append('HEAD        ledger %s; gained %s vs baseline; lost outright %s'
                 % ('parses' if parses else 'DOES NOT PARSE', gained_n, lost))
    lines.append('rates       weekly %.2f %%/h coordinator + %.2f per builder, session %.2f + %.2f (%s)'
                 % (wrates[0], wrates[1], srates[0], srates[1], rate_src))
    pw, ps_ = projected(a.builders)
    lines.append('projection  with %d builders to %s: weekly %.0f%%, session %s (cap %d%%)'
                 % (a.builders, (handoff or horizon_w).strftime('%H:%M'), pw,
                    'n/a' if ps_ is None else '%.0f%%' % ps_, a.cap))
    lines.append('allowance   launch %d more (memory allows %d, usage allows %d)' % (allow, allow_mem, allow_use))
    if not parses or lost:
        lines.append('ACTION      HEAD is unhealthy: fix before any launch')
    lines.append(unlanded())
    print('\n'.join(lines))

    if a.plan and not a.no_log:
        stamp = now.strftime('%H:%M')
        nl = '\r\n' if '\r\n' in plan_text else '\n'
        entry = ('- %s heartbeat: %s; %s; free %.1f GB, pool %d running / %d leased, builders %d; HEAD %s, lost outright %s; allowance %d'
                 % (stamp, lines[0], lines[1], fgb, running, leased, a.builders,
                    'parses' if parses else 'BROKEN', lost, allow))
        with io.open(a.plan, 'a', encoding='utf-8', newline='') as f:
            f.write(entry + nl)


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest='cmd')
    sub.add_parser('usage')
    sub.add_parser('memory')
    sub.add_parser('pool')
    h = sub.add_parser('health')
    h.add_argument('--baseline', default=None)
    hb = sub.add_parser('heartbeat')
    hb.add_argument('--plan', required=True)
    hb.add_argument('--builders', type=int, required=True, help='builders running now')
    hb.add_argument('--baseline', default=None, help="the run's baseline (.work/imports.<sha>.json, ledger.py export --rev) for pairs_cmp")
    hb.add_argument('--accept', default=None,
                    help='comma-separated addresses whose loss the user accepted (D66), not counted as lost')
    hb.add_argument('--handoff', default=None, help='HH:MM when the handoff must be written')
    hb.add_argument('--cap', type=float, default=92.0, help='max projected %% of either window at the handoff')
    hb.add_argument('--floor-weekly', type=float, default=5.0, help='never launch under this %% of the week')
    hb.add_argument('--coordinator-rate', type=float, default=0.6,
                    help="%% of the week per hour for the coordinator alone (burnrate.py's fit)")
    hb.add_argument('--weekly-rate', type=float, default=0.1,
                    help="%% of the week per hour per Opus builder, the coordinator not included (burnrate.py's fit)")
    hb.add_argument('--session-ratio', type=float, default=8.0,
                    help='%% of the session window per %% of the week (assumed; the observed burn corrects it)')
    hb.add_argument('--floor-gb', type=float, default=3.0)
    hb.add_argument('--gb-per-builder', type=float, default=2.0)
    hb.add_argument('--no-log', action='store_true')
    a = p.parse_args()
    if a.cmd == 'usage':
        print('\n'.join(usage_lines(usage())))
    elif a.cmd == 'memory':
        f, t = free_gb()
        print('free %.1f GB of %.1f GB' % (f, t))
    elif a.cmd == 'pool':
        for name, state, who in pool():
            print('%-12s %-8s %s' % (name, state, who or 'free'))
    elif a.cmd == 'health':
        parses, g, lost = health(a.baseline)
        print('ledger %s; gained %s; lost outright %s' % ('parses' if parses else 'DOES NOT PARSE', g, lost))
    elif a.cmd == 'heartbeat':
        heartbeat(a)
    else:
        p.print_help()


if __name__ == '__main__':
    main()
