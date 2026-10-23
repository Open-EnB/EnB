"""Imports EA's Westwood libraries into the repository, file by file and function by function,
keeping whatever reproduces client.exe best.

Evidence comes from tools/vc6/bytematch_all.sh: for every EA copy (R Renegade, G Generals,
Z Zero Hour) and every source file, the client functions it reproduces byte for byte.

    python importer.py plan     --evidence DIR     choose a base copy per library and a copy per file
    python importer.py assemble --evidence DIR     copy the chosen files into the repository
    python importer.py verify                      compile the repository's libraries, byte-match them
    python importer.py refine   --evidence DIR     swap files / splice functions from other copies
                                                   where that adds matches and loses none
    python importer.py report                      write docs/imports.md

State (what came from where, what was merged, what matches) is kept in docs/data/imports.json.
Imported files are not edited except by splicing whole function definitions from another copy,
and by the patches in PATCHES (each documented in docs/imports.md).
"""
import argparse
import collections
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import vc6  # noqa: E402
from clientimage import ClientImage  # noqa: E402
from easource import function_spans  # noqa: E402
from objmatch import match_objects  # noqa: E402

EA = r'C:\EnBDevelopment\EA'
SDK_INCLUDES = [r'C:\EnBDevelopment\SDK\directx8\include', r'C:\EnBDevelopment\SDK\radtools\include']
STATE = os.path.join(REPO, 'docs', 'data', 'imports.json')
WORK = os.path.join(os.environ.get('TEMP', '.'), 'enb_import')

COPY_ROOT = {
    'R': EA + r'\CnC_Renegade\Code',
    'G': EA + r'\CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas',
    'Z': EA + r'\CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas',
}
COPY_NAME = {'R': 'Renegade', 'G': 'Generals', 'Z': 'Zero Hour', 'reconstructed': 'reconstructed'}
COPY_COMMIT = {'R': 'CnC_Renegade@3e00c3a', 'G': 'CnC_Generals_Zero_Hour@0a05454',
               'Z': 'CnC_Generals_Zero_Hour@0a05454'}
LIB_DIR = {
    'wwdebug': {'R': 'wwdebug', 'G': 'WWDebug', 'Z': 'WWDebug'},
    'wwlib': {'R': 'wwlib', 'G': 'WWLib', 'Z': 'WWLib'},
    'wwmath': {'R': 'WWMath', 'G': 'WWMath', 'Z': 'WWMath'},
    'wwsaveload': {'R': 'wwsaveload', 'G': 'WWSaveLoad', 'Z': 'WWSaveLoad'},
    'wwaudio': {'R': 'WWAudio', 'G': 'WWAudio', 'Z': 'WWAudio'},
    'ww3d2': {'R': 'ww3d2', 'G': 'WW3D2', 'Z': 'WW3D2'},
    'wwshade': {'Z': 'wwshade'},
}
LIBS = list(LIB_DIR)
HEADER_EXT = ('.h', '.hpp', '.inl')

# Local changes to imported files. Each keeps the file's line count, so __LINE__ values (which
# end up in the code) don't move.
PATCHES = {
    ('wwdebug', 'wwdebug.h'): [(
        '#include "..\\..\\..\\..\\gameengine\\include\\common\\debug.h"',
        '#include "gamedebug.h"',
        "Zero Hour's wwdebug.h includes Generals' GameEngine debug.h; gamedebug.h gives the release "
        "build's definitions of the macros it uses (all compile to nothing)")],
}
# Files reconstructed by us from the client (no EA copy matches): `assemble` keeps the repository's
# version, and refine leaves them alone. Each entry: (library, file) -> what was done.
RECONSTRUCTED = {
    ('wwlib', 'rc4.cpp'): "E&B's RC4Class: a runtime-built initial key, the 8- and 16-byte key setups "
                          "inlined into Prepare_Key, inline assembly in the 16-byte setup and RC4(); "
                          "Print_State and operator= are Zero Hour's. All 5 functions byte-exact "
                          "(docs/library-matching.md §3.6)",
    ('wwlib', 'rc4.h'): "InitialKey / InitialKeyBuilt statics added; Prepare_Key_8bytes/16bytes removed",
}

