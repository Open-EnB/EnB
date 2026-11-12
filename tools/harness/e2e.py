r"""End-to-end scenarios for POC client builds (D237): an input script plus log-event assertions.

    python tools/harness/e2e.py run [NAME...] [--instance ID] [--exe PATH] [--messages] [--keep]
    python tools/harness/e2e.py list                       the scenarios, in order, with their expected-fail notes
    python tools/harness/e2e.py check [NAME...]            parse only (no client)

Each scenario is tools/harness/e2e/NAME.scn and runs in its own client: antprox /REPLAY starts the POC
(tools/harness/clientpoc.py's instance, input and antprox code), the steps run in order, and the run ends at the
last step. Exit 0 only if every scenario passes (an `expected-fail` scenario that fails as expected counts as
passed and is printed XFAIL; one that passes is printed XPASS and fails the run, so the note gets removed).
One line per scenario on stdout; everything else in .work/e2e/<HHMMSS>-<NAME>/ (e2e.log, events.log,
antprox.out, a copy of stubs.log, shots). The instance is removed at the end (clientpoc.py clean) unless --keep.

The format (one step a line, '#' comments, shell-style quoting):
    name: TEXT / expected-fail: REASON / timeout: SECONDS     headers (before the steps)
    include NAME                  NAME.scn's steps here (its headers are not taken)
    wait-event SOURCE PATTERN [within S]   wait until a line of SOURCE matches the regex PATTERN (default 60 s)
    expect-event SOURCE PATTERN [within S] the same; reads as an assertion (default 15 s)
    expect-no SOURCE PATTERN      from here to the end no line of SOURCE may match (checked continuously)
    expect-no CRASH               short for: expect-no stubs '^CRASH:' (every scenario has it from the start)
    click gadget NAME | click X,Y   rclick ... | move ...    mouse at a gadget of e2e/gadgets.txt or client pixels
    key VK                        a key (decimal or 0x hex)     text STRING    characters
    stand-in PATTERN              the goal is not met while stubs.log shows PATTERN from here on (a stand-in's first
                                  call): checked at the end, so the other steps still run; with `expected-fail` the
                                  scenario is XFAIL, and XPASS once the stand-in is retired
    stray click X,Y|gadget NAME / stray key VK   the same input UNtagged, as a person's would be (the tag's
                                  self-test: the client must drop it and log 'NOT REPLAYED')
    alive S                       S seconds pass with the client running and no expect-no line matched
    shot NAME                     the client's own back buffer at the next Present (d3d8shot's D3D8SHOT_TRIGGER)
SOURCE: antprox (antprox.out: antprox, the replay server and the client's stderr), stubs (stubs.log), msg
(messages.log; needs --messages or a 'messages: on' header), any. wait-event / expect-event look at the lines
since the last input step (or the start), so an event the step causes is not missed however fast it comes.

Input is posted with the harness's tag (programs/clientpoc/poc_input.cpp: POC_TAG in wParam's high word) and the
client runs with ENB_POC_INPUT_TAG=1: untagged input, a human's click or keystroke in the test window, is dropped
and logged 'POC input: NOT REPLAYED'; the result line counts such lines. A POC without the hook is refused.

stubs.log's 'STUB first call: gen NAME' lines are summarised per scenario: every generated stand-in the run hit,
and those first hit after the scenario's own first step (its `include` done), the queue's score input (P-M2-6).
"""
import argparse
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
SCN = os.path.join(HERE, 'e2e')
OUT = os.path.join(ROOT, '.work', 'e2e')
ORDER = ['world_entry', 'starmap', 'options', 'pda_pages', 'cargo', 'chat']
STANDIN_HIT = 'stand-in hit:'
SOURCES = ('antprox', 'stubs', 'msg', 'any')
ACTIONS = ('click', 'rclick', 'move', 'key', 'text', 'stray')


class ScenarioError(Exception):
    pass


# ---------------------------------------------------------------- the parser

