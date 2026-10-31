import runpy
_a = runpy.run_path(r'C:\EnBDevelopment\EnB\docs\attempts\46D410\tryv2_v9.py')
_h = runpy.run_path(r'C:\EnBDevelopment\EnB\docs\attempts\46D410\tryv2_v7.py')
F = _a['F']
N12 = [(F, _a['OLD2'], _a['N'] + _a['nC'] + _a['nL'] + _a['nA']), _a['B1N']]
V = {}
for k in ('L_C_S_E', 'L_C_S_E_test_member', 'test_not', 'E_L_C_S'):
    V['n12_' + k] = _h['V'][k] + N12
COMPILE = [F]
WATCH = ['0x46D410']
