// EnB reconstruction: the output of Westwood's shader pre-processor (shdpp.exe, in Zero
// Hour's WWShade) for shd7bumpspecpass1.vsh, which EA's release does not include. Rebuilt from
// client.exe .data 0xBCD974-0xBCDDAB (1080 bytes): the pre-processed shader source text
// (1072 characters and its NUL, zero padded to a DWORD), packed little-endian into DWORDs
// and followed by shdpp's 0x00000000 terminator, in shdpp's own form ("DWORD %s[]={",
// "0x%08x", ",\t0x00000000"; its strings). A DWORD array, not char[]: the caller
// needs ShdHWVertexShader/ShdHWPixelShader::Create(DWORD*), which assembles it with
// D3DXAssembleShader(code, strlen(code)) (0xA0F3E0, 0xA0F5A0); a char[] would bind to
// the file-name overload. Used by shd7bumpspec.cpp, Shd7BumpSpecClass::Init 0xA13870 (0xA138D4).
// The text has the same instructions, in the same order, as Zero Hour's shd7bumpspecpass1.vsh,
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
//
//
//
//	dp4 r11.x, v0, c[1]
//	dp4 r11.y, v0, c[2]
//	dp4 r11.z, v0, c[3]
//	dp4 r11.w, v0, c[4]
//	mov oPos, r11
//
//	mov oD0.w, c[0].y
//	mov oT0, v3
//	mov oT1, v3
//
//	dp3 r1.x, v4, c[12]
//	dp3 r1.y, v4, c[13]
//	dp3 r1.z, v4, c[14]
//	dp3 r2.x, v5, c[12]
//	dp3 r2.y, v5, c[13]
//	dp3 r2.z, v5, c[14]
//	dp3 r3.x, v6, c[12]
//	dp3 r3.y, v6, c[13]
//	dp3 r3.z, v6, c[14]
//
//
//
//
//	dp4 r10.x, v0, c[12]
//	dp4 r10.y, v0, c[13]
//	dp4 r10.z, v0, c[14]
//
//
//
//
//
//
//
//	add r11, c[28], -r10.xyz
//
//	dp3 r11.w, r11, r11
//	rsq r11.w, r11.w
//	mul r11, r11, r11.w
//
//	add r0, c[16], r11
//
//	dp3 r0.w, r0, r0
//	rsq r0.w, r0.w
//	mul r0, r0, r0.w
//	dp3 r4.x, r0, r1
//	dp3 r4.y, r0, r2
//	dp3 r4.z, r0, r3
//
//	dp3 r4.w, r4, r4
//	rsq r4.w, r4.w
//	mul r4, r4, r4.w
//
//	dp3 r1.x, v1, c[12]
//	dp3 r1.y, v1, c[13]
//	dp3 r1.z, v1, c[14]
//
//	dp3 r1.w, r1, r1
//	rsq r1.w, r1.w
//	mul r1, r1, r1.w
//	dp3 r5.w, r0, r1	
//	max r5.w, r5.w, c[0].x	
//	mul r5.w, r5.w, c[20].w	
//	mul r5.w, r5, r5
//	mul r5.w, r5, r5
//	mul r5.w, r5, r5
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
//	mul r9, c[20], r5.w
//
//
//	mul oD1, r9, c[26]
//
//	add r4.xyz, r4, c[0].yyy
//	mul r4.xyz, r4, c[0].zzz
//
//	mul r4.xyz, r4, c[27].zzz
//	add oD0.xyz, r4, c[27].www
//

