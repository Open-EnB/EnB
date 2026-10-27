C = 'app/common/tcpconn.cpp'
FOR = '\tfor (;;) {\n\t\tCon->pumpReads();\n'
WHILE = FOR.replace('for (;;)', 'while (true)')
NB = '\t\t\tnbytes = ntohl(*(unsigned long *)data);\n'
NB2 = '\t\t\tnbytes = *(int *)data; nbytes = ntohl(nbytes);\n'
EB = '\t\t\t\tebytes = ntohl(*(unsigned long *)(data + nbytes + 4));\n'
EB2 = '\t\t\t\tebytes = *(int *)(data + nbytes + 4); ebytes = ntohl(ebytes);\n'
BASE = [(C, FOR, WHILE), (C, NB, NB2), (C, EB, EB2)]
READ = '\t\tCon->read(buffer, ebytes + nbytes + 8, 0);\n'
def dead(k):
    return READ + ''.join('\t\tint d%d = nbytes + %d;\n' % (i, i) for i in range(k))
L1 = '\t\tint len = ntohl(*(unsigned long *)(buffer + offset)); offset += 4;\n'
L1N = '\t\tint len = *(int *)(buffer + offset); len = ntohl(len); offset += 4;\n'
L2 = '\t\tlen = ntohl(*(unsigned long *)(buffer + offset)); offset += 4;\n'
L2N = '\t\tlen = *(int *)(buffer + offset); len = ntohl(len); offset += 4;\n'
V = {
    'dead2': BASE + [(C, READ, dead(2))],
    'dead4': BASE + [(C, READ, dead(4))],
    'dead8': BASE + [(C, READ, dead(8))],
    'dead16': BASE + [(C, READ, dead(16))],
    'ntohl_all': BASE + [(C, L1, L1N), (C, L2, L2N)],
}
COMPILE = [C]
WATCH = ['0x829180', '0x82A990']
EXTRA_INC = ['libs/sgistl']
