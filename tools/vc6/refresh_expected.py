"""Refresh a library's plan-time totals and per-file expected counts from new evidence,
keeping every tree decision (copy, merged, base). Usage: refresh_expected.py EVDIR LIB..."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import importer as I

ev = I.load_evidence(sys.argv[1])
state = I.load_state()
for lib in sys.argv[2:]:
    copies = ev[lib]
    s = state[lib]
    total = {c: len(set().union(*[set(v) for v in copies[c].values()]) if copies[c] else set())
             for c in copies}
    newbase = max(total, key=lambda c: (total[c], c == 'R'))
    print(lib, 'totals', s['totals'], '->', total, '; base', s['base'],
          '(new evidence would pick', newbase + ')')
    s['totals'] = total
    for k, f in s['files'].items():
        exp = {c: len(copies[c].get(k, {})) for c in copies}
        if exp != f['expected']:
            print(f'  {k}: {f["expected"]} -> {exp}')
        f['expected'] = exp
I.save_state(state)
