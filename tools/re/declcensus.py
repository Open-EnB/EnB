#!/usr/bin/env python3
"""Declaration census of the C++ in the tree, by name certainty (decision D56).

    python tools/re/declcensus.py                 # E&B code and EA library code, summary
    python tools/re/declcensus.py --file PATH     # one file's declarations (debugging)

Counts every *declaration* in a file (not every use): types, enumerators, functions (definitions
and prototypes), members, globals, parameters and locals. Each one is scored on D56's scale from
the `// NAME n: evidence` tag on its declaring statement (the same tags tools/re/names.py reads):

  5  the client names it (D56: an untagged name that is not a placeholder counts as 5)
  4  EA source, or a structure-confirmed Net-7 / C# name
  3  built by convention from a client-named file or sibling
  2  describes what the code does
  1  neutral placeholder (a tag of 1, or an untagged `Unknown...` name, D10)

This is a lexical census, not a compiler: it tracks braces and parentheses, tells scopes apart
(namespace / class / enum / function body) and recognises the usual shapes of declarations. It
misses macro-generated declarations and mis-reads some template syntax; the error is the same on
every day's tree, which is what a progress chart needs. A definition outside its class
(`int Foo::Bar()`) takes the score of the tagged declaration `Bar` inside `Foo`, so a name is
scored once however many files restate it.

Imported EA library code (the directories in EA_DIRS) is counted separately as "EA source": it is
EA's naming, outside the scale.

Standard library only.
"""
import argparse
import os
import re
import subprocess
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
EB_DIRS = ('app/', 'stubs/', 'programs/', 'tests/')
EA_DIRS = ('ww3d2/', 'wwlib/', 'wwmath/', 'wwaudio/', 'wwdebug/', 'wwsaveload/', 'wwshade/', 'binkmovie/', 'libs/')
EXTS = ('.cpp', '.h', '.inl', '.c', '.hpp')

TAG = re.compile(r'//\s*NAME\s+([1-5])\s*(?:\[([^\]]+)\])?\s*:')
UNKNOWN = re.compile(r'Unknown')
TOKEN = re.compile(r'''
    (?P<id>[A-Za-z_~][A-Za-z_0-9]*)
  | (?P<num>0[xX][0-9A-Fa-f]+[A-Za-z]*|\d[\w.]*(?:[eE][+-]?\d+)?)
  | (?P<op>::|->|\+\+|--|<<|>>|<=|>=|==|!=|&&|\|\||\.\.\.|[-+*/%&|^!=<>]=?|[{}()\[\];,:?.~#])
  | (?P<nl>\n)
  | (?P<ws>[ \t\r\f\v]+)
  | (?P<other>.)
''', re.X)

BUILTIN = {'int', 'char', 'short', 'long', 'float', 'double', 'void', 'bool', 'unsigned', 'signed',
           'wchar_t', '__int64', '__int8', '__int16', '__int32', 'size_t'}
QUAL = {'const', 'volatile', 'static', 'register', 'extern', 'mutable', 'inline', 'virtual', 'explicit',
        'struct', 'class', 'enum', 'union', 'typename', 'friend', 'far', 'near', '__cdecl', '__stdcall',
        '__fastcall', '__declspec', 'WINAPI', 'CALLBACK', 'FAR', 'PASCAL'}
NOT_DECL = {'return', 'delete', 'goto', 'break', 'continue', 'throw', 'case', 'default', 'new', 'else', 'do',
            'sizeof', 'if', 'while', 'for', 'switch', 'try', 'catch', 'typedef', 'using', 'namespace',
            'public', 'private', 'protected', 'asm', '__asm', 'assert', 'this', 'operator'}
CONTROL = {'if', 'for', 'while', 'switch', 'catch'}


