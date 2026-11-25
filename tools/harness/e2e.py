r"""End-to-end scenarios for client builds (D237): an input script plus log-event assertions.

    python tools/harness/e2e.py run [NAME...] [--instance ID] [--exe PATH] [--messages] [--keep] [--separate]
    python tools/harness/e2e.py run --retail [NAME...] --instance ID    the same scenarios against the retail client
    python tools/harness/e2e.py --live --db-snapshot run sb_...    live, the character's rows put back afterwards
    python tools/harness/e2e.py compare OURS RETAIL    two runs (folder, folders joined by ',', or a run --manifest
                                                       FILE): per scenario verdicts, packets and shots, SAME / DIFF
    python tools/harness/e2e.py retail [NAME...] [--exe PATH] [--instance BASE] [--retail-run MANIFEST] [--detail]
                                                       ours and retail at once, then compare: one line a scenario
    python tools/harness/e2e.py list                       the scenarios, in order, with their expected-fail notes
    python tools/harness/e2e.py check [NAME...]            parse only (no client)

Each scenario is tools/harness/e2e/NAME.scn. A run is ONE client session for all its scenarios: antprox /REPLAY
starts the client (tools/harness/clientrun.py's instance and antprox code; ours is the tree's one client build,
build/programs/client/client.exe, unless --exe), the first scenario's steps run from
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
instance is removed at the end (clientrun.py clean) unless --keep.

The format (one step a line, '#' comments, shell-style quoting):
    name: TEXT / expected-fail: REASON / timeout: SECONDS     headers (before the steps)
    include NAME                  NAME.scn's steps here (its headers are not taken)
    wait-event SOURCE PATTERN [within S]   wait until a line of SOURCE matches the regex PATTERN (default 60 s)
    expect-event SOURCE PATTERN [within S] the same; reads as an assertion (default 15 s)
    wait-quiet SOURCE S [within T]  wait until SOURCE has had no new line for S seconds, counted from the later of
                                  its last line and the last input step (default T 30 s): "the loading this step
                                  started is over" where the client logs no "ready" line (the proxy's file opens)
    expect-no SOURCE PATTERN      from here to the scenario's end no line of SOURCE may match (checked continuously)
    expect-no CRASH               short for: expect-no any '^CRASH:' (the d3d8shot proxy's first-chance CRASH: line in
                                  d3d8shot.log, or an older stand-in exe's in stubs.log; every scenario has it from
                                  the start)
    click gadget NAME | click X,Y   rclick ... | move ...    mouse at a gadget of e2e/gadgets.txt or client pixels
    down X,Y / up X,Y             the left button pressed at a point / moved there held and let go: a drag (the
                                  client's item windows move an item on the button-up, e.g. into a terminal's slot)
    click X,Y until SOURCE PATTERN [redo K] [every S] [within T]   the click (or key KEYS) again every S seconds (default 0.5)
                                  until the line is seen (default T 60 s): for input the client ignores until it is
                                  ready. redo K: each repeat first posts the K input steps before this one again
                                  (select a slot, then Enter: both are ignored while the screen's opening plays)
    key KEYS                      a key: a VK (decimal or 0x hex), a name (enter, esc, tab, space, backspace, up,
                                  f1..f24, num0..num9, ` and control_keys.ini's other punctuation) or a letter; or a
                                  chord, KEY+KEY...: shift+i, ctrl+i, alt+f1 (each key down in order, up in reverse)
    keydown KEYS / keyup KEYS     those keys only pressed / only let go: held across the steps between (keydown
                                  shift, key i, keyup shift); a scenario lets go of every key it holds (checked
                                  when it is parsed; one that fails between is let go at its end)
                                  The modifiers go as a keyboard sends them (test-plan C6): Shift's, Ctrl's, Alt's
                                  own key-down first, and Alt without Ctrl as WM_SYSKEYDOWN / WM_SYSKEYUP with
                                  lParam's bit 29; the client reads them back through GetKeyState / GetAsyncKeyState
                                  (0x4307A0, 0x430CA0, 0x4314C0), which the proxy answers from the keys it passed on;
                                  d3d8shot.log: 'input: replayed key 0x49 +shift'. A lone Alt and Alt+Space are
                                  refused (the window menu's keyboard mode, the system menu: D69)
    text STRING                   characters
    stand-in PATTERN              the goal is not met while stubs.log shows PATTERN from here on (a stand-in's first
                                  call): checked at the end, so the other steps still run; with `expected-fail` the
                                  scenario is XFAIL, and XPASS once the stand-in is retired
    stray click X,Y|gadget NAME / stray key KEYS the same input UNtagged, as a person's would be (the tag's
                                  self-test: the proxy must drop it and log 'NOT REPLAYED')
    alive S                       S seconds pass with the client running and no expect-no line matched
    walk-to X,Y [tol T] [within S]  a starbase: the avatar walks to X,Y (the room's floor plane, units of the
                                  client's camera) in a closed loop on the camera the proxy reports each frame
                                  (D3D8SHOT_CAMERA): turn towards the point (right button drag), hold the walk key,
                                  correct on the way, stop within T (default 2); fails when the avatar does not
                                  move for 2 s (a wall: give a waypoint) or after S (default 30 s)
    face YAW [tol D] [within S]   turn until the camera looks along YAW degrees (default tolerance 2): what a
                                  click on something in the room needs before it (the click is a screen point)
    hold KEY until SOURCE PATTERN [within S]  the key held down until the line is seen (walk on through a door
                                  until the room changes)
    hold KEY for S                the key held down S seconds (flight: thrust, the arrow keys' turn and tilt)
    rdrag X,Y for S               the right button pressed at the window's middle, the pointer held at X,Y for S
                                  seconds, let go back at the middle (flight: the ship's nose follows the pointer)
    pose-mark                     the pose now (camera at rest, at most 3 s) is what expect-pose measures from
    expect-pose KIND MIN          since the pose-mark, the pose went MIN in the KIND direction: forward / back
                                  (units along the marked heading), left / right (yaw degrees), up / down (pitch
                                  degrees), moved (units, any direction); the best of the poses sampled during the
                                  hold / rdrag steps since the mark and the pose now, so a chase camera's lag or an
                                  auto-level after the input does not hide it
    pose                          the avatar's place and heading, written to e2e.log (for writing walk-to steps)
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

--retail (D190, D248): the scenarios drive a COPY of the retail client (clientrun.py's Inst(retail=True): the copy
in the instance folder, never the install), through the same proxy and the same tagged input; the terms box's
"I Agree" is clicked for it. No client writes stubs.log now (the one client build has no stand-in; older stand-in exes did), so
a step on the `stubs` source is skipped on retail (noted in e2e.log) and never seen on ours. A crash on either shows
as the d3d8shot proxy's first-chance `CRASH:` line in d3d8shot.log (source `proxy`; a fault the client handles
itself shows too) and 'the client exited'. `compare` then reads two runs' events.log (ours, retail's) and prints, per scenario, the
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
import math
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
# the session root: --out DIR, else ENB_E2E_OUT, else .work/e2e (baseline.py capture points it into the store's
# .staging); every session folder is named STAMP-NAME, STAMP the local time as %Y%m%d-%H%M%S
OUT = os.environ.get('ENB_E2E_OUT') or os.path.join(ROOT, '.work', 'e2e')
STAMP = '%Y%m%d-%H%M%S'
ORDER = ['world_entry', 'starmap', 'options', 'pda_pages', 'cargo', 'chat']
# Scenarios that must run after everything else (one that leaves the world changed where no following scenario may
# use it: it then costs no extra client start). None today.
LAST = []
STANDIN_HIT = 'stand-in hit:'
SOURCES = ('antprox', 'stubs', 'msg', 'proxy', 'files', 'any')
ACTIONS = ('click', 'rclick', 'move', 'down', 'up', 'key', 'keydown', 'keyup', 'text', 'stray')
WALK_KEY = 0x41         # 'A': the starbase's walk key (the user's recording of 2026-11-15)


def _wrap(deg):
    return (deg + 180.0) % 360.0 - 180.0


def _key_lparam(vk, up=False):
    import clientrun
    return clientrun.key_lparam(vk, up)


class Pilot:
    """The avatar's pose in a starbase from the proxy's camera file (d3d8shot.c D3D8SHOT_CAMERA: the frame's first
    view matrix is the room's camera, which trails the avatar), and the closed loops that steer by it. Measured on
    the retail client 2026-11-15, in the open hangar: the walk key 'A' and a held right button both walk about
    26 units a second along the camera's heading (0.15 s to start, about 4.5 units of coast after); the right
    button also steers, at a RATE set by the cursor's offset from where it went down (about 0.43 degrees a
    second per pixel). Nothing turns the avatar in place (the arrow keys do nothing; 'Z' walks back), so the
    loops steer while walking and watch the pose rather than count pixels or seconds."""
    TRAIL = 6.4         # the camera's distance behind the avatar (two camera places at one avatar place)
    RATE = 0.43         # degrees a second per pixel of drag offset
    REACH = 560         # the largest offset used (pixels from the window's middle)
    COAST = 4.5         # units the avatar still walks after the button or key is let go
    CX, CY = 640, 512
    REPEAT = 0.05       # hold VK for S: a key-down every 50 ms

    def __init__(self, hwnd, camera, tick, note, held=None):
        import ctypes
        self.user32 = ctypes.windll.user32
        self.hwnd, self.camera, self.tick, self.note = hwnd, camera, tick, note
        self.held = set() if held is None else held     # the session's keys held down (keydown steps)

    def _post(self, msg, w, l):
        self.user32.PostMessageW(self.hwnd, msg, w | (0x5043 << 16), l)

    def _mouse(self, msg, w, x, y):
        self._post(msg, w, (int(y) << 16) | (int(x) & 0xFFFF))

    def pose(self, tries=50):
        """{'x', 'y', 'z', 'yaw', 'pitch', 'frame'} of the avatar (x, y: the camera's place moved TRAIL along its
        heading), or None when the proxy wrote no camera file."""
        for _ in range(tries):
            try:
                with open(self.camera) as f:
                    m = re.match(r'frame (\d+) view 0 of \d+:(.*)', f.readline())
                v = [float(t) for t in m.group(2).split()]
                if len(v) == 16:
                    break
            except (OSError, AttributeError, ValueError):
                pass
            time.sleep(0.02)
        else:
            return None
        r = (v[0:3], v[4:7], v[8:11])
        pos = [-(v[12] * r[i][0] + v[13] * r[i][1] + v[14] * r[i][2]) for i in range(3)]
        look = (-v[2], -v[6], -v[10])
        yaw = math.degrees(math.atan2(look[1], look[0]))
        return {'frame': int(m.group(1)), 'yaw': yaw, 'pitch': math.degrees(math.asin(max(-1.0, min(1.0, look[2])))),
                'x': pos[0] + self.TRAIL * math.cos(math.radians(yaw)),
                'y': pos[1] + self.TRAIL * math.sin(math.radians(yaw)), 'z': pos[2]}

    def say(self, p):
        return f'{p["x"]:.1f},{p["y"]:.1f} yaw {p["yaw"]:.1f} pitch {p["pitch"]:.1f}'

    def still(self, within=20.0):
        """Waits until the camera has not moved for 0.6 s (a room's entry move is over: the client ignores the
        turn drag during it); the pose, or None without a camera file."""
        end = time.time() + within
        last, since = None, time.time()
        while True:
            p = self.pose()
            if p is None:
                return None
            if last is None or abs(p['x'] - last['x']) + abs(p['y'] - last['y']) + abs(_wrap(p['yaw'] - last['yaw'])) > 0.05:
                last, since = p, time.time()
            if time.time() - since > 0.6 or time.time() > end or self.tick():
                return p
            time.sleep(0.05)

    def _rdown(self):
        self._mouse(0x200, 0, self.CX, self.CY)
        time.sleep(0.03)
        self._mouse(0x204, 2, self.CX, self.CY)

    def _steer(self, dx):
        self._mouse(0x200, 2, self.CX + max(-self.REACH, min(self.REACH, dx)), self.CY)

    def _rup(self):
        self._mouse(0x200, 2, self.CX, self.CY)
        time.sleep(0.03)
        self._mouse(0x205, 0, self.CX, self.CY)

    def face(self, yaw, tol=2.0, within=15.0):
        """Turns to yaw in short right-button pulses at the full offset: each walks a little (the walk's first
        0.15 s covers about half a unit) and turns a lot."""
        end = time.time() + within
        while True:
            p = self.still(3.0)
            if p is None:
                return 'no camera file from the proxy (run_reference.py build)'
            err = _wrap(yaw - p['yaw'])
            if abs(err) < tol:
                return None
            fail = self.tick()
            if not fail and time.time() > end:
                fail = f'face {yaw:.1f}: at {self.say(p)} after {within:g} s'
            if fail:
                return fail
            self._rdown()
            self._steer(-math.copysign(self.REACH, err))        # a drag to the right turns the heading down
            time.sleep(max(0.04, min(0.16, abs(err) / (self.RATE * self.REACH))))
            self._rup()

    def key(self, vk, down):
        """vk down (again: each a fresh down, no repeat bit; the client's 0x4307A0 drops a repeat outside text entry)
        or up, with the modifiers the session holds (an Alt held makes it WM_SYSKEYDOWN, as a keyboard)."""
        import clientrun
        msg, lp = clientrun.key_message(vk, not down, self.held - {vk})
        self._post(msg, vk, lp)

    def hold_for(self, vk, secs, sample):
        """The key held secs seconds: a down every REPEAT seconds, as the keyboard's auto-repeat, a fixed count of
        them (the client sends a MOVE per down of the thrust keys, so a run's packets compare with another's), the
        pose sampled between them."""
        t0 = time.time()
        try:
            for i in range(max(1, round(secs / self.REPEAT))):
                while time.time() < t0 + i * self.REPEAT:
                    sample(self.pose(5))
                    time.sleep(0.01)
                self.key(vk, True)
                fail = self.tick()
                if fail:
                    return fail
            while time.time() < t0 + secs:
                sample(self.pose(5))
                time.sleep(0.01)
        finally:
            self.key(vk, False)
        return None

    def rdrag(self, x, y, secs, sample):
        """The right button down at the window's middle, the pointer held at x,y for secs seconds (the move posted
        again every frame or so, with MK_RBUTTON), then back to the middle and let go; the pose sampled meanwhile."""
        self._rdown()
        end = time.time() + secs
        try:
            while time.time() < end:
                self._mouse(0x200, 2, x, y)
                sample(self.pose(5))
                fail = self.tick()
                if fail:
                    return fail
                time.sleep(0.03)
        finally:
            self._rup()
        return None

    @staticmethod
    def change(mark, poses, kind):
        """The best change in KIND's direction from mark over poses (see expect-pose), with the pose it came from."""
        ry, rp = math.radians(mark['yaw']), math.radians(mark['pitch'])
        look = (math.cos(rp) * math.cos(ry), math.cos(rp) * math.sin(ry), math.sin(rp))
        best = None
        for p in poses:
            d = (p['x'] - mark['x'], p['y'] - mark['y'], p['z'] - mark['z'])
            v = {'forward': sum(a * b for a, b in zip(d, look)), 'moved': math.sqrt(sum(a * a for a in d)),
                 'left': _wrap(p['yaw'] - mark['yaw']), 'up': p['pitch'] - mark['pitch']}
            v['back'], v['right'], v['down'] = -v['forward'], -v['left'], -v['up']
            if best is None or v[kind] > best[0]:
                best = (v[kind], p)
        return best

    def walk_to(self, x, y, tol=2.0, within=30.0):
        """Walks with the right button held, steering by the cursor's offset; lets go a coast's length before the
        point and closes the rest in short pulses of the walk key."""
        end = time.time() + within
        holding, pulses = False, 0
        moved_at, moved_from = time.time(), None
        fail = None
        self.still()
        try:
            while True:
                p = self.pose()
                if p is None:
                    return 'no camera file from the proxy (run_reference.py build)'
                d = math.hypot(x - p['x'], y - p['y'])
                err = _wrap(math.degrees(math.atan2(y - p['y'], x - p['x'])) - p['yaw'])
                if holding:
                    if d < self.COAST or (abs(err) > 60 and d < 16):     # arriving, or the point is inside the turn
                        self._rup()
                        holding = False
                        self.still(3.0)
                        continue
                    self._steer(-err * 8.0)
                    if moved_from is None or math.hypot(p['x'] - moved_from[0], p['y'] - moved_from[1]) > 0.5:
                        moved_at, moved_from = time.time(), (p['x'], p['y'])
                    elif time.time() - moved_at > 2.0:
                        fail = f'walk-to {x:g},{y:g}: not moving at {self.say(p)} (a wall? give a waypoint)'
                    time.sleep(0.03)
                elif d < tol:
                    self.note(f'  at {self.say(p)}')
                    return None
                elif abs(err) > (8 if d > 6 else 20):
                    fail = self.face(p['yaw'] + err, 4.0, max(1.0, end - time.time()))
                elif d >= self.COAST + 2:
                    self._rdown()
                    holding = True
                    moved_at, moved_from = time.time(), None
                else:
                    pulses += 1
                    if pulses > 12:
                        fail = f'walk-to {x:g},{y:g}: {d:.1f} away at {self.say(p)} after 12 pulses'
                    else:
                        t_end = time.time() + max(0.1, min(0.3, 0.1 + d / 40.0))
                        while time.time() < t_end:
                            self.key(WALK_KEY, True)
                            time.sleep(0.03)
                        self.key(WALK_KEY, False)
                        self.still(3.0)
                fail = fail or self.tick()
                if not fail and time.time() > end:
                    fail = f'walk-to {x:g},{y:g}: at {self.say(p)} after {within:g} s'
                if fail:
                    return fail
        finally:
            if holding:
                self._rup()


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
                raise ScenarioError(f'{where}: stand-in PATTERN (a stubs.log line, e.g. the first call of a stand-in)')
            st.update(source='stubs', pattern=_regex(args[0], where))
        elif op == 'expect-no':
            if args == ['CRASH']:
                args = ['any', '^CRASH:']
            if len(args) != 2 or args[0] not in SOURCES:
                raise ScenarioError(f'{where}: expect-no SOURCE PATTERN | expect-no CRASH')
            st.update(source=_source(args[0]), pattern=_regex(args[1], where))
        elif op in ('click', 'rclick', 'move', 'down', 'up'):
            args, until = _until(op, args, where)
            x, y, g = _point(args, gadgets, where)
            st.update(x=x, y=y, gadget=g, until=until)
        elif op == 'stray':
            if not args or args[0] not in ('click', 'key'):
                raise ScenarioError(f'{where}: stray click X,Y|gadget NAME, or stray key KEYS')
            if args[0] == 'click':
                x, y, g = _point(args[1:], gadgets, where)
                st.update(x=x, y=y, gadget=g, what='click')
            else:
                if len(args) != 2:
                    raise ScenarioError(f'{where}: stray key KEYS')
                st.update(_keys(args[1], where), what='key')
        elif op == 'key':
            args, st['until'] = _until(op, args, where)
            if len(args) != 1:
                raise ScenarioError(f'{where}: key KEYS [until SOURCE PATTERN ...]')
            st.update(_keys(args[0], where))
        elif op in ('keydown', 'keyup'):
            if len(args) != 1:
                raise ScenarioError(f'{where}: {op} KEYS (e.g. {op} shift)')
            st.update(_keys(args[0], where))
        elif op == 'text':
            if len(args) != 1:
                raise ScenarioError(f'{where}: text STRING (quote it when it has spaces)')
            st['text'] = args[0]
        elif op == 'alive':
            if len(args) != 1:
                raise ScenarioError(f'{where}: alive SECONDS')
            st['seconds'] = float(args[0])
        elif op in ('walk-to', 'face'):
            args, st['within'] = _within(args, 30.0 if op == 'walk-to' else 15.0, where)
            st['tol'] = 2.0
            if len(args) == 3 and args[1] == 'tol':
                st['tol'], args = float(args[2]), args[:1]
            try:
                if len(args) != 1:
                    raise ValueError
                st['to'] = [float(v) for v in args[0].split(',')]
                if len(st['to']) != (2 if op == 'walk-to' else 1):
                    raise ValueError
            except ValueError:
                raise ScenarioError(f'{where}: walk-to X,Y [tol T] [within S] / face YAW [tol D] [within S]')
        elif op == 'hold' and len(args) == 3 and args[1] == 'for':
            try:
                st['vk'], st['secs'] = _one_key(args[0], where), float(args[2])
            except ValueError:
                raise ScenarioError(f'{where}: hold VK for SECONDS')
        elif op == 'hold':
            args, st['within'] = _within(args, 30.0, where)
            if len(args) != 4 or args[1] != 'until':
                raise ScenarioError(f'{where}: hold VK until SOURCE PATTERN [within S] / hold VK for SECONDS')
            st['vk'] = _one_key(args[0], where)
            st['source'], st['pattern'] = _source(args[2]), _regex(args[3], where)
        elif op == 'rdrag':
            if len(args) < 3 or args[-2] != 'for':
                raise ScenarioError(f'{where}: rdrag X,Y|gadget NAME for SECONDS')
            x, y, g = _point(args[:-2], gadgets, where)
            try:
                st.update(x=x, y=y, gadget=g, secs=float(args[-1]))
            except ValueError:
                raise ScenarioError(f'{where}: rdrag X,Y|gadget NAME for SECONDS')
        elif op == 'pose-mark':
            if args:
                raise ScenarioError(f'{where}: pose-mark takes nothing')
        elif op == 'expect-pose':
            kinds = ('forward', 'back', 'left', 'right', 'up', 'down', 'moved')
            try:
                if len(args) != 2 or args[0] not in kinds:
                    raise ValueError
                st.update(kind=args[0], min=float(args[1]))
            except ValueError:
                raise ScenarioError(f'{where}: expect-pose {"|".join(kinds)} MIN')
        elif op == 'pose':
            if args:
                raise ScenarioError(f'{where}: pose takes nothing')
        elif op == 'shot':
            if len(args) != 1 or not re.fullmatch(r'[A-Za-z0-9_-]{1,40}', args[0]):
                raise ScenarioError(f'{where}: shot NAME (letters, digits, - and _)')
            st['shot'] = args[0]
        elif op == 'expect-ink':
            try:
                if len(args) != 3 or not re.fullmatch(r'[A-Za-z0-9_-]{1,40}', args[0]):
                    raise ValueError
                box = [int(v) for v in args[1].split(',')]
                if len(box) != 4 or box[2] <= 0 or box[3] <= 0:
                    raise ValueError
                st.update(shot=args[0], box=box, min=int(args[2]))
            except ValueError:
                raise ScenarioError(f'{where}: expect-ink SHOT X,Y,W,H MIN (MIN bright pixels in the box of the '
                                    f'shot taken last under that name)')
        else:
            raise ScenarioError(f'{where}: unknown step {op!r}')
        sc['steps'].append(st)
    if not sc['steps']:
        raise ScenarioError(f'{name}: no steps')
    _check_held(sc['steps'], name)
    return sc