# Text appended to imported files (after everything, so no existing line moves).
APPENDS = {
    ('wwlib', 'always.h'): ("""
/*
**	EnB import: Zero Hour's WW libraries use these macros from its always.h. In a release build
**	without the Generals memory pools they are plain operator new and an empty base class; these
**	are those definitions, for the Zero Hour files imported beside Renegade's always.h.
*/
#ifndef W3DNEW
#define MSGW3DNEW(MSG)					new
#define MSGW3DNEWARRAY(MSG)			new
#define W3DNEW							new
#define W3DNEWARRAY						new
#define W3DMPO_GLUE(ARGCLASS)
class W3DMPO { };
#endif
""", "Zero Hour's allocation macros (W3DNEW, W3DMPO_GLUE, W3DMPO) in their release form, appended "
     "for the Zero Hour files built against Renegade's always.h"),
}

NEW_FILES = {
    ('wwdebug', 'gamedebug.h'): '''/*
**	Earth & Beyond client (reconstruction)
**
**	Release-build definitions of the Generals debug macros that Zero Hour's wwdebug.h expects
**	from GameEngine\\Include\\Common\\debug.h. In Generals' release configuration (no
**	ALLOW_DEBUG_UTILS) every one of them compiles to nothing; these are those definitions.
**	E&B's own header is not known.
*/

#ifndef __GAMEDEBUG_H
#define __GAMEDEBUG_H

#define DEBUG_INIT(f)					((void)0)
#define DEBUG_SHUTDOWN()				((void)0)
#define DEBUG_LOG(m)					((void)0)
#define DEBUG_ASSERTLOG(c, m)			((void)0)
#define DEBUG_CRASH(m)					((void)0)
#define DEBUG_ASSERTCRASH(c, m)		((void)0)
#define BEGIN_PROFILE(uniqueid)
#define END_PROFILE(uniqueid, msg, howoftentolog, howoftentoreset)

#endif // __GAMEDEBUG_H
''',
}


# ------------------------------------------------------------------------------------------ evidence

def key_of(filename):
    return os.path.splitext(os.path.basename(filename))[0].lower()


def load_evidence(evdir):
    """{lib: {copy: {file key: {address: (symbol, name)}}}} from bytematch_all.sh's JSONs."""
    ev = {}
    for lib in LIBS:
        ev[lib] = {}
        for c in LIB_DIR[lib]:
            path = os.path.join(evdir, f'{c}-{lib}.json')
            if not os.path.exists(path):
                continue
            per = collections.defaultdict(dict)
            for r in json.load(open(path)):
                if r['status'] == 'exact':
                    per[key_of(r['object'])][r['addresses'][0]] = (r['symbol'], r['name'])
            ev[lib][c] = per
    return ev


def source_files(lib, copy):
    """{file key: (filename, full path)} for a copy's .cpp files."""
    d = os.path.join(COPY_ROOT[copy], LIB_DIR[lib][copy])
    return {key_of(f): (f, os.path.join(d, f)) for f in os.listdir(d) if f.lower().endswith('.cpp')}


def headers(lib, copy):
    d = os.path.join(COPY_ROOT[copy], LIB_DIR[lib][copy])
    return {f.lower(): (f, os.path.join(d, f)) for f in os.listdir(d) if f.lower().endswith(HEADER_EXT)}


# ------------------------------------------------------------------------------------------ state

def load_state():
    return json.load(open(STATE)) if os.path.exists(STATE) else {}


def save_state(state):
    os.makedirs(os.path.dirname(STATE), exist_ok=True)
    json.dump(state, open(STATE, 'w'), indent=1, sort_keys=True)


# ------------------------------------------------------------------------------------------ plan

