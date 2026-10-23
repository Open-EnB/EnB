"""C-equivalent comments for the inline assembly in the imported Westwood libraries.

Convention (docs/conventions.md, "Inline assembly"): every assembly block, or a function written
as one assembly routine, carries a comment giving its C equivalent. Imported EA files are not
edited by hand: the importer inserts these comments (apply()) every time it assembles a library,
so they survive a re-import. Each entry: (anchor line, occurrence, comment). The comment is
inserted above the given occurrence (0-based) of the line whose stripped text equals the anchor,
indented like it. Comments don't change the compiled code; the library is re-verified after any
change here.
"""

RDTSC_NOTE = """RDTSC (bytes 0F 31) loads the processor's cycle counter into EDX:EAX. It is emitted as raw
bytes because VC6's inline assembler predates the mnemonic. Modern MSVC: __rdtsc()."""

SPIN_LOCK = """C equivalent (a spin lock):

	while (atomic_test_and_set_bit(&Flag, 0)) {	// set bit 0, return its old value, atomically
		ThreadClass::Switch_Thread();				// held by another thread: yield, then retry
	}

ts_lock is the LOCK prefix (0xF0) emitted as a raw byte, making `bts` (bit test and set) atomic.
Why assembly: VC6 had no intrinsic for an atomic bit test-and-set (modern MSVC:
_interlockedbittestandset)."""

RLE_BLIT = """C equivalent: the generic template version of this class above. The source is RLE data:
a zero byte is followed by a count of transparent pixels to skip; any other byte is a pixel,
written as {lookup}. The first `leadskip` pixels are skipped first (clipping), which can end in
the middle of a transparent run; the remainder of that run is skipped before output starts.
Why assembly: a hand-scheduled inner loop for software sprite drawing (Westwood's 2D code)."""