def _keys(spec, where):
    """A key step's KEYS (clientrun.parse_keys: enter, f1, a letter, a VK number, or a chord such as shift+i) ->
    {'keys': [VK...], 'vk': the last one, 'spec': as written back}."""
    import clientrun
    try:
        vks = clientrun.parse_keys(spec)
    except ValueError as e:
        raise ScenarioError(f'{where}: {e}')
    return {'keys': vks, 'vk': vks[-1], 'spec': '+'.join(clientrun.key_name(v) for v in vks)}


def _one_key(spec, where):
    k = _keys(spec, where)['keys']
    if len(k) != 1:
        raise ScenarioError(f'{where}: one key here, not a chord')
    return k[0]


def _check_held(steps, name):
    """Held keys (keydown / keyup) must pair up inside a scenario: a key let go that is not down, a key pressed
    again while down (that is `hold`'s auto-repeat), or a key still down at the end are errors, so no scenario
    leaves the next one of the session a stuck modifier. Alt pressed and let go with no other key between, or Alt
    with Space, are refused: the window procedure turns a lone Alt into the window menu's keyboard mode and
    Alt+Space opens the system menu, a modal menu loop (D69)."""
    held, alt_alone = [], False
    for st in steps:
        op, keys, where = st['op'], st.get('keys'), st['line']
        if op == 'stray' and st.get('what') == 'key':
            op = 'key'
        if op not in ('key', 'keydown', 'keyup', 'hold'):
            continue
        if op == 'hold':
            keys = [st['vk']]
        if (0x12 in held or 0x12 in keys) and 0x20 in keys:
            raise ScenarioError(f'{where}: Alt+Space opens the system menu (a modal menu loop): not allowed')
        if op == 'keyup':
            for vk in keys:
                if vk not in held:
                    raise ScenarioError(f'{where}: keyup 0x{vk:02X}: that key is not down')
                held.remove(vk)
                if vk == 0x12 and alt_alone:
                    raise ScenarioError(f'{where}: Alt pressed and let go alone (the window menu\'s keyboard mode)')
            continue
        for vk in keys:
            if vk in held:
                raise ScenarioError(f'{where}: 0x{vk:02X} is held down already (keyup it first)')
        if op == 'key' and keys[-1] == 0x12:
            raise ScenarioError(f'{where}: Alt pressed and let go with no key after it (the window menu\'s '
                                f'keyboard mode): Alt goes first in a chord, alt+KEY')
        if op == 'keydown':
            held.extend(keys)
        alt_alone = op == 'keydown' and keys[-1] == 0x12
    if held:
        raise ScenarioError(f'{name}: ' + ', '.join(f'0x{vk:02X}' for vk in held)
                            + ' still held at the end (keyup before the scenario ends)')


