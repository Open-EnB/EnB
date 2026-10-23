"""Shows how a compiled function differs from client.exe's, instruction by instruction.

    python objdiff.py file.obj SYMBOL_SUBSTRING CLIENT_ADDRESS [--n INDEX]
    python objdiff.py file.obj --list

Both sides are disassembled; relocated fields in the object are shown as `REL`. Lines are aligned
with difflib on the instruction text (immediates that are relocations are masked on both sides).
"""
import argparse
import difflib
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import capstone  # noqa: E402
from clientimage import ClientImage  # noqa: E402
from objmatch import CoffObject, RELOC_SIZE, demangle  # noqa: E402

MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
HEX = re.compile(r'0x[0-9a-f]+')


def obj_lines(obj, sec, start, end):
    info = obj.sections[sec]
    body = obj.data[info['raw'] + start:info['raw'] + end]
    rel = set()
    for va, _, typ in info['relocs']:
        for k in range(RELOC_SIZE.get(typ, 4)):
            rel.add(va - start + k)
    out = []
    for ins in MD.disasm(body, 0):
        covered = any(o in rel for o in range(ins.address, ins.address + ins.size))
        text = f'{ins.mnemonic} {ins.op_str}'
        if covered or ins.mnemonic in ('call', 'jmp') and ins.size == 5:
            text = f'{ins.mnemonic} REL' if ins.mnemonic in ('call', 'jmp') else HEX.sub('REL', text)
        out.append((ins.address, ins.bytes.hex(), text))
    return out


def client_lines(img, addr, length):
    out = []
    for ins in img.disasm(addr, length + 64):
        text = f'{ins.mnemonic} {ins.op_str}'
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5:
            text = f'{ins.mnemonic} REL'
        elif re.search(r'0x[0-9a-f]{6,}', ins.op_str):  # absolute address: data, strings
            text = re.sub(r'0x[0-9a-f]{6,}', 'REL', text)
        out.append((ins.address, ins.bytes.hex(), text))
        if ins.address - addr >= length:
            break
    return out


def norm(text):
    return re.sub(r'\b(j\w+) 0x[0-9a-f]+', r'\1 <branch>', text)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('obj')
    ap.add_argument('symbol', nargs='?')
    ap.add_argument('address', nargs='?')
    ap.add_argument('--n', type=int, default=0, help='which match of SYMBOL to use')
    ap.add_argument('--list', action='store_true')
    a = ap.parse_args()
    obj = CoffObject(a.obj)
    funcs = obj.functions()
    if a.list or not a.symbol:
        for name, sec, s, e in funcs:
            print(f'{e - s:6d}  {demangle(name):50s} {name}')
        return
    cands = [f for f in funcs if a.symbol in f[0] or a.symbol in demangle(f[0])]
    if not cands:
        raise SystemExit('no such symbol')
    name, sec, s, e = cands[a.n]
    left = obj_lines(obj, sec, s, e)
    right = client_lines(ClientImage(), int(a.address, 16), e - s)
    print(f'{demangle(name)} ({e - s} bytes) vs client 0x{int(a.address, 16):06X}')
    sm = difflib.SequenceMatcher(None, [norm(t) for _, _, t in left], [norm(t) for _, _, t in right],
                                 autojunk=False)
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        for k in range(max(i2 - i1, j2 - j1)):
            l = left[i1 + k] if i1 + k < i2 else None
            r = right[j1 + k] if j1 + k < j2 else None
            mark = ' ' if op == 'equal' else '*'
            lt = f'{l[0]:5x} {l[2]}' if l else ''
            rt = f'{r[0]:06x} {r[2]}' if r else ''
            print(f'{mark} {lt:44.44s} | {rt}')
    print(f'similarity {sm.ratio():.2f}')


if __name__ == '__main__':
    main()
