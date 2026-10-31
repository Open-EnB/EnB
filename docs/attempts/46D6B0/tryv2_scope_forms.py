F = 'app/shared/land.cpp'
W = "\t\tunsigned short *words = (unsigned short *)Unknown120C;\n"
P = ("\t\tunsigned char *p0 = (unsigned char *)Unknown120C;\n\t\tunsigned char *p1 = p0 + 1;\n"
     "\t\tunsigned char *p2 = p0 + 2;\n\t\tunsigned char *p3 = p0 + 3;\n")
S = "\t\tint size = Field->Get_Size();\n"
TOP = "\tSurfaceClass *surface = Unknown120->Get_Surface_Level(0);\n\tif (Is16Bit) {\n"
JL = "\t\tfor (int j = min.J; j < max.J; j++) {\n"
IL = "\t\t\tfor (int i = min.I; i < max.I; i++) {\n"
OLD = TOP + W + P + S + JL + IL


def ind(t, k=1):
    return ''.join('\t' * k + l + '\n' for l in t.splitlines())


V = {
    'size_top': [(F, OLD, "\tSurfaceClass *surface = Unknown120->Get_Surface_Level(0);\n\tint size = Field->Get_Size();\n\tif (Is16Bit) {\n" + W + P + JL + IL)],
    'ptrs_in_j': [(F, OLD, TOP + S + JL + ind(W + P) + IL)],
    'ptrs_in_i': [(F, OLD, TOP + S + JL + IL + ind(W + P, 2))],
    'p_in_j_w_out': [(F, OLD, TOP + W + S + JL + ind(P) + IL)],
    'w_in_j': [(F, OLD, TOP + P + S + JL + ind(W) + IL)],
    'all_top': [(F, OLD, "\tSurfaceClass *surface = Unknown120->Get_Surface_Level(0);\n" + ind(W + P + S, 0).replace('\t\t', '\t') + "\tif (Is16Bit) {\n" + JL + IL)],
}
COMPILE = [F]
WATCH = ['0x46D6B0']