def _until(op, args, where):
    """Splits `... until SOURCE PATTERN [redo K] [every S] [within T]` off an input step's words (click, key)."""
    if 'until' not in args:
        return args, None
    k = args.index('until')
    rest, within = _within(args[k + 1:], 60.0, where)
    rest, every = _within(rest, 0.5, where, 'every')
    rest, redo = _within(rest, 0, where, 'redo')
    if len(rest) != 2 or rest[0] not in SOURCES:
        raise ScenarioError(f'{where}: {op} ... until SOURCE PATTERN [every S] [within S]')
    return args[:k], {'source': _source(rest[0]), 'pattern': _regex(rest[1], where), 'every': every,
                      'within': within, 'redo': int(redo)}


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


def scenario_names(live=False):
    """The default set: under /REPLAY every scenario but the live ones; with --live the station ones (sb_*).
    The space ones (sp_*: live, another character in space, avatar 7) run only when named."""
    have = sorted(f[:-4] for f in os.listdir(SCN) if f.endswith('.scn') and not f.startswith(('_', 'sp_'))
                  and f.startswith('sb_') == bool(live))
    return ([n for n in ORDER if n in have] + [n for n in have if n not in ORDER and n not in LAST]
            + [n for n in LAST if n in have])


# ---------------------------------------------------------------- the event stream and the matcher

_MSG_TS_RE = re.compile(r'^(?:\d\d:\d\d:\d\d\.\d+|\d+\.\d+)\s+')
_ASSERT_RE = re.compile(r'^(?:ERROR\s+(?:\S*,)?)?(?:G_ASSERT|FAIL_IF):\s')
_POLL_CHUNK = 1 << 20
_ASSERT_CAP = 2000
# d3d8shot.log's lines for the stub log's own opens (stubs/stub_log.cpp opens it once a line): not the game's
# loading, so they are not 'proxy' lines. Our client tees every assert there and retail has no stub log: in space
# effect_description.cpp asserts hundreds of times a second on both clients (the data), and these opens kept
# 'wait-quiet proxy' from ever seeing quiet on ours (sp_combat, 2026-11-21; README, Quirks)
_OWN_OPEN = re.compile(r'^CreateFileA\("[^"]*stubs\.log"', re.I)


def _assert_key(src, text):
    """For repeated G_ASSERT / FAIL_IF lines on msg, stubs or antprox (stderr tee), source + message without timestamp."""
    if src not in ('msg', 'stubs', 'antprox'):
        return None
    body = _MSG_TS_RE.sub('', text, count=1) if src == 'msg' else text
    return (src, body) if _ASSERT_RE.match(body) else None


class _Watch:
    """Active pattern watch registered on Events so long runs never drop an expected/forbidden match."""
    def __init__(self, source, pattern, hit=None):
        self.source = source
        self.pattern = pattern
        self.hit = hit


_LIVE_OP = re.compile(r'(Sending \[0x([0-9a-fA-F]{4})\] via UDP|<SERVER TO CLIENT UDP> -+> ([0-9a-fA-F]{4}) )')
_OPCODES = None


def opcode_names():
    """opcode -> name from docs/data/packet-factory.txt (Net-7's table; the proxy's own 0x2000+ / 0x3000+ are not
    in it and stay numbers)."""
    global _OPCODES
    if _OPCODES is None:
        _OPCODES = {}
        try:
            with open(os.path.join(ROOT, 'docs', 'data', 'packet-factory.txt'), encoding='utf-8') as f:
                for line in f:
                    m = re.match(r'\s*\d+ 0x([0-9A-F]{4})\s+(\w+)', line)
                    if m:
                        _OPCODES[int(m.group(1), 16)] = m.group(2)
        except OSError:
            pass
    return _OPCODES


def name_live(line):
    """A live antprox (/OPCODES) logs opcodes as numbers: 'Sending [0x0055] via UDP' (the client's, passed on to the
    server) and '<SERVER TO CLIENT UDP> ----> 0054 [003c]'. The line gets the replay server's words appended,
    'received SELECT_TALK_TREE' / 'sent TALK_TREE', so one scenario step matches under /REPLAY and --live."""
    m = _LIVE_OP.search(line)
    if not m:
        return line
    op = int(m.group(2) or m.group(3), 16)
    return f'{line.rstrip()} = {"received" if m.group(2) else "sent"} {opcode_names().get(op, "OP_%04X" % op)}'


