F = 'app/shared/land.cpp'
OLD = """		for (int j = min.J; j < max.J; j++) {
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


def loop(fmt, pre=''):
    return [(F, OLD, pre + """		for (int j = min.J; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				int n = j * size + i;
				words[n] = """ + ex(fmt) + """;
			}
		}
""")]
V = {
    'kfirst': loop('bytes[%d + n * 4]'),
    'four_n': loop('bytes[4 * n + %d]'),
    'plus': loop('(bytes + %d)[n * 4]'),
    'deref': loop('*(bytes + n * 4 + %d)'),
    'idx4': loop('bytes[(n << 2) + %d]'),
    'ptrk': loop('p%d[n * 4]', """		unsigned char *p0 = bytes;
		unsigned char *p1 = bytes + 1;
		unsigned char *p2 = bytes + 2;
		unsigned char *p3 = bytes + 3;
"""),
}
COMPILE = [F]
WATCH = ['0x46D6B0']