def plan(ev):
    state = {}
    for lib in LIBS:
        copies = ev[lib]
        if not copies:
            continue
        total = {c: len(set().union(*[set(v) for v in copies[c].values()]) if copies[c] else set())
                 for c in copies}
        base = max(total, key=lambda c: (total[c], c == 'R'))
        files = {}
        keys = set().union(*[set(copies[c]) for c in copies])
        for k in sorted(keys):
            best = max(copies, key=lambda c: (len(copies[c].get(k, {})), c == base))
            if not copies[best].get(k):
                continue
            files[k] = {'copy': best, 'expected': {c: len(copies[c].get(k, {})) for c in copies},
                        'merged': []}
        state[lib] = {'base': base, 'totals': total, 'files': files}
        swaps = sum(1 for f in files.values() if f['copy'] != base)
        print(f"{lib:11s} base {base} ({', '.join(f'{c}={n}' for c, n in total.items())}); "
              f"{len(files)} files with evidence, {swaps} from another copy")
    return state


# ------------------------------------------------------------------------------------------ assemble

def dest(lib):
    return os.path.join(REPO, lib)


def apply_patches(lib, path, name):
    for (plib, pname), edits in PATCHES.items():
        if plib == lib and pname == name.lower():
            text = open(path, encoding='latin1', newline='').read()
            for old, new, _ in edits:
                if old not in text:
                    raise SystemExit(f'patch for {lib}/{name} no longer applies: {old}')
                text = text.replace(old, new)
            open(path, 'w', encoding='latin1', newline='').write(text)


def assemble(state):
    for lib, s in state.items():
        d = dest(lib)
        kept = {}
        for (rlib, rname) in RECONSTRUCTED:
            p = os.path.join(d, rname)
            if rlib == lib and os.path.exists(p):
                kept[rname] = open(p, 'rb').read()
        if os.path.isdir(d):
            shutil.rmtree(d)
        os.makedirs(d)
        base = s['base']
        hs = headers(lib, base)
        origin = {}
        for low, (name, path) in hs.items():
            shutil.copy2(path, os.path.join(d, name))
            origin[name] = base
        # Headers only another copy has (a file chosen from that copy may need them).
        for c in LIB_DIR[lib]:
            if c == base:
                continue
            for low, (name, path) in headers(lib, c).items():
                if low not in hs:
                    shutil.copy2(path, os.path.join(d, name))
                    origin[name] = c
                    hs[low] = (name, path)
        base_sources = source_files(lib, base)
        for k, f in s['files'].items():
            f['best'] = f.get('best', f['copy'])
            f['copy'] = base if k in base_sources else f['best']
            name, path = source_files(lib, f['copy'])[k]
            shutil.copy2(path, os.path.join(d, name))
            f['name'] = name
            f['merged'] = []
            f['headers_from'] = None
        # A source file the base copy lacks brings its own header(s) of the same name.
        for k, f in s['files'].items():
            if f['copy'] != base:
                for low, (name, path) in headers(lib, f['copy']).items():
                    if os.path.splitext(low)[0] == k:
                        for old in [n for n in os.listdir(d) if n.lower() == low]:
                            os.remove(os.path.join(d, old))
                        shutil.copy2(path, os.path.join(d, name))
                        origin[name] = f['copy']
        for name in os.listdir(d):
            apply_patches(lib, os.path.join(d, name), name)
            for (alib, aname), (text, _) in APPENDS.items():
                if alib == lib and aname == name.lower():
                    with open(os.path.join(d, name), 'a', newline='\r\n') as fp:
                        fp.write(text)
        for (nlib, nname), text in NEW_FILES.items():
            if nlib == lib:
                open(os.path.join(d, nname), 'w', newline='\r\n').write(text)
                origin[nname] = 'new'
        for rname, data in kept.items():
            for old in [n for n in os.listdir(d) if n.lower() == rname.lower()]:
                os.remove(os.path.join(d, old))
            open(os.path.join(d, rname), 'wb').write(data)
            origin[rname] = 'reconstructed'
            k = key_of(rname)
            if rname.lower().endswith('.cpp') and k in s['files']:
                s['files'][k]['copy'] = 'reconstructed'
                s['files'][k]['name'] = rname
        s['headers'] = origin
        print(f'{lib:11s} {len(s["files"])} sources, {len(origin)} headers')
    return state