class Events:
    """Lines of several growing files, in the order they were read, each (seconds since start, source, text)."""
    def __init__(self, files, clock=time.time, log=None, window=4096):
        self.files = dict(files)                # source -> path
        self.pos = {s: 0 for s in self.files}
        self.part = {s: '' for s in self.files}
        self.lines = []                         # bounded sliding window of the last `window` stored tuples
        self.base = 0                           # global index of self.lines[0]
        self.total = 0                          # total stored lines seen (base + len(self.lines))
        self.window = window
        self.clock = clock
        self.t0 = clock()
        self.last = {}                          # source -> the time its newest line was read
        self.tag_hook = False
        self._log = open(log, 'w', encoding='utf-8', newline='\n') if log else None
        self._log_dirty = False
        self._asserts = {}                      # (src, body) -> repeat tracking
        self._assert_epoch = 0
        self._watches = []
        self._listeners = []

    def _write_log(self, t, src, text):
        if self._log is not None:
            self._log.write(f'{t:8.2f} {src:7s} {text}\n')
            self._log_dirty = True

    def _flush_log(self):
        if self._log is not None and self._log_dirty:
            self._log.flush()
            self._log_dirty = False

    def _flush_entry(self, src, entry):
        if entry['repeats'] > 0:
            self._write_log(entry['last_t'], src, f'[repeated {entry["repeats"]}x] {entry["sample"]}')
            entry['repeats'] = 0

    def _emit(self, now, src, text):
        key = _assert_key(src, text)
        if key is not None:
            entry = self._asserts.get(key)
            if entry is not None and entry['last_seq'] >= self._assert_epoch:
                entry['repeats'] += 1
                entry['total'] += 1
                entry['last_t'] = now
                entry['sample'] = text
                return
            if entry is not None:
                self._flush_entry(src, entry)
                entry['last_seq'] = self.total
                entry['total'] += 1
                entry['last_t'] = now
                entry['sample'] = text
            elif len(self._asserts) < _ASSERT_CAP:
                self._asserts[key] = {'last_seq': self.total, 'repeats': 0, 'total': 1,
                                      'last_t': now, 'sample': text}
        item = (now, src, text)
        self.lines.append(item)
        self.total += 1
        if self.window is not None and len(self.lines) > self.window:
            drop = len(self.lines) - self.window
            del self.lines[:drop]
            self.base += drop
        self._write_log(now, src, text)
        if src == 'proxy' and 'tagged input read' in text:
            self.tag_hook = True
        for w in self._watches:
            if w.hit is None and (w.source == 'any' or w.source == src) and w.pattern.search(text):
                w.hit = item
        for fn in self._listeners:
            fn(now, src, text)

    def poll(self):
        for src, path in self.files.items():
            try:
                with open(path, 'rb') as f:
                    f.seek(self.pos[src])
                    while True:
                        data = f.read(_POLL_CHUNK)
                        if not data:
                            break
                        self.pos[src] += len(data)
                        text = self.part[src] + data.decode('latin1')
                        *done, self.part[src] = text.split('\n')
                        now = self.clock() - self.t0
                        kept = False
                        for l in done:
                            l = l.rstrip('\r')
                            if src == 'proxy':      # d3d8shot.log: '%10lu ' (GetTickCount) before each line
                                l = re.sub(r'^\s*\d+ ', '', l, count=1)
                                if _OWN_OPEN.match(l):
                                    continue
                            elif src == 'antprox':
                                l = name_live(l)
                            self._emit(now, src, l)
                            kept = True
                        if kept:
                            self.last[src] = now
                        if len(data) < _POLL_CHUNK:
                            break
            except OSError:
                continue
        self._flush_log()
        return self.total

    def now(self):
        return self.clock() - self.t0

    def add(self, src, text):           # tests, and the harness's own notes
        self._emit(self.clock() - self.t0, src, text)
        self._flush_log()

    def find(self, source, pattern, start=0):
        rel = max(0, start - self.base)
        i = find(self.lines, source, pattern, rel)
        return self.lines[i] if i is not None else None

    def watch(self, source, pattern, start=0):
        w = _Watch(source, pattern, self.find(source, pattern, start))
        self._watches.append(w)
        return w

    def unwatch(self, w):
        if w in self._watches:
            self._watches.remove(w)

    def add_listener(self, fn):
        self._listeners.append(fn)

    def remove_listener(self, fn):
        if fn in self._listeners:
            self._listeners.remove(fn)

    def mark_epoch(self):
        self._assert_epoch = self.total

    def flush_repeats(self):
        for (src, _), entry in self._asserts.items():
            self._flush_entry(src, entry)
        self._flush_log()

    def close(self):
        self.flush_repeats()
        if self._log is not None:
            self._log.close()
            self._log = None


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
        import clientrun
        self.cp = clientrun
        self.args = args
        tag = scs[0]['name'] if len(scs) == 1 else 'session'
        # Two runs started in the same second (another instance) must not share a folder: their antprox.out and
        # e2e.log would be one file, and each run would wait on the other's lines.
        base = os.path.join(OUT, time.strftime(STAMP) + '-' + tag)
        self.out, n = base, 1
        while True:
            try:
                os.makedirs(self.out)
                break
            except FileExistsError:
                n += 1
                self.out = f'{base}-{n}'
        self.elog = open(os.path.join(self.out, 'e2e.log'), 'w', encoding='utf-8', newline='\n')
        self.retail = bool(getattr(args, 'retail', False))
        self.inst = clientrun.Inst(args.instance, None if self.retail else args.exe, self.retail)
        self.inst.prepare()
        env = dict(os.environ)
        # record: a person plays, so untagged input passes (the proxy logs it 'input: untagged ...')
        self.record = bool(getattr(args, 'record', False))
        self.held = set()       # keys a keydown step holds down (clientrun.post_keys); let go at a scenario's end
        self.proxy = self.inst.use_proxy(env, self.out, input=True, tag=not self.record)
        # test-plan C2: the Miles calls to audio.jsonl (audiolog.py reads it); off unless --audio / ENB_E2E_AUDIO=1
        self.audio = bool(getattr(args, 'audio', False)) or os.environ.get('ENB_E2E_AUDIO') == '1'
        if self.audio:
            self.inst.use_audio_log(env, os.path.join(self.out, 'audio.jsonl'))
        self.trigger = os.path.join(self.out, 'trigger.txt')
        self.shot_frames = {}           # shot NAME -> the newest frame before its step (expect-ink reads a newer one)
        self.camera = os.path.join(self.out, 'camera.txt')
        env.update(D3D8SHOT_CAMERA=self.camera)
        env.update(D3D8SHOT_DRAWLOG='1')    # every shot also writes shot_NAME_<frame>.json, its draw log (test-plan C3)
        env.update(D3D8SHOT_TRIGGER=self.trigger, D3D8SHOT_MAX=str(3000 if self.record else 20 * len(scs)))
        self.messages = args.messages or any(sc['messages'] for sc in scs)
        files = {'antprox': os.path.join(self.out, 'antprox.out'), 'stubs': self.inst.log,
                 'proxy': os.path.join(self.out, 'd3d8shot.log')}
        if self.messages:
            files['msg'] = os.path.join(self.out, 'messages.log')
        self.note(f'session of {len(scs)}: {", ".join(sc["name"] for sc in scs)}; exe {self.inst.src_exe}, '
                  f'instance {self.inst.name}, out {self.out}')
        self.a, self.pid, _ = clientrun.start_antprox(self.inst, self.out, env,
                                                      ['-ENABLE_MESSAGES'] if self.messages else [],
                                                      mode=live_mode(args))
        self.ev = Events(files, log=os.path.join(self.out, 'events.log'))
        self.note(f'antprox pid {self.a.pid}, client pid {self.pid}' + (' (the retail client)' if self.retail else ''))
        if self.retail and self.pid:
            import threading
            threading.Thread(target=clientrun.click_agree, args=(self.pid, 60), daemon=True).start()
        self.listener = None
        if self.messages and self.pid:
            self.listener = clientrun.start_listener(self.pid, self.out, sum(sc['timeout'] for sc in scs) + 10)
        self.hwnd = None
        self.mark = 0               # ev.total index of the last input step
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

    def _shots(self, name):
        """[(frame, file)] of the session's shot_NAME_<frame>.bmp|png, by frame."""
        return sorted((int(m.group(1)), f) for f in os.listdir(self.out)
                      for m in [re.match(re.escape(f'shot_{name}_') + r'(\d+)\.(?:bmp|png)$', f)] if m)

    def _ink(self, name, box, seconds=5.0):
        """(bright pixels, path) in BOX (x, y, w, h) of the newest shot_NAME_*.bmp: a pixel counts when its
        brightest channel is at least INK_LEVEL (the UI's text is near white on the dark blue panels)."""
        end = time.time() + seconds          # the proxy saves the shot just after it takes the trigger: wait for
        size = -1                             # the file, and for its size to hold still
        while True:
            shots = [x for x in self._shots(name) if x[0] > self.shot_frames.get(name, -1)]   # this step's shot
            path = os.path.join(self.out, shots[-1][1]) if shots else None
            rows = None
            if path:
                try:
                    now = os.path.getsize(path)
                    if now == size:
                        w, h, n, rows = read_shot(path)
                    size = now
                except (OSError, ValueError):
                    rows = None
            if rows is not None:
                break
            if time.time() >= end:
                return 0, path
            time.sleep(0.2)
        x0, y0, bw, bh = box
        count = 0
        for y in range(max(0, y0), min(h, y0 + bh)):
            row = rows[y]
            for x in range(max(0, x0), min(w, x0 + bw)):
                if max(row[x * n], row[x * n + 1], row[x * n + 2]) >= INK_LEVEL:
                    count += 1
        return count, path

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
        start_line = ev.total
        own_from = 0
        seen = {'stubs': [], 'human': 0, 'marker': False}

        def on_event(t, src, text):
            if src == 'stubs':
                if text.startswith('MARK '):
                    seen['marker'] = True
                if text.startswith(('STUB first call: ', 'CRASH:')):
                    seen['stubs'].append(text)
            elif src == 'proxy' and text.startswith('input: NOT REPLAYED'):
                seen['human'] += 1
            elif src == 'proxy' and text.startswith('CRASH:'):        # d3d8shot.c Safe_Crash_Log, first chance
                seen['stubs'].append(text)

        ev.flush_repeats()
        ev.mark_epoch()
        ev.add_listener(on_event)
        try:
            first = not self.logged_in
            t_start = ev.now()
            tick_start = gettickcount()
            deadline = time.time() + sc['timeout']
            for src in ('stubs', 'proxy'):        # an older stand-in exe's handler; the d3d8shot proxy's (any exe)
                standing.append({'source': src, 'pattern': re.compile('^CRASH:'), 'line': '(always)',
                                 'watch': ev.watch(src, re.compile('^CRASH:'), start_line)})
            note(f'scenario {sc["name"]}' + ('' if first else ' (own steps only: the session is in the world)'))
            ev.add('e2e', f'scenario {sc["name"]}')

            def tick():
                ev.poll()
                for s in standing:
                    hit = s['watch'].hit
                    if hit is not None:
                        return f'{s["line"]}: expect-no {s["source"]} {s["pattern"].pattern!r} matched: {hit[2][:160]}'
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

            def wait_for(source, pattern, within, what, watch=None):
                end = time.time() + within
                w = watch or ev.watch(source, pattern, self.mark)
                try:
                    while True:
                        f = tick()
                        if f:
                            return f, False
                        if w.hit is not None:
                            note(f'  matched at {w.hit[0]:.1f} s: {w.hit[2][:200]}')
                            return None, True
                        if time.time() > end:
                            return what, False
                        time.sleep(0.02)
                finally:
                    if watch is None:
                        ev.unwatch(w)

            def post(op, st, tag=True):
                ev.poll()
                self.mark = ev.total
                ev.mark_epoch()
                self.mark_time = ev.now()
                if op in ('key', 'keydown', 'keyup'):
                    cp.post_step(self.hwnd, op, '+'.join(str(v) for v in st['keys']), tag=tag, held=self.held)
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
                        self.gen_before = gen_lines(seen['stubs'])
                        self.logged_in = True
                        note(f'own steps start; {len(self.gen_before)} gen stand-ins hit so far')
                    # the stand-ins' first-call flags start over (the exe's Stub_Log_Mark, through the proxy)
                    if self.marker is not False and self.hwnd is not None:
                        took = self._trigger('mark ' + sc['name'], 5)
                        ev.poll()
                        if self.marker is None:
                            self.marker = took and seen['marker']
                            if not self.marker:
                                note('  no stand-in marker (an exe without Stub_Log_Mark, or a proxy without triggers): '
                                     'later scenarios list only the stand-ins no earlier one hit')
                    own_from = len(seen['stubs'])
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
                    ev.mark_epoch()
                    standing.append(dict(st, watch=ev.watch(st['source'], st['pattern'], self.mark)))
                elif op == 'stand-in':
                    ev.mark_epoch()
                    standins.append(dict(st, watch=ev.watch('stubs', st['pattern'], self.mark)))
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
                    if op not in ('stray', 'keydown', 'keyup') and not st.get('until'):     # redo K: not a held key
                        posted.append(st)
                    if op == 'stray':                         # a person's input, as the harness's self-test: untagged
                        post('key' if st['what'] == 'key' else 'click', st, tag=False)
                    elif st.get('until'):
                        u = st['until']
                        post(op, st)
                        first_mark, end, clicks = self.mark, time.time() + u['within'], 1
                        w_until = ev.watch(u['source'], u['pattern'], first_mark)
                        try:
                            while True:
                                self.mark = first_mark
                                f, ok = wait_for(u['source'], u['pattern'], u['every'], None, watch=w_until)
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
                        finally:
                            ev.unwatch(w_until)
                    else:
                        post(op, st)
                elif op in ('pose-mark', 'expect-pose'):
                    pilot = Pilot(self.hwnd, self.camera, tick, note, self.held)
                    p = pilot.still(3.0)
                    if p is None:
                        fail = f'{st["line"]}: {op}: no camera file from the proxy (run_reference.py build)'
                    elif op == 'pose-mark':
                        self.pose_mark, self.pose_samples = p, []
                        note(f'  marked at {pilot.say(p)} z {p["z"]:.1f}')
                    elif getattr(self, 'pose_mark', None) is None:
                        fail = f'{st["line"]}: expect-pose before any pose-mark'
                    else:
                        v, at = Pilot.change(self.pose_mark, self.pose_samples + [p], st['kind'])
                        note(f'  {st["kind"]} {v:.1f} (min {st["min"]:g}; best at {pilot.say(at)} z {at["z"]:.1f}; '
                             f'now {pilot.say(p)} z {p["z"]:.1f})')
                        if v < st['min']:
                            fail = (f'{st["line"]}: expect-pose {st["kind"]} {st["min"]:g}: {v:.1f} '
                                    f'(from {pilot.say(self.pose_mark)} to {pilot.say(p)})')
                elif op in ('walk-to', 'face', 'hold', 'pose', 'rdrag'):
                    ev.poll()
                    self.mark = ev.total
                    ev.mark_epoch()
                    pilot = Pilot(self.hwnd, self.camera, tick, note, self.held)
                    samples = getattr(self, 'pose_samples', None)
                    sample = (lambda p: p is not None and samples is not None and samples.append(p))
                    if op not in ('hold', 'rdrag'):
                        pilot.still()
                    if op == 'rdrag':
                        fail = pilot.rdrag(st['x'], st['y'], st['secs'], sample)
                    elif op == 'hold' and 'secs' in st:
                        fail = pilot.hold_for(st['vk'], st['secs'], sample)
                    elif op == 'walk-to':
                        fail = pilot.walk_to(st['to'][0], st['to'][1], st['tol'], st['within'])
                    elif op == 'face':
                        fail = pilot.face(st['to'][0], st['tol'], st['within'])
                    elif op == 'pose':
                        p = pilot.pose()
                        note('  pose: ' + (pilot.say(p) if p else 'no camera file from the proxy'))
                    else:
                        w = ev.watch(st['source'], st['pattern'], self.mark)
                        end = time.time() + st['within']
                        try:
                            while w.hit is None and not fail:
                                pilot.key(st['vk'], True)
                                time.sleep(0.03)
                                fail = tick()
                                if not fail and time.time() > end:
                                    fail = (f'hold 0x{st["vk"]:X}: no {st["source"]} line {st["pattern"].pattern!r} '
                                            f'in {st["within"]:g} s')
                        finally:
                            pilot.key(st['vk'], False)
                            ev.unwatch(w)
                    self.mark_time = ev.now()
                elif op == 'alive':
                    end = time.time() + st['seconds']
                    while time.time() < end and not fail:
                        fail = tick()
                        time.sleep(0.05)
                elif op == 'shot':
                    fail = tick()
                    if not fail:
                        self.shot_frames[st['shot']] = max(self._shots(st['shot']) or [(-1, '')])[0]
                        if self._trigger(st['shot'], 5):
                            note(f'  shot {st["shot"]} taken')
                        else:
                            note('  shot not taken in 5 s (a proxy without D3D8SHOT_TRIGGER? run_reference.py build)')
                elif op == 'expect-ink':
                    n, path = self._ink(st['shot'], st['box'])
                    note(f'  ink {st["shot"]} {",".join(map(str, st["box"]))}: {n} bright pixels (min {st["min"]})')
                    if n < st['min']:
                        fail = (f'{sc["name"]}:{st["line"]}: expect-ink {st["shot"]}: {n} bright pixels in the box, '
                                f'fewer than {st["min"]} ({path})')
            if not fail:
                fail = tick()
            if not fail:
                for s in standins:
                    hit = s['watch'].hit
                    if hit is not None:
                        fail = f'{STANDIN_HIT} {s["line"]}: {hit[2][:120]}'
                        break
            ev.poll()
            stub_text = seen['stubs']
            gens = gen_lines(stub_text)
            own_text = seen['stubs'][own_from:]
            own = [g for g in gen_lines(own_text) if g not in self.gen_before]
            hand_own = [t[len('STUB first call: POC '):][:80] for t in own_text     # an older stand-in exe's hand ones
                        if t.startswith('STUB first call: POC ') and 'watchdog' not in t]
            crash = [t for t in stub_text if t.startswith('CRASH:')]
            if crash:
                self.dead = self.dead or 'CRASH'
            res = {'name': sc['name'], 'passed': fail is None, 'failure': fail, 'expected_fail': sc['expected_fail'],
                   'gen': gens, 'gen_own': own, 'hand_own': hand_own, 'human_dropped': seen['human'], 'crash': crash[:1],
                   'out': self.out, 'seconds': round(ev.now() - t_start, 1),
                   'tick_start': tick_start, 'tick_end': gettickcount(),
                   'login_failed': fail is not None and first and not reached_own}
            if crash and self.inst.map and os.path.exists(self.inst.map):
                import symbolize
                syms = symbolize.load(self.inst.map)
                res['crash_at'] = [f'{v:08X} {symbolize.name_of(syms, v)}' for v in symbolize.crash_addrs(crash[0])[:5]]
            note(f'result: {json.dumps(res)}')
            with open(os.path.join(self.out, f'result-{sc["name"]}.json'), 'w', encoding='utf-8') as f:
                json.dump(res, f, indent=1)
            return res
        finally:
            if self.held and self.hwnd is not None:      # a scenario that failed between its keydown and keyup
                note('  let go of ' + ', '.join(f'0x{vk:02X}' for vk in sorted(self.held)))
                cp.post_keys(self.hwnd, [(vk, True) for vk in sorted(self.held)], self.held, tag=True)
            for s in standing:
                ev.unwatch(s['watch'])
            for s in standins:
                ev.unwatch(s['watch'])
            ev.remove_listener(on_event)
            ev.flush_repeats()

    def close(self):
        if self.pid:
            subprocess.run(['taskkill', '/F', '/PID', str(self.pid)], capture_output=True)
        subprocess.run(['taskkill', '/F', '/T', '/PID', str(self.a.pid)], capture_output=True)
        time.sleep(1)
        if os.path.exists(self.proxy):
            os.remove(self.proxy)
        if self.audio:
            self.inst.unuse_audio_log()
        self.cp.stop_listener(self.listener, self.inst, self.out if self.messages else None)
        self.ev.poll()
        self.ev.close()
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