def load_gadgets(path=None):
    """e2e/gadgets.txt: NAME X Y [# evidence] -> {NAME: (X, Y)} (client pixels at 1280x1024)."""
    path = path or os.path.join(SCN, 'gadgets.txt')
    g = {}
    if not os.path.exists(path):
        return g
    with open(path, encoding='utf-8') as f:
        text = f.read()
    for n, line in enumerate(text.splitlines(), 1):
        line = line.split('#', 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) != 3:
            raise ScenarioError(f'{path}:{n}: NAME X Y expected')
        g[parts[0]] = (int(parts[1]), int(parts[2]))
    return g


def _point(args, gadgets, where):
    if len(args) == 2 and args[0] == 'gadget':
        if args[1] not in gadgets:
            raise ScenarioError(f'{where}: no gadget {args[1]!r} in gadgets.txt')
        x, y = gadgets[args[1]]
        return x, y, args[1]
    if len(args) == 1 and re.fullmatch(r'\d+,\d+', args[0]):
        x, y = (int(v) for v in args[0].split(','))
        return x, y, None
    raise ScenarioError(f'{where}: gadget NAME or X,Y expected')


def _within(args, default, where):
    if len(args) >= 2 and args[-2] == 'within':
        try:
            return args[:-2], float(args[-1])
        except ValueError:
            raise ScenarioError(f'{where}: within SECONDS')
    return args, default


def parse(text, name='?', gadgets=None, loader=None, _depth=0):
    """A scenario's text -> {'name', 'expected_fail', 'timeout', 'messages', 'steps': [dict], 'own': index of the
    first own step (after the includes before it)}. loader(NAME) -> text, for include."""
    gadgets = gadgets if gadgets is not None else {}
    sc = {'name': name, 'expected_fail': None, 'timeout': 300, 'messages': False, 'steps': [], 'own': 0}
    seen_step = False
    for n, raw in enumerate(text.splitlines(), 1):
        where = f'{name}:{n}'
        line = raw.strip()
        if not line or line.startswith('#'):
            continue
        m = re.match(r'([a-z-]+):\s*(.*)$', line)
        if m and not seen_step:
            key, val = m.groups()
            if key == 'name':
                sc['title'] = val
            elif key == 'expected-fail':
                sc['expected_fail'] = val
            elif key == 'timeout':
                sc['timeout'] = float(val)
            elif key == 'messages':
                sc['messages'] = val.lower() in ('on', 'yes', '1')
            else:
                raise ScenarioError(f'{where}: unknown header {key!r}')
            continue
        try:
            words = shlex.split(line, comments=True, posix=True)
        except ValueError as e:
            raise ScenarioError(f'{where}: {e}')
        op, args = words[0], words[1:]
        seen_step = True
        st = {'op': op, 'line': where}
        if op == 'include':
            if len(args) != 1 or loader is None or _depth > 4:
                raise ScenarioError(f'{where}: include NAME')
            sub = parse(loader(args[0]), args[0], gadgets, loader, _depth + 1)
            if sub['messages']:
                sc['messages'] = True
            sc['steps'].extend(sub['steps'])
            sc['own'] = len(sc['steps'])
            continue
        if op in ('wait-event', 'expect-event'):
            args, within = _within(args, 60.0 if op == 'wait-event' else 15.0, where)
            if len(args) != 2 or args[0] not in SOURCES:
                raise ScenarioError(f'{where}: {op} SOURCE PATTERN [within S] (SOURCE: {", ".join(SOURCES)})')
            st.update(source=args[0], pattern=_regex(args[1], where), within=within)
        elif op == 'stand-in':
            if len(args) != 1:
                raise ScenarioError(f'{where}: stand-in PATTERN (a stubs.log line, e.g. the first call of a POC stand-in)')
            st.update(source='stubs', pattern=_regex(args[0], where))
        elif op == 'expect-no':
            if args == ['CRASH']:
                args = ['stubs', '^CRASH:']
            if len(args) != 2 or args[0] not in SOURCES:
                raise ScenarioError(f'{where}: expect-no SOURCE PATTERN | expect-no CRASH')
            st.update(source=args[0], pattern=_regex(args[1], where))
        elif op in ('click', 'rclick', 'move'):
            x, y, g = _point(args, gadgets, where)
            st.update(x=x, y=y, gadget=g)
        elif op == 'stray':
            if not args or args[0] not in ('click', 'key'):
                raise ScenarioError(f'{where}: stray click X,Y|gadget NAME, or stray key VK')
            if args[0] == 'click':
                x, y, g = _point(args[1:], gadgets, where)
                st.update(x=x, y=y, gadget=g, what='click')
            else:
                try:
                    st.update(vk=int(args[1], 0), what='key')
                except (IndexError, ValueError):
                    raise ScenarioError(f'{where}: stray key VK')
        elif op == 'key':
            if len(args) != 1:
                raise ScenarioError(f'{where}: key VK')
            try:
                st['vk'] = int(args[0], 0)
            except ValueError:
                raise ScenarioError(f'{where}: key VK (decimal or 0x hex)')
        elif op == 'text':
            if len(args) != 1:
                raise ScenarioError(f'{where}: text STRING (quote it when it has spaces)')
            st['text'] = args[0]
        elif op == 'alive':
            if len(args) != 1:
                raise ScenarioError(f'{where}: alive SECONDS')
            st['seconds'] = float(args[0])
        elif op == 'shot':
            if len(args) != 1 or not re.fullmatch(r'[A-Za-z0-9_-]{1,40}', args[0]):
                raise ScenarioError(f'{where}: shot NAME (letters, digits, - and _)')
            st['shot'] = args[0]
        else:
            raise ScenarioError(f'{where}: unknown step {op!r}')
        sc['steps'].append(st)
    if not sc['steps']:
        raise ScenarioError(f'{name}: no steps')
    return sc