def strip_source(text):
    """Remove comments, strings, character literals and preprocessor lines; keep the line structure.
    Returns (cleaned text, {line: score of the NAME tag on it})."""
    tags = {}
    for i, line in enumerate(text.split('\n'), 1):
        if '//' in line:
            m = TAG.search(line)
            if m:
                tags[i] = (int(m.group(1)), m.group(2))
    out = []
    i, n = 0, len(text)
    at_line_start = True
    while i < n:
        c = text[i]
        if c == '/' and text.startswith('//', i):
            j = text.find('\n', i)
            i = n if j < 0 else j
        elif c == '/' and text.startswith('/*', i):
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append('\n' * text.count('\n', i, j))
            out.append(' ')
            i = j
        elif c == '"' or c == "'":
            j = i + 1
            while j < n and text[j] != c and text[j] != '\n':
                j += 2 if text[j] == '\\' else 1
            out.append('""' if c == '"' else "0")
            i = j + 1
        elif c == '#' and at_line_start:
            # preprocessor line with continuations
            j = i
            while True:
                k = text.find('\n', j)
                k = n if k < 0 else k
                if k > 0 and text[k - 1] == '\\' and k < n:
                    j = k + 1
                    out.append('\n')
                    continue
                if k > 0 and text[k - 1] == '\r' and k > 1 and text[k - 2] == '\\' and k < n:
                    j = k + 1
                    out.append('\n')
                    continue
                break
            i = k
        else:
            if c == '\n':
                at_line_start = True
            elif c not in ' \t\r':
                at_line_start = False
            out.append(c)
            i += 1
    return ''.join(out), tags


def tokenize(clean):
    toks = []
    line = 1
    for m in TOKEN.finditer(clean):
        k = m.lastgroup
        if k == 'nl':
            line += 1
        elif k == 'ws':
            continue
        elif k == 'other':
            continue
        else:
            toks.append((m.group(), line))
    return toks


def split_top(toks, sep=','):
    """Split a token list at top-level separators (outside (), [], {}, <>)."""
    parts, cur, depth = [], [], 0
    for t in toks:
        s = t[0]
        if s in '([{':
            depth += 1
        elif s in ')]}':
            depth -= 1
        elif s == '<':
            depth += 1
        elif s == '>':
            depth = max(0, depth - 1)
        elif s == '>>':
            depth = max(0, depth - 2)
        if s == sep and depth == 0:
            parts.append(cur)
            cur = []
        else:
            cur.append(t)
    parts.append(cur)
    return parts


def is_ident(s):
    return bool(s) and (s[0].isalpha() or s[0] in '_~') and s not in NOT_DECL


def param_name(ptoks):
    """The declared name of one parameter, or None (unnamed, void, ...)."""
    toks = list(ptoks)
    # cut a default value
    for i, t in enumerate(toks):
        if t[0] == '=':
            toks = toks[:i]
            break
    # function pointer: ret (*name)(args)
    for i, t in enumerate(toks):
        if t[0] == '(' and i + 2 < len(toks) and toks[i + 1][0] == '*' and is_ident(toks[i + 2][0]):
            return toks[i + 2]
    # strip array suffix
    while toks and toks[-1][0] == ']':
        j = len(toks) - 1
        while j >= 0 and toks[j][0] != '[':
            j -= 1
        toks = toks[:j] if j >= 0 else toks[:-1]
    ids = [t for t in toks if is_ident(t[0]) and t[0] not in QUAL]
    if not ids or toks and toks[-1][0] == '...':
        return None
    last = ids[-1]
    if last[0] in BUILTIN:
        return None
    if toks and toks[-1] is not last and toks[-1][0] not in ('[', ']'):
        # the last token is not the name (e.g. `const Foo &`): unnamed
        if toks[-1][0] in ('*', '&', '>', '::'):
            return None
    others = [t for t in ids[:-1]]
    ptr = any(t[0] in ('*', '&') for t in toks)
    if others or ptr and ids:
        if not others and ptr:
            return last
        return last
    return None


def params_of(group):
    """Parameter name tokens from the tokens between a function's parentheses."""
    res = []
    if not group:
        return res
    for p in split_top(group):
        nm = param_name(p)
        if nm:
            res.append(nm)
    return res


def match_close(toks, i, open_='(', close=')'):
    depth = 0
    for j in range(i, len(toks)):
        if toks[j][0] == open_:
            depth += 1
        elif toks[j][0] == close:
            depth -= 1
            if depth == 0:
                return j
    return len(toks) - 1