def live_mode(args):
    """antprox's arguments for --live (a live server in place of /REPLAY), else None."""
    if not getattr(args, 'live', None):
        return None
    return ['/SERVER:' + args.live, '/NET7BIN:' + args.net7bin, '/OPCODES'] + list(getattr(args, 'antprox_arg', None) or [])


_REC_INPUT = re.compile(r'input: untagged (?:mouse 0x(20[1-9])|key 0x([0-9A-Fa-f]+))(?: at (-?\d+), (-?\d+))?')
# every button message d3d8shot logs (In_Take: all but WM_MOUSEMOVE): the downs get a shot, the double-clicks
# (0x203 / 0x206 / 0x209) and the ups (0x202 / 0x205 / 0x208: where a drag let go) are rows of their own
_REC_BUTTON = {'201': 'click', '202': 'up', '203': 'dblclick', '204': 'rclick', '205': 'rup', '206': 'rdblclick',
               '207': 'mclick', '208': 'mup', '209': 'mdblclick'}
_REC_PACKET = re.compile(r'\b(received|sent) ([A-Z][A-Z0-9_]{3,})')
_REC_NOISE = re.compile(r'KEEP_?ALIVE|TIME_REQUEST|CLIENT_SET_TIME|DEBUG|PING|STARBASE_AVATAR_CHANGE|OP_[23]')


def record(args):
    """A person plays one session (--live: against a live server). Every button message (down, up, double-click)
    and key-down the proxy logs is a row; a left or right button-down is followed by a back-buffer shot (shot_click_NNNN_*.png). The session is written as
    recorded.txt: each input with the packets that followed it. The source for new scenarios and gadgets.txt."""
    s = Session(args, [{'name': 'record', 'messages': True, 'timeout': 86400}])
    print(f'e2e record: play in the client window; close the client (or Ctrl+C here) to end. Logs: {s.out}',
          flush=True)
    rows = []                                   # [time, input, {(direction, packet): count}, shot name, pose]
    state = {'pose': None, 'pilot': None}
    track = open(os.path.join(s.out, 'track.txt'), 'w', encoding='utf-8', newline='\n')

    def where():
        p = state['pose']
        return f'at {p["x"]:.1f},{p["y"]:.1f} yaw {p["yaw"]:.0f}' if p else ''

    def on_line(t, src, text):
        m = _REC_INPUT.search(text) if src == 'proxy' else None
        if m:
            what = (f'key 0x{m.group(2)}' if m.group(2) else
                    _REC_BUTTON[m.group(1)] + f' {m.group(3)},{m.group(4)}')
            rows.append([t, what, {}, '', where()])
            return
        m = re.search(r'Changing room from (-?\d+) to (-?\d+)', text) if src == 'msg' else None
        if m:
            track.write(f'{t:8.2f} room {m.group(1)} -> {m.group(2)}\n')
            rows.append([t, f'(room {m.group(1)}->{m.group(2)})', {}, '', where()])
            return
        m = _REC_PACKET.search(text) if src == 'antprox' else None
        if m and not _REC_NOISE.search(m.group(2)):
            if not rows:
                rows.append([t, '(start)', {}, '', ''])
            k = (m.group(1), m.group(2))
            rows[-1][2][k] = rows[-1][2].get(k, 0) + 1
    # the input line first: a poll reads the files in order, and a packet must not land on the row before the
    # click that caused it
    s.ev.files = dict(sorted(s.ev.files.items(), key=lambda kv: kv[0] != 'proxy'))
    s.ev.add_listener(on_line)
    shot = 0
    try:
        while s.alive() and s.a.poll() is None:
            n = len(rows)
            if state['pilot'] is None and s.pid and os.path.exists(s.camera):
                hwnd = s.cp.find_window(s.pid, s.retail)
                if hwnd:
                    state['pilot'] = Pilot(hwnd, s.camera, lambda: None, s.note)
            if state['pilot'] is not None:
                p, last = state['pilot'].pose(tries=1), state['pose']
                if p and (last is None or math.hypot(p['x'] - last['x'], p['y'] - last['y']) > 1.0
                          or abs(_wrap(p['yaw'] - last['yaw'])) > 3.0):
                    state['pose'] = p
                    track.write(f'{s.ev.now():8.2f} {p["x"]:8.1f} {p["y"]:8.1f} yaw {p["yaw"]:7.1f} pitch {p["pitch"]:6.1f}\n')
            s.ev.poll()
            if len(rows) > n and rows[-1][1].split()[0] in ('click', 'rclick'):
                shot += 1
                rows[-1][3] = f'click_{shot:04d}'
                s._trigger(rows[-1][3], 2)
            time.sleep(0.05)
    except KeyboardInterrupt:
        pass
    finally:
        track.close()
        s.close()
    path = os.path.join(s.out, 'recorded.txt')
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        for t, what, packets, name, at in rows:
            pk = ' '.join(('<' if d == 'sent' else '>') + n + (f'x{c}' if c > 1 else '')
                          for (d, n), c in packets.items())
            f.write(f'{t:8.2f} {what:18s} {name:11s} {at:28s} {pk}\n')
    print(f'e2e record: {len(rows)} inputs, {shot} shots; {path}  (> the client sent, < the server sent)')
    return 0


def crash_place(res):
    """Where a scenario's CRASH: line says the client faulted: the exception code, the faulting instruction and the
    first four return addresses on the stack (not the data address, which moves from run to run), or None."""
    if not res.get('crash'):
        return None
    m = re.match(r'CRASH: code (\w+) at (\w+) \(address \w+\) stack:((?: \w+)*)', res['crash'][0])
    return (m.group(1), m.group(2)) + tuple(m.group(3).split()[:4]) if m else res['crash'][0][:60]


def gettickcount():
    """GetTickCount (ms; the clock of d3d8shot.log and of the audio log's "tick"), None off Windows. A result's
    tick_start / tick_end bound its scenario in a session's logs (baseline.py: each scenario's audio)."""
    try:
        import ctypes
        f = ctypes.windll.kernel32.GetTickCount
        f.restype = ctypes.c_uint32
        return int(f())
    except (AttributeError, OSError):
        return None


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
                       'gen_own': [], 'hand_own': [], 'human_dropped': 0, 'crash': [], 'out': results[-1]['out'],
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
    if st['op'] in ('click', 'rclick', 'move', 'down', 'up'):
        return ((f'gadget {st["gadget"]} ' if st['gadget'] else '') + f'({st["x"]},{st["y"]})'
                + (f' until {st["until"]["source"]} {st["until"]["pattern"].pattern!r}' if st.get('until') else ''))
    if st['op'] == 'wait-quiet':
        return f'{st["source"]} {st["quiet"]:g}'
    if st['op'] in ('walk-to', 'face'):
        return ','.join(f'{v:g}' for v in st['to']) + f' tol {st["tol"]:g}'
    if st['op'] == 'hold' and 'secs' in st:
        return f'0x{st["vk"]:X} for {st["secs"]:g} s'
    if st['op'] == 'rdrag':
        return (f'gadget {st["gadget"]} ' if st['gadget'] else '') + f'({st["x"]},{st["y"]}) for {st["secs"]:g} s'
    if st['op'] == 'expect-pose':
        return f'{st["kind"]} {st["min"]:g}'
    if st['op'] == 'hold':
        return f'0x{st["vk"]:X} until {st["source"]} {st["pattern"].pattern!r}'
    if st['op'] in ('key', 'keydown', 'keyup') or (st['op'] == 'stray' and st.get('what') == 'key'):
        return st['spec'] + (f' until {st["until"]["source"]} {st["until"]["pattern"].pattern!r}'
                              if st.get('until') else '')
    return str(st.get('vk', st.get('text', st.get('seconds', st.get('shot', '')))))


def _has_tag_hook(ev):
    return ev.tag_hook or any(s == 'proxy' and 'tagged input read' in t for _, s, t in ev.lines)


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
    count follows the wall clock, not the scenario. Packets the replay server logs in the same millisecond on
    different connections are put in name order: which of the two antprox reads first is a race (2026-11-19: /stop's
    VERB_REQUEST to the sector and MASTER_JOIN came in either order, on retail and on ours, run to run)."""
    out, cur, stamp = {}, None, {}
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
            elif src == 'proxy' and cur and re.search(r': mark (\S+)', text) and text.split(': mark ')[1].split()[0] == cur:
                out[cur] = []           # the scenario's first own step: the session's first scenario drops its includes
            elif src == 'antprox' and cur:
                p = re.search(r'(?:(\d\d:\d\d:\d\d\.\d+) \w+ )?connection #(\d+): received (\w+) 0x[0-9a-f]+, '
                              r'(\d+) bytes', text)
                if p and not p.group(3).startswith(skip):
                    item = f'{p.group(3)} {p.group(4)}'
                    lst, at = out[cur], (p.group(1), p.group(2))
                    prev = stamp.get(cur)
                    if at[0] and prev and prev[0] == at[0] and prev[1] != at[1]:
                        k = len(lst)            # into name order among this millisecond's packets
                        while k > prev[2] and lst[k - 1] > item:
                            k -= 1
                        lst.insert(k, item)
                        stamp[cur] = (at[0], at[1], prev[2])
                    else:
                        lst.append(item)
                        stamp[cur] = (at[0], at[1], len(lst) - 1)
                # --live: 'Sending [0x0027] via UDP = received INVENTORY_MOVE' (name_live's, no size); the walk's
                # STARBASE_AVATAR_CHANGE count follows the pilot's closed loops, not the scenario
                p = None if p else re.search(r'Sending \[0x([0-9a-f]+)\] via \w+ = received (\w+)$', text)
                if p and not p.group(2).startswith(skip + ('STARBASE_AVATAR_CHANGE',)):
                    out[cur].append(p.group(2) if p.group(2) != 'OP_' else 'OP_' + p.group(1))
    return out


def run_dirs(arg):
    """A run as compare reads it: one run folder, several joined by ',' (a run whose failure started a fresh
    session wrote one folder per session), or a run manifest (`run --manifest FILE`: every folder it wrote)."""
    if arg.endswith('.json'):
        with open(arg, encoding='utf-8') as f:
            m = json.load(f)
        dirs = []
        for r in m['scenarios']:
            if r.get('out') and r['out'] not in dirs:
                dirs.append(r['out'])
        return dirs
    return [d for d in arg.split(',') if d]


def run_packets(dirs):
    out = {}
    for d in dirs:
        if os.path.exists(os.path.join(d, 'events.log')):
            out.update({k: v for k, v in sent_packets(d).items() if k not in out or v})
    return out


def run_results(dirs):
    """{scenario: its result-NAME.json} over a run's folders (the last folder that has one wins)."""
    out = {}
    for d in dirs:
        for f in sorted(os.listdir(d)) if os.path.isdir(d) else []:
            if f.startswith('result-') and f.endswith('.json'):
                with open(os.path.join(d, f), encoding='utf-8') as fh:
                    out[f[7:-5]] = json.load(fh)
    return out


