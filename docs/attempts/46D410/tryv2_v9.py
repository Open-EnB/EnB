F = 'app/shared/land.cpp'
A = "\t\t\t\tunsigned char *add = &extra[j * size + i];\n"
Lt = "\t\t\t\tunsigned char *light = &lightmap[(j * size + i) * 4];\n"
C = "\t\t\t\tunsigned char *color = &colors[(j * size + i) * 3];\n"
OLD2 = A + Lt + C
B1_OLD = Lt + C + "\t\t\t\tvalue = (color[0] * light[0]) >> 8;\n"
B1_CL = C + Lt + "\t\t\t\tvalue = (color[0] * light[0]) >> 8;\n"
BASE = [(F, OLD2, C + Lt + A), (F, B1_OLD, B1_CL)]
N = "\t\t\t\tunsigned int n = j * size + i;\n"
nC = "\t\t\t\tunsigned char *color = &colors[n * 3];\n"
nL = "\t\t\t\tunsigned char *light = &lightmap[n * 4];\n"
nA = "\t\t\t\tunsigned char *add = &extra[n];\n"
iN = "\t\t\t\tint n = j * size + i;\n"
HDR2_OLD = "\t} else {\n\t\tfor (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {\n"
B1N = (F, B1_OLD, N + nC + nL + "\t\t\t\tvalue = (color[0] * light[0]) >> 8;\n")
xA = "\t\t\t\tunsigned char *add = &extra[j * size + i];\n"
pA = "\t\t\t\tunsigned char *add = extra + j * size + i;\n"
V = {
    'CLxA': [(F, OLD2, N + nC + nL + xA), B1N],
    'xACL': [(F, OLD2, xA + N + nC + nL), B1N],
    'CLpA': [(F, OLD2, N + nC + nL + pA), B1N],
    'pACL': [(F, OLD2, pA + N + nC + nL), B1N],
    'CxAL': [(F, OLD2, N + nC + xA + nL), B1N],
}
COMPILE = [F]
WATCH = ['0x46D410']