def func_info(toks):
    """(qualified name tokens, name token, params group) of a function declarator, or None."""
    n = len(toks)
    # operator: the name is `operator` plus its symbol (`operator()` has its own parentheses)
    for i, t in enumerate(toks):
        if t[0] == 'operator':
            j = i + 1
            if j + 1 < n and toks[j][0] == '(' and toks[j + 1][0] == ')':
                j += 2
            else:
                while j < n and toks[j][0] != '(':
                    j += 1
            if j < n and toks[j][0] == '(':
                k = match_close(toks, j)
                return ('operator' + ''.join(s[0] for s in toks[i + 1:j]), t[1], toks[j + 1:k], i, None)
            return None
    for i, t in enumerate(toks):
        if t[0] == '(':
            if i == 0:
                return None
            prev = toks[i - 1]
            if prev[0] in CONTROL:
                return None
            if prev[0] == ')' or prev[0] == '*' or prev[0] == '&':
                # `void (*cb)(int)`: a variable, not a function
                return None
            if not (prev[0][0].isalpha() or prev[0][0] in '_~'):
                return None
            k = match_close(toks, i)
            return (prev[0], prev[1], toks[i + 1:k], i - 1, None)
    return None


class Decl:
    __slots__ = ('kind', 'name', 'line', 'cls', 'score', 'path', 'tagged')

    def __init__(self, kind, name, line, cls, path):
        self.kind, self.name, self.line, self.cls, self.path = kind, name, line, cls, path
        self.score, self.tagged = None, False


