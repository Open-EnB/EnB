"""Loads the retail client.exe and answers the questions the matching tools ask of it.

Addresses are virtual addresses (image base 0x400000). The image is checked against the SHA-256
of the retail client so every address in the docs refers to the same bytes.
"""
import collections
import hashlib
import os
import re
import struct
import sys

CLIENT_SHA256 = '249f852496f341c00361be95b66e7c0becc2c39e7cba026f9227e38b602651c4'
DEFAULT_CLIENT = os.path.join(os.environ.get('ENB_RETAIL_INSTALL') or r'C:\Program Files (x86)\EA GAMES\Earth & Beyond',
                              'release', 'client.exe')


class ClientImage:
    def __init__(self, path=DEFAULT_CLIENT, check=True):
        self.data = open(path, 'rb').read()
        if check:
            sha = hashlib.sha256(self.data).hexdigest()
            if sha != CLIENT_SHA256:
                sys.exit(f'{path}: SHA-256 {sha} is not the retail client these tools are for')
        d = self.data
        pe = struct.unpack_from('<I', d, 0x3c)[0]
        count = struct.unpack_from('<H', d, pe + 6)[0]
        opt = struct.unpack_from('<H', d, pe + 20)[0]
        self.base = struct.unpack_from('<I', d, pe + 52)[0]
        self.sections = {}
        for i in range(count):
            name, vsize, va, rsize, raw = struct.unpack_from('<8sIIII', d, pe + 24 + opt + 40 * i)
            self.sections[name.rstrip(b'\0').decode()] = (self.base + va, vsize, raw, rsize)
        self._md = None

    def section(self, name):
        """(start VA, end VA, file offset) of a section's raw bytes."""
        va, vsize, raw, rsize = self.sections[name]
        return va, va + min(vsize, rsize), raw

    def offset(self, va):
        for start, vsize, raw, rsize in self.sections.values():
            if start <= va < start + rsize:
                return va - start + raw
        return None

    def u32(self, va):
        return struct.unpack_from('<I', self.data, self.offset(va))[0]

    def cstr(self, va, limit=512):
        o = self.offset(va)
        if o is None:
            return None
        e = self.data.find(b'\0', o, o + limit)
        return self.data[o:e].decode('latin1') if e > o else None

    def strings(self, min_len=4):
        """Every printable C string in .rdata and .data, as {VA: text}."""
        out = {}
        for name in ('.rdata', '.data'):
            start, end, raw = self.section(name)
            blob = self.data[raw:raw + (end - start)]
            for m in re.finditer(rb'[\x20-\x7e\t\r\n]{%d,}\x00' % min_len, blob):
                out[start + m.start()] = m.group()[:-1].decode('latin1')
        return out

    def incremental_thunks(self):
        """The incremental-link table at the start of .text: {thunk VA: target VA}.

        The client was linked with /INCREMENTAL, so nearly every function is reached through a
        five-byte `jmp rel32` stub. The table is a run of such stubs (padded with int3).
        """
        start, end, raw = self.section('.text')
        d = self.data
        out = {}
        va, o = start, raw
        misses = 0
        while va < end and misses < 64:
            if d[o] == 0xE9:
                target = va + 5 + struct.unpack_from('<i', d, o + 1)[0]
                if start <= target < end:
                    out[va] = target
                    va += 5; o += 5; misses = 0
                    continue
            va += 1; o += 1; misses += 1
        return out

    def function_starts(self, verified=True):
        """Function entry points, and the incremental thunk table.

        Objects the linker placed incrementally (the app's own code, up to about 0x8C2000) are
        reached through thunks. Code pulled from static libraries after them (WW3D2, WWLib, the
        CRT, ...) is not, so there a start is a direct `call rel32` target, or the first byte
        after inter-function padding (int3 / nop runs ending on a 16-byte boundary).

        A call target counts only when a linear sweep from the thunk targets and padding starts
        decodes a `call rel32` instruction there (every tool uses this). `verified=False` takes
        any `E8` byte's rel32 target instead: 216 more starts, all false (an E8 inside another
        instruction, often landing mid-instruction, 0x95A482 is one); kept only for auditing the
        difference (and pseudocode.py's cleanup of functions earlier runs made at those).

        A padding boundary whose first byte is itself int3 is not a start (2026-11-11): inside a
        long int3 run (the linker's fill, `fill_runs()`) the 1-15 byte rule fired every 240 bytes,
        1,734 false starts in 0xA420AC-0xAA1C00 and 0xAE2830-end. No call targets change (int3
        decodes one byte at a time, so the sweep resynchronises at the same seeds).
        """
        start, end, raw = self.section('.text')
        thunks = self.incremental_thunks()
        starts = set(thunks.values())
        lib_start = max(starts)
        body = self.data[raw:raw + (end - start)]
        lib_off = lib_start - start
        for m in re.finditer(rb'(?:\xCC|\x90){1,15}', body[lib_off:]):
            va = lib_start + m.end()
            if va < end and body[va - start] == 0xCC:
                continue
            if va % 16 == 0 and body[lib_off + m.end() - 1] in (0xCC, 0x90) and m.end() - m.start() >= 1:
                prev = body[lib_off + m.start() - 1]
                if prev in (0xC3, 0xC2, 0xCC, 0xE9) or body[lib_off + m.start() - 3] == 0xC2:
                    starts.add(va)
        if verified:
            calls = self._swept_calls(sorted(starts | {start}), start, end, body)
        else:
            unpack = struct.unpack_from
            calls = set()
            for m in re.finditer(rb'\xE8', body):
                o = m.start()
                if o + 5 > len(body):
                    break
                calls.add(start + o + 5 + unpack('<i', body, o + 1)[0])
        starts |= {c for c in calls if lib_start <= c < end}
        return sorted(starts), thunks

    def _swept_calls(self, seeds, start, end, body):
        """Targets of the `call rel32` instructions met disassembling linearly from each seed to
        the next (skipping a byte wherever decoding fails)."""
        if self._md is None:
            import capstone
            self._md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        calls = set()
        for k, s in enumerate(seeds):
            e = seeds[k + 1] if k + 1 < len(seeds) else end
            va = s
            while va < e:
                for addr, size, mn, op in self._md.disasm_lite(body[va - start:e - start], va):
                    if mn == 'call' and size == 5 and body[addr - start] == 0xE8:
                        calls.add(int(op, 16))
                    va = addr + size
                if va < e:
                    va += 1
        return calls

    def fill_runs(self, min_len=4096):
        """[(lo, hi)]: the int3 runs of .text at least `min_len` bytes long (the linker's fill; in
        client.exe 0x419650, 0x7F6A30, 0xA420AC-0xAA1C00 and 0xAE2830-0xAE68DD). Not code."""
        start, end, raw = self.section('.text')
        body = self.data[raw:raw + (end - start)]
        return [(start + m.start(), start + m.end()) for m in re.finditer(rb'\xCC{%d,}' % min_len, body)]

    def eh_stub_area(self):
        """The compiler-generated /GX stubs the linker gathers after the library code: unwind
        funclets and frame-handler thunks (`mov eax, offset FuncInfo; jmp ___CxxFrameHandler`,
        FuncInfo's magic 0x19930520). Returns (lo, hi, {thunk VA: owner site VA or None}): the area
        runs from the end of the fill before the first thunk to the start of the fill after the last
        (client.exe 0xAA1C00-0xAE2830, 4,416 thunks). A thunk's owner site is where a function
        installs it (`push offset thunk`, or `mov eax, offset thunk` before `call __EH_prolog`), the
        lowest such site below the area; the function containing that site owns the thunk's frame.
        Its starts stay in function_starts() (they bound the funclets); reports count them apart."""
        if getattr(self, '_eh', None) is not None:
            return self._eh
        start, end, raw = self.section('.text')
        body = self.data[raw:raw + (end - start)]
        data = self.data
        stubs = []
        for m in re.finditer(rb'\xB8(....)\xE9', body, re.S):
            fi = struct.unpack_from('<I', body, m.start() + 1)[0]
            off = self.offset(fi)
            if off is None or off + 4 > len(data) or struct.unpack_from('<I', data, off)[0] != 0x19930520:
                continue
            stubs.append(start + m.start())
        if not stubs:
            self._eh = (end, end, {})
            return self._eh
        fills = self.fill_runs()
        lo = max([h for _, h in fills if h <= stubs[0]] or [stubs[0]])
        hi = min([l for l, _ in fills if l > stubs[-1]] or [end])
        want = set(stubs)
        owner = dict.fromkeys(stubs)
        for m in re.finditer(rb'(?=[\x68\xB8](....))', body[:lo - start], re.S):
            v = struct.unpack_from('<I', body, m.start() + 1)[0]
            if v in want and owner[v] is None:
                owner[v] = start + m.start()
        self._eh = (lo, hi, owner)
        return self._eh

    def dword_refs(self, targets):
        """Every place in .text whose 4 bytes equal one of `targets`: {target: [VA, ...]}."""
        start, end, raw = self.section('.text')
        body = self.data[raw:raw + (end - start)]
        want = set(targets)
        out = {}
        unpack = struct.unpack_from
        for o in range(len(body) - 3):
            v = unpack('<I', body, o)[0]
            if v in want:
                out.setdefault(v, []).append(start + o)
        return out

    def disasm(self, va, size=0x400):
        if self._md is None:
            import capstone
            self._md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        o = self.offset(va)
        return self._md.disasm(self.data[o:o + size], va)