# ------------------------------------------------------------------------------------------ verify

_img = None


def image():
    global _img
    if _img is None:
        _img = ClientImage()
    return _img


def tree_includes():
    return [dest(l) for l in LIBS if os.path.isdir(dest(l))] + SDK_INCLUDES


def compile_and_match(paths, tag):
    """Compile files from the repository; {file key: set(addresses)} plus compiler output."""
    out = os.path.join(WORK, tag)
    if os.path.isdir(out):
        shutil.rmtree(out)
    rc, text = vc6.compile_sources(paths, tree_includes(), out=out)
    objs = [os.path.join(out, f) for f in os.listdir(out)] if os.path.isdir(out) else []
    found = collections.defaultdict(set)
    for r in match_objects(image(), objs) if objs else []:
        if r['status'] == 'exact':
            found[key_of(r['object'])].add(r['addresses'][0])
    for p in paths:
        found.setdefault(key_of(p), set())
    return found, text


def verify(state, libs=None):
    for lib in libs or state:
        s = state[lib]
        paths = [os.path.join(dest(lib), f['name']) for f in s['files'].values()]
        found, text = compile_and_match(paths, f'verify-{lib}')
        errors = [l for l in text.splitlines() if ' error ' in l]
        total = set()
        short = 0
        for k, f in s['files'].items():
            f['in_tree'] = sorted(found.get(k, set()))
            total |= found.get(k, set())
            if len(f['in_tree']) < f['expected'].get(f['copy'], 0):
                short += 1
        s['in_tree_total'] = len(total)
        s['compile_errors'] = errors[:50]
        print(f'{lib:11s} in tree: {len(total)} client functions (best copy alone: '
              f'{max(s["totals"].values())}); {short} files below their copy; {len(errors)} errors')
    return state


# ------------------------------------------------------------------------------------------ refine

def splice(text, donor_text, names):
    """Replace each named function definition in `text` with the donor's. Overloads are paired
    by order of definition. Returns (new text, names spliced)."""
    spans = function_spans(text)
    dspans = function_spans(donor_text)
    done = []
    edits = []
    for name in names:
        mine = [s for s in spans if s[0] == name]
        theirs = [s for s in dspans if s[0] == name]
        if not mine or not theirs or len(mine) != len(theirs):
            continue
        for (_, h, _, e), (_, dh, _, de) in zip(mine, theirs):
            edits.append((h, e + 1, donor_text[dh:de + 1]))
        done.append(name)
    for h, e, new in sorted(edits, reverse=True):
        text = text[:h] + new + text[e:]
    return text, done


def try_file(lib, f, text):
    """Write `text` as the file, compile it, and return the set of matched addresses."""
    path = os.path.join(dest(lib), f['name'])
    open(path, 'w', encoding='latin1', newline='').write(text)
    found, _ = compile_and_match([path], f'try-{lib}')
    return found.get(key_of(f['name']), set())


def tree_totals(state):
    return {lib: s.get('in_tree_total', 0) for lib, s in state.items()}


