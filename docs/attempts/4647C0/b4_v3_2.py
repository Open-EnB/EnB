HYPOTHESIS = ('B4 fn3 batch 2 (same lever): in the client tmin_x / tmax_x (-0x14 / -0x10) share the slot of the (Size - 1) '
              'int64 conversion temporary, so they are not live at the conversion: limit (and dx / dy) come before the '
              'tmin / tmax initialisations; every order of the seven declarations with limit before tmin_x')
FILE = 'app/shared/heightfield.cpp'
SYMBOL = ''
ADDRESS = 0x4647C0
import itertools
D = {
    'a': '\t\tfloat tmin_x = 0.0f;\n',
    'b': '\t\tfloat tmax_x = 1.0f;\n',
    'c': '\t\tfloat tmin_y = 0.0f;\n',
    'd': '\t\tfloat tmax_y = 1.0f;\n',
    'L': '\t\tfloat limit = (float)(Size - 1);\n',
    'x': '\t\tfloat dx = x1 - x0;\n',
    'y': '\t\tfloat dy = y1 - y0;\n',
}
OLD = ''.join(D[k] for k in 'abcdLxy')
ORDERS = ['Labcdxy', 'Lxyabcd', 'xyLabcd', 'Lxabcdy', 'Lcdabxy', 'xLyabcd', 'Lxycdab', 'cdLabxy', 'abLcdxy', 'Lbadcxy']
FORMS = {'o_' + o: [(OLD, ''.join(D[k] for k in o))] for o in ORDERS}
