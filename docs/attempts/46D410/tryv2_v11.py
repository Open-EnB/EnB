import runpy
_a = runpy.run_path(r'C:\EnBDevelopment\EnB\docs\attempts\46D410\tryv2_v9.py')
_h = runpy.run_path(r'C:\EnBDevelopment\EnB\docs\attempts\46D410\tryv2_v7.py')
F = _a['F']
N, nC, nL = _a['N'], _a['nC'], _a['nL']
HEAD = _h['V']['L_C_S_E_test_member']


def b2(*parts):
    return HEAD + [(F, _a['OLD2'], ''.join(parts)), _a['B1N']]


T = '\t\t\t\t'
V = {
    'a_sub': b2(N, nC, nL, T + 'unsigned char *add = &(extra + j * size)[i];\n'),
    'b_plus': b2(N, nC, nL, T + 'unsigned char *add = extra + n;\n'),
    'd_row': b2(T + 'unsigned int row = j * size;\n', T + 'unsigned int n = row + i;\n', nC, nL, T + 'unsigned char *add = &extra[row + i];\n'),
    'e_add_first_expr': b2(N, nC, nL, _a['nA']) + [(F, '((light[0] + add[0]) * color[0])', '((add[0] + light[0]) * color[0])')],
}
COMPILE = [F]
WATCH = ['0x46D410']
