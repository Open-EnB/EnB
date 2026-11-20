"""The budget-device audit (the user, 2026-11-19): every function held by a VC6 inline-budget device, measured.

An exact function may hold its inline budget (/Ob2, workflow.md 3.1.1) by a form other than the client's: dead code
leaves no bytes, so the bytes cannot tell a dead read from the client's own source. This tool lists every device in
app/ by kind, the function it sits in (address, size, exact or not), the source lines and how it was found, then
measures each mechanically removable one: the file compiled without the device (trybatch.py, lock-free, --file-check)
against the unmodified file, so the pairs the device holds are known.

Device kinds (code: found from the code; comment: from a comment that names the budget):
  dead-read        an expression statement whose value is discarded: an accessor read (size, length, c_str, end,
                   begin, Peek, Is_*, Get_*, ...), a bare `std::char_traits<char>::length;`, a find whose result is
                   dropped (Unknown_Find_Field / Unknown_Find_Valid), a compare (code; D272, formerly D130)
  auto-inline      a function between `#pragma auto_inline(off)` and `(on)`: kept out of line in its same-file callers
                   (code); the rows are the wrapped function, `held` its same-file callers
  copy-operator    a user-written `operator=(const T &)` of an app class (code): spends budget the implicit one does not
  inline-helper    an `inline` / file-static function of a .cpp with no client address (code): a layer the client may
                   not have (invented layer or helper split)
  inline-layer     a comment calling a layer / wrapper / helper / accessor a budget spender (comment)
  statement-order  a comment tying a statement order to the budget (comment)
  class-view       a class the file defines that another app/ file defines too (matchaudit.py's test)
  interim-forwarder an inline body on a line marked interim (matchaudit.py's test)
  budget-other     any other comment that says a form spends the budget (comment)

    python tools/re/budgetdev.py scan                    the list: .work/budgetdev/scan.json, then the table
    python tools/re/budgetdev.py forms FILE              the measurable devices of FILE as trybatch forms (printed)
    python tools/re/budgetdev.py measure [FILE...] [--top N] [--vms K] [--kinds K,..] [--freeset] [--redo]
                                                         compile each file without each device (trybatch run_spec,
                                                         --file-check), biggest held function first; results to
                                                         .work/budgetdev/measure-<kinds>.json (kept across runs: two
                                                         kind sets may run at once), then the table. --freeset: each
                                                         file once more with every device measured `free` removed
                                                         together (the source change a builder would make)
    python tools/re/budgetdev.py table                   docs/data/budget-devices.md and .json (generated)

Measured verdicts (per device row): `holds` (the file without the device loses exact pairs: listed), `free` (nothing
lost: the device is not needed at this state of the file), `gains` (the file without it gains pairs), `error` (the
form did not compile). `evidence` is the client evidence for keeping the device's form (none found -> `source
unproven`); the tool never edits the tree. Standard library plus trybatch / matchaudit (measure needs the VMs).
"""
import argparse
import contextlib
import io
import json
import os
import re
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, 'tools', 'vc6'))

WORK = os.path.join(ROOT, '.work', 'budgetdev')
OUT_MD = os.path.join(ROOT, 'docs', 'data', 'budget-devices.md')
OUT_JSON = os.path.join(ROOT, 'docs', 'data', 'budget-devices.json')
SHARDS = os.path.join(ROOT, 'docs', 'data', 'imports')
KINDS = ('dead-read', 'auto-inline', 'copy-operator', 'inline-helper', 'inline-layer', 'statement-order',
         'class-view', 'interim-forwarder', 'budget-other')
MEASURABLE = ('dead-read', 'auto-inline')
NL = '\n'

# ---------------------------------------------------------------- the text

STR = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'')
COMMENT = re.compile(r'//[^\n]*|/\*.*?\*/', re.S)
PP = re.compile(r'^[ \t]*#[^\n]*(?:\\\n[^\n]*)*', re.M)


def blank(s):
    return re.sub(r'[^\n]', ' ', s)


