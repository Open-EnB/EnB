r"""End-to-end scenarios for client builds (D237): an input script plus log-event assertions.

    python tools/harness/e2e.py run [NAME...] [--instance ID] [--exe PATH] [--messages] [--keep] [--separate]
    python tools/harness/e2e.py run --retail [NAME...] --instance ID    the same scenarios against the retail client
    python tools/harness/e2e.py compare DIR_OURS DIR_RETAIL    the two runs' packets per scenario, side by side
    python tools/harness/e2e.py list                       the scenarios, in order, with their expected-fail notes
    python tools/harness/e2e.py check [NAME...]            parse only (no client)

Each scenario is tools/harness/e2e/NAME.scn. A run is ONE client session for all its scenarios: antprox /REPLAY
starts the client (tools/harness/clientpoc.py's instance and antprox code), the first scenario's steps run from
its `include`s (the login), and every later scenario runs only its own steps in the same world; a scenario leaves
the world as it found it (it closes what it opens). After a scenario that fails, the client is stopped and the
rest run in a fresh session (a crash or a window left open must not fail the others); so do they after an XFAIL,
whose stand-in left the world changed. The run stops, the rest
printed FAIL 'not run', when a second crash is at the same place as an earlier one (the same exception code,
address and first return addresses: repeating it tells nothing new), or when two sessions in a row fail before
their first scenario's own steps (the login). --separate gives every
scenario its own client, as before 2026-11-14. Exit 0 only if every scenario passes (an `expected-fail` scenario
that fails as expected counts as passed and is printed XFAIL; one that passes is printed XPASS and fails the
run, so the note gets removed). One line per scenario on stdout; everything else in .work/e2e/<HHMMSS>-<NAME or
'session'>/ (e2e.log, events.log, antprox.out, d3d8shot.log, a copy of stubs.log, result-NAME.json, shots). The
instance is removed at the end (clientpoc.py clean) unless --keep.

The format (one step a line, '#' comments, shell-style quoting):
    name: TEXT / expected-fail: REASON / timeout: SECONDS     headers (before the steps)
    include NAME                  NAME.scn's steps here (its headers are not taken)
    wait-event SOURCE PATTERN [within S]   wait until a line of SOURCE matches the regex PATTERN (default 60 s)
    expect-event SOURCE PATTERN [within S] the same; reads as an assertion (default 15 s)
    wait-quiet SOURCE S [within T]  wait until SOURCE has had no new line for S seconds, counted from the later of
                                  its last line and the last input step (default T 30 s): "the loading this step
                                  started is over" where the client logs no "ready" line (the proxy's file opens)
    expect-no SOURCE PATTERN      from here to the scenario's end no line of SOURCE may match (checked continuously)
    expect-no CRASH               short for: expect-no stubs '^CRASH:' (every scenario has it from the start)
    click gadget NAME | click X,Y   rclick ... | move ...    mouse at a gadget of e2e/gadgets.txt or client pixels
    click X,Y until SOURCE PATTERN [redo K] [every S] [within T]   the click again every S seconds (default 0.5)
                                  until the line is seen (default T 60 s): for input the client ignores until it is
                                  ready. redo K: each repeat first posts the K input steps before this one again
                                  (select a slot, then Enter: both are ignored while the screen's opening plays)
    key VK                        a key (decimal or 0x hex)     text STRING    characters
    stand-in PATTERN              the goal is not met while stubs.log shows PATTERN from here on (a stand-in's first
                                  call): checked at the end, so the other steps still run; with `expected-fail` the
                                  scenario is XFAIL, and XPASS once the stand-in is retired
    stray click X,Y|gadget NAME / stray key VK   the same input UNtagged, as a person's would be (the tag's
                                  self-test: the proxy must drop it and log 'NOT REPLAYED')
    alive S                       S seconds pass with the client running and no expect-no line matched
    shot NAME                     the client's own back buffer at the next Present (d3d8shot's D3D8SHOT_TRIGGER)
SOURCE: antprox (antprox.out: antprox, the replay server and the client's stderr), stubs (stubs.log), msg
(messages.log; needs --messages or a 'messages: on' header), proxy (d3d8shot.log without its tick column: the
proxy's log of the client's file opens, CreateFileA, and of the input it passed on; `files` is the same source),
any. wait-event / expect-event look at the lines since the last input step (or the start), so an event the step
causes is not missed however fast it comes.

The exe carries no test code. The d3d8shot proxy (tools/harness/d3d8shot.c, copied beside the instance's exe)
gives the client a virtual cursor and keyboard (D3D8SHOT_INPUT=1), our build and the retail client alike. Input
is posted with the harness's tag (IN_TAG in wParam's high word) and the proxy runs with D3D8SHOT_INPUT_TAG=1:
untagged input, a person's click or keystroke in the test window, is dropped and logged 'input: NOT REPLAYED';
the result line counts such lines.

--retail (D190, D248): the scenarios drive a COPY of the retail client (clientpoc.py's Inst(retail=True): the copy
in the instance folder, never the install), through the same proxy and the same tagged input; the terms box's
"I Agree" is clicked for it. The retail client writes no stubs.log, so a step on the `stubs` source is ours only:
wait-event / expect-event / expect-no / stand-in steps on it are skipped (noted in e2e.log), and a crash shows as
'the client exited'. `compare` then reads two runs' events.log (ours, retail's) and prints, per scenario, the
packets the client sent (antprox.out's 'received NAME' lines between the scenario's marks) where the two differ:
the retail run is the expected result.

stubs.log's 'STUB first call: gen NAME' lines are summarised per scenario: every generated stand-in hit while
it ran, and those hit during its own steps that the session's login had not hit (the queue's score input,
P-M2-6). At each scenario's first own step the harness writes a marker through the proxy ('mark NAME' in the
trigger file -> the exe's exported Stub_Log_Mark, stubs/stub_log.cpp): the stand-ins' first-call flags start
over, so a stand-in an earlier scenario of the session hit is listed again for this one. A build without the
export still runs; its later scenarios then list only what no earlier one hit ('no stand-in marker' in e2e.log).
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
# Scenarios that must run after everything else (one that leaves the world changed where no following scenario may
# use it: it then costs no extra client start). None today.
LAST = []
STANDIN_HIT = 'stand-in hit:'
SOURCES = ('antprox', 'stubs', 'msg', 'proxy', 'files', 'any')
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


def _within(args, default, where, word='within'):
    if len(args) >= 2 and args[-2] == word:
        try:
            return args[:-2], float(args[-1])
        except ValueError:
            raise ScenarioError(f'{where}: {word} SECONDS')
    return args, default


def _source(name):
    return 'proxy' if name == 'files' else name     # the proxy's log, by its older name


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
            st.update(source=_source(args[0]), pattern=_regex(args[1], where), within=within)
        elif op == 'wait-quiet':
            args, within = _within(args, 30.0, where)
            try:
                if len(args) != 2 or args[0] not in SOURCES or args[0] == 'any':
                    raise ValueError
                st.update(source=_source(args[0]), quiet=float(args[1]), within=within)
            except ValueError:
                raise ScenarioError(f'{where}: wait-quiet SOURCE SECONDS [within S]')
        elif op == 'stand-in':
            if len(args) != 1:
                raise ScenarioError(f'{where}: stand-in PATTERN (a stubs.log line, e.g. the first call of a POC stand-in)')
            st.update(source='stubs', pattern=_regex(args[0], where))
        elif op == 'expect-no':
            if args == ['CRASH']:
                args = ['stubs', '^CRASH:']
            if len(args) != 2 or args[0] not in SOURCES:
                raise ScenarioError(f'{where}: expect-no SOURCE PATTERN | expect-no CRASH')
            st.update(source=_source(args[0]), pattern=_regex(args[1], where))
        elif op in ('click', 'rclick', 'move'):
            until = None
            if 'until' in args:
                k = args.index('until')
                rest, within = _within(args[k + 1:], 60.0, where)
                rest, every = _within(rest, 0.5, where, 'every')
                rest, redo = _within(rest, 0, where, 'redo')
                if len(rest) != 2 or rest[0] not in SOURCES:
                    raise ScenarioError(f'{where}: {op} ... until SOURCE PATTERN [every S] [within S]')
                until = {'source': _source(rest[0]), 'pattern': _regex(rest[1], where), 'every': every,
                         'within': within, 'redo': int(redo)}
                args = args[:k]
            x, y, g = _point(args, gadgets, where)
            st.update(x=x, y=y, gadget=g, until=until)
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
    return ([n for n in ORDER if n in have] + [n for n in have if n not in ORDER and n not in LAST]
            + [n for n in LAST if n in have])


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
        self.last = {}                          # source -> the time its newest line was read

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
                l = l.rstrip('\r')
                if src == 'proxy':              # d3d8shot.log: '%10lu ' (GetTickCount) before each line
                    l = re.sub(r'^\s*\d+ ', '', l, count=1)
                self.lines.append((now, src, l))
            if done:
                self.last[src] = now
        return len(self.lines)

    def now(self):
        return self.clock() - self.t0

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
    """The proxy's lines for untagged input it dropped (a person's click or key in the test window)."""
    return [l for l in text_lines if l.startswith('input: NOT REPLAYED')]


# ---------------------------------------------------------------- the runner

class Session:
    """One client under antprox /REPLAY, for one scenario or several in a row."""

    def __init__(self, args, scs):
        import clientpoc
        self.cp = clientpoc
        self.args = args
        tag = scs[0]['name'] if len(scs) == 1 else 'session'
        self.out = os.path.join(OUT, time.strftime('%H%M%S') + '-' + tag)
        os.makedirs(self.out, exist_ok=True)
        self.elog = open(os.path.join(self.out, 'e2e.log'), 'w', encoding='utf-8', newline='\n')
        self.retail = bool(getattr(args, 'retail', False))
        self.inst = clientpoc.Inst(args.instance, None if self.retail else args.exe, self.retail)
        self.inst.prepare()
        env = dict(os.environ)
        env.update(ENB_POC_WINDOWED='1')
        self.proxy = self.inst.use_proxy(env, self.out, input=True, tag=True)
        self.trigger = os.path.join(self.out, 'trigger.txt')
        env.update(D3D8SHOT_TRIGGER=self.trigger, D3D8SHOT_MAX=str(20 * len(scs)))
        self.messages = args.messages or any(sc['messages'] for sc in scs)
        files = {'antprox': os.path.join(self.out, 'antprox.out'), 'stubs': self.inst.log,
                 'proxy': os.path.join(self.out, 'd3d8shot.log')}
        if self.messages:
            files['msg'] = os.path.join(self.out, 'messages.log')
        self.note(f'session of {len(scs)}: {", ".join(sc["name"] for sc in scs)}; exe {self.inst.src_exe}, '
                  f'instance {self.inst.name}, out {self.out}')
        self.a, self.pid, _ = clientpoc.start_antprox(self.inst, self.out, env,
                                                      ['-ENABLE_MESSAGES'] if self.messages else [])
        self.ev = Events(files)
        self.note(f'antprox pid {self.a.pid}, client pid {self.pid}' + (' (the retail client)' if self.retail else ''))
        if self.retail and self.pid:
            import threading
            threading.Thread(target=clientpoc.click_agree, args=(self.pid, 60), daemon=True).start()
        self.listener = None
        if self.messages and self.pid:
            self.listener = clientpoc.start_listener(self.pid, self.out, sum(sc['timeout'] for sc in scs) + 10)
        self.hwnd = None
        self.mark = 0               # ev.lines index of the last input step
        self.mark_time = 0.0
        self.logged_in = False      # a scenario's includes ran in this session
        self.gen_before = []        # the generated stand-ins hit before the first scenario's own steps
        self.marker = None          # does the exe export Stub_Log_Mark? (None: not asked yet)
        self.dead = None

    def note(self, s):
        self.elog.write(f'{time.strftime("%H:%M:%S")} {s}\n')
        self.elog.flush()

    def alive(self):
        return self.a.poll() is None and bool(self.pid) and _alive(self.pid)

    def _trigger(self, line, seconds):
        """Writes the proxy's trigger file; True once the proxy took it (deleted it at a Present)."""
        with open(self.trigger + '.tmp', 'w') as f:        # renamed in place: the proxy never reads it half written
            f.write(line + '\n')
        os.replace(self.trigger + '.tmp', self.trigger)
        end = time.time() + seconds
        while time.time() < end and self.alive():
            if not os.path.exists(self.trigger):
                return True
            time.sleep(0.02)
        if os.path.exists(self.trigger):
            os.remove(self.trigger)
            return False
        return True

    def run(self, sc):
        ev, cp = self.ev, self.cp
        note = self.note
        fail, standing, standins = None, [], []
        posted = []                 # this scenario's input steps so far (click ... until ... redo K)
        reached_own = False
        first = not self.logged_in
        start_line = len(ev.lines)
        own_from = start_line
        t_start = ev.now()
        deadline = time.time() + sc['timeout']
        standing.append({'source': 'stubs', 'pattern': re.compile('^CRASH:'), 'line': '(always)', 'from': start_line})
        note(f'scenario {sc["name"]}' + ('' if first else ' (own steps only: the session is in the world)'))
        ev.add('e2e', f'scenario {sc["name"]}')

        def tick():
            ev.poll()
            for s in standing:
                i = find(ev.lines, s['source'], s['pattern'], s.get('from', 0))
                if i is not None:
                    return f'{s["line"]}: expect-no {s["source"]} {s["pattern"].pattern!r} matched: {ev.lines[i][2][:160]}'
            if not self.alive():
                self.dead = 'the client exited'
                return self.dead
            if time.time() > deadline:
                return f'scenario timeout ({sc["timeout"]:.0f} s)'
            if self.hwnd is None and self.pid:
                self.hwnd = cp.find_window(self.pid, self.retail)      # retail: not its terms dialog
                if self.hwnd:
                    note(f'window 0x{self.hwnd:X}')
            return None

        def wait_for(source, pattern, within, what):
            end = time.time() + within
            while True:
                f = tick()
                if f:
                    return f, False
                i = find(ev.lines, source, pattern, self.mark)
                if i is not None:
                    note(f'  matched at {ev.lines[i][0]:.1f} s: {ev.lines[i][2][:200]}')
                    return None, True
                if time.time() > end:
                    return what, False
                time.sleep(0.02)

        def post(op, st, tag=True):
            ev.poll()
            self.mark = len(ev.lines)
            self.mark_time = ev.now()
            if op == 'key':
                cp.post_step(self.hwnd, 'key', str(st['vk']), tag=tag)
            elif op == 'text':
                cp.post_step(self.hwnd, 'text', st['text'], tag=tag)
            else:
                cp.post_step(self.hwnd, op, f'{st["x"]} {st["y"]}', tag=tag)

        if not self.pid:
            fail = self.dead = 'no client process under antprox'
        steps = sc['steps'] if first else sc['steps'][sc['own']:]
        own_at = sc['own'] if first else 0
        for k, st in enumerate(steps):
            if fail:
                break
            if k == own_at:
                reached_own = True
                ev.poll()
                if first and sc['own']:
                    self.gen_before = gen_lines([t for _, s, t in ev.lines if s == 'stubs'])
                    self.logged_in = True
                    note(f'own steps start; {len(self.gen_before)} gen stand-ins hit so far')
                # the stand-ins' first-call flags start over (the exe's Stub_Log_Mark, through the proxy)
                if self.marker is not False and self.hwnd is not None:
                    took = self._trigger('mark ' + sc['name'], 5)
                    ev.poll()
                    if self.marker is None:
                        self.marker = took and find(ev.lines, 'stubs', re.compile(r'^MARK '), start_line) is not None
                        if not self.marker:
                            note('  no stand-in marker (an exe without Stub_Log_Mark, or a proxy without triggers): '
                                 'later scenarios list only the stand-ins no earlier one hit')
                own_from = len(ev.lines)
            note(f'step {st["line"]}: {st["op"]} {_desc(st)}')
            op = st['op']
            if self.retail and st.get('source') == 'stubs' and op in ('wait-event', 'expect-event', 'expect-no', 'stand-in'):
                note('  skipped: the retail client writes no stubs.log (a step of ours only)')
                continue
            if op in ('wait-event', 'expect-event'):
                fail, _ = wait_for(st['source'], st['pattern'], st['within'],
                                   f'{st["line"]}: {op} {st["source"]} {st["pattern"].pattern!r}: not seen within '
                                   f'{st["within"]:.0f} s')
            elif op == 'wait-quiet':
                end = time.time() + st['within']
                while True:
                    fail = tick()
                    if fail:
                        break
                    since = max(ev.last.get(st['source'], 0.0), self.mark_time)
                    if ev.now() - since >= st['quiet']:
                        note(f'  quiet at {ev.now():.1f} s (last {st["source"]} line at {ev.last.get(st["source"], 0.0):.1f} s)')
                        break
                    if time.time() > end:
                        fail = f'{st["line"]}: wait-quiet {st["source"]} {st["quiet"]:g}: not quiet within {st["within"]:.0f} s'
                        break
                    time.sleep(0.02)
            elif op == 'expect-no':
                standing.append(dict(st, **{'from': self.mark}))
            elif op == 'stand-in':
                standins.append(dict(st, **{'from': self.mark}))
            elif op in ACTIONS:
                while self.hwnd is None and not fail:
                    fail = tick()
                    time.sleep(0.02)
                if fail:
                    break
                end = time.time() + 15
                while not _has_tag_hook(ev) and time.time() < end and not fail:
                    fail = tick()
                    time.sleep(0.02)
                if fail:
                    break
                if not _has_tag_hook(ev):
                    fail = ('no tagged virtual input (d3d8shot.log has no "tagged input read" line): rebuild the '
                            'proxy, python tools/harness/run_reference.py build')
                    break
                if op != 'stray' and not st.get('until'):
                    posted.append(st)
                if op == 'stray':                         # a person's input, as the harness's self-test: untagged
                    post('key' if st['what'] == 'key' else 'click', st, tag=False)
                elif st.get('until'):
                    u = st['until']
                    post(op, st)
                    first_mark, end, clicks = self.mark, time.time() + u['within'], 1
                    while True:
                        self.mark = first_mark
                        f, ok = wait_for(u['source'], u['pattern'], u['every'], None)
                        if ok or f:
                            fail = f
                            break
                        if time.time() > end:
                            fail = (f'{st["line"]}: {op} until {u["source"]} {u["pattern"].pattern!r}: not seen within '
                                    f'{u["within"]:.0f} s ({clicks} clicks)')
                            break
                        for again in posted[len(posted) - u['redo']:] if u['redo'] else []:
                            post(again['op'], again)
                            time.sleep(0.2)
                        post(op, st)
                        clicks += 1
                    note(f'  {clicks} click(s)')
                else:
                    post(op, st)
            elif op == 'alive':
                end = time.time() + st['seconds']
                while time.time() < end and not fail:
                    fail = tick()
                    time.sleep(0.05)
            elif op == 'shot':
                fail = tick()
                if not fail:
                    if self._trigger(st['shot'], 5):
                        note(f'  shot {st["shot"]} taken')
                    else:
                        note('  shot not taken in 5 s (a proxy without D3D8SHOT_TRIGGER? run_reference.py build)')
        if not fail:
            fail = tick()
        if not fail:
            for s in standins:
                i = find(ev.lines, 'stubs', s['pattern'], s['from'])
                if i is not None:
                    fail = f'{STANDIN_HIT} {s["line"]}: {ev.lines[i][2][:120]}'
                    break
        ev.poll()
        mine = ev.lines[start_line:]
        stub_text = [t for _, s, t in mine if s == 'stubs']
        gens = gen_lines(stub_text)
        own_text = [t for _, s, t in ev.lines[own_from:] if s == 'stubs']
        own = [g for g in gen_lines(own_text) if g not in self.gen_before]
        poc_own = [t[len('STUB first call: POC '):][:80] for t in own_text
                   if t.startswith('STUB first call: POC ') and 'watchdog' not in t]
        human = human_lines([t for _, s, t in mine if s == 'proxy'])
        crash = [t for t in stub_text if t.startswith('CRASH:')]
        if crash:
            self.dead = self.dead or 'CRASH'
        res = {'name': sc['name'], 'passed': fail is None, 'failure': fail, 'expected_fail': sc['expected_fail'],
               'gen': gens, 'gen_own': own, 'poc_own': poc_own, 'human_dropped': len(human), 'crash': crash[:1],
               'out': self.out, 'seconds': round(ev.now() - t_start, 1),
               'login_failed': fail is not None and first and not reached_own}
        if crash and self.inst.map and os.path.exists(self.inst.map):
            import symbolize
            syms = symbolize.load(self.inst.map)
            a0 = re.findall(r'\b([0-9A-F]{8})\b', crash[0])
            res['crash_at'] = [f'{int(x, 16):08X} {symbolize.name_of(syms, int(x, 16))}' for x in a0[1:2] + a0[3:7]
                               if 0x401000 <= int(x, 16) < 0x1000000]
        note(f'result: {json.dumps(res)}')
        with open(os.path.join(self.out, f'result-{sc["name"]}.json'), 'w', encoding='utf-8') as f:
            json.dump(res, f, indent=1)
        return res

    def close(self):
        if self.pid:
            subprocess.run(['taskkill', '/F', '/PID', str(self.pid)], capture_output=True)
        subprocess.run(['taskkill', '/F', '/T', '/PID', str(self.a.pid)], capture_output=True)
        time.sleep(1)
        if os.path.exists(self.proxy):
            os.remove(self.proxy)
        self.cp.stop_listener(self.listener, self.inst, self.out if self.messages else None)
        self.ev.poll()
        with open(os.path.join(self.out, 'events.log'), 'w', encoding='utf-8', newline='\n') as f:
            for t, s, text in self.ev.lines:
                f.write(f'{t:8.2f} {s:7s} {text}\n')
        if os.path.exists(self.inst.log):
            shutil.copyfile(self.inst.log, os.path.join(self.out, 'stubs.log'))
        for name in sorted(os.listdir(self.out)):
            if name.endswith('.bmp'):
                import bmp2png
                bmp = os.path.join(self.out, name)
                w, h, rows = bmp2png.read_bmp(bmp)
                bmp2png.write_png(bmp[:-4] + '.png', w, h, rows)
                os.remove(bmp)
        self.elog.close()