ANNOTATIONS = {
    # --------------------------------------------------------------------------------- wwmath
    ('wwmath', 'wwmath.h'): [
        ('WWINLINE long WWMath::Float_To_Long(float f)', 0, """C equivalent:

	return lrintf(f);	// (long)f, but ROUNDED to nearest instead of truncated

fld/fistp converts with the FPU's current rounding mode (round to nearest by default), so
2.7f gives 3 where (long)2.7f gives 2.
Why assembly: VC6 compiled (long)f into a call to _ftol, which switches the FPU to truncation and
back again: dozens of cycles and a pipeline stall, in a function a 3D engine calls constantly.
fld + fistp is two instructions."""),
        ('__asm fld	qword ptr [f]', 0, """C equivalent: return lrint(f); -- as the float version above, for a double."""),
        ('WWINLINE float WWMath::Cos(float val)', 0, """C equivalent: return (float)cos(val);
Why assembly: the FPU's own fcos instruction, without the C library's call overhead."""),
        ('WWINLINE float WWMath::Sin(float val)', 0, """C equivalent: return (float)sin(val);
Why assembly: the FPU's own fsin instruction, without the C library's call overhead."""),
        ('WWINLINE float WWMath::Sqrt(float val)', 0, """C equivalent: return (float)sqrt(val);
Why assembly: the FPU's own fsqrt instruction, without the C library's call overhead."""),
        ('WWINLINE __declspec(naked) float __fastcall WWMath::Inv_Sqrt(float a)', 0, """C equivalent:

	return 1.0f / sqrtf(a);

computed without a divide or a square root: an integer operation on the float's bit pattern
(with the magic constant 0xBE6EB508) gives a first estimate r, then three Newton-Raphson steps
r = r * (1.5 - (a/2) * r * r) refine it. The function is naked (no compiler prologue): it reads
its argument at [esp+4], uses the stack below esp as scratch and returns with `ret 4` itself.
Why assembly: fdiv and fsqrt were the slowest FPU instructions of the day."""),
    ],
    ('wwmath', 'matrix3d.cpp'): [
        ('void Matrix3D::Multiply(const Matrix3D & A,const Matrix3D & B,Matrix3D * set_res)', 1, """C equivalent: the commented-out C version inside this function (Westwood's own): *set_res = A * B
for 3x4 transforms, the rotation parts multiplied and A's translation added; when set_res is
&A the result is built in a temporary first.
Why assembly: matrix multiplication is in every transform update; the FPU stack is used directly
instead of through memory temporaries."""),
    ],
    ('wwmath', 'quat.cpp'): [
        ('void __cdecl Fast_Slerp(Quaternion& res, const Quaternion & p,const Quaternion & q,float alpha)', 0, """Not compiled: this assembly version is inside #if 0. The C version after #else is the one
built: spherical linear interpolation between p and q using table-based acos and sin."""),
    ],
    ('wwmath', 'vp.cpp'): [
        ('#define	BROADCAST(XMM, INDEX)	__asm	shufps	XMM,XMM,(((INDEX)&3)<< 6|((INDEX)&3)<<4|((INDEX)&3)<< 2|((INDEX)&3))', 0,
         """SSE helper macros, used only by the Intel-compiler path below (__ICL). BROADCAST copies one
lane of an XMM register to all four lanes; TRANSPOSE transposes the 4x4 matrix held in four XMM
registers."""),
        ('void VectorProcessorClass::Prefetch(void* address)', 0, """C equivalent: nothing. The assembly is commented out, and only compiled by Intel's compiler."""),
        ('void VectorProcessorClass::Transform (Vector3* dst,const Vector3 *src, const Matrix3D& mtx, const int count)', 0, """C equivalent: for (i = 0; i < count; i++) dst[i] = mtx * src[i]; -- the loop at the end of
this function, which is what VC6 builds. The SSE assembly version is only compiled by Intel's
compiler (__ICL)."""),
    ],
    # --------------------------------------------------------------------------------- wwlib
    ('wwlib', 'cpudetect.cpp'): [
        ('#define ASM_RDTSC _asm _emit 0x0f _asm _emit 0x31', 0, RDTSC_NOTE),
        ('static unsigned Calculate_Processor_Speed(__int64& ticks_per_second)', 0, """C equivalent:

	t0 = __rdtsc();
	start = timeGetTime();
	while ((elapsed = timeGetTime() - start) < 200) t1 = __rdtsc();
	t = t1 - t0;
	ticks_per_second = 5 * t;			// cycles in 200 ms, times 5
	return t / (elapsed * 1000);		// MHz

Note: the blocks store EAX (the low half) in the fields named *_h and EDX (the high half) in
*_l. Because each _h field comes first in memory, reading &timerN_h as an __int64 is still
right; only the names are swapped.
Why assembly: RDTSC has no C equivalent in VC6."""),
        ('void CPUDetectClass::Init_CPUID_Instruction()', 0, """C equivalent:

	cpuid_available = (bit 21 of EFLAGS can be changed);

The block saves EFLAGS, flips the ID bit (0x00200000), reads EFLAGS back to see whether the flip
held, and restores them. Processors without CPUID (386, early 486) don't let the bit change.
Why assembly: C has no access to EFLAGS."""),
        ('bool CPUDetectClass::CPUID(', 0, """C equivalent (modern MSVC):

	int r[4];
	__cpuid(r, cpuid_type);
	u_eax_ = r[0]; u_ebx_ = r[1]; u_ecx_ = r[2]; u_edx_ = r[3];

pushad/popad keep every register intact, since CPUID overwrites EBX, which the compiler expects
to be preserved.
Why assembly: VC6 had no CPUID intrinsic."""),
    ],
    ('wwlib', 'mpu.cpp'): [
        ('unsigned long Get_CPU_Clock(unsigned long & high)', 0, """C equivalent: unsigned __int64 t = __rdtsc(); high = (unsigned long)(t >> 32); return (unsigned long)t;
""" + RDTSC_NOTE),
        ('#define ASM_RDTSC _asm _emit 0x0f _asm _emit 0x31', 0, RDTSC_NOTE),
        ('void RDTSC(void)', 0, """C equivalent: unsigned __int64 t = __rdtsc(); TSC_Low = (unsigned long)t; TSC_High = (unsigned long)(t >> 32);"""),
        ('_asm	mov	stamp0, EAX', 0, """C equivalent of ASM_RDTSC plus this line: stamp0 = (DWORD)__rdtsc(); (the low 32 bits)"""),
        ('_asm	mov	stamp1, EAX', 0, """C equivalent of ASM_RDTSC plus this line: stamp1 = (DWORD)__rdtsc(); (the low 32 bits)"""),
    ],
    ('wwlib', 'mutex.h'): [
        ('void Thread_Safe_Set_Flag()', 0, SPIN_LOCK),
    ],
    ('wwlib', 'except.cpp'): [
        ('_asm int 0x03;', 0, """C equivalent: __debugbreak(); -- stop in the debugger (a pure virtual function was called)."""),
        ('mov	eax,fp_data_ptr', 0, """C equivalent: fp_value = (double)*(long double *)fp_data_ptr;
Converts one 80-bit x87 register image from the crash context to a double. Why assembly: VC6's
long double is 64 bits, so C can't read the 80-bit format; fld tbyte can."""),
        ('lea	eax,here', 0, """C equivalent: reg_eip = (address of this instruction); reg_ebp = the frame pointer;
reg_esp = the stack pointer -- the starting point for walking the stack with StackWalk.
Why assembly: C can't read registers (modern MSVC: _ReturnAddress, _AddressOfReturnAddress)."""),
    ],
    ('wwlib', 'lcw.cpp'): [
        ('int LCW_Comp(void const * source, void * dest, int datasize)', 0, """LCW_Comp is one assembly routine, split into blocks by C labels. C equivalent (greedy LCW,
"Westwood LZ"), with the codes listed in the first block:

	*out++ = 0x81; *out++ = *src++;		// the first byte is always a one-byte literal
	while (src < end) {
		if (src[0] == src[64] && the run of bytes equal to src[0] is 65 long or more) {
			emit 0xFE, run length (word), the byte;		// fill
			src += run length;
			continue;
		}
		search every earlier position for the longest match with the bytes at src;
		if (match length <= 2) {
			append *src++ to the current literal (codes 0x80..0xBF hold 0-63 literals;
			start a new literal code when the current one is full or a copy came between);
		} else if (match length <= 10 && distance back <= 0xFFF) {
			emit 0LLLDDDD DDDDDDDD;						// L = length - 3, D = distance back
		} else if (match length <= 64) {
			emit 0xC0 | (length - 3), offset from the start of data (word);
		} else {
			emit 0xFF, length (word), offset from the start of data (word);
		}
		src += match length;	// (for copies)
	}
	*out++ = 0x80;										// end of data
	return out - dest;

Why assembly: carried over from Westwood's DOS-era library; the brute-force match search
uses the string instructions (repne scasb, repe cmpsb) directly."""),
    ],
    ('wwlib', 'blitblit.h'): [
        ('inline void BlitTrans<unsigned char>::BlitForward(void * dest, void const * source, int len) const', 0, """C equivalent: for (i = 0; i < len; i++) if (src[i] != 0) dst[i] = src[i];
(8-bit pixels, 0 is transparent; the generic BlitTrans<T> above does the same.)
Why assembly: hand-tuned software sprite drawing."""),
        ('inline void BlitTransXlat<unsigned short>::BlitForward(void * dest, void const * source, int len) const', 0, """C equivalent: for (i = 0; i < len; i++) if (src[i] != 0) dst[i] = TranslateTable[src[i]];
(8-bit source pixels translated to 16-bit; 0 is transparent.)
Why assembly: hand-tuned software sprite drawing."""),
        ('inline void BlitTransRemapXlat<unsigned short>::BlitForward(void * dest, void const * source, int len) const', 0, """C equivalent: for (i = 0; i < len; i++) if (src[i] != 0) dst[i] = TranslateTable[RemapTable[src[i]]];
(an 8-bit remap, such as a house colour, then translation to 16-bit; 0 is transparent.)
Why assembly: hand-tuned software sprite drawing."""),
    ],
    ('wwlib', 'rlerle.h'): [
        ('void RLEBlitTransZRemapXlat<unsigned short>::Blit(void * dest, void const * source, int len, int leadskip) const', 0,
         RLE_BLIT.format(lookup='TranslateTable[(*RemapTable)[pixel]]')),
        ('void RLEBlitTransRemapXlat<unsigned short>::Blit(void * dest, void const * source, int len, int leadskip) const', 0,
         RLE_BLIT.format(lookup='TranslateTable[RemapTable[pixel]]')),
        ('void RLEBlitTransXlat<unsigned short>::Blit(void * dest, void const * source, int len, int leadskip) const', 0,
         RLE_BLIT.format(lookup='TranslateTable[pixel]')),
    ],
    # --------------------------------------------------------------------------------- wwdebug
    ('wwdebug', 'wwdebug.h'): [
        ('#define WWDEBUG_BREAK							_asm int 0x03', 0, """C equivalent: __debugbreak(); -- stop in the debugger. (Both branches define the same thing.)"""),
    ],
    ('wwdebug', 'wwdebug.cpp'): [
        ('_asm int 3;', 0, """C equivalent: __debugbreak(); -- the user chose Retry on the assert dialog: stop in the debugger."""),
    ],
    ('wwdebug', 'wwprofile.cpp'): [
        ('inline void WWProfile_Get_Ticks(_int64 * ticks)', 0, """C equivalent: *ticks = __rdtsc();
""" + RDTSC_NOTE),
    ],
    ('wwdebug', 'wwmemlog.cpp'): [
        ('volatile unsigned& nFlag=_MemLogSemaphore;', 0, SPIN_LOCK.replace('&Flag', '&_MemLogSemaphore')),
    ],
    # --------------------------------------------------------------------------------- ww3d2
    ('ww3d2', 'dx8wrapper.h'): [
        ('WWINLINE unsigned int DX8Wrapper::Convert_Color(const Vector3& color,float alpha)', 0, """C equivalent:

	unsigned a = (unsigned)(alpha * 255.0f), r = (unsigned)(color.X * 255.0f),
	         g = (unsigned)(color.Y * 255.0f), b = (unsigned)(color.Z * 255.0f);
	return (a << 24) | (r << 16) | (g << 8) | b;		// D3DCOLOR, AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB

The conversions truncate, as (unsigned) does: the block switches the FPU to truncation if it
isn't already, and restores it. The inputs must already be in 0..1 (see Clamp_Color).
Why assembly: four float-to-int conversions with one rounding-mode switch, instead of four
calls to _ftol that each switch it twice."""),
        ('WWINLINE void DX8Wrapper::Clamp_Color(Vector4& color)', 0, """C equivalent: the loop at the top of this function (each component clamped to 0..1).
The assembly does it without branches, on the floats' bit patterns: a negative value becomes 0
(the sign bit spread into a mask), and anything at or above 1.0 (0x3F800000; positive floats
compare like integers) is replaced by 1.0 with cmov. CMOV needs a Pentium Pro or later, hence
the check.
Why assembly: no branches to mispredict, and VC6 never generated cmov."""),
    ],
}


def apply(lib, path, name):
    """Insert this file's annotations (idempotent: an annotation already present is skipped)."""
    entries = ANNOTATIONS.get((lib, name.lower()))
    if not entries:
        return 0
    data = open(path, 'rb').read().decode('latin1')
    nl = '\r\n' if '\r\n' in data else '\n'
    lines = data.split(nl)
    inserts = []
    for anchor, occurrence, comment in entries:
        hits = [i for i, l in enumerate(lines) if l.strip() == anchor.strip()]
        if len(hits) <= occurrence:
            raise SystemExit(f'annotation anchor not found in {lib}/{name}: {anchor!r} #{occurrence}')
        i = hits[occurrence]
        indent = lines[i][:len(lines[i]) - len(lines[i].lstrip())]
        block = [indent + '//'] + [indent + ('//\t' + c if c else '//') for c in comment.split('\n')] + [indent + '//']
        if lines[max(0, i - len(block)):i] == block:
            continue   # already annotated
        inserts.append((i, block))
    for i, block in sorted(inserts, reverse=True):
        lines[i:i] = block
    open(path, 'wb').write(nl.join(lines).encode('latin1'))
    return len(inserts)