def _regex(p, where):
    try:
        return re.compile(p)
    except re.error as e:
        raise ScenarioError(f'{where}: bad pattern {p!r}: {e}')


def file_loader(name):
    path = os.path.join(SCN, name + '.scn')
    if not os.path.exists(path):
        raise ScenarioError(f'no scenario {name} ({path})')
    with open(path, encoding='utf-8') as f:
        return f.read()


def load(name, gadgets=None):
    return parse(file_loader(name), name, gadgets if gadgets is not None else load_gadgets(), file_loader)


def scenario_names():
    have = sorted(f[:-4] for f in os.listdir(SCN) if f.endswith('.scn') and not f.startswith('_'))
    return [n for n in ORDER if n in have] + [n for n in have if n not in ORDER]


# ---------------------------------------------------------------- the event stream and the matcher

class Events:
    """Lines of several growing files, in the order they were read, each (seconds since start, source, text)."""
    def __init__(self, files, clock=time.time):
        self.files = dict(files)                # source -> path
        self.pos = {s: 0 for s in self.files}
        self.part = {s: '' for s in self.files}
        self.lines = []
        self.clock = clock
        self.t0 = clock()

    def poll(self):
        for src, path in self.files.items():
            try:
                with open(path, 'rb') as f:
                    f.seek(self.pos[src])
                    data = f.read()
            except OSError:
                continue
            if not data:
                continue
            self.pos[src] += len(data)
            text = self.part[src] + data.decode('latin1')
            *done, self.part[src] = text.split('\n')
            now = self.clock() - self.t0
            for l in done:
                self.lines.append((now, src, l.rstrip('\r')))
        return len(self.lines)

    def add(self, src, text):           # tests, and the harness's own notes
        self.lines.append((self.clock() - self.t0, src, text))


def find(lines, source, pattern, start=0):
    """Index of the first line at or after start whose source matches (any: every source) and whose text the
    regex pattern searches, or None."""
    for i in range(start, len(lines)):
        _, src, text = lines[i]
        if (source == 'any' or src == source) and pattern.search(text):
            return i
    return None


def gen_lines(text_lines):
    """The generated stand-ins' first calls, in order: 'STUB first call: gen NAME' -> NAME."""
    out = []
    for l in text_lines:
        m = re.match(r'STUB first call: gen (\S+)', l)
        if m and m.group(1) not in out:
            out.append(m.group(1))
    return out


def human_lines(text_lines):
    return [l for l in text_lines if l.startswith('POC input: NOT REPLAYED')]


# ---------------------------------------------------------------- the runner

