"""Inventory of E&B's own code (the app region of client.exe): what each function is, where known.

    python app_inventory.py

For every function in the app region (0x401000-0x8C1B50; incremental-link thunk targets):
  file     the source file: the function references that file's __FILE__ string (asserts), so it
           was compiled from it. Functions with no assert stay unattributed, but neighbours in the
           same object usually are (the linker keeps an object's functions together).
  name     a "Class::Method" string the function references (profile labels and assert messages
           name the function they are in), when there is exactly one
  rtti     classes with RTTI type descriptors: their vtables (via the RTTI complete object
           locator) and the functions in them
  thunk    how many app functions are reached through the incremental-link table

Writes docs/data/app-functions.json and docs/app-inventory.md.
"""
import bisect
import collections
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from clientimage import ClientImage  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
DOCS = os.path.normpath(os.path.join(HERE, '..', '..', 'docs'))
APP_END = 0x8C1B50
SOURCE = re.compile(r'^e:\\build\\beta\\G\\(.+\.(cpp|h|inl))$', re.I)
CLASS_METHOD = re.compile(r'^\s*~?([A-Z][A-Za-z0-9_]+)::(~?[A-Za-z_][A-Za-z0-9_]*)(?![A-Za-z0-9_:])')


def normalise(path):
    parts = []
    for p in path.replace('/', '\\').lower().split('\\'):
        if p == '..':
            parts.pop()
        elif p not in ('.', ''):
            parts.append(p)
    return '\\'.join(parts)


def main():
    img = ClientImage()
    starts, thunks = img.function_starts()
    t0, t1, raw = img.section('.text')
    app = [s for s in starts if s < APP_END]
    targets = set(thunks.values())
    strings = img.strings(5)

    files = {va: normalise(m.group(1)) for va, s in strings.items() if (m := SOURCE.match(s))}
    names = {va: CLASS_METHOD.match(s).group(1) + '::' + CLASS_METHOD.match(s).group(2)
             for va, s in strings.items() if CLASS_METHOD.match(s)}
    refs = img.dword_refs(list(files) + list(names))

    def owner(site):
        i = bisect.bisect_right(starts, site) - 1
        return starts[i] if i >= 0 else None

    func_files = collections.defaultdict(set)
    func_names = collections.defaultdict(set)
    for va, sites in refs.items():
        for site in sites:
            f = owner(site)
            if f is None or f >= APP_END:
                continue
            if va in files:
                func_files[f].add(files[va])
            if va in names:
                func_names[f].add(names[va])

    # spread file attribution to unattributed functions between two functions of the same file
    ordered = sorted(app)
    attributed = {}
    for f in ordered:
        if len(func_files.get(f, ())) == 1:
            attributed[f] = (next(iter(func_files[f])), 'assert')
    keys = sorted(attributed)
    for i in range(len(keys) - 1):
        a, b = keys[i], keys[i + 1]
        if attributed[a][0] == attributed[b][0]:
            lo = bisect.bisect_right(ordered, a)
            hi = bisect.bisect_left(ordered, b)
            for f in ordered[lo:hi]:
                attributed.setdefault(f, (attributed[a][0], 'between'))

    # RTTI: type descriptors -> complete object locators -> vtables
    rtti = {}
    for va, s in strings.items():
        if s.startswith('.?AV') and '@std@@' not in s and not s.startswith('.?AV?$'):
            td = va - 8                       # TypeDescriptor: vftable ptr, spare, name
            col_refs = img.dword_refs_data(td) if hasattr(img, 'dword_refs_data') else None
            rtti[s[4:].split('@@')[0]] = td
    rdata_lo, rdata_hi, rdata_raw = img.section('.rdata')
    rdata = img.data[rdata_raw:rdata_raw + (rdata_hi - rdata_lo)]
    vtables = {}
    for cls, td in rtti.items():
        # CompleteObjectLocator: signature 0, offset, cdOffset, pTypeDescriptor, pClassHierarchy
        for m in re.finditer(re.escape(struct.pack('<I', td)), rdata):
            col = rdata_lo + m.start() - 12
            if m.start() < 12:
                continue
            sig, off, cd = struct.unpack_from('<III', rdata, m.start() - 12)
            if sig != 0 or off != 0:
                continue
            # the vtable is preceded by a pointer to its COL
            for n in re.finditer(re.escape(struct.pack('<I', col)), rdata):
                vt = rdata_lo + n.start() + 4
                slots = []
                for k in range(512):
                    o = n.start() + 4 + 4 * k
                    if o + 4 > len(rdata):
                        break
                    v = struct.unpack_from('<I', rdata, o)[0]
                    if not (t0 <= v < t1):
                        break
                    slots.append(thunks.get(v, v))
                vtables.setdefault(cls, []).append({'vtable': f'0x{vt:06X}', 'slots': [f'0x{s:06X}' for s in slots]})

    rows = []
    for f in ordered:
        e = {'address': f'0x{f:06X}', 'thunked': f in targets}
        if f in attributed:
            e['file'], e['file_evidence'] = attributed[f]
        if len(func_names.get(f, ())) == 1:
            e['name'] = next(iter(func_names[f]))
        elif func_names.get(f):
            e['names'] = sorted(func_names[f])
        rows.append(e)
    json.dump({'functions': rows, 'rtti_vtables': vtables}, open(os.path.join(DOCS, 'data', 'app-functions.json'), 'w', newline='\n'), indent=0)

    by_file = collections.Counter(r['file'] for r in rows if 'file' in r)
    by_dir = collections.Counter(r['file'].rsplit('\\', 1)[0] for r in rows if 'file' in r)
    named = [r for r in rows if 'name' in r]
    lines = ['# E&B app code: inventory (generated by tools/re/app_inventory.py)', '',
             f'Functions in the app region (0x401000-0x8C1B50, reached through the incremental-link table): '
             f'**{len(rows)}**.', '',
             f'* Attributed to a source file: **{sum(1 for r in rows if "file" in r)}** '
             f'({sum(1 for r in rows if r.get("file_evidence") == "assert")} by their own `__FILE__` reference, '
             f'the rest because they lie between two functions of the same file).',
             f'* Named by a unique `Class::Method` string they reference: **{len(named)}**.',
             f'* Classes with RTTI and a located vtable: **{len(vtables)}** '
             f'({sum(len(v[0]["slots"]) for v in vtables.values())} virtual-function slots).', '',
             '## By directory', '', '| Directory | Functions |', '|---|---|']
    for d, n in by_dir.most_common():
        lines.append(f'| `{d}` | {n} |')
    lines += ['', '## Largest source files (by attributed functions)', '', '| File | Functions |', '|---|---|']
    for fname, n in by_file.most_common(40):
        lines.append(f'| `{fname}` | {n} |')
    lines += ['', '## RTTI classes and their vtables', '', '| Class | vtable | Slots |', '|---|---|---|']
    for cls in sorted(vtables):
        for v in vtables[cls]:
            lines.append(f"| `{cls}` | {v['vtable']} | {len(v['slots'])} |")
    lines += ['', '## Named functions (sample)', '', '| Address | Name | File |', '|---|---|---|']
    for r in named[:80]:
        lines.append(f"| {r['address']} | `{r['name']}` | `{r.get('file', '')}` |")
    open(os.path.join(DOCS, 'app-inventory.md'), 'w', newline='\n', encoding='utf-8').write('\n'.join(lines) + '\n')
    print(f"{len(rows)} app functions; {sum(1 for r in rows if 'file' in r)} attributed to files; "
          f"{len(named)} named; {len(vtables)} RTTI classes with vtables")


if __name__ == '__main__':
    main()
