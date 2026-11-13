"""Type-aware rename of a virtual slot across one class hierarchy (P-L12-1, D239).

    python tools/re/slotrename.py --class GadgetClass --slot Unknown_08=New_Name [--slot ...]
                                  [--tree DIR] [--write] [--strict]

A slot name such as `Unknown_08` is declared by many unrelated classes, so a text replace renames
the wrong ones. This tool renames only the occurrences that belong to CLASS or a class derived from
it (directly or not), deciding each one by the class of the object it is used on:

  * a declaration inside a class body: renamed when that class is in the hierarchy;
  * `Q::slot` (a definition, a qualified call, a mention in a comment): renamed when Q is;
  * `expr->slot` / `expr.slot`: the static type of expr is resolved from the source: `this`, a
    variable, parameter or member (the nearest declaration before the use in the same file, then
    the members of the enclosing class and its bases, then a declaration anywhere that is the only
    one of that name), a call's declared return type, a cast `((T *)x)`, an element `x[i]` of a
    `T *x[N]` / `Container<T *>`; `RefCountPtr<T>` and the like resolve to T;
  * an unqualified `slot(...)`: the enclosing member function's class.

What cannot be resolved is listed as `UNRESOLVED file:line` and left as it is (`--strict`: exit 1
when any is left). A missed or wrong rename does not compile (the old name is gone from the
hierarchy, the new one exists only there), so a build checks the result; a class deriving from the
hierarchy that the scan does not see (a macro-made base) would keep the old name as a new virtual:
the report lists every non-hierarchy class that declares the old name and derives from nothing the
scan knows, to look at. Scans app/, stubs/, programs/, tests/ (*.h, *.hpp, *.cpp, *.inl); comments
and strings are only touched in their `Q::slot` form. Without --write it prints what it would do.
"""
import argparse
import os
import re
import sys

DIRS = ('app', 'stubs', 'programs', 'tests')
EXTS = ('.h', '.hpp', '.cpp', '.inl')
WRAPPERS = {'RefCountPtr', 'RefPtr', 'auto_ptr', 'SmartPtr', 'const_iterator', 'iterator'}
KEYWORDS = {'return', 'new', 'delete', 'else', 'case', 'goto', 'throw', 'sizeof', 'const', 'virtual',
            'static', 'inline', 'unsigned', 'signed', 'struct', 'class', 'typename', 'public', 'private',
            'protected', 'operator', 'if', 'while', 'for', 'switch', 'do', 'using', 'typedef', 'enum',
            'namespace', 'template', 'friend', 'extern', 'volatile', 'register', 'explicit'}


def mask(text):
    """The text with comments and string / char literals blanked (same length, newlines kept)."""
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == '/' and i + 1 < n and text[i + 1] == '/':
            j = text.find('\n', i)
            j = n if j < 0 else j
        elif c == '/' and i + 1 < n and text[i + 1] == '*':
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
        elif c in '"\'':
            j = i + 1
            while j < n and text[j] != c and text[j] != '\n':
                j += 2 if text[j] == '\\' else 1
            j = min(j + 1, n)
        else:
            i += 1
            continue
        for k in range(i, j):
            if out[k] != '\n':
                out[k] = ' '
        i = j
    return ''.join(out)


def match_back(m, j, open_c, close_c):
    """Index of the bracket opening the one closing at m[j], or -1."""
    depth = 0
    while j >= 0:
        if m[j] == close_c:
            depth += 1
        elif m[j] == open_c:
            depth -= 1
            if depth == 0:
                return j
        j -= 1
    return -1


def match_fwd(m, j, open_c, close_c):
    depth = 0
    while j < len(m):
        if m[j] == open_c:
            depth += 1
        elif m[j] == close_c:
            depth -= 1
            if depth == 0:
                return j
        j += 1
    return -1


CLASS_RE = re.compile(r'\b(?:class|struct)\s+(\w+)\s*(?:final\s*)?(:[^{;()]*)?\{')
DEF_RE = re.compile(r'\b(\w+)\s*::\s*~?\w+\s*\([^;{}]*\)\s*(?:const\s*)?(?::[^{;]*)?\{')
ITER_RE = re.compile(r'<\s*(?:const\s+)?(\w+)\s*\*?\s*>\s*::\s*(?:const_)?iterator\s+(\w+)')
DECL_RE = re.compile(r'(?<![\w.>:])(?:const\s+)?(\w+)(\s*<[^;(){}<>]*(?:<[^;(){}<>]*>[^;(){}<>]*)*>)?\s*((?:const\s*)?[*&]+\s*(?:const\s+)?|\s+)(\w+)\s*(\[[^\]]*\]\s*)?([;,=)(\[:])')