def code_of(text):
    """text with comments, string / char literals' insides and preprocessor lines blanked (positions kept)."""
    def lit(m):
        s = m.group(0)
        return s[0] + ' ' * (len(s) - 2) + s[-1]

    out, i = [], 0
    for m in re.finditer(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', text, re.S):
        out.append(text[i:m.start()])
        out.append(lit(m) if m.group(0)[0] in '"\'' else blank(m.group(0)))
        i = m.end()
    out.append(text[i:])
    code = ''.join(out)
    return PP.sub(lambda m: blank(m.group(0)), code)


def line_of(text, pos):
    return text.count(NL, 0, pos) + 1


# ---------------------------------------------------------------- functions of a file

SCOPE_KW = re.compile(r'^(?:template\s*<[^{};]*>\s*)?(?:typedef\s+)?(?:class|struct|union|enum|namespace)\b'
                      r'|^extern\s*"\s*"\s*$|^extern\s*"[^"]*"\s*$')
NAME_BEFORE = re.compile(r'((?:[A-Za-z_~][\w]*(?:\s*<[^()]*?>)?\s*::\s*)*(?:~?[A-Za-z_]\w*|operator\s*(?:\(\s*\)|[^\s(]+)))\s*$')


def first_paren(header):
    """Index of the parameter list's '(' in a function header (not __declspec's or a template's), or -1."""
    depth = 0
    for i, c in enumerate(header):
        if c == '<':
            depth += 1
        elif c == '>' and depth:
            depth -= 1
        elif c == '(' and not depth:
            before = header[:i].rstrip()
            if re.search(r'\b(__declspec|throw|sizeof)$', before):
                continue
            if before.endswith('operator') and header[i + 1:].lstrip().startswith(')'):
                continue              # operator()'s own name
            return i
    return -1


def functions(text, code=None):
    """[{name, start, open, end, header_line, end_line, scope}] of the function bodies in text (members defined in a
    class body included), by brace tracking on the code (comments, strings, preprocessor lines blanked)."""
    code = code if code is not None else code_of(text)
    out, stack, last = [], [], 0
    for i, c in enumerate(code):
        if c == ';':
            if not stack or stack[-1][0] in ('scope', 'top'):
                last = i + 1
        elif c == '{':
            if stack and stack[-1][0] in ('func', 'block', 'init'):
                stack.append(('block', i, None))
                continue
            header = code[last:i].strip()
            header = re.sub(r'^(?:(?:public|private|protected)\s*:\s*)+', '', header)
            k = first_paren(header)
            if SCOPE_KW.search(header) and (k < 0 or re.match(r'^(?:template\s*<[^{};]*>\s*)?(?:class|struct|union|'
                                                              r'enum|namespace)\b[^(]*$', header[:k] if k >= 0 else header)):
                stack.append(('scope', i, None))
            elif re.match(r'^extern\s*"', header) and k < 0:
                stack.append(('scope', i, None))
            elif k > 0 and '=' not in re.sub(r'operator\s*\S+', '', header[:k]):
                m = NAME_BEFORE.search(header[:k])
                name = re.sub(r'\s+', '', m.group(1)) if m else ''
                if name in ('if', 'while', 'for', 'switch', 'catch'):
                    stack.append(('block', i, None))
                    continue
                hs = last + (len(code[last:i]) - len(code[last:i].lstrip()))
                stack.append(('func', i, {'name': name, 'start': hs, 'open': i, 'header': ' '.join(header.split())[:200],
                                          'scope': [s[2] for s in stack if s[0] == 'scope' and s[2]]}))
            else:
                stack.append(('init', i, None))
            last = i + 1
        elif c == '}':
            if not stack:
                last = i + 1
                continue
            kind, _, f = stack.pop()
            if kind == 'func':
                f['end'] = i
                f['header_line'] = line_of(code, f['start'])
                f['body_line'] = line_of(code, f['open'])
                f['end_line'] = line_of(code, i)
                out.append(f)
            if not stack or stack[-1][0] == 'scope':
                last = i + 1
    return out


ADDR = re.compile(r'\b0x([0-9A-Fa-f]{6})\b')


def comment_above(lines, k):
    """The comment lines directly above line k (1-based) and the definition's own lines' trailing comments."""
    out, j = [], k - 2
    while j >= 0 and k - 2 - j < 16:
        s = lines[j].strip()
        if s.startswith('//') or s.startswith('**') or s.startswith('/*') or s.startswith('*'):
            out.insert(0, s)
        elif s.startswith('#pragma') or not s:
            if not s and out:
                break
        else:
            break
        j -= 1
    return out


def address_of(f, lines, known):
    """The client address a definition's comment names: the first address in the comment block above it (or on its
    header line) that is a known function start."""
    above = comment_above(lines, f['header_line'])
    own = [lines[n - 1] for n in range(f['header_line'], min(f['body_line'], f['header_line'] + 3) + 1)]
    marker = [l for l in above if re.match(r'//\s*@?0x[0-9A-Fa-f]{6}\b', l)]   # the `// 0xADDR` marker line first
    named = re.search(r'_([0-9A-Fa-f]{6})$', f['name'])                        # then an Unknown_XXXXXX name
    if named and int(named.group(1), 16) in known and not marker:
        return int(named.group(1), 16), ' '.join(above)[:300]
    for text in marker + above + [l.split('//', 1)[1] for l in own if '//' in l]:
        for m in ADDR.finditer(text):
            a = int(m.group(1), 16)
            if a in known:
                return a, ' '.join(above)[:300]
    return None, ' '.join(above)[:300]


# ---------------------------------------------------------------- devices

ACC = (r'(?:size|length|c_str|data|empty|begin|end|rbegin|rend|front|back|capacity|Peek|Peek_\w+|Is_\w+|Get_\w+|'
       r'Count|Length|Length2)')
DEAD_ACCESS = re.compile(r'^[A-Za-z_(*][\w\s.\->\[\]()*&:<>,]*?(?:\.|->)' + ACC + r'\s*\(\s*\)$')
DEAD_TRAITS = re.compile(r'^std\s*::\s*char_traits\s*<\s*\w+\s*>\s*::\s*length$')
DEAD_FIND = re.compile(r'^Unknown_Find_(?:Field|Valid)\s*<.*>\s*\(.*\)$|^[\w.\->\[\]]+(?:\.|->)find\s*\(.*\)$')
DEAD_CMP = re.compile(r'^[\w.\->\[\]()*]+\s*(?:==|!=)\s*[\w.\->\[\]()*"\' ]+$')
STMT_KW = re.compile(r'^(?:return|if|while|for|case|else|do|delete|throw|goto|break|continue|default|typedef|using)\b')


def statements(code, lo, hi):
    """(start, end, text) of the statements between lo and hi at paren depth 0 (for headers' `;` skipped)."""
    out, depth, start = [], 0, lo
    for i in range(lo, hi):
        c = code[i]
        if c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
        elif depth == 0 and c in ';{}':
            if c == ';':
                out.append((start, i, code[start:i]))
            start = i + 1
        elif depth == 0 and c == ':' and code[start:i].strip() in ('default', 'public', 'private', 'protected') \
                or depth == 0 and c == ':' and re.match(r'^\s*case\b', code[start:i]) and code[i + 1:i + 2] != ':' \
                and code[i - 1:i] != ':':
            start = i + 1
    return out


def dead_kind(s):
    s = ' '.join(s.split())
    if not s or STMT_KW.match(s) or s.count('(') != s.count(')'):
        return None
    if re.search(r'(?<![=!<>])=(?!=)|\+\+|--|<<|>>|[+|^%/?]|&&', s):
        return None
    if DEAD_TRAITS.match(s):
        return 'char_traits length'
    if DEAD_ACCESS.match(s):
        return 'accessor ' + re.search(r'(\w+)\s*\(\s*\)$', s).group(1)
    if DEAD_FIND.match(s):
        return 'find'
    if DEAD_CMP.match(s) and top_level_compare(s):
        return 'compare'
    return None


def top_level_compare(s):
    """True when s's == / != sits at paren depth 0 (not inside a macro's or a call's argument list)."""
    depth = 0
    for i, c in enumerate(s):
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        elif not depth and s[i:i + 2] in ('==', '!='):
            return True
    return False


BUDGET = re.compile(r'\bbudget\b', re.I)
PRAGMA_AI = re.compile(r'^[ \t]*#[ \t]*pragma[ \t]+auto_inline[ \t]*\(\s*(on|off)\s*\)', re.M)
COPY_OP = re.compile(r'operator\s*=$')


def comment_kind(c):
    lc = c.lower()
    if re.search(r'\bdead\b', lc):
        return 'dead-read'
    if 'auto_inline' in lc:
        return 'auto-inline'
    if 'operator=' in lc or 'assigned, not constructed' in lc:
        return 'copy-operator'
    if re.search(r'\b(layer|layers|wrapper|helper|accessor)\b', lc):
        return 'inline-layer'
    if re.search(r'\border\b', lc):
        return 'statement-order'
    if re.search(r'\bview\b', lc):
        return 'class-view'
    return 'budget-other'


def scan_file(path, rel, known, ledger_exact, sizes, others=None):
    """The device rows of one source file."""
    with open(path, encoding='latin-1') as fh:
        text = fh.read()
    lines = text.splitlines()
    code = code_of(text)
    funcs = functions(text, code)
    for f in funcs:
        f['address'], f['note'] = address_of(f, lines, known)
    rows = []

    def func_at(line):
        best = None
        for f in funcs:
            if f['header_line'] <= line <= f['end_line'] and (best is None or f['header_line'] >= best['header_line']):
                best = f
        return best

    def callers(name):
        short = re.sub(r'^.*::', '', name)
        if not short or short.startswith('operator'):
            return []
        pat = re.compile(r'(?<![\w~])' + re.escape(short) + r'\s*\(')
        return [g for g in funcs if g['name'] != name and pat.search(code[g['open']:g['end']])]

    def row(kind, f, line_list, src, found, held=None):
        a = f['address'] if f else None
        r = {'kind': kind, 'file': rel, 'function': f['name'] if f else '', 'address': f'0x{a:06X}' if a else None,
             'lines': line_list, 'source': src[:240], 'found': found}
        r['exact'] = bool(a and a in ledger_exact)
        r['size'] = sizes.get(a, 0) if a else 0
        if held is not None:
            r['held'] = held
        rows.append(r)
        return r

    # dead reads: per function, its dead statements
    for f in funcs:
        dead = []
        for s0, s1, s in statements(code, f['open'] + 1, f['end']):
            k = dead_kind(s)
            if k:
                st = s0 + len(s) - len(s.lstrip())
                dead.append((line_of(code, st), k, text[st:s1 + 1], st, s1 + 1))
        if dead:
            r = row('dead-read', f, sorted({d[0] for d in dead}), '; '.join(f'{d[2].strip()}' for d in dead[:8])
                    + (f' (+{len(dead) - 8})' if len(dead) > 8 else ''),
                    'code: ' + ', '.join(sorted({d[1] for d in dead})) + f' x{len(dead)}')
            r['spans'] = [[d[3], d[4]] for d in dead]
            body = lines[f['body_line']:f['end_line'] - 1]
            if any(re.search(r'\b(G_ASSERT|FAIL_IF|WWASSERT|WWDEBUG_\w+)\s*\(', l) for l in body):
                pad = sum(1 for l in body if not l.strip())
                if pad:     # the client's source had lines here that ours keeps blank for __LINE__: room, not proof
                    r['hint'] = f'{pad} blank lines kept for __LINE__ inside the function'
            if not f['address']:
                r['held'] = [f'0x{g["address"]:06X}' for g in callers(f['name']) if g['address']]
    # #pragma auto_inline(off) ... (on)
    offs = []
    for m in PRAGMA_AI.finditer(text):
        offs.append((m.group(1), line_of(text, m.start()), m.start(), m.end()))
    for k, (state, ln, p0, p1) in enumerate(offs):
        if state != 'off':
            continue
        nxt = next((o for o in offs[k + 1:] if o[0] == 'on'), None)
        hi = nxt[1] if nxt else len(lines) + 1
        inside = [f for f in funcs if ln < f['header_line'] < hi and not f['scope']]
        spans = [[p0, p1]] + ([[nxt[2], nxt[3]]] if nxt else [])
        for f in inside or [None]:
            held = [f'0x{g["address"]:06X}' for g in (callers(f['name']) if f else []) if g['address']]
            r = row('auto-inline', f, [ln] + ([nxt[1]] if nxt else []),
                    f'#pragma auto_inline(off) .. (on) around {f["name"] if f else "nothing"}', 'code: pragma',
                    held=held)
            r['spans'] = spans
    # user-written copy operators, inline helpers without an address
    for f in funcs:
        if COPY_OP.search(f['name']) and re.search(r'\(\s*const\s+[\w:<>, ]+&', f['header']):
            row('copy-operator', f, [f['header_line']], f['header'], 'code: user-written operator=')
        elif rel.endswith('.cpp') and not f['address'] and not f['scope'] and \
                re.search(r'\b(inline|__inline|__forceinline)\b', f['header'].split('(')[0]):
            held = [f'0x{g["address"]:06X}' for g in callers(f['name']) if g['address']]
            row('inline-helper', f, [f['header_line']], f['header'], 'code: inline in a .cpp, no client address',
                held=held)
    # comments that name the budget (not NOT EXACT notes, not the end-of-file notes block)
    for n, l in enumerate(lines, 1):
        if '//' not in l:
            continue
        c = l.split('//', 1)[1]
        if not BUDGET.search(c) or 'NOT EXACT' in c or re.search(r'poly[ _]budget|\bBudget \(\+', c, re.I):
            continue
        f = func_at(n)
        if f is None:                      # a comment block above a definition
            f = next((g for g in funcs if 0 < g['header_line'] - n <= 16 and all(
                lines[j].strip().startswith('//') or not lines[j].strip() or lines[j].strip().startswith('#pragma')
                for j in range(n, g['header_line'] - 1))), None)
        kind = comment_kind(c)
        twin = next((r for r in rows if f is not None and r['kind'] == kind and r['function'] == f['name']
                     and r['found'].startswith('code')), None)
        if twin:                           # the comment explains a device the code scan found
            twin['found'] += ' + comment'
            twin.setdefault('comment', c.strip()[:240])
            continue
        held = None
        if f is None:
            held = sorted({f'0x{int(m, 16):06X}' for m in ADDR.findall(c) if int(m, 16) in known})
        row(kind, f, [n], c.strip(), 'comment', held=held)
    # class views and interim forwarders (matchaudit's tests)
    if others is not None:
        import matchaudit
        for kind, n, detail in matchaudit.scan_devices(text, others, rel):
            if kind in ('class-view', 'interim-forwarder'):
                row(kind, func_at(n), [n], detail, 'code: matchaudit ' + kind)
    return rows


# ---------------------------------------------------------------- the ledger, the client's functions

def ledger():
    """({rel source path: set(exact addresses)}, {address: lib file key}) from the ledger shards (app only)."""
    import matchaudit
    by_path = {}
    for lib in ('app',):
        d = os.path.join(SHARDS, lib)
        for dirpath, _, names in os.walk(d):
            for n in names:
                if not n.endswith('.json'):
                    continue
                e = json.load(open(os.path.join(dirpath, n), encoding='utf-8'))
                p = matchaudit.source_path(lib, e)
                if p:
                    by_path[p] = {int(x, 16) for x in e.get('in_tree', []) or []}
    return by_path


def known_starts():
    """({function start}, {address: size}) from symbols.json and app-functions.json."""
    known, sizes = set(), {}
    for s in json.load(open(os.path.join(ROOT, 'docs', 'data', 'symbols.json'), encoding='utf-8')):
        if s.get('kind') == 'function':
            a = int(s['address'], 16)
            known.add(a)
            if s.get('length'):
                sizes[a] = s['length']
    fs = json.load(open(os.path.join(ROOT, 'docs', 'data', 'app-functions.json'), encoding='utf-8'))['functions']
    starts = sorted(int(f['address'], 16) for f in fs)
    for a, b in zip(starts, starts[1:] + [0]):
        known.add(a)
        if b and a not in sizes:
            sizes[a] = b - a
    return known, sizes


def source_files():
    out = []
    for d, _, names in os.walk(os.path.join(ROOT, 'app')):
        for n in sorted(names):
            if n.lower().endswith(('.cpp', '.h', '.inl')):
                p = os.path.join(d, n)
                out.append((p, os.path.relpath(p, ROOT).replace(os.sep, '/')))
    return sorted(out, key=lambda x: x[1])


def cmd_scan(a):
    import matchaudit
    known, sizes = known_starts()
    led = ledger()
    allexact = set().union(*led.values()) if led else set()
    others = matchaudit.class_index()
    rows = []
    for p, rel in source_files():
        rows += scan_file(p, rel, known, led.get(rel, allexact if rel.endswith(('.h', '.inl')) else set()), sizes, others)
    os.makedirs(WORK, exist_ok=True)
    json.dump({'rows': rows}, open(os.path.join(WORK, 'scan.json'), 'w', encoding='utf-8'), indent=0)
    by = {}
    for r in rows:
        by.setdefault(r['kind'], [0, 0])
        by[r['kind']][0] += 1
        by[r['kind']][1] += r['exact']
    print('scan: ' + ', '.join(f'{k} {by[k][0]} ({by[k][1]} in exact functions)' for k in KINDS if k in by))
    return cmd_table(a)


# ---------------------------------------------------------------- measuring

def load(name, default=None):
    p = os.path.join(WORK, name + '.json')
    return json.load(open(p, encoding='utf-8')) if os.path.exists(p) else default


def load_measures():
    """Every measure-*.json merged (one per kind set measured)."""
    out = {}
    if os.path.isdir(WORK):
        for n in sorted(os.listdir(WORK)):
            if n.startswith('measure') and n.endswith('.json'):
                out.update(json.load(open(os.path.join(WORK, n), encoding='utf-8')))
    return out


def row_key(r):
    return f"{r['kind']}|{r['file']}|{r['address'] or r['function']}|{r['lines'][0]}"


def without(text, spans):
    """text with each span blanked on its own lines (line count kept: __LINE__ unchanged)."""
    out, i = [], 0
    for s0, s1 in sorted(spans):
        out.append(text[i:s0])
        out.append(re.sub(r'[^\n]', '', text[s0:s1]))
        i = s1
    out.append(text[i:])
    return ''.join(out)


def forms_of(rel, rows):
    """forms_of_text on the tree file rel."""
    raw = open(os.path.join(ROOT, rel), encoding='latin-1', newline='').read()
    return forms_of_text(raw.replace('\r\n', '\n'), rows)


def forms_of_text(text, rows):
    """(text, {form name: (row keys, new text)}) for one file's measurable rows: one form per device (rows sharing
    the same spans, a pragma pair around several functions, are one form); with several, also `all` (every device
    removed). The spans are the scan's, on the text with LF line ends."""
    out, every, by_spans = {}, [], {}
    for k, r in enumerate(rows):
        if r['kind'] not in MEASURABLE or not r.get('spans'):
            continue
        spans = tuple(tuple(s) for s in sorted(r['spans']))
        if spans in by_spans:                  # one pragma pair around several functions: one form, several rows
            out[by_spans[spans]][0].append(row_key(r))
            continue
        name = f"r{k}_{re.sub(r'[^A-Za-z0-9]', '', r['kind'])}_{(r['address'] or 'x').replace('0x', '')}"
        by_spans[spans] = name
        out[name] = ([row_key(r)], without(text, r['spans']))
        every += r['spans']
    if len(out) > 1:
        out['all'] = ([k for v in out.values() for k in v[0]], without(text, list({tuple(s) for s in every})))
    return text, out


def cmd_forms(a):
    rel = a.file.replace('\\', '/')
    rows = [r for r in (load('scan') or {}).get('rows', []) if r['file'] == rel]
    text, forms = forms_of(rel, rows)
    for name, (keys, new) in forms.items():
        print(name, keys)
        import difflib
        for l in list(difflib.unified_diff(text.splitlines(), new.splitlines(), lineterm='', n=0))[2:12]:
            print('   ', l[:150])
    return 0


def pick_symbols(rows, rel):
    """(address, symbol) candidates for trybatch's per-batch function: the measured rows' (exact and biggest first),
    then the file's exact ledger addresses."""
    rs = sorted((r for r in rows if r['address']), key=lambda r: (-r['exact'], -r['size']))
    out = [(int(r['address'], 16), r['function']) for r in rs]
    out += [(x, '') for x in sorted(ledger().get(rel) or ())]
    seen, got = set(), []
    for x in out:
        if x[0] not in seen:
            seen.add(x[0])
            got.append(x)
    return got[:3]


def freeset_form(rel, rows, meas):
    """{'freeset': (keys, text)}: every device of rel measured `free` alone, removed together (what a source change
    would remove), or {} when fewer than one."""
    free = [r for r in rows if (meas.get(row_key(r)) or {}).get('verdict') == 'free' and r.get('spans')]
    if not free:
        return {}
    raw = open(os.path.join(ROOT, rel), encoding='latin-1', newline='').read().replace('\r\n', '\n')
    spans = list({tuple(s) for r in free for s in r['spans']})
    return {'freeset': ([row_key(r) for r in free], without(raw, spans))}


def measure_file(rel, rows, vms, logf, forms=None):
    import trybatch
    if forms is None:
        _, forms = forms_of(rel, rows)
    if not forms:
        return {}
    raw = open(os.path.join(ROOT, rel), encoding='latin-1', newline='').read()
    res, tries = None, 0
    cands = pick_symbols(rows, rel)
    while cands:
        addr, sym = cands[0]
        spec = {'hypothesis': f'budget-device audit: {rel} without each device', 'file': rel, 'symbol': sym,
                'address': addr, 'forms': {n: [(raw.replace('\r\n', '\n'), new)] for n, (_, new) in forms.items()},
                'flags': None, 'unit': None}
        args = argparse.Namespace(only=None, no_lease=False, vms=vms, slots=False, file_check=True, record=None)
        buf = io.StringIO()
        try:
            with contextlib.redirect_stdout(buf):
                res = trybatch.run_spec(args, spec, 'aud-' + re.sub(r'\W', '_', os.path.basename(rel)), None)
        except SystemExit as e:
            logf.write(f'{rel} (0x{addr:06X}): {e}{NL}{buf.getvalue()}{NL}')
            res = {'_error': str(e)[:200]}
            if 'no VM leased' in str(e):       # the pool is shared: wait (bounded) and try the same function again
                tries += 1
                if tries > 6:
                    return {'_novm': True}
                time.sleep(90)
                continue
            cands.pop(0)
            continue
        logf.write(f'== {rel}{NL}{buf.getvalue()}{NL}')
        cands.pop(0)
        if 'file_set' in res['results'].get('base', {}):
            break
    if res is None:
        return {'_error': 'no function to measure'}
    if '_error' in res:
        return res
    results = res['results']
    base = results.get('base', {})
    out = {}
    led = ledger().get(rel) or set()
    for name, (keys, _) in forms.items():
        m = results.get(name, {})
        if 'error' in m or 'base' not in results or 'error' in base:
            v = {'verdict': 'error', 'detail': (m.get('error') or base.get('error') or '')[:160]}
        else:
            lost = [a for s, a in m.get('file_lost', [])]
            gained = [a for s, a in m.get('file_gained', [])]
            v = {'verdict': 'holds' if lost else ('gains' if gained else 'free'), 'mixed': bool(lost and gained),
                 'lost': [f'0x{x:06X}' for x in sorted(set(lost))],
                 'lost_ledger': [f'0x{x:06X}' for x in sorted(set(lost) & led)],
                 'gained': [f'0x{x:06X}' for x in sorted(set(gained))],
                 'base_exact': len(base.get('file_set', ()))}
        v['form'] = name
        if name == 'freeset':
            v['rows'] = keys
            out['_freeset|' + rel] = v
            continue
        for k in keys:
            out.setdefault(k, v) if name == 'all' else out.__setitem__(k, v)
        if name == 'all':
            out['_all|' + rel] = v
    return out


def cmd_measure(a):
    scan = load('scan')
    if not scan:
        raise SystemExit('measure: run `budgetdev.py scan` first')
    kinds = set(a.kinds.split(',')) if a.kinds else set(MEASURABLE)
    tag = ('freeset-' if a.freeset else '') + '-'.join(sorted(kinds))
    meas = load_measures()
    mine = load('measure-' + tag, {})          # one file per kind set: parallel runs never overwrite each other
    files = {}
    for r in scan['rows']:
        if r['kind'] in kinds and r.get('spans') and r['file'].endswith('.cpp'):
            files.setdefault(r['file'], []).append(r)
    if a.files:
        want = {f.replace('\\', '/') for f in a.files}
        files = {f: v for f, v in files.items() if f in want}
    if a.freeset:
        files = {f: v for f, v in files.items() if freeset_form(f, v, meas)}
        if not a.redo:
            files = {f: v for f, v in files.items() if '_freeset|' + f not in meas}
    elif not a.redo:
        files = {f: v for f, v in files.items() if not all(row_key(r) in meas for r in v)}
    order = sorted(files, key=lambda f: -max((r['size'] for r in files[f] if r['exact']), default=0))
    if a.top:
        order = order[:a.top]
    os.makedirs(WORK, exist_ok=True)
    logp = os.path.join(WORK, f'measure-{tag}.log')
    print(f'measure: {len(order)} files; log {os.path.relpath(logp, ROOT)}', flush=True)
    with open(logp, 'a', encoding='utf-8') as logf:
        for k, rel in enumerate(order, 1):
            got = measure_file(rel, files[rel], a.vms, logf,
                               freeset_form(rel, files[rel], meas) if a.freeset else None)
            logf.flush()
            mine.update({(f'_all|{tag}|{rel}' if kk.startswith('_all|') else kk): v for kk, v in got.items()
                         if kk not in ('_error', '_novm')})
            if '_error' in got:
                mine[f'_error|{tag}|{rel}'] = {'verdict': 'error', 'detail': got['_error']}
            json.dump(mine, open(os.path.join(WORK, f'measure-{tag}.json'), 'w', encoding='utf-8'), indent=0)
            vs = [v['verdict'] for kk, v in got.items() if not kk.startswith('_') or kk.startswith('_freeset')]
            print(f'[{k}/{len(order)}] {rel}: ' + (', '.join(f'{x} {vs.count(x)}' for x in sorted(set(vs))) or
                                                    got.get('_error', 'nothing')), flush=True)
    return cmd_table(a)


# ---------------------------------------------------------------- client evidence and the table

EVIDENCE = {   # (file, function): client evidence named by a call (coordinator's, D246) that the scan cannot see
    ('app/shared/unknown_46c730.cpp', 'Land_Normalize'):
        "the client's own slot and copy-in bytes (v = s before the length test) at every call site (C9, run 2026-11-19b)",
}


def evidence_of(r):
    """Client evidence that supports keeping a device's form, from the row alone (empty: source unproven)."""
    if (r['file'], r['function']) in EVIDENCE:
        return EVIDENCE[(r['file'], r['function'])]
    if r['kind'] == 'copy-operator' and r.get('exact') and r.get('address'):
        return f"the client's own out-of-line operator= at {r['address']} (exact)"
    return ''


def cmd_table(a):
    scan = load('scan') or {'rows': []}
    meas = load_measures()
    rows = []
    for r in scan['rows']:
        r = {k: v for k, v in r.items() if k != 'spans'}
        m = meas.get(row_key(r))
        if m:
            r['measure'] = m
        ev = evidence_of(r)
        if m and m['verdict'] == 'holds':
            r['status'] = 'budget-held, ' + ('client-evidenced: ' + ev if ev else 'source unproven')
        elif m and m['verdict'] in ('free', 'gains'):
            r['status'] = 'both exact (device not needed)' if m['verdict'] == 'free' else 'device costs pairs'
        elif m:
            r['status'] = 'measure failed'
        elif ev:
            r['status'] = 'client-evidenced: ' + ev
        elif r['kind'] in MEASURABLE:
            r['status'] = 'unmeasured'
        else:
            r['status'] = 'unmeasured (not mechanically removable)'
        rows.append(r)
    rows.sort(key=lambda r: (KINDS.index(r['kind']), -r['exact'], -r['size'], r['file'], r['lines'][0]))
    summ = {}
    for r in rows:
        s = summ.setdefault(r['kind'], {'rows': 0, 'exact': 0, 'measured': 0, 'holds': 0, 'free': 0, 'gains': 0,
                                        'error': 0, 'evidenced': 0})
        s['rows'] += 1
        s['exact'] += r['exact']
        if 'measure' in r:
            s['measured'] += 1
            s[r['measure']['verdict']] += 1
            s['evidenced'] += r['status'].startswith('budget-held, client')
    held = sorted({(r['file'], x) for r in rows if r.get('measure', {}).get('verdict') == 'holds'
                   for x in r['measure'].get('lost_ledger', [])})
    freesets = {k.split('|', 1)[1]: v for k, v in meas.items() if k.startswith('_freeset|')}
    summary = {'rows': len(rows), 'kinds': summ, 'held_pairs': len(held),
               'freesets': {f: {'verdict': v['verdict'], 'rows': len(v.get('rows', [])), 'lost': v.get('lost', []),
                                'gained': v.get('gained', [])} for f, v in sorted(freesets.items())}}
    with open(OUT_JSON, 'w', encoding='utf-8', newline='\n') as f:
        head = json.dumps({'summary': summary}, indent=1, sort_keys=True)
        f.write(head[:-2] + ',\n "rows": [\n')
        f.write(',\n'.join('  ' + json.dumps(r, sort_keys=True) for r in rows))
        f.write('\n ]\n}\n')
    md = ['# Budget devices', '',
          'Generated by `tools/re/budgetdev.py` (never edit by hand): every function of app/ held by a VC6 inline-budget '
          'device (dead reads, `#pragma auto_inline`, user-written copy operators, invented inline layers, class views, '
          'interim forwarders, statement order), and for the mechanically removable kinds the file compiled without the '
          'device (trybatch.py --file-check). The kinds and verdicts are in the tool\'s docstring; rows in '
          'budget-devices.json.', '',
          f"Rows: {len(rows)}. Ledger pairs a measured device holds: {len(held)}.", '',
          '| Kind | Rows | In exact functions | Measured | Holds pairs | Not needed | Costs pairs | Failed | Client-evidenced |',
          '|---|---|---|---|---|---|---|---|---|']
    for k in KINDS:
        if k in summ:
            s = summ[k]
            md.append(f"| {k} | {s['rows']} | {s['exact']} | {s['measured']} | {s['holds']} | {s['free']} | {s['gains']} "
                      f"| {s['error']} | {s['evidenced']} |")
    md += ['', '## Measured', '', '| Kind | File | Function | Address | Size | Lines | Verdict | Pairs lost without it |',
           '|---|---|---|---|---|---|---|---|']
    for r in rows:
        if 'measure' in r:
            m = r['measure']
            md.append(f"| {r['kind']} | {r['file']} | {r['function'][:40]} | {r['address'] or '-'} | {r['size']} | "
                      f"{', '.join(map(str, r['lines'][:6]))} | {r['status']} | {' '.join(m.get('lost', [])[:8]) or '-'} |")
    if freesets:
        md += ['', '## Free sets (per file, every device measured not needed alone, removed together)', '',
               '| File | Devices | Verdict | Lost | Gained |', '|---|---|---|---|---|']
        md += [f"| {f} | {len(v.get('rows', []))} | {v['verdict']} | {' '.join(v.get('lost', [])) or '-'} | "
               f"{' '.join(v.get('gained', [])) or '-'} |" for f, v in sorted(freesets.items())]
    md += ['', '## Unmeasured, by kind (exact functions first; every row is in the .json)', '',
           '| Kind | File | Function | Address | Size | Lines | Found |', '|---|---|---|---|---|---|---|']
    for r in rows:
        if 'measure' not in r:
            md.append(f"| {r['kind']} | {r['file']} | {r['function'][:40].replace('|', '/')} | {r['address'] or '-'}"
                      f"{'' if r['exact'] else ' (not exact)'} | {r['size']} | {', '.join(map(str, r['lines'][:6]))} | "
                      f"{r['found'][:60].replace('|', '/')} |")
    with open(OUT_MD, 'w', encoding='utf-8', newline='\n') as f:
        f.write(NL.join(md) + NL)
    print(f'table: {len(rows)} rows, {sum(s["measured"] for s in summ.values())} measured, '
          f'{sum(s["holds"] for s in summ.values())} hold pairs ({len(held)} ledger pairs), '
          f'{sum(s["free"] for s in summ.values())} not needed; {os.path.relpath(OUT_MD, ROOT)}')
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    sub.add_parser('scan')
    f = sub.add_parser('forms')
    f.add_argument('file')
    m = sub.add_parser('measure')
    m.add_argument('files', nargs='*')
    m.add_argument('--top', type=int, default=0, help='only the N files with the biggest held exact function')
    m.add_argument('--vms', type=int, default=2, help='VMs per file batch (trybatch --vms)')
    m.add_argument('--kinds', help='comma list of kinds to measure (default: dead-read,auto-inline)')
    m.add_argument('--redo', action='store_true', help='measure files already measured again')
    m.add_argument('--freeset', action='store_true',
                   help='per file, every device measured free alone removed together (the source change to make)')
    sub.add_parser('table')
    a = ap.parse_args(argv)
    return {'scan': cmd_scan, 'forms': cmd_forms, 'measure': cmd_measure, 'table': cmd_table}[a.cmd](a)


if __name__ == '__main__':
    raise SystemExit(main())
