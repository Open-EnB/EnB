"""Observed weekly-usage burn per builder-hour from the plan logs' heartbeat lines.

Each heartbeat line: '- HH:MM heartbeat: weekly_all used NN% ... builders running K'.
Per interval: dt hours, dW = weekly % change, B = builders at the interval's start.
Reports per run: totals, and a least-squares fit dW = a*dt + b*(B*dt) over all intervals
(a = coordinator's own burn per hour, b = per builder-hour).
"""
import glob, re, sys

LINE = re.compile(r'^- (\d\d):(\d\d) heartbeat: weekly_all\s+used\s+(\d+)%.*?builders running (\d+)')
rows = []
for plan in sorted(glob.glob('.work/run-*/plan.md')):
    run = plan.split('/')[-2] if '/' in plan else plan
    if 'agy' in plan or 'api' in plan: continue
    pts = []
    day = 0
    last = None
    for ln in open(plan, encoding='utf-8', errors='replace'):
        m = LINE.match(ln)
        if not m:
            continue
        t = int(m[1]) * 60 + int(m[2])
        if last is not None and t + day * 1440 < last - 30:
            day += 1
        tt = t + day * 1440
        last = tt
        pts.append((tt, int(m[3]), int(m[4])))
    ivs = []
    for (t0, w0, b0), (t1, w1, b1) in zip(pts, pts[1:]):
        dt = (t1 - t0) / 60
        dw = w1 - w0
        if dt <= 0 or dt > 3 or dw < 0:   # gaps / weekly reset
            continue
        ivs.append((dt, dw, b0))
    if not ivs:
        continue
    H = sum(i[0] for i in ivs); W = sum(i[1] for i in ivs); BH = sum(i[0] * i[2] for i in ivs)
    print(f'{run:28s} hours {H:5.1f}  builder-h {BH:5.1f}  dweekly {W:3d}%  '
          f'per builder-h (all burn) {W / BH if BH else float("nan"):.2f}')
    rows += [(run,) + i for i in ivs]

# least squares dW = a*dt + b*B*dt
import itertools
sxx = sxy = syy = sx = sy = 0.0
for _, dt, dw, b in rows:
    x1, x2 = dt, b * dt
    sxx += x1 * x1; sxy += x1 * x2; syy += x2 * x2; sx += x1 * dw; sy += x2 * dw
det = sxx * syy - sxy * sxy
a = (sx * syy - sy * sxy) / det
b = (sy * sxx - sx * sxy) / det
TH = sum(r[1] for r in rows); TBH = sum(r[1] * r[3] for r in rows); TW = sum(r[2] for r in rows)
print(f'\nall runs: hours {TH:.1f}, builder-h {TBH:.1f}, dweekly {TW}%  -> {TW / TBH:.2f}%/builder-h (coordinator included)')
print(f'fit: coordinator {a:.2f}%/h + {b:.2f}%/builder-h')