def run_one(sc, args, log):
    import clientpoc
    out = os.path.join(OUT, time.strftime('%H%M%S') + '-' + sc['name'])
    os.makedirs(out, exist_ok=True)
    elog = open(os.path.join(out, 'e2e.log'), 'w', encoding='utf-8', newline='\n')

    def note(s):
        elog.write(f'{time.strftime("%H:%M:%S")} {s}\n')
        elog.flush()

    inst = clientpoc.Inst(args.instance, args.exe)
    inst.prepare()
    env = dict(os.environ)
    env.update(ENB_POC_INPUT='1', ENB_POC_WINDOWED='1', ENB_POC_INPUT_TAG='1')
    proxy = os.path.join(inst.dir, 'd3d8.dll')
    trigger = os.path.join(out, 'trigger.txt')
    shots = any(s['op'] == 'shot' for s in sc['steps'])
    if shots:
        if not os.path.exists(clientpoc.PROXY):
            raise SystemExit(f'{clientpoc.PROXY} missing: python tools/harness/run_reference.py build')
        shutil.copyfile(clientpoc.PROXY, proxy)
        env.update(D3D8SHOT_DIR=out, D3D8SHOT_TRIGGER=trigger, D3D8SHOT_MAX='20')
    messages = sc['messages'] or args.messages
    cargs = ['-ENABLE_MESSAGES'] if messages else []
    files = {'antprox': os.path.join(out, 'antprox.out'), 'stubs': inst.log}
    if messages:
        files['msg'] = os.path.join(out, 'messages.log')
    note(f'scenario {sc["name"]}: exe {inst.src_exe}, instance {inst.name}, out {out}')
    a, pid, _ = clientpoc.start_antprox(inst, out, env, cargs)
    ev = Events(files)
    listener = None
    fail, mark, standing, hwnd = None, 0, [], None
    gen_before, own_from, standins = [], 0, []
    deadline = time.time() + sc['timeout']
    standing.append({'source': 'stubs', 'pattern': re.compile('^CRASH:'), 'line': '(always)'})

    def tick():
        nonlocal hwnd
        ev.poll()
        for s in standing:
            i = find(ev.lines, s['source'], s['pattern'], s.get('from', 0))
            if i is not None:
                return f'{s["line"]}: expect-no {s["source"]} {s["pattern"].pattern!r} matched: {ev.lines[i][2][:160]}'
        if a.poll() is not None or (pid and not _alive(pid)):
            return 'the client exited'
        if time.time() > deadline:
            return f'scenario timeout ({sc["timeout"]:.0f} s)'
        if hwnd is None and pid:
            hwnd = clientpoc.find_window(pid)
            if hwnd:
                note(f'window 0x{hwnd:X}')
        return None

    try:
        note(f'antprox pid {a.pid}, client pid {pid}')
        if not pid:
            fail = 'no client process under antprox'
        if messages and pid:
            listener = clientpoc.start_listener(pid, out, sc['timeout'] + 10)
        for k, st in enumerate(sc['steps']):
            if fail:
                break
            if k == sc['own'] and sc['own']:
                ev.poll()
                gen_before = gen_lines([t for _, s, t in ev.lines if s == 'stubs'])
                own_from = len(ev.lines)
                note(f'own steps start; {len(gen_before)} gen stand-ins hit so far')
            note(f'step {st["line"]}: {st["op"]} {_desc(st)}')
            op = st['op']
            if op in ('wait-event', 'expect-event'):
                end = time.time() + st['within']
                while True:
                    fail = tick()
                    if fail:
                        break
                    i = find(ev.lines, st['source'], st['pattern'], mark)
                    if i is not None:
                        note(f'  matched at {ev.lines[i][0]:.1f} s: {ev.lines[i][2][:200]}')
                        break
                    if time.time() > end:
                        fail = (f'{st["line"]}: {op} {st["source"]} {st["pattern"].pattern!r}: not seen within '
                                f'{st["within"]:.0f} s')
                        break
                    time.sleep(0.05)
            elif op == 'expect-no':
                standing.append(dict(st, **{'from': mark}))
            elif op == 'stand-in':
                standins.append(dict(st, **{'from': mark}))
            elif op in ACTIONS:
                while hwnd is None and not fail:
                    fail = tick()
                    time.sleep(0.05)
                if fail:
                    break
                if not _has_tag_hook(ev):
                    fail = 'this POC has no input tag hook (programs/clientpoc/poc_input.cpp): rebuild it'
                    break
                ev.poll()
                mark = len(ev.lines)
                if op == 'stray':                         # a human's input, as the harness's self-test: untagged
                    clientpoc.post_step(hwnd, st['what'], str(st['vk']) if st['what'] == 'key'
                                        else f'{st["x"]} {st["y"]}', tag=False)
                elif op == 'key':
                    clientpoc.post_step(hwnd, 'key', str(st['vk']), tag=True)
                elif op == 'text':
                    clientpoc.post_step(hwnd, 'text', st['text'], tag=True)
                else:
                    clientpoc.post_step(hwnd, op, f'{st["x"]} {st["y"]}', tag=True)
            elif op == 'alive':
                end = time.time() + st['seconds']
                while time.time() < end and not fail:
                    fail = tick()
                    time.sleep(0.1)
            elif op == 'shot':
                with open(trigger + '.tmp', 'w') as f:      # renamed in place: the proxy never reads it half written
                    f.write(st['shot'] + '\n')
                os.replace(trigger + '.tmp', trigger)
                end = time.time() + 15
                shot_log = os.path.join(out, 'd3d8shot.log')
                while time.time() < end and not fail:
                    fail = tick()
                    if not os.path.exists(trigger):
                        break
                    time.sleep(0.1)
                if not fail and os.path.exists(trigger):
                    note('  shot not taken in 15 s (a proxy without D3D8SHOT_TRIGGER? run_reference.py build)')
                    os.remove(trigger)
                elif not fail:
                    note(f'  shot {st["shot"]} taken ({shot_log})')
        if not fail:
            fail = tick()
        if not fail:
            for s in standins:
                i = find(ev.lines, 'stubs', s['pattern'], s['from'])
                if i is not None:
                    fail = f'{STANDIN_HIT} {s["line"]}: {ev.lines[i][2][:120]}'
                    break
    finally:
        if pid:
            subprocess.run(['taskkill', '/F', '/PID', str(pid)], capture_output=True)
        subprocess.run(['taskkill', '/F', '/T', '/PID', str(a.pid)], capture_output=True)
        time.sleep(1)
        if shots and os.path.exists(proxy):
            os.remove(proxy)
        clientpoc.stop_listener(listener, inst, out if messages else None)
    ev.poll()
    stub_text = [t for _, s, t in ev.lines if s == 'stubs']
    with open(os.path.join(out, 'events.log'), 'w', encoding='utf-8', newline='\n') as f:
        for t, s, text in ev.lines:
            f.write(f'{t:8.2f} {s:7s} {text}\n')
    if os.path.exists(inst.log):
        shutil.copyfile(inst.log, os.path.join(out, 'stubs.log'))
    if shots:
        for name in sorted(os.listdir(out)):
            if name.endswith('.bmp'):
                import bmp2png
                bmp = os.path.join(out, name)
                w, h, rows = bmp2png.read_bmp(bmp)
                bmp2png.write_png(bmp[:-4] + '.png', w, h, rows)
                os.remove(bmp)
    gens = gen_lines(stub_text)
    own = [g for g in gens if g not in gen_before] if sc['own'] else gens
    poc_own = [t[len('STUB first call: POC '):][:80] for _, src, t in ev.lines[own_from:]
               if src == 'stubs' and t.startswith('STUB first call: POC ') and 'watchdog' not in t]
    human = human_lines(stub_text)
    crash = [t for t in stub_text if t.startswith('CRASH:')]
    res = {'name': sc['name'], 'passed': fail is None, 'failure': fail, 'expected_fail': sc['expected_fail'],
           'gen': gens, 'gen_own': own, 'poc_own': poc_own, 'human_dropped': len(human), 'crash': crash[:1], 'out': out,
           'shots': sorted(f for f in os.listdir(out) if f.startswith('shot_'))}
    if crash and inst.map and os.path.exists(inst.map):
        import symbolize
        syms = symbolize.load(inst.map)
        a0 = re.findall(r'\b([0-9A-F]{8})\b', crash[0])
        res['crash_at'] = [f'{int(x, 16):08X} {symbolize.name_of(syms, int(x, 16))}' for x in a0[:1] + a0[2:6]
                           if 0x401000 <= int(x, 16) < 0x1000000]
    note(f'result: {json.dumps(res)}')
    with open(os.path.join(out, 'result.json'), 'w', encoding='utf-8') as f:
        json.dump(res, f, indent=1)
    elog.close()
    return res


