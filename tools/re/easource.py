"""Indexes EA's published Westwood sources: which function each string literal or assert is in.

The parser is deliberately rough (brace depth plus the text before each top-level `{`), which is
enough to attribute string literals to `Class::Method` in Westwood's code, whose formatting is
consistent. Comments and preprocessor lines are stripped first so they do not unbalance braces.
"""
import os
import re

EA_ROOT = r'C:\EnBDevelopment\EA'

# Library copies the matcher knows about: (tag, path under EA_ROOT, library name).
LIBRARIES = [
    ('R', r'CnC_Renegade\Code\wwlib', 'wwlib'),
    ('R', r'CnC_Renegade\Code\ww3d2', 'ww3d2'),
    ('R', r'CnC_Renegade\Code\WWMath', 'wwmath'),
    ('R', r'CnC_Renegade\Code\WWAudio', 'wwaudio'),
    ('R', r'CnC_Renegade\Code\wwdebug', 'wwdebug'),
    ('R', r'CnC_Renegade\Code\wwsaveload', 'wwsaveload'),
    ('R', r'CnC_Renegade\Code\wwnet', 'wwnet'),
    ('R', r'CnC_Renegade\Code\wwphys', 'wwphys'),
    ('R', r'CnC_Renegade\Code\wwutil', 'wwutil'),
    ('R', r'CnC_Renegade\Code\wwui', 'wwui'),
    ('R', r'CnC_Renegade\Code\wwbitpack', 'wwbitpack'),
    ('R', r'CnC_Renegade\Code\wwtranslatedb', 'wwtranslatedb'),
    ('R', r'CnC_Renegade\Code\WWOnline', 'wwonline'),
    ('R', r'CnC_Renegade\Code\BinkMovie', 'binkmovie'),
    ('G', r'CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas\WWLib', 'wwlib'),
    ('G', r'CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas\WW3D2', 'ww3d2'),
    ('G', r'CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas\WWMath', 'wwmath'),
    ('G', r'CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas\WWAudio', 'wwaudio'),
    ('G', r'CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas\WWDebug', 'wwdebug'),
    ('G', r'CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas\WWSaveLoad', 'wwsaveload'),
    ('Z', r'CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WWLib', 'wwlib'),
    ('Z', r'CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WW3D2', 'ww3d2'),
    ('Z', r'CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WWMath', 'wwmath'),
    ('Z', r'CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WWAudio', 'wwaudio'),
    ('Z', r'CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WWDebug', 'wwdebug'),
    ('Z', r'CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\WWSaveLoad', 'wwsaveload'),
    ('Z', r'CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas\wwshade', 'wwshade'),
]

SOURCE_EXT = ('.cpp', '.c', '.h', '.inl', '.hpp')
BS = chr(92)
LITERAL = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
ASSERTS = ('WWASSERT', 'WWRELEASE_ASSERT', 'assert', 'ASSERT', 'WWASSERT_PRINT',
           'DEBUG_ASSERTCRASH', 'TCR_ASSERT')
ASSERT_CALL = re.compile(r'\b(' + '|'.join(ASSERTS) + r')\s*\(')
FUNC_NAME = re.compile(r'([A-Za-z_~][\w]*(?:\s*::\s*~?[A-Za-z_][\w]*)*(?:\s*::\s*operator\s*[^\s(]+)?)\s*\([^;{}]*\)'
                       r'\s*(?:const)?\s*(?::[^{;]*)?$', re.S)
KEYWORDS = {'if', 'for', 'while', 'switch', 'catch', 'return', 'sizeof', 'do', 'else'}


def unescape(s):
    out, i = [], 0
    simple = {'n': '\n', 't': '\t', 'r': '\r', '0': '\0', '"': '"', "'": "'", BS: BS}
    while i < len(s):
        c = s[i]
        if c == BS and i + 1 < len(s):
            n = s[i + 1]
            if n in simple:
                out.append(simple[n]); i += 2; continue
            if n == 'x':
                m = re.match(r'[0-9a-fA-F]{1,2}', s[i + 2:])
                if m:
                    out.append(chr(int(m.group(), 16))); i += 2 + len(m.group()); continue
            out.append(n); i += 2; continue
        out.append(c); i += 1
    return ''.join(out)