def crash_place(res):
    """Where a scenario's CRASH: line says the client faulted: the exception code, the faulting instruction and the
    first four return addresses on the stack (not the data address, which moves from run to run), or None."""
    if not res.get('crash'):
        return None
    m = re.match(r'CRASH: code (\w+) at (\w+) \(address \w+\) stack:((?: \w+)*)', res['crash'][0])
    return (m.group(1), m.group(2)) + tuple(m.group(3).split()[:4]) if m else res['crash'][0][:60]


def run_all(scs, args, report):
    """Runs the scenarios in as few client sessions as their results allow (one, when all pass); report(res) per
    scenario as it ends. Returns the results in order."""
    results, pending, login_failures, places, stop = [], list(scs), 0, {}, None
    while pending:
        if login_failures >= 2:             # the login is broken: six more clients would only say so again
            stop = f'two sessions in a row failed before the world ({results[-1]["failure"][:80]})'
        if stop:
            for sc in pending:
                res = {'name': sc['name'], 'passed': False, 'expected_fail': sc['expected_fail'], 'gen': [],
                       'gen_own': [], 'poc_own': [], 'human_dropped': 0, 'crash': [], 'out': results[-1]['out'],
                       'seconds': 0, 'failure': 'not run: ' + stop}
                results.append(res)
                report(res)
            break
        group = pending[:1] if args.separate else list(pending)
        sess = Session(args, group)
        try:
            for sc in group:
                res = sess.run(sc)
                results.append(res)
                pending.remove(sc)
                report(res)
                login_failures = login_failures + 1 if res.get('login_failed') else 0
                place = crash_place(res)
                if place is not None and place in places:
                    stop = (f'{sc["name"]} crashed at the same place as {places[place]} '
                            f'({(res.get("crash_at") or [place[1]])[0]})')
                elif place is not None:
                    places[place] = sc['name']
                if not keeps_session(res) or sess.dead:
                    break                   # the rest get a fresh client: this one's state is not trusted
        finally:
            sess.close()
    return results


