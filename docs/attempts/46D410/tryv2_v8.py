F = 'app/shared/land.cpp'
A = "\t\t\t\tunsigned char *add = &extra[j * size + i];\n"
Lt = "\t\t\t\tunsigned char *light = &lightmap[(j * size + i) * 4];\n"
C = "\t\t\t\tunsigned char *color = &colors[(j * size + i) * 3];\n"
OLD = A + Lt + C
B1_OLD = """				unsigned char *light = &lightmap[(j * size + i) * 4];
				unsigned char *color = &colors[(j * size + i) * 3];
				value = (color[0] * light[0]) >> 8;
"""
B1_CL = """				unsigned char *color = &colors[(j * size + i) * 3];
				unsigned char *light = &lightmap[(j * size + i) * 4];
				value = (color[0] * light[0]) >> 8;
"""
V = {
    'L_C_A': [(F, OLD, Lt + C + A)],
    'C_L_A': [(F, OLD, C + Lt + A)],
    'L_A_C': [(F, OLD, Lt + A + C)],
    'C_L_A_b1CL': [(F, OLD, C + Lt + A), (F, B1_OLD, B1_CL)],
    'b1CL': [(F, B1_OLD, B1_CL)],
}
COMPILE = [F]
WATCH = ['0x46D410']