def run_shots(dirs):
    """{scenario: [(shot name, bmp path)]} over a run's folders: e2e.log's 'scenario NAME' and '  shot X taken'
    lines give each shot its scenario; the k-th 'shot X taken' is the k-th shot_X_NNNNN.bmp by frame number (.png
    once the session closed: Session.close converts them, bmp2png)."""
    out = {}
    for d in dirs:
        log = os.path.join(d, 'e2e.log')
        if not os.path.exists(log):
            continue
        files = {}
        for f in os.listdir(d):
            m = re.match(r'shot_(.+)_(\d+)\.(bmp|png)$', f)
            if m:
                files.setdefault(m.group(1), []).append((int(m.group(2)), os.path.join(d, f)))
        for v in files.values():
            v.sort()
        used, cur = {}, None
        with open(log, encoding='utf-8', errors='replace') as fh:
            for line in fh:
                m = re.match(r'\S+ scenario (\S+)', line)
                if m:
                    cur = m.group(1)
                    out.setdefault(cur, [])
                    continue
                m = re.match(r'\S+   shot (\S+) taken$', line.rstrip('\n'))
                if m and cur:
                    k = used.get(m.group(1), 0)
                    used[m.group(1)] = k + 1
                    have = files.get(m.group(1), [])
                    if k < len(have):
                        out[cur].append((m.group(1), have[k][1]))
    return out


def read_bmp(path):
    """A d3d8shot shot (an uncompressed 24- or 32-bit BMP) -> (width, height, bytes per pixel, rows top-down)."""
    import struct
    with open(path, 'rb') as f:
        data = f.read()
    if data[:2] != b'BM':
        raise ValueError(f'{path}: not a BMP')
    off, = struct.unpack_from('<I', data, 10)
    w, h, _, bpp, comp = struct.unpack_from('<iiHHI', data, 18)
    if bpp not in (24, 32) or comp not in (0, 3):
        raise ValueError(f'{path}: {bpp}-bit, compression {comp}')
    n = bpp // 8
    stride = (w * n + 3) & ~3
    rows = [data[off + y * stride: off + y * stride + w * n] for y in range(abs(h))]
    if h > 0:
        rows.reverse()                  # bottom-up
    return w, abs(h), n, rows


def read_png(path):
    """A shot as bmp2png.write_png writes it (8-bit RGB, not interlaced) -> (width, height, 3, rows top-down);
    every filter type is undone (bmp2png writes filter 0 only, the fast path)."""
    import struct
    import zlib
    with open(path, 'rb') as f:
        data = f.read()
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError(f'{path}: not a PNG')
    pos, idat, w = 8, [], None
    while pos < len(data):
        n, tag = struct.unpack_from('>I4s', data, pos)
        body = data[pos + 8: pos + 8 + n]
        if tag == b'IHDR':
            w, h, depth, ctype, _, _, inter = struct.unpack('>IIBBBBB', body)
            if depth != 8 or ctype not in (2, 6) or inter:
                raise ValueError(f'{path}: depth {depth}, colour type {ctype}, interlace {inter}')
            bpp = 3 if ctype == 2 else 4
        elif tag == b'IDAT':
            idat.append(body)
        elif tag == b'IEND':
            break
        pos += 12 + n
    raw = zlib.decompress(b''.join(idat))
    stride = w * bpp
    rows, prev = [], bytes(stride)
    for y in range(h):
        ft, line = raw[y * (stride + 1)], raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)]
        if ft:
            cur = bytearray(line)
            for i in range(stride):
                a = cur[i - bpp] if i >= bpp else 0
                b = prev[i]
                c = prev[i - bpp] if i >= bpp else 0
                if ft == 1:
                    v = a
                elif ft == 2:
                    v = b
                elif ft == 3:
                    v = (a + b) >> 1
                else:
                    pa, pb, pc = abs(b - c), abs(a - c), abs(a + b - 2 * c)
                    v = a if pa <= pb and pa <= pc else (b if pb <= pc else c)
                cur[i] = (cur[i] + v) & 0xFF
            line = bytes(cur)
        rows.append(line)
        prev = line
    return w, h, bpp, rows


def read_shot(path):
    """A shot (.bmp from the proxy, .png after the session) -> (width, height, bytes per pixel, RGB rows top-down)."""
    if path.lower().endswith('.png'):
        return read_png(path)
    w, h, n, rows = read_bmp(path)
    out = []
    for row in rows:
        rgb = bytearray(row)
        rgb[0::n], rgb[2::n] = row[2::n], row[0::n]
        out.append(bytes(rgb))
    return w, h, n, out


INK_LEVEL = 200        # expect-ink: a pixel is ink when its brightest channel reaches this (0-255)
SHOT_GRID = 16          # the shot compare's cells: GRID x GRID
SHOT_TOL = 16           # a cell differs when its mean |difference| per channel (0-255) is above this
SHOT_STEP = 2           # every STEP-th pixel and row is compared
SHOT_MASK = os.path.join(SCN, 'shot_mask.txt')


def load_shot_mask(path=SHOT_MASK):
    """e2e/shot_mask.txt -> [(shot-name glob, rule)], one line each ('#' comments):
        GLOB X0,Y0-X1,Y1   a region that follows the wall clock on either client (a planet turning), left out of the
                           cell compare (client pixels at 1280x1024); rule (x0, y0, x1, y1)
        GLOB mean T        a shot whose content follows the wall clock all over (a recording's playback: ships and
                           effects move): only the frame's mean brightness is compared, within T of 255; rule ('mean', T)
        GLOB skip          not compared at all (retail differs from retail more than any tolerance); rule ('skip',)
        GLOB cells         the plain cell compare (an exception put before a wider mean / skip line); rule ('cells',)
    The first mean / skip / cells line that matches a shot decides; regions apply to the cell compare."""
    out = []
    if not os.path.exists(path):
        return out
    with open(path, encoding='utf-8') as f:
        for n, line in enumerate(f, 1):
            line = line.split('#')[0].strip()
            if not line:
                continue
            m = re.match(r'(\S+)\s+(\d+),(\d+)-(\d+),(\d+)$', line)
            if m:
                out.append((m.group(1), tuple(int(v) for v in m.groups()[1:])))
                continue
            m = re.match(r'(\S+)\s+(mean\s+[\d.]+|skip|cells)$', line)
            if not m:
                raise ValueError(f'{path}:{n}: GLOB X0,Y0-X1,Y1 | GLOB mean T | GLOB skip | GLOB cells')
            w = m.group(2).split()
            out.append((m.group(1), ('mean', float(w[1])) if w[0] == 'mean' else (w[0],)))
    return out


def mask_for(shot, mask):
    """-> (the regions to leave out, the shot's mean / skip rule or None: the cell compare)."""
    import fnmatch
    rules = [r for g, r in mask if fnmatch.fnmatchcase(shot, g)]
    rule = next((r for r in rules if len(r) != 4), None)
    return [r for r in rules if len(r) == 4], (None if rule == ('cells',) else rule)


def shot_mean(path, step=4):
    """The frame's mean brightness (0-255, every channel, every STEP-th pixel and row)."""
    w, h, n, rows = read_shot(path)
    tot = cnt = 0
    for y in range(0, h, step):
        r = rows[y]
        for c in range(3):
            v = r[c::n * step]
            tot += sum(v)
            cnt += len(v)
    return tot / max(1, cnt)