def _desc(st):
    if st['op'] in ('wait-event', 'expect-event', 'expect-no'):
        return f'{st["source"]} {st["pattern"].pattern!r}'
    if st['op'] in ('click', 'rclick', 'move'):
        return ((f'gadget {st["gadget"]} ' if st['gadget'] else '') + f'({st["x"]},{st["y"]})'
                + (f' until {st["until"]["source"]} {st["until"]["pattern"].pattern!r}' if st.get('until') else ''))
    if st['op'] == 'wait-quiet':
        return f'{st["source"]} {st["quiet"]:g}'
    return str(st.get('vk', st.get('text', st.get('seconds', st.get('shot', '')))))


def _has_tag_hook(ev):
    return any(s == 'proxy' and 'tagged input read' in t for _, s, t in ev.lines)


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


def sent_packets(run_dir):
    """A run folder's events.log -> {scenario: [packet names the client sent, in order]} ('received NAME 0x..' lines
    of antprox.out between the harness's 'scenario NAME' notes). Keep-alives and time requests are left out: their
    count follows the wall clock, not the scenario."""
    out, cur = {}, None
    skip = ('KEEP_ALIVE', 'KEEPALIVE', 'REQUEST_TIME', 'DEBUG')
    with open(os.path.join(run_dir, 'events.log'), encoding='utf-8', errors='replace') as f:
        for line in f:
            m = re.match(r'\s*[\d.]+ (\S+)\s+(.*)', line)
            if not m:
                continue
            src, text = m.groups()
            if src == 'e2e' and text.startswith('scenario '):
                cur = text.split()[1]
                out.setdefault(cur, [])
            elif src == 'antprox' and cur:
                p = re.search(r'connection #\d+: received (\w+) 0x[0-9a-f]+, (\d+) bytes', text)
                if p and not p.group(1).startswith(skip):
                    out[cur].append(f'{p.group(1)} {p.group(2)}')
    return out


