"""Class-name proposals from a local model (an OpenAI-compatible /v1/chat/completions server), one class per
request, N requests in parallel. The contexts are tier2_ctx3.py's (one file per class); the output is a proposals
TSV (`CLASS<TAB>ours<TAB>candidate`) that tier2_eval.py scores.

  --pilot   measure the model on classes whose retail name is already confirmed: the contexts of the classes round 3
            confirmed (written before they were), recall = the retail name among the candidates. Nothing is
            scored against the hash and nothing tracked is written.

Usage:
  python tools/re/thunknames/tier2_local.py --pilot [--modes off,low,medium] [--samples 1]
  python tools/re/thunknames/tier2_local.py --classes FILE --out DIR/L00.tsv [--mode off] [--samples 4]
Server: --url (default http://127.0.0.1:8080/v1/chat/completions), --model, --parallel 16.
`--mode off` sends chat_template_kwargs {"enable_thinking": false}; any other mode {"reasoning_effort": mode}.
"""
import argparse
import collections
import concurrent.futures
import csv
import json
import os
import re
import sys
import time
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402

SYSTEM = """We recover the original C++ class names of the game Earth & Beyond's client.exe (Westwood Studios, 2002,
MSVC 6). A hash oracle checks every proposal exactly, so a wrong guess costs nothing and a right one is confirmed:
give breadth, not caution.

You get one class: its destructor slots, our decorated method names, every name ALREADY TRIED (all wrong: do not
repeat them or trivial variants), confirmed retail classes with the same name start, in its vtable family and in
its source files (these show retail's real spelling style), source files and strings.

Propose {n} new candidate class names, most likely first. Think about:
- the source file name (gd_text_editor.cpp -> TextEditorGadget): retail names often follow the file name, spelled
  in the family's style;
- the family's style: whether siblings carry `Class` (DynamicWindowClass) or not (DynamicWindowElement,
  TextEditorGadget, LinearBuffer, BitSet), `Struct`, `Interface` (UI screens are `<X>Interface`: MFDInterface,
  VerbInterface), `Packet` / `PacketClass`, `Manager` / `ManagerClass`, `Data`, `Node`, `RObj`;
- abbreviations spelled out or kept (ProxyRObj -> ProxyRenderObjClass, Gps -> GPS, Inv -> Inventory, CircBuff ->
  CircularBuffer) and reordered words (InventorySortPacket -> InvSortPacketClass, KVPairString ->
  KVStringPairClass);
- a placeholder name (Unknown..., Packet_XXXX) says nothing: use the methods, strings, file and relatives.
This is Westwood's style (FooClass, FooManagerClass, Get_Bar), never MFC's (CFoo, CFooMgr).

Output only the candidate names, one per line, no numbering, no commentary."""

IDENT = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')


def ask(url, model, mode, n, ctx, temperature, seed, timeout=900, system=None):
    body = dict(model=model, temperature=temperature, seed=seed,
                max_tokens=600 if mode == 'off' else 6000,
                messages=[dict(role='system', content=(system or SYSTEM).replace('{n}', str(n))),
                          dict(role='user', content=ctx)],
                chat_template_kwargs=({'enable_thinking': False} if mode == 'off' else {'reasoning_effort': mode}))
    req = urllib.request.Request(url, json.dumps(body).encode(), {'Content-Type': 'application/json'})
    t = time.time()
    r = json.load(urllib.request.urlopen(req, timeout=timeout))
    text = r['choices'][0]['message'].get('content') or ''
    names = []
    for line in text.splitlines():
        w = line.strip().strip('-*`. ').split('\t')[-1].strip()
        w = re.sub(r'^\d+[.)]\s*', '', w)
        if IDENT.fullmatch(w) and w not in names:
            names.append(w)
    u = r.get('usage', {})
    return names, u.get('prompt_tokens', 0), u.get('completion_tokens', 0), time.time() - t