def try_with_headers(state, lib, f, c, k, donor):
    """Swap a file and its same-named header(s) to copy c; keep it only if no library's total
    drops. Returns the file's matched set, or None (and restores everything) if rejected."""
    d = dest(lib)
    pairs = [(n, p) for low, (n, p) in headers(lib, c).items() if os.path.splitext(low)[0] == k]
    if not pairs:
        return None
    saved = {}
    for n, p in pairs:
        for old in [x for x in os.listdir(d) if x.lower() == n.lower()]:
            saved[old] = open(os.path.join(d, old), 'rb').read()
            os.remove(os.path.join(d, old))
        shutil.copy2(p, os.path.join(d, n))
    path = os.path.join(d, f['name'])
    original = open(path, 'rb').read()
    open(path, 'w', encoding='latin1', newline='').write(donor)
    before = tree_totals(state)
    trial = json.loads(json.dumps(state))
    verify(trial)
    after = tree_totals(trial)
    got = set(trial[lib]['files'][k].get('in_tree', []))
    if all(after[l] >= before[l] for l in before) and sum(after.values()) > sum(before.values()):
        for l in state:
            state[l]['in_tree_total'] = trial[l]['in_tree_total']
            for kk, ff in state[l]['files'].items():
                ff['in_tree'] = trial[l]['files'][kk].get('in_tree', [])
        return got
    for n, _ in pairs:
        os.remove(os.path.join(d, n))
    for old, data in saved.items():
        open(os.path.join(d, old), 'wb').write(data)
    open(path, 'wb').write(original)
    return None


def refine(state, ev, libs=None):
    for lib in libs or state:
        s = state[lib]
        gained = 0
        for k, f in sorted(s['files'].items()):
            if (lib, f['name'].lower()) in RECONSTRUCTED:
                continue
            path = os.path.join(dest(lib), f['name'])
            cur = set(f.get('in_tree', []))
            text = open(path, encoding='latin1', newline='').read()
            improved = True
            while improved:
                improved = False
                for c in sorted(ev[lib], key=lambda c: -len(set(ev[lib][c].get(k, {})) - cur)):
                    wanted = {a: v for a, v in ev[lib][c].get(k, {}).items() if a not in cur}
                    if not wanted or k not in source_files(lib, c):
                        continue
                    donor_path = source_files(lib, c)[k][1]
                    donor = open(donor_path, encoding='latin1', newline='').read()
                    # 1. the whole file from the other copy, if the current one is clearly worse
                    if c != f['copy'] and len(ev[lib][c][k]) > len(cur):
                        got = try_file(lib, f, donor)
                        if len(got) > len(cur):
                            gained += len(got) - len(cur)
                            print(f'  {lib}/{f["name"]}: whole file from {c}: {len(cur)} -> {len(got)}')
                            f['copy'], f['merged'], cur, text = c, [], got, donor
                            improved = True
                            break
                        # with the other copy's header(s) of the same name, if nothing else loses
                        got = try_with_headers(state, lib, f, c, k, donor)
                        if got is not None and len(got) > len(cur):
                            gained += len(got) - len(cur)
                            print(f'  {lib}/{f["name"]}: whole file and header from {c}: {len(cur)} -> {len(got)}')
                            f['copy'], f['merged'], f['headers_from'], cur, text = c, [], c, got, donor
                            improved = True
                            break
                        open(path, 'w', encoding='latin1', newline='').write(text)
                    # 2. function by function
                    names = sorted({v[1] for v in wanted.values()})
                    new_text, done = splice(text, donor, names)
                    if not done:
                        continue
                    got = try_file(lib, f, new_text)
                    if cur <= got and len(got) > len(cur):
                        accepted = done
                    else:
                        # all together didn't work: one at a time
                        accepted, got = [], cur
                        new_text = text
                        for n in done:
                            t2, d2 = splice(new_text, donor, [n])
                            if not d2:
                                continue
                            g2 = try_file(lib, f, t2)
                            if got <= g2 and len(g2) > len(got):
                                new_text, got = t2, g2
                                accepted.append(n)
                    open(path, 'w', encoding='latin1', newline='').write(new_text if accepted else text)
                    if accepted:
                        gained += len(got) - len(cur)
                        print(f'  {lib}/{f["name"]}: {len(accepted)} function(s) from {c}: '
                              f'{len(cur)} -> {len(got)} ({", ".join(accepted)[:120]})')
                        f['merged'] += [{'copy': c, 'function': n} for n in accepted]
                        cur, text = got, new_text
                        improved = True
                        break
            f['in_tree'] = sorted(cur)
        print(f'{lib:11s} refine gained {gained}')
    return state


# ------------------------------------------------------------------------------------------ report