def template_arg(targs):
    m = re.search(r'<\s*(?:const\s+)?(\w+)', targs or '')
    return m.group(1) if m else None


class Source:
    def __init__(self, path, rel):
        self.path, self.rel = path, rel
        with open(path, 'r', encoding='latin-1', newline='') as f:
            self.text = f.read()
        self.m = mask(self.text)
        self.lines = [0]
        for k, c in enumerate(self.text):
            if c == '\n':
                self.lines.append(k + 1)
        self.classes = []   # (name, open, close)
        self.defs = []      # (class, open, close): out-of-line member function bodies
        self.decls = []     # (offset, name, type, element type, is_function)

    def line(self, off):
        lo, hi = 0, len(self.lines) - 1
        while lo < hi:
            mid = (lo + hi + 1) // 2
            if self.lines[mid] <= off:
                lo = mid
            else:
                hi = mid - 1
        return lo + 1


class Index:
    def __init__(self, root):
        self.root = root
        self.files = []
        for d in DIRS:
            for base, _, names in os.walk(os.path.join(root, d)):
                for nm in sorted(names):
                    if nm.lower().endswith(EXTS):
                        p = os.path.join(base, nm)
                        self.files.append(Source(p, os.path.relpath(p, root).replace('\\', '/')))
        self.bases = {}      # class -> set of base names
        self.members = {}    # class -> {name: (type, element type)}
        self.globals = {}    # name -> set of (type, element type)
        self.by_name = {}    # file base name (lower case) -> [Source]
        for s in self.files:
            self.by_name.setdefault(os.path.basename(s.rel).lower(), []).append(s)
            self._scan(s)

    def _scan(self, s):
        m = s.m
        for c in CLASS_RE.finditer(m):
            name, bases = c.group(1), c.group(2) or ''
            close = match_fwd(m, c.end() - 1, '{', '}')
            s.classes.append((name, c.end() - 1, close if close >= 0 else len(m)))
            bl = self.bases.setdefault(name, set())
            for b in bases[1:].split(','):
                b = re.sub(r'\b(public|protected|private|virtual)\b', '', b).strip()
                ident = re.match(r'(?:\w+\s*::\s*)*(\w+)', b)
                if ident:
                    bl.add(ident.group(1))
        for d in DEF_RE.finditer(m):
            close = match_fwd(m, d.end() - 1, '{', '}')
            s.defs.append((d.group(1), d.end() - 1, close if close >= 0 else len(m)))
        s.includes = [x.replace('\\', '/').split('/')[-1].lower() for x in re.findall(r'#\s*include\s*"([^"]+)"', s.text)]
        for d in ITER_RE.finditer(m):
            s.decls.append((d.start(2), d.group(2), 'iterator', d.group(1), False))
        for d in DECL_RE.finditer(m):
            typ, targs, name, arr, nxt = d.group(1), d.group(2), d.group(4), d.group(5), d.group(6)
            if typ in KEYWORDS or name in KEYWORDS or typ.isdigit():
                continue
            ptr = d.group(3).strip()
            elem = None
            if typ in WRAPPERS:
                typ = template_arg(targs) or typ
            elif targs:
                elem = template_arg(targs)
            if arr:
                elem = typ
            is_fn = nxt == '('
            if not ptr and not targs and not arr and nxt in ')(':
                # `a b)` / `a b(`: a parameter or a function only with a type-looking first word
                if not typ[:1].isupper():
                    continue
            s.decls.append((d.start(4), name, typ, elem, is_fn))
            cls = self.enclosing_class(s, d.start(4))
            entry = (typ, elem)
            if cls:
                self.members.setdefault(cls, {}).setdefault(name, entry)
            self.globals.setdefault(name, set()).add(entry)

    @staticmethod
    def enclosing_class(s, off):
        best = None
        for name, o, c in s.classes:
            if o < off < c and (best is None or o > best[1]):
                best = (name, o)
        return best[0] if best else None

    def enclosing_def(self, s, off):
        best = None
        for name, o, c in s.defs:
            if o < off < c and (best is None or o > best[1]):
                best = (name, o)
        return best[0] if best else None

    def derived(self, root):
        h = {root}
        grew = True
        while grew:
            grew = False
            for c, bs in self.bases.items():
                if c not in h and bs & h:
                    h.add(c)
                    grew = True
        return h

    def ancestors(self, cls):
        seen, todo = [], [cls]
        while todo:
            c = todo.pop()
            if c in seen:
                continue
            seen.append(c)
            todo.extend(sorted(self.bases.get(c, ())))
        return seen

    def member(self, cls, name):
        for c in self.ancestors(cls):
            e = self.members.get(c, {}).get(name)
            if e:
                return e
        return None

    def lookup(self, s, off, name, want_fn=False):
        """(type, element type) of `name` at offset off of s, or None."""
        best = None
        for o, n, typ, elem, is_fn in s.decls:
            if n == name and o < off and is_fn == want_fn:
                best = (typ, elem)
        if best and not want_fn:
            return best
        cls = self.enclosing_def(s, off) or self.enclosing_class(s, off)
        if cls:
            e = self.member(cls, name)
            if e:
                return e
        if best:
            return best
        seen, todo = set(), list(s.includes)
        while todo and len(seen) < 200:
            inc = todo.pop(0)
            if inc in seen:
                continue
            seen.add(inc)
            found = set()
            for h in self.by_name.get(inc, ()):
                found |= {(typ, elem) for o, n, typ, elem, is_fn in h.decls if n == name and is_fn == want_fn}
                todo.extend(h.includes)
            if len({f[0] for f in found}) == 1:
                return next(iter(found))
        g = self.globals.get(name, set())
        types = {t for t, _ in g}
        if len(types) == 1:
            return next(iter(g))
        return None

    def object_type(self, s, j):
        """Static class of the expression ending just before m[j] (j: index of '-' of '->' or of '.')."""
        m = s.m
        k = j - 1
        while k >= 0 and m[k].isspace():
            k -= 1
        if k < 0:
            return None
        if m[k] == ')':
            o = match_back(m, k, '(', ')')
            if o < 0:
                return None
            inner = m[o + 1:k]
            deref = re.match(r'\s*\*\s*(\w+)\s*$', inner)
            if deref:
                return self.var_type(s, o + 1 + deref.end(1), element=True)
            cast = re.match(r'\s*\(\s*(?:const\s+)?(\w+)\s*[*&]+\s*\)', inner)
            if cast:
                return cast.group(1)
            cast = re.match(r'\s*(?:static_cast|dynamic_cast|reinterpret_cast)\s*<\s*(?:const\s+)?(\w+)', inner)
            b = o - 1
            while b >= 0 and m[b].isspace():
                b -= 1
            e = b + 1
            while b >= 0 and (m[b].isalnum() or m[b] == '_'):
                b -= 1
            callee = m[b + 1:e]
            mc = re.search(r'(?:static_cast|dynamic_cast|reinterpret_cast)\s*<\s*(?:const\s+)?(\w+)', m[max(0, b - 60):o])
            if callee in ('static_cast', 'dynamic_cast', 'reinterpret_cast') or (not callee and mc and m[o - 1] == '>'):
                return mc.group(1) if mc else None
            if not callee:
                inner_end = k - 1
                while inner_end > o and m[inner_end].isspace():
                    inner_end -= 1
                return self.object_type(s, inner_end + 1) if inner_end > o else None
            prev = b
            while prev >= 0 and m[prev].isspace():
                prev -= 1
            if prev >= 1 and (m[prev] == '.' or m[prev - 1:prev + 1] == '->'):
                start = prev if m[prev] == '.' else prev - 1
                owner = self.object_type(s, start)
                if owner:
                    e = self.member(owner, callee)
                    return e[0] if e else None
                return None
            e = self.lookup(s, b + 1, callee, want_fn=True)
            return e[0] if e else None
        if m[k] == ']':
            o = match_back(m, k, '[', ']')
            if o < 0:
                return None
            e = self.var_type(s, o, element=True)
            return e
        return self.var_type(s, k + 1)

    def var_type(self, s, end, element=False):
        m = s.m
        b = end - 1
        while b >= 0 and m[b].isspace():
            b -= 1
        e = b + 1
        while b >= 0 and (m[b].isalnum() or m[b] == '_'):
            b -= 1
        name = m[b + 1:e]
        if not name:
            return None
        if name == 'this':
            return self.enclosing_def(s, e) or self.enclosing_class(s, e)
        prev = b
        while prev >= 0 and m[prev].isspace():
            prev -= 1
        if prev >= 1 and (m[prev] == '.' or m[prev - 1:prev + 1] == '->'):
            start = prev if m[prev] == '.' else prev - 1
            owner = self.object_type(s, start)
            ent = self.member(owner, name) if owner else None
        elif prev >= 1 and m[prev - 1:prev + 1] == '::':
            ent = None
        else:
            ent = self.lookup(s, b + 1, name)
        if not ent:
            return None
        return (ent[1] or ent[0]) if element else ent[0]