def run(a, classes, mode):
    """-> {class: [candidates]}, totals. `--samples` requests per class (seeds 0..), candidates merged in order."""
    jobs = [(c, s) for c in classes for s in range(a.samples)]
    out = collections.defaultdict(list)
    tin = tout = 0
    fails = 0
    t0 = time.time()

    def one(job):
        c, s = job
        ctx = open(os.path.join(a.ctx, c + '.txt')).read()
        return c, ask(a.url, a.model, mode, a.n, ctx, a.temperature, a.seed + s)

    with concurrent.futures.ThreadPoolExecutor(a.parallel) as ex:
        futs = [ex.submit(one, j) for j in jobs]
        for f in concurrent.futures.as_completed(futs):
            try:
                c, (names, pi, po, _) = f.result()
            except Exception as e:      # one failed request must not lose the batch
                fails += 1
                print(f'  request failed: {e}', flush=True)
                continue
            tin += pi
            tout += po
            for x in names:
                if x not in out[c]:
                    out[c].append(x)
    return out, dict(seconds=time.time() - t0, tokens_in=tin, tokens_out=tout, failed=fails, requests=len(jobs))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--url', default='http://127.0.0.1:8080/v1/chat/completions')
    ap.add_argument('--model', default='qwen3.8-27b')
    ap.add_argument('--parallel', type=int, default=16)
    ap.add_argument('--ctx', default=os.path.join(paths.WORK, 'tier2', 'ctx3'), help='tier2_ctx3.py\'s context files')
    ap.add_argument('--n', type=int, default=20, help='candidates asked per request')
    ap.add_argument('--samples', type=int, default=1, help='requests per class (different seeds)')
    ap.add_argument('--temperature', type=float, default=0.8)
    ap.add_argument('--seed', type=int, default=0)
    ap.add_argument('--pilot', action='store_true')
    ap.add_argument('--modes', default='off,low,medium')
    ap.add_argument('--mode', default='off')
    ap.add_argument('--classes', help='file with one class name per line')
    ap.add_argument('--out', help='proposals TSV to write')
    a = ap.parse_args()
    if a.pilot:
        truth = {r['ours']: r['retail'] for r in csv.DictReader(open(paths.RENAMES), delimiter='\t')
                 if r['first_step'] == 'r3' and os.path.exists(os.path.join(a.ctx, r['ours'] + '.txt'))}
        print(f'pilot: {len(truth)} classes round 3 confirmed, {a.samples} request(s) per class, {a.n} names asked')
        outdir = paths.workdir('local_pilot')
        for mode in a.modes.split(','):
            out, tot = run(a, sorted(truth), mode)
            ranks = [out[c].index(x) + 1 for c, x in truth.items() if x in out[c]]
            ncand = sum(len(v) for v in out.values())
            with open(os.path.join(outdir, f'{mode}.tsv'), 'w', newline='') as f:
                for c in sorted(out):
                    for x in out[c]:
                        f.write(f'CLASS\t{c}\t{x}\n')
            hrs = tot['seconds'] / 3600
            print(f'  mode {mode:7s} found {len(ranks)} of {len(truth)} (ranks {sorted(ranks)}); candidates {ncand}; '
                  f'{tot["seconds"]:.0f} s, tokens in {tot["tokens_in"]} out {tot["tokens_out"]}, failed '
                  f'{tot["failed"]}; {len(ranks) / hrs:.0f} found per hour, '
                  f'{tot["tokens_out"] / max(1, len(ranks)):.0f} output tokens per find', flush=True)
        return
    classes = [x.strip() for x in open(a.classes) if x.strip()]
    out, tot = run(a, classes, a.mode)
    os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
    with open(a.out, 'w', newline='') as f:
        for c in classes:
            for x in out.get(c, ()):
                f.write(f'CLASS\t{c}\t{x}\n')
    print(f'{a.out}: {len(out)} of {len(classes)} classes, {sum(len(v) for v in out.values())} candidates; {tot}')


if __name__ == '__main__':
    main()