class Census:
    def __init__(self, path, text):
        self.path = path
        clean, self.tags = strip_source(text)
        self.toks = tokenize(clean)
        self.decls = []
        self.stack = []       # [kind, name]  kind in ns, class, enum, func, block, init
        self.run()

    # -- scopes
    def scope(self):
        return self.stack[-1][0] if self.stack else 'ns'

    def cls(self):
        for k, nm in reversed(self.stack):
            if k == 'class':
                return nm
        return None

    def add(self, kind, tok, qual=None):
        d = Decl(kind, tok[0], tok[1], qual or self.cls(), self.path)
        self.decls.append(d)
        return d

    # -- driver
    def run(self):
        toks = self.toks
        n = len(toks)
        i = 0
        stmt = []
        paren = 0
        while i < n:
            t = toks[i]
            s = t[0]
            if s in ('(', '['):
                paren += 1
                stmt.append(t)
            elif s in (')', ']'):
                paren -= 1
                stmt.append(t)
            elif paren > 0:
                stmt.append(t)
            elif s == '{':
                self.open_brace(stmt, i)
                stmt = []
            elif s == '}':
                self.close_brace(stmt)
                stmt = []
                if i + 1 < n and toks[i + 1][0] == ';':
                    i += 1
            elif s == ';':
                self.statement(stmt)
                stmt = []
            elif s == ':' and stmt and stmt[-1][0] in ('public', 'private', 'protected') and not stmt[:-1]:
                stmt = []
            elif s == ':' and self.scope() == 'func' and stmt and stmt[0][0] in ('case', 'default'):
                stmt = []
            else:
                stmt.append(t)
            i += 1

    # -- braces
    def open_brace(self, stmt, i):
        sc = self.scope()
        toks = self.strip_template(stmt)
        words = [t[0] for t in toks]
        if sc in ('func', 'block', 'init'):
            if sc == 'init' or (words and words[-1] == '=') or (words and words[-1] in (',', '{', '(')):
                self.stack.append(['init', None])
                return
            # a control statement header, `for (int i = 0; ...) {`
            if words and words[0] in ('for',):
                self.for_header(toks)
            elif words and words[0] in ('catch',):
                pass
            else:
                self.local(toks)
            self.stack.append(['block', None])
            return
        if 'namespace' in words or (words[:2] == ['extern'] and len(words) <= 2):
            self.stack.append(['ns', None])
            return
        if 'enum' in words:
            idx = words.index('enum')
            nm = next((t for t in toks[idx + 1:] if is_ident(t[0]) and t[0] not in QUAL), None)
            if nm:
                self.add('type', nm)
            self.stack.append(['enum', nm[0] if nm else None])
            return
        kw = next((k for k, w in enumerate(words) if w in ('class', 'struct', 'union')), None)
        if kw is not None and '(' not in words[:kw + 3]:
            # name: first identifier after the keyword that is not a macro-like qualifier
            nm = None
            for t in toks[kw + 1:]:
                if t[0] in (':', '<'):
                    break
                if is_ident(t[0]) and t[0] not in QUAL:
                    nm = t
            if nm:
                self.add('type', nm)
                self.stack.append(['class', nm[0]])
                return
            self.stack.append(['class', None])
            return
        if words and words[-1] == '=':
            self.stack.append(['init', None])
            return
        info = func_info(toks) if '(' in words else None
        if info is not None:
            self.function(toks, info, definition=True)
            self.stack.append(['func', None])
            return
        if '=' in words or (words and words[0] not in NOT_DECL and '(' not in words and sc != 'class'):
            self.stack.append(['init', None])
            return
        self.stack.append(['block', None])

    def close_brace(self, stmt):
        if stmt and self.stack and self.stack[-1][0] == 'enum':
            self.enumerators(stmt)
        if self.stack:
            self.stack.pop()

    # -- statements
    def strip_template(self, toks):
        if toks and toks[0][0] == 'template' and len(toks) > 1 and toks[1][0] == '<':
            depth = 0
            for k, t in enumerate(toks):
                if t[0] == '<':
                    depth += 1
                elif t[0] == '>':
                    depth -= 1
                elif t[0] == '>>':
                    depth -= 2
                if depth <= 0 and k > 0:
                    return toks[k + 1:]
        return toks

    def statement(self, stmt):
        sc = self.scope()
        toks = self.strip_template(stmt)
        if not toks:
            return
        w0 = toks[0][0]
        if sc == 'enum':
            self.enumerators(toks)
            return
        if sc == 'init':
            return
        if sc in ('func', 'block'):
            if w0 == 'for':
                self.for_header(toks)
            else:
                self.local(toks)
            return
        # class or namespace / file scope
        if w0 in ('using', 'static_assert', 'public', 'private', 'protected', 'namespace'):
            return
        if w0 == 'friend':
            return
        if w0 == 'typedef':
            self.typedef(toks)
            return
        words = [t[0] for t in toks]
        if words[0] in ('class', 'struct', 'union', 'enum') and len(toks) <= 3 and '(' not in words:
            return      # forward declaration
        if '(' in words:
            info = func_info(toks)
            if info:
                # `Foo x(1);` at global scope reads as a prototype; good enough
                self.function(toks, info, definition=False)
                return
        self.variables(toks, 'member' if sc == 'class' else 'global')

    def typedef(self, toks):
        words = [t[0] for t in toks]
        # typedef ret (*Name)(args);
        for i, t in enumerate(toks):
            if t[0] == '(' and i + 2 < len(toks) and toks[i + 1][0] == '*' and is_ident(toks[i + 2][0]):
                self.add('type', toks[i + 2])
                return
        # typedef struct X {...} Y;  handled at the brace; the trailing declarator is a type name
        ids = [t for t in toks if is_ident(t[0]) and t[0] not in QUAL]
        if len(ids) >= 2:
            self.add('type', ids[-1])

    def enumerators(self, toks):
        for p in split_top(toks):
            if p and is_ident(p[0][0]):
                self.add('member', p[0])

    def function(self, toks, info, definition):
        name, line, group, idx, _ = info
        # qualified name: tokens before idx joined by ::
        qual = None
        j = idx
        if j >= 2 and toks[j - 1][0] == '::' and is_ident(toks[j - 2][0]):
            qual = toks[j - 2][0]
        nm = (name, line)
        if name.startswith('operator'):
            nm = (name, line)
        d = self.add('function', nm, qual if qual else None)
        d.cls = qual if qual else self.cls()
        for p in params_of(group):
            self.add('param', p, None)

    def variables(self, toks, kind):
        words = [t[0] for t in toks]
        # drop leading qualifiers and the type; each top-level declarator contributes one name
        parts = split_top(toks)
        for pi, p in enumerate(parts):
            if not p:
                continue
            # cut initialiser and bit-field
            cut = len(p)
            depth = 0
            for k, t in enumerate(p):
                if t[0] in '([{':
                    depth += 1
                elif t[0] in ')]}':
                    depth -= 1
                elif depth == 0 and t[0] in ('=', ':') :
                    cut = k
                    break
            q = p[:cut]
            while q and q[-1][0] == ']':
                j = len(q) - 1
                while j >= 0 and q[j][0] != '[':
                    j -= 1
                q = q[:j] if j >= 0 else q[:-1]
            # function-pointer member: ret (*name)(args)
            fp = None
            for k, t in enumerate(q):
                if t[0] == '(' and k + 2 < len(q) and q[k + 1][0] == '*' and is_ident(q[k + 2][0]):
                    fp = q[k + 2]
                    break
            if fp:
                self.add(kind, fp)
                continue
            ids = [t for t in q if is_ident(t[0]) and t[0] not in QUAL]
            if not ids:
                continue
            if pi == 0 and len(ids) < 2 and not any(t[0] in ('*', '&') for t in q):
                continue        # a lone identifier: a macro use such as DECLARE_FOO(x)
            nm = ids[-1]
            if nm[0] in BUILTIN:
                continue
            if q and q[-1][0] not in ('::',) and q[-1] is not nm:
                continue
            qual = None
            k = q.index(nm)
            if k >= 2 and q[k - 1][0] == '::' and is_ident(q[k - 2][0]):
                qual = q[k - 2][0]
            d = self.add(kind, nm, qual)
            if qual:
                d.cls = qual

    # -- function-body declarations
    def local(self, toks):
        if not toks:
            return
        w0 = toks[0][0]
        if w0 in NOT_DECL or w0 in CONTROL:
            return
        # type-ish prefix
        k = 0
        n = len(toks)
        saw_type = False
        while k < n and toks[k][0] in QUAL:
            k += 1
        if k >= n:
            return
        if not is_ident(toks[k][0]) and toks[k][0] not in BUILTIN:
            return
        # qualified type name, template args
        while k < n and (is_ident(toks[k][0]) or toks[k][0] in BUILTIN or toks[k][0] in QUAL):
            saw_type = True
            k += 1
            if k < n and toks[k][0] == '::':
                k += 1
                continue
            if k < n and toks[k][0] == '<':
                depth = 0
                while k < n:
                    if toks[k][0] == '<':
                        depth += 1
                    elif toks[k][0] == '>':
                        depth -= 1
                    elif toks[k][0] == '>>':
                        depth -= 2
                    k += 1
                    if depth <= 0:
                        break
                if k < n and toks[k][0] == '::':
                    k += 1
                    continue
            # a second identifier right away means: that was the type, the next is the name
            break
        # now at pointer/reference markers or the name; multi-word builtin types (unsigned long x)
        while k < n and (toks[k][0] in BUILTIN or toks[k][0] in QUAL):
            k += 1
        while k < n and toks[k][0] in ('*', '&', 'const'):
            k += 1
        if not saw_type or k >= n or not is_ident(toks[k][0]) or toks[k][0] in BUILTIN:
            return
        nxt = toks[k + 1][0] if k + 1 < n else ';'
        if nxt not in ('=', ';', ',', '[', '(', ':'):
            return
        # first declarator
        rest = toks[k:]
        # `Type name(args)`: a constructor-style declaration; `Type name(...)` after only a single type ident
        # is accepted only when the type is not a plain call (we required two identifiers already)
        for p in split_top(rest):
            if not p:
                continue
            ids = [t for t in p if is_ident(t[0])]
            j = 0
            while j < len(p) and p[j][0] in ('*', '&'):
                j += 1
            if j < len(p) and is_ident(p[j][0]):
                self.add('local', p[j])

    def for_header(self, toks):
        # for ( init ; cond ; step ) : the init may declare a variable
        if len(toks) < 3 or toks[1][0] != '(':
            return
        k = match_close(toks, 1)
        inner = toks[2:k]
        parts = split_top(inner, ';')
        if parts and parts[0]:
            self.local(parts[0])


