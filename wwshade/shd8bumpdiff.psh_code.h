// Earth & Beyond client (reconstruction): the output of Westwood's shader pre-processor (shdpp.exe, in Zero
// Hour's WWShade) for shd8bumpdiff.psh, which EA's release does not include. Rebuilt from
// client.exe .data 0xBCC8FC-0xBCC99F (164 bytes): the pre-processed shader source text
// (156 characters and its NUL, zero padded to a DWORD), packed little-endian into DWORDs
// and followed by shdpp's 0x00000000 terminator, in shdpp's own form ("DWORD %s[]={",
// "0x%08x", ",\t0x00000000"; its strings). A DWORD array, not char[]: the caller
// needs ShdHWVertexShader/ShdHWPixelShader::Create(DWORD*), which assembles it with
// D3DXAssembleShader(code, strlen(code)) (0xA0F3E0, 0xA0F5A0); a char[] would bind to
// the file-name overload. Used by shd8bumpdiff.cpp, Shd8BumpDiffClass::Init 0xA11A30 (0xA11A33).
// The text has the same instructions, in the same order, as Zero Hour's shd8bumpdiff.psh,
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

DWORD shd8bumpdiff_psh_code[]={
0x0a0a0a0a,	0x0a0a0a0a,	0x73700a0a,	0x312e312e,	0x7865740a,	0x09307409,	0x7865740a,	0x09317409,
0x70640a0a,	0x30742033,	0x3074202c,	0x3278625f,	0x3076202c,	0x3278625f,	0x756d0a0a,	0x3174206c,
0x6267722e,	0x3176202c,	0x3174202c,	0x756d0a0a,	0x61735f6c,	0x31722074,	0x3174202c,	0x742d202c,
0x6d0a0a30,	0x735f6c75,	0x72207461,	0x74202c30,	0x74202c31,	0x610a0a30,	0x72206464,	0x72202c30,
0x72202c30,	0x6d0a0a31,	0x72206461,	0x74202c30,	0x76202c31,	0x2c612e30,	0x0a307220,	0x00000000,
0x00000000};
