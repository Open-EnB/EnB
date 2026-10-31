F = 'app/shared/land.cpp'
HEAD = """void LandClass::Apply_Colors(void)
{
	unsigned char *lightmap = (unsigned char *)Unknown120C;
	unsigned char *colors = Colors;
	unsigned char *extra = Unknown1214;
	int size = Field->Get_Size();
	unsigned int i, j, value;
	if (extra == NULL) {
"""
L = "\tunsigned char *lightmap = (unsigned char *)Unknown120C;\n"
C = "\tunsigned char *colors = Colors;\n"
E = "\tunsigned char *extra = Unknown1214;\n"
S = "\tint size = Field->Get_Size();\n"
T = "\tunsigned int i, j, value;\n\tif (extra == NULL) {\n"


def h(*parts, test=T):
    return [(F, HEAD, "void LandClass::Apply_Colors(void)\n{\n" + ''.join(parts) + test)]


V = {
    'S_L_C_E': h(S, L, C, E),
    'L_C_S_E': h(L, C, S, E),
    'E_L_C_S': h(E, L, C, S),
    'L_C_E_S_ijv_first': [(F, HEAD, "void LandClass::Apply_Colors(void)\n{\n\tunsigned int i, j, value;\n" + L + C + E + S + "\tif (extra == NULL) {\n")],
    'test_not': h(L, C, E, S, test="\tunsigned int i, j, value;\n\tif (!extra) {\n"),
    'L_C_S_E_test_member': h(L, C, S, E, test="\tunsigned int i, j, value;\n\tif (Unknown1214 == NULL) {\n"),
}
COMPILE = [F]
WATCH = ['0x46D410']