# ---------------------------------------------------------------------------------------------

def score_all(censuses):
    """Fill in Decl.score / tagged for every declaration, across files."""
    table = {}      # (class, name) -> score
    glob = {}
    # pass 1: a tag on a declaration (the tag's line is inside the statement that declares it)
    for c in censuses:
        by_line = {}
        for d in c.decls:
            by_line.setdefault(d.line, []).append(d)
        for ln, (score, subject) in c.tags.items():
            cands = by_line.get(ln, [])
            if subject:
                cands = [d for d in cands if d.name == subject.split('::')[-1]]
            if not cands:
                # the tag may sit on the line after the declaration's name (multi-line header) or before
                for off in (-1, 1, -2, 2):
                    cands = [d for d in by_line.get(ln + off, []) if d.kind != 'param' and d.kind != 'local']
                    if cands:
                        break
            if not cands:
                continue
            d = next((x for x in cands if x.kind not in ('param', 'local')), cands[0])
            d.score, d.tagged = score, True
            table[(d.cls, d.name)] = score
            glob.setdefault(d.name, score)
    # pass 2
    for c in censuses:
        for d in c.decls:
            if d.tagged:
                continue
            sc = table.get((d.cls, d.name)) if d.cls else None
            if sc is not None and d.kind in ('function', 'member', 'global', 'type'):
                d.score, d.tagged = sc, True
                continue
            if UNKNOWN.search(d.name):
                d.score = 1
            else:
                d.score = 5
    return censuses


