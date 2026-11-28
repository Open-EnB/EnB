HYPOTHESIS = ('B4 fn3 batch 6 (on nest + Lxabcdy, masked 73): the client stores Max(tmin_x, tmin_y) into tmin_y\'s own slot '
              '(-0x20, a self-copy in the else path) and keeps tmax_y on the FPU into the Min: no tmin / tmax named locals, '
              'the y slab\'s variables take the results; its Max has two arms (the else a self-copy): a ternary, not WWMath::Max')
FILE = 'app/shared/heightfield.cpp'
SYMBOL = ''
ADDRESS = 0x4647C0
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
ORD = [(OLD, ''.join(D[k] for k in 'Lxabcdy'))]
SB_OLD = '\t\traytest.Result->StartBad = true;\n\t\treturn hit;\n\t}\n'
SB_NEW = '\t\traytest.Result->StartBad = true;\n\t} else {\n\t{\n'
END_OLD = '\t\t\tj += step_y;\n\t\t}\n\t}\n\treturn hit;\n}\n'
END_NEW = '\t\t\tj += step_y;\n\t\t}\n\t}\n\t}} return hit;\n}\n'
NEST = [(SB_OLD, SB_NEW), (END_OLD, END_NEW)]
MM_OLD = ('\t\tfloat tmin = WWMath::Max(tmin_x, tmin_y);\n\t\tfloat tmax = WWMath::Min(tmax_x, tmax_y);\n'
          '\t\tif (tmin > 1.0f) {\n\t\t\treturn false;\n\t\t}\n\t\tif (tmax < 0.0f) {\n\t\t\treturn false;\n\t\t}\n'
          '\t\tif (tmin < 0.0f) {\n\t\t\ttmin = 0.0f;\n\t\t}\n\t\tif (tmax > 1.0f) {\n\t\t\ttmax = 1.0f;\n\t\t}\n'
          '\t\tx1 = tmax * dx + x0;\n\t\ty1 = tmax * dy + y0;\n\t\tx0 = tmin * dx + x0;\n\t\ty0 = tmin * dy + y0;\n')


def mm(tmin, tmax, first):
    s = MM_OLD.replace('float tmin = ', first[0]).replace('float tmax = ', first[1])
    s = s.replace('(tmin ', '(' + tmin + ' ').replace('\ttmin = ', '\t' + tmin + ' = ').replace('= tmin *', '= ' + tmin + ' *')
    s = s.replace('(tmax ', '(' + tmax + ' ').replace('\ttmax = ', '\t' + tmax + ' = ').replace('= tmax *', '= ' + tmax + ' *')
    return s


Y = mm('tmin_y', 'tmax_y', ('tmin_y = ', 'tmax_y = '))
TMAX = 'WWMath::Max(tmin_x, tmin_y)'
TMIN = 'WWMath::Min(tmax_x, tmax_y)'
T1 = Y.replace(TMAX, 'tmin_x > tmin_y ? tmin_x : tmin_y')
T2 = T1.replace(TMIN, 'tmax_x < tmax_y ? tmax_x : tmax_y')
T3 = Y.replace(TMIN, 'tmax_x < tmax_y ? tmax_x : tmax_y')
T4 = Y.replace(TMAX, 'MAX(tmin_x, tmin_y)').replace(TMIN, 'MIN(tmax_x, tmax_y)')
FORMS = {
    'tmax_tern': ORD + NEST + [(MM_OLD, T1)],
    'both_tern': ORD + NEST + [(MM_OLD, T2)],
    'tmin_tern': ORD + NEST + [(MM_OLD, T3)],
    'mac': ORD + NEST + [(MM_OLD, T4)],
    'tmax_tern_b': NEST + [(MM_OLD, T1)],
    'both_tern_b': NEST + [(MM_OLD, T2)],
}