def _desc(st):
    if st['op'] in ('wait-event', 'expect-event', 'expect-no'):
        return f'{st["source"]} {st["pattern"].pattern!r}'
    if st['op'] in ('click', 'rclick', 'move'):
        return (f'gadget {st["gadget"]} ' if st['gadget'] else '') + f'({st["x"]},{st["y"]})'
    return str(st.get('vk', st.get('text', st.get('seconds', st.get('shot', '')))))


def _has_tag_hook(ev):
    return any(s == 'stubs' and 'tagged input read' in t for _, s, t in ev.lines)


def _alive(pid):
    import ctypes
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, pid)
    if not h:
        return False
    code = ctypes.c_ulong()
    k.GetExitCodeProcess(h, ctypes.byref(code))
    k.CloseHandle(h)
    return code.value == 259                     # STILL_ACTIVE


def verdict(res):
    """PASS / FAIL / XFAIL (expected-fail, and it failed only on a `stand-in` step: every other step held) / XPASS
    (expected-fail but passed: the stand-in is gone, remove the note). Any other failure is FAIL, expected-fail or not."""
    if res['passed']:
        return 'XPASS' if res['expected_fail'] else 'PASS'
    if res['expected_fail'] and (res['failure'] or '').startswith(STANDIN_HIT):
        return 'XFAIL'
    return 'FAIL'