def compare(ours, retail):
    """Prints, per scenario both runs have, whether the client sent the same packets (name and size, in order) and
    the first difference. Exit 1 when any differs."""
    import difflib
    a, b = sent_packets(ours), sent_packets(retail)
    bad = 0
    for name in a:
        if name not in b:
            print(f'-     {name}: not in the retail run')
            continue
        if a[name] == b[name]:
            print(f'SAME  {name}: {len(a[name])} packets sent')
            continue
        bad += 1
        d = [l for l in difflib.unified_diff(b[name], a[name], 'retail', 'ours', lineterm='', n=1)][2:]
        print(f'DIFF  {name}: ours {len(a[name])} packets, retail {len(b[name])}; first differences (- retail, + ours):')
        for l in d[:12]:
            print('        ' + l)
    print(f'compare: {len([n for n in a if n in b]) - bad} of {len([n for n in a if n in b])} scenarios send the same packets')
    return 1 if bad else 0


def keeps_session(res):
    """Whether the next scenario may run in this client: only after a PASS. An XFAIL's stand-in left its goal unmet
    (browser_credits: the pop-up browser is never destroyed), so the world is not as the scenario found it."""
    return verdict(res) == 'PASS'


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
    if res.get('seconds'):
        tail.append(f'{res["seconds"]:.0f} s')
    return f'{v:5s} {res["name"]}: {gen}' + ('; ' + '; '.join(tail) if tail else '')


