// Earth & Beyond client (reconstruction): the output of Westwood's shader pre-processor (shdpp.exe, in Zero
// Hour's WWShade) for shd6bumpdiff.vsh, which EA's release does not include. Rebuilt from
// client.exe .data 0xBCB8FC-0xBCBB9F (676 bytes): the pre-processed shader source text
// (669 characters and its NUL, zero padded to a DWORD), packed little-endian into DWORDs
// and followed by shdpp's 0x00000000 terminator, in shdpp's own form ("DWORD %s[]={",
// "0x%08x", ",\t0x00000000"; its strings). A DWORD array, not char[]: the caller
// needs ShdHWVertexShader/ShdHWPixelShader::Create(DWORD*), which assembles it with
// D3DXAssembleShader(code, strlen(code)) (0xA0F3E0, 0xA0F5A0); a char[] would bind to
// the file-name overload. Used by shd6bumpdiff.cpp, Shd6BumpDiffClass::Init 0xA10090 (0xA1009D).
// The text has the same instructions, in the same order, as Zero Hour's shd6bumpdiff.vsh,
// with the #define names substituted and comments and directives gone (blank lines left);
// it reads:
//
//
//
//	vs.1.1
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
//
//
//
//
//
//
//	dp4 oPos.x, v0, c[1]
//	dp4 oPos.y, v0, c[2]
//	dp4 oPos.z, v0, c[3]
//	dp4 oPos.w, v0, c[4]
//	dp3 r1.x, v1, c[12]
//	dp3 r1.y, v1, c[13]
//	dp3 r1.z, v1, c[14]
//
//	dp3 r1.w, r1, r1
//	rsq r1.w, r1.w
//	mul r1, r1, r1.w
//
//	dp3 r5.w, r1, c[16]	
//	max r5.w, r5.w, c[0].x	
//	mul r5.w, r5.w, c[20].w	
//
//	dp3 r6.w, r1, c[17]	
//	max r6.w, r6.w, c[0].x	
//	mul r6.w, r6.w, c[21].w	
//
//	dp3 r7.w, r1, c[18]	
//	max r7.w, r7.w, c[0].x	
//	mul r7.w, r7.w, c[22].w	
//
//	dp3 r8.w, r1, c[19]	
//	max r8.w, r8.w, c[0].x	
//	mul r8.w, r8.w, c[23].w	
//
//	mul r9, c[20], r5.w
//	mad r9, c[21], r6.w, r9
//	mad r9, c[22], r7.w, r9
//	mad r9, c[23], r8.w, r9
//
//	mul r9, r9, v2
//	mul r9, r9, c[25]
//	add oD0, r9, c[24]
//	mov oT0, v3
//

DWORD shd6bumpdiff_vsh_code[]={
0x73760a0a,	0x312e312e,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,
0x3470640a,	0x6f506f20,	0x2c782e73,	0x2c307620,	0x315b6320,	0x70640a5d,	0x506f2034,	0x792e736f,
0x3076202c,	0x5b63202c,	0x640a5d32,	0x6f203470,	0x2e736f50,	0x76202c7a,	0x63202c30,	0x0a5d335b,
0x20347064,	0x736f506f,	0x202c772e,	0x202c3076,	0x5d345b63,	0x3370640a,	0x2e317220,	0x76202c78,
0x63202c31,	0x5d32315b,	0x3370640a,	0x2e317220,	0x76202c79,	0x63202c31,	0x5d33315b,	0x3370640a,
0x2e317220,	0x76202c7a,	0x63202c31,	0x5d34315b,	0x70640a0a,	0x31722033,	0x202c772e,	0x202c3172,
0x720a3172,	0x72207173,	0x2c772e31,	0x2e317220,	0x756d0a77,	0x3172206c,	0x3172202c,	0x3172202c,
0x0a0a772e,	0x20337064,	0x772e3572,	0x3172202c,	0x5b63202c,	0x095d3631,	0x78616d0a,	0x2e357220,
0x72202c77,	0x2c772e35,	0x305b6320,	0x09782e5d,	0x6c756d0a,	0x2e357220,	0x72202c77,	0x2c772e35,
0x325b6320,	0x772e5d30,	0x640a0a09,	0x72203370,	0x2c772e36,	0x2c317220,	0x315b6320,	0x0a095d37,
0x2078616d,	0x772e3672,	0x3672202c,	0x202c772e,	0x5d305b63,	0x0a09782e,	0x206c756d,	0x772e3672,
0x3672202c,	0x202c772e,	0x31325b63,	0x09772e5d,	0x70640a0a,	0x37722033,	0x202c772e,	0x202c3172,
0x38315b63,	0x6d0a095d,	0x72207861,	0x2c772e37,	0x2e377220,	0x63202c77,	0x2e5d305b,	0x6d0a0978,
0x72206c75,	0x2c772e37,	0x2e377220,	0x63202c77,	0x5d32325b,	0x0a09772e,	0x3370640a,	0x2e387220,
0x72202c77,	0x63202c31,	0x5d39315b,	0x616d0a09,	0x38722078,	0x202c772e,	0x772e3872,	0x5b63202c,
0x782e5d30,	0x756d0a09,	0x3872206c,	0x202c772e,	0x772e3872,	0x5b63202c,	0x2e5d3332,	0x0a0a0977,
0x206c756d,	0x202c3972,	0x30325b63,	0x72202c5d,	0x0a772e35,	0x2064616d,	0x202c3972,	0x31325b63,
0x72202c5d,	0x2c772e36,	0x0a397220,	0x2064616d,	0x202c3972,	0x32325b63,	0x72202c5d,	0x2c772e37,
0x0a397220,	0x2064616d,	0x202c3972,	0x33325b63,	0x72202c5d,	0x2c772e38,	0x0a397220,	0x6c756d0a,
0x2c397220,	0x2c397220,	0x0a327620,	0x206c756d,	0x202c3972,	0x202c3972,	0x35325b63,	0x64610a5d,
0x446f2064,	0x72202c30,	0x63202c39,	0x5d34325b,	0x766f6d0a,	0x30546f20,	0x3376202c,	0x0000000a,
0x00000000};