def report(state):
    lines = ['# Imported Westwood libraries', '',
             'Generated by `tools/vc6/importer.py report` from `docs/data/imports.json`. Each library '
             'was imported from EA\'s published copies (R Renegade, G Generals, Z Zero Hour; commits '
             + ', '.join(f'{COPY_NAME[c]} `{COPY_COMMIT[c]}`' for c in 'RGZ') + '), file by file, '
             'keeping the copy whose compiled code matches client.exe best, then splicing in whole '
             'function definitions from other copies where that adds byte-exact functions without '
             'losing any. "In tree": client functions our own source reproduces byte for byte '
             '(compiled with VC6 SP4, `/MT /W3 /O2 /Ob2`). See docs/library-matching.md Â§3.5.', '']
    lines += ['## Summary', '', '| Library | Base copy | Source files | From another copy | Functions merged | In tree | Best single copy |',
              '|---|---|---|---|---|---|---|']
    for lib, s in state.items():
        files = s['files']
        other = sum(1 for f in files.values() if f['copy'] != s['base'])
        merged = sum(len(f['merged']) for f in files.values())
        lines.append(f"| {lib} | {COPY_NAME[s['base']]} | {len(files)} | {other} | {merged} | "
                     f"{s.get('in_tree_total', '?')} | {max(s['totals'].values())} |")
    lines += ['', '## Local changes', '']
    for (lib, name), edits in PATCHES.items():
        for _, _, why in edits:
            lines.append(f'* `{lib}/{name}`: {why}.')
    for (lib, name), why in RECONSTRUCTED.items():
        lines.append(f'* `{lib}/{name}`: **reconstructed** from the client. {why}.')
    for (lib, name), (_, why) in APPENDS.items():
        lines.append(f'* `{lib}/{name}`: {why}.')
    for (lib, name) in NEW_FILES:
        lines.append(f'* `{lib}/{name}`: new file (see its header comment).')
    for lib, s in state.items():
        lines += ['', f'## {lib}', '', f"Base copy: {COPY_NAME[s['base']]}. Headers: "
                  + ', '.join(f'{n} from {COPY_NAME.get(c, c)}' for c, n in
                              collections.Counter(s.get('headers', {}).values()).items()) + '.', '']
        if s.get('compile_errors'):
            lines += ['Compile errors in the tree (first few):', '']
            lines += [f'    {e.strip()[:160]}' for e in s['compile_errors'][:8]] + ['']
        lines += ['| File | From | In tree | R | G | Z | Merged functions |', '|---|---|---|---|---|---|---|']
        for k, f in sorted(s['files'].items()):
            exp = f['expected']
            merged = '; '.join(f"{m['function']} ({m['copy']})" for m in f['merged'])
            lines.append(f"| {f.get('name', k)} | {f['copy']}{'+h' if f.get('headers_from') else ''} | {len(f.get('in_tree', []))} | "
                         + ' | '.join(str(exp.get(c, '-')) for c in 'RGZ') + f' | {merged} |')
    open(os.path.join(REPO, 'docs', 'imports.md'), 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('docs/imports.md written')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['plan', 'assemble', 'verify', 'refine', 'report', 'all'])
    ap.add_argument('--evidence', help="bytematch_all.sh's work directory")
    ap.add_argument('--lib', action='append', help='limit verify/refine to these libraries')
    a = ap.parse_args()
    if a.command in ('plan', 'assemble', 'refine', 'all') and not a.evidence:
        raise SystemExit('--evidence is required')
    ev = load_evidence(a.evidence) if a.evidence else None
    if a.command in ('plan', 'all'):
        state = plan(ev)
        save_state(state)
    state = load_state()
    if a.command in ('assemble', 'all'):
        save_state(assemble(state))
    if a.command in ('verify', 'all'):
        save_state(verify(state, a.lib))
    if a.command in ('refine', 'all'):
        save_state(refine(state, ev, a.lib))
    if a.command in ('report', 'all'):
        report(state)


if __name__ == '__main__':
    main()