def file_list(dirs, ref=None):
    out = subprocess.run(['git', 'ls-tree', '-r', '--name-only', ref] if ref else ['git', 'ls-files'],
                         cwd=ROOT, capture_output=True, text=True, check=True).stdout.splitlines()
    return [p for p in out if p.startswith(dirs) and p.lower().endswith(EXTS)]


def census_files(read, paths):
    cs = []
    for p in paths:
        data = read(p)
        if data is None:
            continue
        cs.append(Census(p, data.decode('utf-8', 'replace')))
    return cs


def summarize(read, ref=None):
    """{'eb': {1..5: count}, 'untagged5': n, 'ea': n, 'kinds': {...}, 'files': n} for the tree."""
    eb_paths = file_list(EB_DIRS, ref)
    ea_paths = file_list(EA_DIRS, ref)
    eb = score_all(census_files(read, eb_paths))
    ea = census_files(read, ea_paths)
    counts = {s: 0 for s in (1, 2, 3, 4, 5)}
    kinds = {}
    untagged5 = 0
    for c in eb:
        for d in c.decls:
            counts[d.score] += 1
            kinds[d.kind] = kinds.get(d.kind, 0) + 1
            if d.score == 5 and not d.tagged:
                untagged5 += 1
    return {'eb': counts, 'untagged5': untagged5, 'ea': sum(len(c.decls) for c in ea), 'kinds': kinds,
            'files': len(eb_paths), 'ea_files': len(ea_paths)}


def read_worktree(path):
    try:
        with open(os.path.join(ROOT, path), 'rb') as f:
            return f.read()
    except OSError:
        return None


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--file', help='print one file\'s declarations')
    a = ap.parse_args()
    if a.file:
        c = score_all([Census(a.file, open(a.file, encoding='utf-8', errors='replace').read())])[0]
        for d in c.decls:
            print('%5d %-9s %-4s %s%s' % (d.line, d.kind, d.score, (d.cls + '::') if d.cls else '', d.name))
        return
    s = summarize(read_worktree)
    tot = sum(s['eb'].values())
    print('E&B code: %d files, %d declarations' % (s['files'], tot))
    for k in (5, 4, 3, 2, 1):
        print('  score %d: %6d  %5.1f%%' % (k, s['eb'][k], 100.0 * s['eb'][k] / tot if tot else 0))
    print('  untagged, counted as 5: %d' % s['untagged5'])
    print('  by kind:', ', '.join('%s %d' % kv for kv in sorted(s['kinds'].items())))
    print('EA source (%d files): %d declarations' % (s['ea_files'], s['ea']))


if __name__ == '__main__':
    sys.exit(main())