def default_exe():
    p = os.path.join(ROOT, 'build', 'programs', 'clientpoc', 'clientpoc.exe')
    return p if os.path.exists(p) else None


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('command', choices=['run', 'list', 'check', 'compare'])
    ap.add_argument('names', nargs='*')
    ap.add_argument('--instance', default='e2e', help='clientpoc.py instance name (default e2e)')
    ap.add_argument('--exe', help='the POC exe (default: this tree\'s build/programs/clientpoc/clientpoc.exe, '
                                  'else ClientPOC\\release\'s)')
    ap.add_argument('--messages', action='store_true', help='-ENABLE_MESSAGES and msglisten (source msg)')
    ap.add_argument('--keep', action='store_true', help='keep the instance (default: clientpoc.py clean it at the end)')
    ap.add_argument('--separate', action='store_true', help='a client of its own for every scenario (default: one session)')
    ap.add_argument('--retail', action='store_true', help='run a copy of the retail client (needs --instance; D190)')
    args = ap.parse_args(argv)
    if args.command == 'compare':
        if len(args.names) != 2:
            print('e2e: compare DIR_OURS DIR_RETAIL (two run folders under .work/e2e)')
            return 2
        return compare(args.names[0], args.names[1])
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
    if args.retail and (args.exe or args.instance == 'e2e'):
        print('e2e: --retail needs --instance NAME of its own and no --exe')
        return 2
    if args.exe is None and not args.retail:
        args.exe = default_exe()
    os.makedirs(OUT, exist_ok=True)
    sys.path.insert(0, HERE)
    t0 = time.time()
    results = run_all(scs, args, lambda res: print(summary_line(res), flush=True))
    bad = sum(verdict(res) in ('FAIL', 'XPASS') for res in results)
    print(f'e2e: {len(scs) - bad} of {len(scs)} as expected in {time.time() - t0:.0f} s; logs under {OUT}')
    if not args.keep:
        import clientpoc
        clientpoc.clean(args.instance)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
