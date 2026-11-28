HYPOTHESIS = ('0x7AD090: batch 1 showed 4+ dead sums grow the pool but the growth inlines the else branch Translate_X '
              '(0x711450, which the client calls) before it reaches name\'s ~RefCountPtr at the end; B31 pool lever: '
              'a dead read after the else Translate_X site and before the end spends the growth there, so the else call '
              'stays out of line and the dtor expands')
FILE = 'app/client/galaxy_map_render.cpp'
SYMBOL = 'GalaxyMapRenderGateClass::Unknown_7AD090'
ADDRESS = 0x7AD090

TOP = '\tfloat angle = 0.0f;\n\tPeek_RObj(); GalaxyMapLineRObjClass *line'
ELSE_TX = '\t\t\ttm.Translate_X(2.0f);\n\t\t\tname->Set_Justify(0, 3);\n\t\t}\n\t\tname->Set_Transform(tm);\n'
RET = '\t\tname->Unknown_6CE7D0(2);\n\t}\n\treturn(angle);\n}\n\n\nfloat GalaxyMapRenderGateClass::Unknown_7AD5B0'


def sums(n, v='place'):
    return ''.join(' (void)(%s + %d);' % (v, k) for k in range(n))


def top(n):
    return (TOP, '\tfloat angle = 0.0f;' + sums(n) + '\n\tPeek_RObj(); GalaxyMapLineRObjClass *line')


def at(pos, r):
    if pos == 'tx':
        return (ELSE_TX, '\t\t\ttm.Translate_X(2.0f); ' + r + '\n\t\t\tname->Set_Justify(0, 3);\n\t\t}\n\t\tname->Set_Transform(tm);\n')
    if pos == 'js':
        return (ELSE_TX, '\t\t\ttm.Translate_X(2.0f);\n\t\t\tname->Set_Justify(0, 3); ' + r + '\n\t\t}\n\t\tname->Set_Transform(tm);\n')
    if pos == 'st':
        return (ELSE_TX, '\t\t\ttm.Translate_X(2.0f);\n\t\t\tname->Set_Justify(0, 3);\n\t\t}\n\t\t' + r + ' name->Set_Transform(tm);\n')
    if pos == 'rt':
        return (RET, RET.replace('\treturn(angle);', '\t' + r + ' return(angle);'))


FORMS = {}
for n in (4, 8):
    for pos in ('tx', 'js', 'st', 'rt'):
        FORMS['s%d_%s_np' % (n, pos)] = [top(n), at(pos, 'name.Peek();')]
        FORMS['s%d_%s_lp' % (n, pos)] = [top(n), at(pos, 'Line.Peek();')]
for pos in ('tx', 'rt'):
    FORMS['s4_%s_np2' % pos] = [top(4), at(pos, 'name.Peek(); name.Peek();')]
    FORMS['s3_%s_np' % pos] = [top(3), at(pos, 'name.Peek();')]
