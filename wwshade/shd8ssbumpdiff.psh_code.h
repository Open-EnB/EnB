// Earth & Beyond client (reconstruction): the output of Westwood's shader pre-processor (shdpp.exe, in Zero
// Hour's WWShade) for shd8ssbumpdiff.psh, which EA's release does not include. Rebuilt from
// client.exe .data 0xBCD050-0xBCD08B (60 bytes): the pre-processed shader source text
// (53 characters and its NUL, zero padded to a DWORD), packed little-endian into DWORDs
// and followed by shdpp's 0x00000000 terminator, in shdpp's own form ("DWORD %s[]={",
// "0x%08x", ",\t0x00000000"; its strings). A DWORD array, not char[]: the caller
// needs ShdHWVertexShader/ShdHWPixelShader::Create(DWORD*), which assembles it with
// D3DXAssembleShader(code, strlen(code)) (0xA0F3E0, 0xA0F5A0); a char[] would bind to
// the file-name overload. Used by shd8bumpdiff.cpp, Shd8BumpDiffClass::Init 0xA11A30 (0xA11A9E).
// The text has the same instructions, in the same order, as Zero Hour's shd8ssbumpdiff.psh,
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
//
//	mov r0,t0
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
//
//
//
//
//
//
//

DWORD shd8ssbumpdiff_psh_code[]={
0x0a0a0a0a,	0x0a0a0a0a,	0x73700a0a,	0x312e312e,	0x7865740a,	0x09307409,	0x6f6d0a0a,	0x30722076,
0x0a30742c,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0000000a,	0x00000000};