DWORD shd7bumpspecpass1_vsh_code[]={
0x73760a0a,	0x312e312e,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,
0x0a0a0a0a,	0x20347064,	0x2e313172,	0x76202c78,	0x63202c30,	0x0a5d315b,	0x20347064,	0x2e313172,
0x76202c79,	0x63202c30,	0x0a5d325b,	0x20347064,	0x2e313172,	0x76202c7a,	0x63202c30,	0x0a5d335b,
0x20347064,	0x2e313172,	0x76202c77,	0x63202c30,	0x0a5d345b,	0x20766f6d,	0x736f506f,	0x3172202c,
0x6d0a0a31,	0x6f20766f,	0x772e3044,	0x5b63202c,	0x792e5d30,	0x766f6d0a,	0x30546f20,	0x3376202c,
0x766f6d0a,	0x31546f20,	0x3376202c,	0x70640a0a,	0x31722033,	0x202c782e,	0x202c3476,	0x32315b63,
0x70640a5d,	0x31722033,	0x202c792e,	0x202c3476,	0x33315b63,	0x70640a5d,	0x31722033,	0x202c7a2e,
0x202c3476,	0x34315b63,	0x70640a5d,	0x32722033,	0x202c782e,	0x202c3576,	0x32315b63,	0x70640a5d,
0x32722033,	0x202c792e,	0x202c3576,	0x33315b63,	0x70640a5d,	0x32722033,	0x202c7a2e,	0x202c3576,
0x34315b63,	0x70640a5d,	0x33722033,	0x202c782e,	0x202c3676,	0x32315b63,	0x70640a5d,	0x33722033,
0x202c792e,	0x202c3676,	0x33315b63,	0x70640a5d,	0x33722033,	0x202c7a2e,	0x202c3676,	0x34315b63,
0x0a0a0a5d,	0x70640a0a,	0x31722034,	0x2c782e30,	0x2c307620,	0x315b6320,	0x640a5d32,	0x72203470,
0x792e3031,	0x3076202c,	0x5b63202c,	0x0a5d3331,	0x20347064,	0x2e303172,	0x76202c7a,	0x63202c30,
0x5d34315b,	0x0a0a0a0a,	0x0a0a0a0a,	0x20646461,	0x2c313172,	0x325b6320,	0x202c5d38,	0x3031722d,
0x7a79782e,	0x70640a0a,	0x31722033,	0x2c772e31,	0x31317220,	0x3172202c,	0x73720a31,	0x31722071,
0x2c772e31,	0x31317220,	0x6d0a772e,	0x72206c75,	0x202c3131,	0x2c313172,	0x31317220,	0x0a0a772e,
0x20646461,	0x202c3072,	0x36315b63,	0x72202c5d,	0x0a0a3131,	0x20337064,	0x772e3072,	0x3072202c,
0x3072202c,	0x7173720a,	0x2e307220,	0x72202c77,	0x0a772e30,	0x206c756d,	0x202c3072,	0x202c3072,
0x772e3072,	0x3370640a,	0x2e347220,	0x72202c78,	0x72202c30,	0x70640a31,	0x34722033,	0x202c792e,
0x202c3072,	0x640a3272,	0x72203370,	0x2c7a2e34,	0x2c307220,	0x0a337220,	0x3370640a,	0x2e347220,
0x72202c77,	0x72202c34,	0x73720a34,	0x34722071,	0x202c772e,	0x772e3472,	0x6c756d0a,	0x2c347220,
0x2c347220,	0x2e347220,	0x640a0a77,	0x72203370,	0x2c782e31,	0x2c317620,	0x315b6320,	0x640a5d32,
0x72203370,	0x2c792e31,	0x2c317620,	0x315b6320,	0x640a5d33,	0x72203370,	0x2c7a2e31,	0x2c317620,
0x315b6320,	0x0a0a5d34,	0x20337064,	0x772e3172,	0x3172202c,	0x3172202c,	0x7173720a,	0x2e317220,
0x72202c77,	0x0a772e31,	0x206c756d,	0x202c3172,	0x202c3172,	0x772e3172,	0x3370640a,	0x2e357220,
0x72202c77,	0x72202c30,	0x6d0a0931,	0x72207861,	0x2c772e35,	0x2e357220,	0x63202c77,	0x2e5d305b,
0x6d0a0978,	0x72206c75,	0x2c772e35,	0x2e357220,	0x63202c77,	0x5d30325b,	0x0a09772e,	0x206c756d,
0x772e3572,	0x3572202c,	0x3572202c,	0x6c756d0a,	0x2e357220,	0x72202c77,	0x72202c35,	0x756d0a35,
0x3572206c,	0x202c772e,	0x202c3572,	0x0a0a3572,	0x0a0a0a0a,	0x0a0a0a0a,	0x0a0a0a0a,	0x756d0a0a,
0x3972206c,	0x5b63202c,	0x2c5d3032,	0x2e357220,	0x0a0a0a77,	0x206c756d,	0x2c31446f,	0x2c397220,
0x325b6320,	0x0a0a5d36,	0x20646461,	0x782e3472,	0x202c7a79,	0x202c3472,	0x5d305b63,	0x7979792e,
0x6c756d0a,	0x2e347220,	0x2c7a7978,	0x2c347220,	0x305b6320,	0x7a7a2e5d,	0x6d0a0a7a,	0x72206c75,
0x79782e34,	0x72202c7a,	0x63202c34,	0x5d37325b,	0x7a7a7a2e,	0x6464610a,	0x30446f20,	0x7a79782e,
0x3472202c,	0x5b63202c,	0x2e5d3732,	0x0a777777,	0x00000000,	0x00000000};