def strip_comments(text):
    """Blank out comments and preprocessor lines, keeping offsets and string literals."""
    out = list(text)
    i, n = 0, len(text)
    line_start = True
    while i < n:
        c = text[i]
        if line_start and c in ' \t':
            i += 1; continue
        if line_start and c == '#':
            j = i
            while j < n and text[j] != '\n':
                if text[j] == BS and j + 1 < n and text[j + 1] == '\n':
                    j += 2; continue
                j += 1
            for k in range(i, j):
                if out[k] != '\n': out[k] = ' '
            i = j; continue
        line_start = False
        if c == '\n':
            line_start = True; i += 1; continue
        if c == '"' or c == "'":
            q = c; i += 1
            while i < n and text[i] != q and text[i] != '\n':
                i += 2 if text[i] == BS else 1
            i += 1; continue
        if text.startswith('//', i):
            j = text.find('\n', i)
            j = n if j < 0 else j
            for k in range(i, j): out[k] = ' '
            i = j; continue
        if text.startswith('/*', i):
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if out[k] != '\n': out[k] = ' '
            i = j; continue
        i += 1
    return ''.join(out)


def balanced_args(text, start):
    """The text of a parenthesised argument list beginning at text[start] == '('."""
    depth, i = 0, start
    while i < len(text):
        c = text[i]
        if c == '"':
            i += 1
            while i < len(text) and text[i] != '"':
                i += 2 if text[i] == BS else 1
        elif c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0:
                return text[start + 1:i]
        i += 1
    return None


def functions_in(text):
    """[(name, body_start, body_end)] for each top-level (or class-inline) function body."""
    clean = strip_comments(text)
    result = []
    depth = 0
    stack = []  # (kind, name, start)
    last_stmt_end = 0
    i, n = 0, len(clean)
    while i < n:
        c = clean[i]
        if c == '"' or c == "'":
            q = c; i += 1
            while i < n and clean[i] != q and clean[i] != '\n':
                i += 2 if clean[i] == BS else 1
            i += 1; continue
        if c == '{':
            head = clean[last_stmt_end:i].strip()
            kind, name = 'block', None
            in_func = any(k == 'func' for k, _, _ in stack)
            if not in_func:
                if re.match(r'^(template\s*<.*>\s*)?(class|struct|union|namespace|enum)\b', head, re.S) or \
                        re.search(r'\b(class|struct|union|enum)\s+\w*\s*(:[^{]*)?$', head, re.S):
                    kind = 'scope'
                    m = re.search(r'\b(?:class|struct|union|namespace)\s+(\w+)', head)
                    name = m.group(1) if m else None
                else:
                    m = FUNC_NAME.search(head)
                    if m:
                        fname = re.sub(r'\s+', '', m.group(1))
                        if fname.split('::')[-1] not in KEYWORDS:
                            scopes = [nm for k, nm, _ in stack if k == 'scope' and nm]
                            if '::' not in fname and scopes:
                                fname = '::'.join(scopes[-1:] + [fname])
                            kind, name = 'func', fname
            stack.append((kind, name, i))
            depth += 1
            last_stmt_end = i + 1
        elif c == '}':
            if stack:
                kind, name, start = stack.pop()
                if kind == 'func':
                    result.append((name, start, i))
            depth -= 1
            last_stmt_end = i + 1
        elif c == ';':
            last_stmt_end = i + 1
        i += 1
    return result


def strings_by_function(path):
    """{function name: set(strings)} for one source file; strings outside functions go to ''."""
    text = open(path, encoding='latin1').read()
    funcs = functions_in(text)
    clean = strip_comments(text)

    def owner(pos):
        best = None
        for name, s, e in funcs:
            if s <= pos <= e and (best is None or s > best[1]):
                best = (name, s)
        return best[0] if best else ''

    out = {}
    for m in LITERAL.finditer(clean):
        if clean[m.start()] != '"':
            continue
        s = unescape(m.group(1))
        out.setdefault(owner(m.start()), set()).add(s)
    # Preprocessor lines are blanked in `clean`; keep the literals of #defines (registry value
    # names and the like are often defined that way), unattributed.
    for line in re.findall(r'^[ \t]*#[ \t]*define\b.*$', text, re.M):
        for m in LITERAL.finditer(line.split('//')[0]):
            out.setdefault('', set()).add(unescape(m.group(1)))
    for m in ASSERT_CALL.finditer(clean):
        args = balanced_args(clean, m.end() - 1)
        if args:
            out.setdefault(owner(m.start()), set()).add(re.sub(r'\s+', ' ', args.strip()))
    return out


def walk_library(path):
    root = os.path.join(EA_ROOT, path)
    for dp, _, files in os.walk(root):
        for f in files:
            if f.lower().endswith(SOURCE_EXT):
                full = os.path.join(dp, f)
                yield os.path.relpath(full, root).replace(BS, '/'), full