def shot_diff(path_a, path_b, grid=SHOT_GRID, tol=SHOT_TOL, step=SHOT_STEP, ignore=()):
    """Two back-buffer shots -> {'cells', 'differ' (cells over tol), 'box' (the differing cells' pixel box or None),
    'mean' (the whole image's mean |difference| per channel)}; {'size': ...} when the sizes differ. A cell that
    overlaps a rectangle of ignore (x0, y0, x1, y1) is not counted (load_shot_mask)."""
    wa, ha, na, ra = read_shot(path_a)
    wb, hb, nb, rb = read_shot(path_b)
    if (wa, ha) != (wb, hb):
        return {'size': f'{wa}x{ha} vs {wb}x{hb}', 'cells': grid * grid, 'differ': grid * grid, 'box': None,
                'mean': 255.0}
    cw, ch = -(-wa // grid), -(-ha // grid)
    sums, counts = [0] * (grid * grid), [0] * (grid * grid)
    xs = range(0, wa, step)
    per_cell_x = [0] * grid
    for x in xs:
        per_cell_x[x // cw] += 3
    total = 0
    for y in range(0, ha, step):
        base = (y // ch) * grid
        for i in range(grid):
            counts[base + i] += per_cell_x[i]
        la, lb = ra[y], rb[y]
        if la == lb and na == nb:
            continue
        for x in xs:
            oa, ob = x * na, x * nb
            d = abs(la[oa] - lb[ob]) + abs(la[oa + 1] - lb[ob + 1]) + abs(la[oa + 2] - lb[ob + 2])
            if d:
                sums[base + x // cw] += d
                total += d
    def masked(i):
        x0, y0 = (i % grid) * cw, (i // grid) * ch
        return any(x0 < r[2] and r[0] < x0 + cw and y0 < r[3] and r[1] < y0 + ch for r in ignore)
    bad = [i for i in range(grid * grid) if counts[i] and sums[i] / counts[i] > tol and not masked(i)]
    box = None
    if bad:
        cx = [i % grid for i in bad]
        cy = [i // grid for i in bad]
        box = (min(cx) * cw, min(cy) * ch, min(wa, (max(cx) + 1) * cw), min(ha, (max(cy) + 1) * ch))
    return {'cells': grid * grid, 'differ': len(bad), 'box': box, 'mean': total / max(1, sum(counts))}


def compare_runs(ours, retail, shots=True, grid=SHOT_GRID, tol=SHOT_TOL):
    """ours, retail: lists of run folders. -> [(name, 'SAME' | 'DIFF' | '-', one-line summary, detail lines)], one
    per scenario our run has, in its order. A scenario differs when its verdicts differ (PASS / FAIL / XFAIL; the
    retail run's own failure is the expected result), when the packets it sent differ (name and size, in order),
    or when a shot both runs took differs in some cell (shot_diff)."""
    import difflib
    pa, pb = run_packets(ours), run_packets(retail)
    va, vb = run_results(ours), run_results(retail)
    sa, sb = (run_shots(ours), run_shots(retail)) if shots else ({}, {})
    mask = load_shot_mask() if shots else []
    names = []                          # the run's order: events.log, then e2e.log, then any other result
    for n in list(pa) + list(sa) + list(va):
        if n not in names:
            names.append(n)
    rows = []
    for name in names:
        if name not in vb and name not in pb:
            rows.append((name, '-', 'not in the retail run', []))
            continue
        what, detail = [], []
        ra_, rb_ = va.get(name), vb.get(name)
        if ra_ and rb_ and verdict(ra_) != verdict(rb_):
            def v(r):
                return verdict(r) + (f' ({r["failure"][:70]})' if r.get('failure') else '')
            what.append(f'ours {v(ra_)}, retail {v(rb_)}')
        a, b = pa.get(name, []), pb.get(name, [])
        if a != b:
            d = [l for l in difflib.unified_diff(b, a, 'retail', 'ours', lineterm='', n=1)][2:]
            first = next((l for l in d if l[:1] in '-+'), '')
            what.append(f'packets ours {len(a)}, retail {len(b)} (first {first.strip()})')
            detail += ['packets (- retail, + ours):'] + ['    ' + l for l in d[:12]]
        alike, total = 0, 0
        shot_bad = []
        if shots:
            left = list(sb.get(name, []))
            for shot, path in sa.get(name, []):
                j = next((i for i, (s, _) in enumerate(left) if s == shot), None)
                if j is None:
                    continue
                other = left.pop(j)[1]
                regions, rule = mask_for(shot, mask)
                if rule and rule[0] == 'skip':
                    continue
                total += 1
                if rule:                        # ('mean', T)
                    ma, mb = shot_mean(path), shot_mean(other)
                    if abs(ma - mb) > rule[1]:
                        shot_bad.append(f'{shot} brightness {ma:.0f} vs {mb:.0f}')
                        detail.append(f'shot {shot}: mean brightness ours {ma:.1f}, retail {mb:.1f} (tolerance '
                                      f'{rule[1]:g}); {path}')
                    else:
                        alike += 1
                    continue
                r = shot_diff(path, other, grid, tol, ignore=regions)
                if r['differ']:
                    box = r.get('size') or (f'{r["box"][0]},{r["box"][1]}-{r["box"][2]},{r["box"][3]}')
                    shot_bad.append(f'{shot} {r["differ"]}/{r["cells"]} cells at {box}')
                    detail.append(f'shot {shot}: {r["differ"]} of {r["cells"]} cells differ, box {box}, '
                                  f'mean {r["mean"]:.1f}; {path}')
                else:
                    alike += 1
            if shot_bad:
                what.append('shots ' + '; '.join(shot_bad[:3]) + (' ...' if len(shot_bad) > 3 else ''))
        summary = f'{len(a)} packets' + (f', shots {alike} of {total} alike' if shots else '')
        rows.append((name, 'DIFF' if what else 'SAME', ('; '.join(what) if what else summary), detail))
    return rows


def compare(ours, retail, shots=True, detail=True, grid=SHOT_GRID, tol=SHOT_TOL):
    """Prints one line per scenario (SAME / DIFF: what / '-' not in the retail run) and, with detail, the first
    packet differences and each differing shot. Exit 1 when any differs."""
    rows = compare_runs(run_dirs(ours), run_dirs(retail), shots, grid, tol)
    for name, st, summary, det in rows:
        print(f'{st:5s} {name}: {summary}')
        if detail:
            for l in det:
                print('        ' + l)
    both = [r for r in rows if r[1] != '-']
    bad = sum(r[1] == 'DIFF' for r in both)
    print(f'compare: {len(both) - bad} of {len(both)} scenarios the same as retail'
          + (' (packets, verdicts, shots)' if shots else ' (packets, verdicts)'))
    return 1 if bad else 0


def retail_check(args):
    """`retail`: our build and the retail client through the same scenarios at once (two instances), then compare;
    --retail-run MANIFEST reuses a retail run instead of running it again."""
    stamp = time.strftime(STAMP)
    me = os.path.abspath(__file__)
    jobs = {}
    sides = [('ours', ['--instance', args.instance + '-ours'] + (['--exe', args.exe] if args.exe else []))]
    if not args.retail_run:
        sides.append(('retail', ['--retail', '--instance', args.instance + '-retail']))
    for side, extra in sides:
        man = os.path.join(OUT, f'retail-{stamp}-{side}.json')
        log = os.path.join(OUT, f'retail-{stamp}-{side}.log')
        cmd = [sys.executable, me, 'run'] + list(args.names) + extra + ['--manifest', man, '--out', OUT] + \
              (['--messages'] if args.messages else [])
        jobs[side] = (subprocess.Popen(cmd, stdout=open(log, 'w', encoding='utf-8'), stderr=subprocess.STDOUT,
                                       cwd=ROOT), man, log)
        time.sleep(2)                   # not the same second: the two session folders get distinct names anyway
    for side, (p, man, log) in jobs.items():
        p.wait()
        print(f'retail: {side} run: exit {p.returncode}, log {log}', flush=True)
    ours_man = jobs['ours'][1]
    retail_man = args.retail_run or jobs['retail'][1]
    for m in (ours_man, retail_man):
        if not os.path.exists(m):
            print(f'retail: no manifest {m} (the run stopped early: see its log)')
            return 2
    return compare(ours_man, retail_man, shots=not args.no_shots, detail=args.detail)


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
    """This tree's one client build (a worktree's own build/ in a worktree); clientrun.py refuses a missing one."""
    return os.path.join(ROOT, 'build', 'programs', 'client', 'client.exe')


# --db-snapshot (D270): the live station scenarios change the character (a warhead bought, a job item per accepted
# job, credits). Net-7 writes every change through to MySQL as it happens (SaveManager's queue) and reads the avatar's
# rows at each login (Player::ReadSavedData), but a session that ends by stopping the client leaves the player in the
# server until its link-dead drop, which queues hull, sector and ammo saves of its own (PlayerManager::
# DropPlayerFromSector). So the rows are put back with Net7.exe stopped, then the server is started again.
DB_CONTAINER = 'net7-mysql'
DB_SCHEMA = 'net7_user'
# net7_user's tables keyed by avatar_id that play changes (identity, account, guild and social tables are left alone)
DB_TABLES = ('avatar_ammo', 'avatar_equipment', 'avatar_exploration', 'avatar_faction_level', 'avatar_info',
             'avatar_inventory_items', 'avatar_level_info', 'avatar_mission_progress', 'avatar_position',
             'avatar_recipes', 'avatar_skill_levels', 'avatar_trade_items', 'avatar_vault_items', 'faction_data',
             'missions_completed', 'ship_data', 'ship_info')
# every net7_user table keyed by avatar_id: the whole character, for the store's database state (baseline.py
# inputs --save-db) that must also create the character on a stock database (01_import.sh's)
DB_AVATAR_TABLES = DB_TABLES + ('avatar_data', 'avatar_gm_items', 'friends_lists', 'guild_members', 'ignore_lists',
                                'warning_levels')
NET7_READY_PORT = 3693          # the last sector port Net7.exe opens ("Port: 3693" in its log, UDP)
NET7_CONTROL_PORT = 3810        # Net7.exe's loopback control port (Net7\Net7\ControlPort.cpp; control_port= in its cfg)


def db_exec(sql_cmd):
    """docker exec argv running SQL_CMD (a shell command line) in the MySQL container; @PW becomes the root
    password from the container's own environment."""
    return ['docker', 'exec', '-i', DB_CONTAINER, 'sh', '-c', sql_cmd.replace('@PW', '-p"$MYSQL_ROOT_PASSWORD"')]


def db_dump_cmd(avatar, tables=DB_TABLES):
    """The argv that prints AVATAR's rows of TABLES as INSERTs only (no CREATE / DROP, no locks)."""
    return db_exec(f'mysqldump -uroot @PW --no-create-info --skip-triggers --complete-insert --skip-add-locks '
                   f'--skip-comments --compact --where="avatar_id={int(avatar)}" {DB_SCHEMA} ' + ' '.join(tables))


def db_restore_cmd():
    return db_exec(f'mysql -uroot @PW {DB_SCHEMA}')


def db_restore_sql(avatar, inserts, tables=DB_TABLES):
    """One transaction: delete AVATAR's rows of TABLES, insert the dumped ones. Nothing else is touched. The
    session's foreign key checks are off (avatar_data, ship_data and ship_info reference avatar_info, ON DELETE
    RESTRICT): the avatar_info row goes back with the same key in the same transaction."""
    head = ['SET FOREIGN_KEY_CHECKS=0;', 'SET autocommit=0;', 'START TRANSACTION;']
    head += [f'DELETE FROM `{t}` WHERE `avatar_id` = {int(avatar)};' for t in tables]
    return '\n'.join(head) + '\n' + inserts.rstrip() + '\nCOMMIT;\nSET FOREIGN_KEY_CHECKS=1;\n'


def _db_run(argv, stdin=None):
    r = subprocess.run(argv, input=stdin, capture_output=True, text=True, encoding='utf-8')
    err = '\n'.join(ln for ln in r.stderr.splitlines() if 'password on the command line' not in ln)
    if r.returncode:
        raise RuntimeError(f'{argv[-1][:60]}...: exit {r.returncode}: {err.strip()[:300]}')
    return r.stdout


def db_snapshot(avatar, folder, tables=DB_TABLES):
    """Dumps AVATAR's rows of TABLES into FOLDER/avatar<N>.sql (the restore script, ready to run: it deletes the
    avatar's rows of TABLES and inserts the dumped ones, so it also creates them where absent). Returns its path."""
    os.makedirs(folder, exist_ok=True)
    path = os.path.join(folder, f'avatar{int(avatar)}.sql')
    sql = db_restore_sql(avatar, _db_run(db_dump_cmd(avatar, tables)), tables)
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write(sql)
    return path


def db_account_avatars(account):
    """The avatar ids of ACCOUNT (net7_user.avatar_info), ascending."""
    out = _db_run(db_exec(f'mysql -uroot @PW -N -B -e "SELECT avatar_id FROM avatar_info WHERE account_id='
                          f'{int(account)} ORDER BY avatar_id" {DB_SCHEMA}'))
    return [int(x) for x in out.split()]


def db_account_snapshot(account, folder):
    """Dumps ACCOUNT's row of net7_user.accounts into FOLDER/account<N>.sql: delete it, insert it (one transaction).
    Returns its path."""
    os.makedirs(folder, exist_ok=True)
    rows = _db_run(db_exec(f'mysqldump -uroot @PW --no-create-info --skip-triggers --complete-insert '
                           f'--skip-add-locks --skip-comments --compact --where="id={int(account)}" {DB_SCHEMA} accounts'))
    sql = ('SET FOREIGN_KEY_CHECKS=0;\nSET autocommit=0;\nSTART TRANSACTION;\n'
           f'DELETE FROM `accounts` WHERE `id` = {int(account)};\n' + rows.rstrip() +
           '\nCOMMIT;\nSET FOREIGN_KEY_CHECKS=1;\n')
    path = os.path.join(folder, f'account{int(account)}.sql')
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write(sql)
    return path


def _net7_pids(net7bin, name):
    import json
    ps = (f"Get-CimInstance Win32_Process -Filter \"name='{name}'\" | "
          "Select-Object ProcessId,ExecutablePath | ConvertTo-Json -Compress")
    r = subprocess.run(['powershell', '-NoProfile', '-Command', ps], capture_output=True, text=True)
    rows = json.loads(r.stdout) if r.stdout.strip() else []
    rows = [rows] if isinstance(rows, dict) else rows
    want = os.path.normcase(os.path.abspath(net7bin))
    return [x['ProcessId'] for x in rows
            if not x.get('ExecutablePath') or os.path.normcase(os.path.dirname(x['ExecutablePath'])) == want]


def net7_stop(net7bin):
    """Stops NET7BIN's Net7.exe and the Net7SSL.exe it started (it holds port 443 for the next start)."""
    for name in ('Net7.exe', 'Net7SSL.exe'):
        for pid in _net7_pids(net7bin, name):
            subprocess.run(['taskkill', '/F', '/T', '/PID', str(pid)], capture_output=True)


def _udp_bound(port):
    r = subprocess.run(['netstat', '-ano', '-p', 'UDP'], capture_output=True, text=True)
    return any(f':{port} ' in ln for ln in r.stdout.splitlines())


def net7_control_port(net7bin=None):
    """Net7.exe's control port: machine.py `net7_control_port` (env ENB_NET7_CONTROL_PORT) when set, else the
    last `control_port=` line of NET7BIN's Net7Config.cfg (Net7.cpp reads it at start; NET7BIN default machine.py
    net7_bin), else NET7_CONTROL_PORT. 0: the server runs without it."""
    sys.path.insert(0, HERE)
    import machine
    v = machine.get('net7_control_port')
    if v not in (None, ''):
        return int(v)
    port = NET7_CONTROL_PORT
    try:
        with open(os.path.join(net7bin or machine.get('net7_bin'), 'Net7Config.cfg'), encoding='latin1') as f:
            for ln in f:
                k, sep, val = ln.partition('=')
                if sep and k.strip().lower() == 'control_port':
                    try:
                        port = int(val.strip())
                    except ValueError:
                        pass
    except OSError:
        pass
    return port


def net7_control(cmd, timeout=2.0, port=None):
    """Sends CMD to Net7.exe's control port (PORT, default net7_control_port()); its one-line reply, or None (no
    Net7.exe, or one built without the control port: an older build, or control_port=0)."""
    import socket
    port = net7_control_port() if port is None else port
    if not port:
        return None
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(timeout)
    try:
        s.sendto(cmd.encode(), ('127.0.0.1', port))
        return s.recv(512).decode(errors='replace')
    except OSError:                 # timeout, or WSAECONNRESET when nothing listens on the port
        return None
    finally:
        s.close()


def net7_status(line):
    """STATUS's reply as a dict: {'state': 'READY'|'STARTING', 'players', 'saves', 'ssl', 'quiesced'} (strings);
    None for no reply. ControlPort.cpp: "READY players=N saves=idle|busy ssl=0|1 quiesced=0|1"."""
    if not line:
        return None
    words = line.split()
    d = {'state': words[0]}
    for w in words[1:]:
        k, sep, v = w.partition('=')
        if sep:
            d[k] = v
    return d


def net7_ready(port=None):
    """True when Net7.exe takes logins: every sector up, Net7SSL registered and no QUIESCE in force (control port
    STATUS: READY, ssl=1, quiesced=0), or, for a build without the control port, its last sector port bound."""
    st = net7_status(net7_control('STATUS', port=port))
    if st is not None:
        return st['state'] == 'READY' and st.get('ssl') == '1' and st.get('quiesced') == '0'
    return _udp_bound(NET7_READY_PORT)


NET7_SAVE_WAIT = 20.0           # s the restart path waits for an idle save queue before it stops Net7.exe


def net7_wait_saves(port=None, within=NET7_SAVE_WAIT):
    """Waits (at most WITHIN s) until STATUS shows the save queue idle (saves=idle) -> (idle, the last STATUS
    line); idle None when Net7.exe does not answer."""
    end = time.time() + within
    while True:
        line = net7_control('STATUS', port=port)
        st = net7_status(line)
        if st is None:
            return None, None
        if st.get('saves') == 'idle':
            return True, line
        if time.time() >= end:
            return False, line
        time.sleep(0.25)


_DETACH_CHILD = r'''
import subprocess, sys
f = subprocess.CREATE_NEW_CONSOLE | subprocess.CREATE_NEW_PROCESS_GROUP
try:
    subprocess.Popen(sys.argv[2:], cwd=sys.argv[1], creationflags=f | subprocess.CREATE_BREAKAWAY_FROM_JOB)
except OSError:
    subprocess.Popen(sys.argv[2:], cwd=sys.argv[1], creationflags=f)
'''


def _spawn_detached(argv, cwd):
    """Starts ARGV in CWD outside the caller's process tree (2026-11-24: stopping a capture task killed the
    Net7.exe it had started). A short-lived Python in between starts it (new console, new process group, out of
    the caller's job object when the job allows it) and exits, so the server's parent is a process that no longer
    exists: a tree kill of the harness or of its shell (taskkill /T, a task stop) does not reach it. Off Windows a
    new session (start_new_session)."""
    if os.name != 'nt':
        subprocess.Popen(argv, cwd=cwd, start_new_session=True, stdin=subprocess.DEVNULL,
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return
    f = subprocess.CREATE_NEW_PROCESS_GROUP | subprocess.CREATE_NO_WINDOW
    cmd = [sys.executable, '-I', '-c', _DETACH_CHILD, cwd] + list(argv)
    kw = dict(stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, close_fds=True)
    try:
        p = subprocess.Popen(cmd, creationflags=f | subprocess.CREATE_BREAKAWAY_FROM_JOB, **kw)
    except OSError:
        p = subprocess.Popen(cmd, creationflags=f, **kw)
    p.wait(30)


def net7_start(net7bin, within=240, port=None):
    """Starts Net7.exe in its own console (as start_server.bat does) and waits until it is ready (net7_ready; its
    log cannot be read while it runs: it holds the file without sharing). Returns True when ready."""
    port = net7_control_port(net7bin) if port is None else port
    _spawn_detached([os.path.join(net7bin, 'Net7.exe')], net7bin)
    end = time.time() + within
    while time.time() < end:
        if net7_ready(port):
            return True
        time.sleep(0.5)
    return False


def _db_apply(paths):
    """Runs each restore script, one transaction each, in order -> (applied paths, the failed path or None, its
    error). Stops at the first failure: the scripts before it stay applied (each is its own transaction)."""
    done = []
    for p in paths:
        try:
            with open(p, encoding='utf-8') as f:
                _db_run(db_restore_cmd(), f.read())
        except (OSError, RuntimeError) as e:
            return done, p, str(e)
        done.append(p)
    return done, None, None


def _db_failed(tag, paths, done, bad, err):
    return (f'{tag}: restore FAILED at {bad} ({len(done)} of {len(paths)} scripts applied before it, the rest not '
            f'run): {err}')


def db_restore(path, net7bin, log=print, tag='e2e: --db-snapshot', restart=None, info=None):
    """Runs the restore script PATH (or a list of them, in order; one transaction each) and returns True when
    every script applied and Net7.exe is ready again.

    A running Net7.exe with the control port is restored in place, in about a second: QUIESCE (every player
    logged out, the save queue drained, new logins refused), the scripts, RESUME. Player state is read from the
    database at login, so nothing else needs reloading; world state (MOBs, asteroids) carries over from the last
    run. Otherwise, or with RESTART (default: env ENB_NET7_RESTART=1), Net7.exe is stopped for the restore and
    started again (also when the restore fails). A QUIESCE that fails is undone (RESUME) before that restart, and
    the restart waits (NET7_SAVE_WAIT) for STATUS to show the save queue idle before it stops the server.
    INFO (a dict) gets: restore 'in-place' | 'restart', seconds, control_port, status (STATUS after), scripts,
    failed (the script that failed, or None), ok."""
    paths = [path] if isinstance(path, str) else list(path)
    if restart is None:
        restart = os.environ.get('ENB_NET7_RESTART') == '1'
    port = net7_control_port(net7bin)
    info = {} if info is None else info
    info.update(control_port=port, scripts=list(paths), failed=None)
    t0 = time.time()

    def done_(mode, ok, bad):
        info.update(restore=mode, seconds=round(time.time() - t0, 2), failed=bad, ok=bool(ok),
                    status=net7_control('STATUS', port=port))
        return bool(ok)

    st = None
    for _ in range(3):              # a busy main loop (a player being dropped) can miss one 2 s wait (2026-11-24)
        st = net7_control('STATUS', port=port)
        if st is not None or not port:
            break
    if not restart and (st or '').startswith('READY'):
        q = net7_control('QUIESCE', timeout=40, port=port)
        if q == 'QUIESCED':
            try:
                done, bad, err = _db_apply(paths)
            finally:
                r = net7_control('RESUME', timeout=10, port=port)
            log(_db_failed(tag, paths, done, bad, err) if bad else
                f'{tag}: rows restored in place ({", ".join(paths)})')
            ok = r == 'RESUMED' and net7_ready(port)
            log(f'{tag}: Net7.exe ' + (f'ready in {time.time() - t0:.1f} s (no restart)' if ok else
                                       f'not ready after the in-place restore (RESUME: {r!r})'))
            return done_('in-place', ok and not bad, bad)
        r = net7_control('RESUME', timeout=10, port=port)
        log(f'{tag}: QUIESCE failed ({q!r}), RESUME sent ({r!r}); restarting Net7.exe for the restore instead')
    elif not restart:
        log(f'{tag}: no in-place restore: STATUS on port {port} answered {st!r}')
    if net7_status(st) is not None:
        idle, last = net7_wait_saves(port)
        if idle is False:
            log(f'{tag}: the save queue is still busy after {NET7_SAVE_WAIT:.0f} s ({last!r}); stopping anyway')
    log(f'{tag}: stopping Net7.exe to restore {", ".join(paths)}')
    net7_stop(net7bin)
    end = time.time() + 30
    while _udp_bound(NET7_READY_PORT) and time.time() < end:
        time.sleep(1)
    done, bad = [], None
    try:
        done, bad, err = _db_apply(paths)
        log(_db_failed(tag, paths, done, bad, err) if bad else f'{tag}: rows restored')
    finally:
        t1 = time.time()
        ok = net7_start(net7bin, port=port)
        log(f'{tag}: Net7.exe ' + (f'ready in {time.time() - t1:.0f} s' if ok else
                                   'not ready (no READY with ssl=1 quiesced=0 on the control port, or port '
                                   f'{NET7_READY_PORT} not bound)'))
    return done_('restart', ok and not bad, bad)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('command', choices=['run', 'list', 'check', 'compare', 'record', 'retail'])
    ap.add_argument('names', nargs='*')
    ap.add_argument('--instance', default='e2e', help='clientrun.py instance name (default e2e)')
    ap.add_argument('--exe', help='our exe, its .map beside it (default: this tree\'s build/programs/client/client.exe)')
    ap.add_argument('--messages', action='store_true', help='-ENABLE_MESSAGES and msglisten (source msg)')
    ap.add_argument('--audio', action='store_true',
                    help='the mss32log proxy: audio.jsonl in the run folder (test-plan C2; env ENB_E2E_AUDIO=1)')
    ap.add_argument('--keep', action='store_true', help='keep the instance (default: clientrun.py cleans it at the end)')
    ap.add_argument('--separate', action='store_true', help='a client of its own for every scenario (default: one session)')
    ap.add_argument('--retail', action='store_true', help='run a copy of the retail client (needs --instance; D190)')
    ap.add_argument('--live', nargs='?', const='localhost', metavar='SERVER',
                    help='a live server in place of antprox /REPLAY (default localhost: the local Net-7 server)')
    ap.add_argument('--net7bin', help="--live: antprox's /NET7BIN (default machine.py net7_bin)")
    ap.add_argument('--out', metavar='DIR', help='the session root in place of .work/e2e (env ENB_E2E_OUT)')
    ap.add_argument('--antprox-arg', action='append', metavar='ARG',
                    help='--live: one more antprox argument (repeatable; an ARG starting with - needs --antprox-arg=ARG)')
    ap.add_argument('--db-snapshot', nargs='?', const=6, type=int, metavar='AVATAR',
                    help="--live: dump the avatar's rows (default 6) before the run and put them back after it "
                         '(Net7.exe stopped for the restore, then started again)')
    ap.add_argument('--manifest', metavar='FILE', help="run: write the run (each scenario's verdict and folder) to "
                                                      'FILE as JSON; compare and retail read it')
    ap.add_argument('--no-shots', action='store_true', help='compare / retail: packets and verdicts only')
    ap.add_argument('--detail', action='store_true', help='retail: the packet and shot details under each DIFF')
    ap.add_argument('--retail-run', metavar='MANIFEST', help='retail: reuse this retail run instead of a new one')
    args = ap.parse_args(argv)
    args.record = args.command == 'record'
    global OUT
    if args.out:
        OUT = os.path.abspath(args.out)
    if args.net7bin is None:
        sys.path.insert(0, HERE)
        import machine
        args.net7bin = machine.get('net7_bin')
    if args.record:
        if args.retail and args.instance == 'e2e':
            print('e2e: --retail needs --instance NAME of its own')
            return 2
        if args.exe is None and not args.retail:
            args.exe = default_exe()
        os.makedirs(OUT, exist_ok=True)
        sys.path.insert(0, HERE)
        return record(args)
    if args.command == 'compare':
        if len(args.names) != 2:
            print('e2e: compare DIR_OURS DIR_RETAIL (two run folders under .work/e2e)')
            return 2
        return compare(args.names[0], args.names[1], shots=not args.no_shots)
    if args.command == 'retail':
        if args.live:
            print('e2e: retail compares /REPLAY runs only')
            return 2
        if args.exe:
            args.exe = os.path.abspath(args.exe)
        if args.instance == 'e2e':
            args.instance = 'cmp'
        os.makedirs(OUT, exist_ok=True)
        return retail_check(args)
    names = args.names or scenario_names(args.live)
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
    snap = None
    if args.db_snapshot is not None:
        if not args.live:
            print('e2e: --db-snapshot needs --live')
            return 2
        snap = db_snapshot(args.db_snapshot, os.path.join(OUT, time.strftime(STAMP) + '-db-snapshot'))
        print(f'e2e: --db-snapshot: avatar {args.db_snapshot} saved to {snap}', flush=True)
    t0 = time.time()
    try:
        results = run_all(scs, args, lambda res: print(summary_line(res), flush=True))
    finally:
        if snap:
            db_restore(snap, args.net7bin, lambda m: print(m, flush=True))
    bad = sum(verdict(res) in ('FAIL', 'XPASS') for res in results)
    if args.manifest:
        with open(args.manifest, 'w', encoding='utf-8') as f:
            json.dump({'instance': args.instance, 'retail': bool(args.retail), 'exe': args.exe,
                       'scenarios': [{'name': r['name'], 'verdict': verdict(r), 'out': r.get('out'),
                                      'failure': r.get('failure')} for r in results]}, f, indent=1)
    print(f'e2e: {len(scs) - bad} of {len(scs)} as expected in {time.time() - t0:.0f} s; logs under {OUT}')
    if not args.keep:
        import clientrun
        clientrun.clean(args.instance)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