def summary_line(res):
    v = verdict(res)
    own = res['gen_own']
    gen = f'gen {len(res["gen"])} hit, {len(own)} own' + (f' ({", ".join(own[:6])}{", ..." if len(own) > 6 else ""})'
                                                          if own else '')
    tail = []
    if res['failure']:
        tail.append(res['failure'])
    if res.get('crash_at'):
        tail.append('at ' + res['crash_at'][0])
    if res['expected_fail']:
        tail.append('expected-fail: ' + res['expected_fail'])
    if res['human_dropped']:
        tail.append(f'{res["human_dropped"]} untagged (human) input dropped')
    return f'{v:5s} {res["name"]}: {gen}' + ('; ' + '; '.join(tail) if tail else '')


def default_exe():
    p = os.path.join(ROOT, 'build', 'programs', 'clientpoc', 'clientpoc.exe')
    return p if os.path.exists(p) else None


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('command', choices=['run', 'list', 'check'])
    ap.add_argument('names', nargs='*')
    ap.add_argument('--instance', default='e2e', help='clientpoc.py instance name (default e2e)')
    ap.add_argument('--exe', help='the POC exe (default: this tree\'s build/programs/clientpoc/clientpoc.exe, '
                                  'else ClientPOC\\release\'s)')
    ap.add_argument('--messages', action='store_true', help='-ENABLE_MESSAGES and msglisten (source msg)')
    ap.add_argument('--keep', action='store_true', help='keep the instance (default: clientpoc.py clean it at the end)')
    args = ap.parse_args(argv)
    names = args.names or scenario_names()
    try:
        gadgets = load_gadgets()
        scs = [load(n, gadgets) for n in names]
    except ScenarioError as e:
        print(f'e2e: {e}')
        return 2
    if args.command in ('list', 'check'):
        for sc in scs:
            print(f'{sc["name"]}: {len(sc["steps"])} steps ({len(sc["steps"]) - sc["own"]} own)'
                  + (f'; expected-fail: {sc["expected_fail"]}' if sc['expected_fail'] else ''))
        return 0
    if args.exe is None:
        args.exe = default_exe()
    os.makedirs(OUT, exist_ok=True)
    sys.path.insert(0, HERE)
    bad = 0
    for sc in scs:
        res = run_one(sc, args, None)
        print(summary_line(res), flush=True)
        bad += verdict(res) in ('FAIL', 'XPASS')
    print(f'e2e: {len(scs) - bad} of {len(scs)} as expected; logs under {OUT}')
    if not args.keep:
        import clientpoc
        clientpoc.clean(args.instance)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