def plan(index, root, slots):
    """[(source, offset, old, new, why)] and [(source, offset, old)] unresolved."""
    h = index.derived(root)
    edits, unresolved = [], []
    pat = re.compile(r'\b(' + '|'.join(re.escape(o) for o in slots) + r')\b')
    for s in index.files:
        for mo in pat.finditer(s.text):
            old, off = mo.group(1), mo.start()
            masked = s.m[off] == ' ' and s.text[off] != ' '
            t = s.text
            k = off - 1
            while k >= 0 and t[k] in ' \t':
                k -= 1
            if k >= 1 and t[k - 1:k + 1] == '::':
                q = k - 2
                while q >= 0 and t[q] in ' \t':
                    q -= 1
                e = q + 1
                while q >= 0 and (t[q].isalnum() or t[q] == '_'):
                    q -= 1
                qual = t[q + 1:e]
                if qual in h:
                    edits.append((s, off, old, slots[old], 'qualified by ' + qual))
                continue
            if masked:
                continue
            m = s.m
            k = off - 1
            while k >= 0 and m[k].isspace():
                k -= 1
            if m[k] == '.' or (k >= 1 and m[k - 1:k + 1] == '->'):
                start = k if m[k] == '.' else k - 1
                typ = index.object_type(s, start)
                if typ is None:
                    unresolved.append((s, off, old))
                elif typ in h:
                    edits.append((s, off, old, slots[old], 'on a ' + typ))
                continue
            cls = index.enclosing_class(s, off)
            fn = index.enclosing_def(s, off)
            owner = fn or cls
            if owner is None:
                unresolved.append((s, off, old))
            elif owner in h:
                edits.append((s, off, old, slots[old], ('in ' if cls and not fn else 'called in ') + owner))
    return h, edits, unresolved


