F = 'app/shared/land.cpp'
OLD = """		unsigned char *bytes = (unsigned char *)Unknown120C;
		unsigned short *words = (unsigned short *)Unknown120C;
		int size = Field->Get_Size();
		for (int j = min.J; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				int n = j * size + i;
				words[n] = (unsigned short)((((unsigned short)(bytes[n * 4 + 3] & 0xF0) << 4) | (unsigned short)(bytes[n * 4 + 2] & 0xF0)) << 4)
					| (unsigned short)(unsigned char)((bytes[n * 4 + 1] & 0xF0) | (bytes[n * 4] >> 4));
			}
		}
"""
EXPR = """(unsigned short)((((unsigned short)(B(3) & 0xF0) << 4) | (unsigned short)(B(2) & 0xF0)) << 4)
					| (unsigned short)(unsigned char)((B(1) & 0xF0) | (B(0) >> 4))"""


def ex(fmt):
    e = EXPR
    for k in range(4):
        e = e.replace('B(%d)' % k, fmt % k)
    return e


def mk(decl, idx='p%d[n * 4]', words='words[n]', loopvar='int n = j * size + i;'):
    return [(F, OLD, decl + """		int size = Field->Get_Size();
		for (int j = min.J; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				""" + loopvar + """
				""" + words + """ = """ + ex(idx) + """;
			}
		}
""")]

W = "		unsigned short *words = (unsigned short *)Unknown120C;\n"
P0123 = """		unsigned char *p0 = (unsigned char *)Unknown120C;
		unsigned char *p1 = p0 + 1;
		unsigned char *p2 = p0 + 2;
		unsigned char *p3 = p0 + 3;
"""
P0123m = """		unsigned char *p0 = (unsigned char *)Unknown120C;
		unsigned char *p1 = (unsigned char *)Unknown120C + 1;
		unsigned char *p2 = (unsigned char *)Unknown120C + 2;
		unsigned char *p3 = (unsigned char *)Unknown120C + 3;
"""
P3210 = """		unsigned char *p3 = (unsigned char *)Unknown120C + 3;
		unsigned char *p2 = (unsigned char *)Unknown120C + 2;
		unsigned char *p1 = (unsigned char *)Unknown120C + 1;
		unsigned char *p0 = (unsigned char *)Unknown120C;
"""
P2 = """		unsigned char *bytes = (unsigned char *)Unknown120C;
		unsigned char *p0 = bytes;
		unsigned char *p1 = bytes + 1;
		unsigned char *p2 = bytes + 2;
		unsigned char *p3 = bytes + 3;
"""
def mk2(decl, body):
    return [(F, OLD, decl + """		int size = Field->Get_Size();
		for (int j = min.J; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				int n = j * size + i;
""" + body + """			}
		}
""")]
T_LO = """				unsigned char lo = (unsigned char)((p1[n * 4] & 0xF0) | (p0[n * 4] >> 4));
				words[n] = (unsigned short)((((unsigned short)(p3[n * 4] & 0xF0) << 4) | (unsigned short)(p2[n * 4] & 0xF0)) << 4) | lo;
"""
T_ALL = """				unsigned char b = p0[n * 4];
				unsigned char g = p1[n * 4];
				unsigned char r = p2[n * 4];
				unsigned char a = p3[n * 4];
				words[n] = (unsigned short)((((unsigned short)(a & 0xF0) << 4) | (unsigned short)(r & 0xF0)) << 4) | (unsigned short)(unsigned char)((g & 0xF0) | (b >> 4));
"""
T_K = """				int k = n * 4;
				words[n] = (unsigned short)((((unsigned short)(p3[k] & 0xF0) << 4) | (unsigned short)(p2[k] & 0xF0)) << 4)
					| (unsigned short)(unsigned char)((p1[k] & 0xF0) | (p0[k] >> 4));
"""
T_SWAP = """				words[n] = (unsigned short)(unsigned char)((p1[n * 4] & 0xF0) | (p0[n * 4] >> 4))
					| (unsigned short)((((unsigned short)(p3[n * 4] & 0xF0) << 4) | (unsigned short)(p2[n * 4] & 0xF0)) << 4);
"""
def mk3(decl, body):
    return [(F, OLD, decl + """		for (int j = min.J; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				int n = j * size + i;
""" + body + """			}
		}
""")]
SZ = "\t\tint size = Field->Get_Size();\n"
T0 = """				words[n] = (unsigned short)((((unsigned short)(p3[n * 4] & 0xF0) << 4) | (unsigned short)(p2[n * 4] & 0xF0)) << 4)
					| (unsigned short)(unsigned char)((p1[n * 4] & 0xF0) | (p0[n * 4] >> 4));
"""
V = {
    'sz_w_p': mk3(SZ + W + P0123, T0),
    'w_sz_p': mk3(W + SZ + P0123, T0),
    'sz_p_w': mk3(SZ + P0123 + W, T0),
    'p_sz_w': mk3(P0123 + SZ + W, T0),
    'sz_w_p3210': mk3(SZ + W + P3210, T0),
}
COMPILE = [F]
WATCH = ['0x46D6B0']
