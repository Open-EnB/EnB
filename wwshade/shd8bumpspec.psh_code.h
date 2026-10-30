// Earth & Beyond client (reconstruction): the output of Westwood's shader pre-processor (shdpp.exe, in Zero
// Hour's WWShade) for shd8bumpspec.psh, which EA's release does not include. Rebuilt from
// client.exe .data 0xBCE3F0-0xBCE4CB (220 bytes): the pre-processed shader source text
// (212 characters and its NUL, zero padded to a DWORD), packed little-endian into DWORDs
// and followed by shdpp's 0x00000000 terminator, in shdpp's own form ("DWORD %s[]={",
// "0x%08x", ",\t0x00000000"; its strings). A DWORD array, not char[]: the caller
// needs ShdHWVertexShader/ShdHWPixelShader::Create(DWORD*), which assembles it with
// D3DXAssembleShader(code, strlen(code)) (0xA0F3E0, 0xA0F5A0); a char[] would bind to
// the file-name overload. Used by shd8bumpspec.cpp, Shd8BumpSpecClass::Init 0xA147A0 (0xA147A3).
// The text has the same instructions, in the same order, as Zero Hour's shd8bumpspec.psh,
// with the #define names substituted and comments and directives gone (blank lines left);
// it reads:
//
//
//
//
//
//
//
//
//
//
//
//	ps.1.1
//	tex	t0	
//	tex	t1	
//	texcoord t2	
//
//	dp3 t0, t0_bx2, v0_bx2
//
//	mul t1.rgb, v1, t1
//
//	mul_sat r1, t1, -t0
//
//	mul_sat r0, t1, t0
//
//	add r0, r0, r1
//
//	mad r0, t1, v0.a, r0
//
//
//	mul_sat r1, t1.a, t0
//
//	mad r0, t2, r1, r0
//

DWORD shd8bumpspec_psh_code[]={
0x0a0a0a0a,	0x0a0a0a0a,	0x73700a0a,	0x312e312e,	0x7865740a,	0x09307409,	0x7865740a,	0x09317409,
0x7865740a,	0x726f6f63,	0x32742064,	0x640a0a09,	0x74203370,	0x74202c30,	0x78625f30,	0x76202c32,
0x78625f30,	0x6d0a0a32,	0x74206c75,	0x67722e31,	0x76202c62,	0x74202c31,	0x6d0a0a31,	0x735f6c75,
0x72207461,	0x74202c31,	0x2d202c31,	0x0a0a3074,	0x5f6c756d,	0x20746173,	0x202c3072,	0x202c3174,
0x0a0a3074,	0x20646461,	0x202c3072,	0x202c3072,	0x0a0a3172,	0x2064616d,	0x202c3072,	0x202c3174,
0x612e3076,	0x3072202c,	0x6d0a0a0a,	0x735f6c75,	0x72207461,	0x74202c31,	0x2c612e31,	0x0a307420,
0x64616d0a,	0x2c307220,	0x2c327420,	0x2c317220,	0x0a307220,	0x00000000,	0x00000000};
