HYPOTHESIS = ('B4 fn3 batch 8 (tree: nest + Lxabcdy, masked 73; tmaxy cut r1 36 -> 33 but r9 8 -> 28): the client keeps the '
              'clamped count in eax and tests it (test; mov [count]; jle) where ours stores 1 and skips the test: a count '
              'written through the conditional form the client tests (ternary, !count, a separate assignment); with and '
              'without tmax_y taking the Min')
FILE = 'app/shared/heightfield.cpp'
SYMBOL = ''
ADDRESS = 0x4647C0


def ind(s):
    return '\n'.join(('\t' + l) if l else l for l in s.split('\n'))


MM_OLD = ind('\t\tfloat tmin = WWMath::Max(tmin_x, tmin_y);\n\t\tfloat tmax = WWMath::Min(tmax_x, tmax_y);\n'
             '\t\tif (tmin > 1.0f) {\n\t\t\treturn false;\n\t\t}\n\t\tif (tmax < 0.0f) {\n\t\t\treturn false;\n\t\t}\n'
             '\t\tif (tmin < 0.0f) {\n\t\t\ttmin = 0.0f;\n\t\t}\n\t\tif (tmax > 1.0f) {\n\t\t\ttmax = 1.0f;\n\t\t}\n'
             '\t\tx1 = tmax * dx + x0;\n\t\ty1 = tmax * dy + y0;\n\t\tx0 = tmin * dx + x0;\n\t\ty0 = tmin * dy + y0;\n')
MM_TY = (MM_OLD.replace('float tmax = ', 'tmax_y = ').replace('(tmax ', '(tmax_y ').replace('\ttmax = ', '\ttmax_y = ')
         .replace('= tmax *', '= tmax_y *'))
C_OLD = ('\t\tint count = WWMath::Float_To_Long(ceil(dist_y) + ceil(dist_x));\n'
         '\t\tif (count == 0) {\n\t\t\tcount = 1;\n\t\t}\n')
CF = {
    'tern': '\t\tint count = WWMath::Float_To_Long(ceil(dist_y) + ceil(dist_x));\n\t\tcount = count ? count : 1;\n\n\n',
    'not': '\t\tint count = WWMath::Float_To_Long(ceil(dist_y) + ceil(dist_x));\n\t\tif (!count) {\n\t\t\tcount = 1;\n\t\t}\n',
    'decl': '\t\tint count;\n\t\tcount = WWMath::Float_To_Long(ceil(dist_y) + ceil(dist_x));\n\t\tif (count == 0) count = 1;\n\n',
    'inc': '\t\tint count = WWMath::Float_To_Long(ceil(dist_y) + ceil(dist_x));\n\t\tif (count == 0) {\n\t\t\tcount++;\n\t\t}\n',
}
FORMS = {'ty': [(MM_OLD, MM_TY)]}
for k, v in CF.items():
    FORMS[k] = [(C_OLD, v)]
    FORMS['ty_' + k] = [(MM_OLD, MM_TY), (C_OLD, v)]
