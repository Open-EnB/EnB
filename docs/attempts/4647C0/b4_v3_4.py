HYPOTHESIS = ('B4 fn3 batch 4 (on Lxabcdy, masked 74): the client\'s StartBad exit reloads hit from its slot (mov al, '
              '[ebp-5]) where ours returns the constant: the StartBad case falls through to the function\'s single final '
              '"return hit" (the body in an else), VC6 duplicating the short epilogue; the tmin / tmax as if-forms of Max / Min')
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
MM_OLD = '\t\tfloat tmin = WWMath::Max(tmin_x, tmin_y);\n\t\tfloat tmax = WWMath::Min(tmax_x, tmax_y);\n'
MM_TERN = '\t\tfloat tmin = tmin_x > tmin_y ? tmin_x : tmin_y;\n\t\tfloat tmax = tmax_x < tmax_y ? tmax_x : tmax_y;\n'
MM_MAC = '\t\tfloat tmin = MAX(tmin_x, tmin_y);\n\t\tfloat tmax = MIN(tmax_x, tmax_y);\n'
FORMS = {
    'nest': ORD + NEST,
    'nest_base': NEST,
    'tern': ORD + [(MM_OLD, MM_TERN)],
    'mac': ORD + [(MM_OLD, MM_MAC)],
    'nest_mac': ORD + NEST + [(MM_OLD, MM_MAC)],
}