def suspects(index, h, slots):
    """Classes outside the hierarchy that declare an old slot name and have no base the scan knows."""
    out = []
    for c, mem in sorted(index.members.items()):
        if c in h:
            continue
        if any(o in mem for o in slots) and not any(b in index.bases for b in index.bases.get(c, ())) and index.bases.get(c):
            out.append(c)
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--class', dest='cls', required=True, help='the class that introduces the slot')
    ap.add_argument('--slot', action='append', required=True, help='OLD=NEW (repeatable)')
    ap.add_argument('--tree', default=os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
    ap.add_argument('--write', action='store_true', help='apply the renames (default: print them)')
    ap.add_argument('--strict', action='store_true', help='exit 1 when anything is left unresolved')
    ap.add_argument('--verbose', action='store_true', help='print every rename with its reason')
    a = ap.parse_args(argv)
    slots = {}
    for sl in a.slot:
        old, sep, new = sl.partition('=')
        if not sep or not re.fullmatch(r'\w+', old) or not re.fullmatch(r'\w+', new):
            ap.error('--slot takes OLD=NEW: ' + sl)
        slots[old] = new
    root = os.path.abspath(a.tree)
    index = Index(root)
    if a.cls not in index.bases:
        print('slotrename: class %s not found under %s' % (a.cls, root))
        return 2
    h, edits, unresolved = plan(index, a.cls, slots)
    per_file = {}
    for s, off, old, new, why in edits:
        per_file.setdefault(s, []).append((off, old, new, why))
    for s in sorted(per_file, key=lambda x: x.rel):
        items = per_file[s]
        print('%s: %d' % (s.rel, len(items)))
        if a.verbose:
            for off, old, new, why in items:
                print('  %d: %s -> %s (%s)' % (s.line(off), old, new, why))
        if a.write:
            text = s.text
            for off, old, new, _ in sorted(items, reverse=True):
                text = text[:off] + new + text[off + len(old):]
            with open(s.path, 'w', encoding='latin-1', newline='') as f:
                f.write(text)
    for s, off, old in unresolved:
        print('UNRESOLVED %s:%d %s' % (s.rel, s.line(off), old))
    for c in suspects(index, h, slots):
        print('CHECK %s: declares an old name, derives from a class the scan does not know' % c)
    print('slotrename: %d classes in %s\'s hierarchy, %d renamed in %d files, %d unresolved%s' % (
        len(h), a.cls, len(edits), len(per_file), len(unresolved), '' if a.write else ' (dry run: --write applies)'))
    return 1 if a.strict and unresolved else 0


if __name__ == '__main__':
    sys.exit(main())
