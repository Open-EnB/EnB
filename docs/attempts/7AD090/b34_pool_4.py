HYPOTHESIS = ('0x7AD090: growth is per statement (3 sums: Set_Color inlines, 4: Translate_X too), Peek reads cost ~0; '
              'heavier pure inline spenders (Vector3::Length2, Matrix3D::Get_Translation) between the sites the client '
              'calls (Set_Color, the else Translate_X) and name\'s ~RefCountPtr at the end, with 3-4 sums: those sites '
              'called and the dtor expanded')
FILE = 'app/client/galaxy_map_render.cpp'
SYMBOL = 'GalaxyMapRenderGateClass::Unknown_7AD090'
ADDRESS = 0x7AD090

TOP = '\tfloat angle = 0.0f;\n\tPeek_RObj(); GalaxyMapLineRObjClass *line'
ELSE_TX = '\t\t\ttm.Translate_X(2.0f);\n\t\t\tname->Set_Justify(0, 3);\n'
COLOR = '\t\tVector3 color; name->Set_Color(To->Unknown_7ABA70(color, false));\n'
END = '\t\tname->Unknown_6CE7D0(0);\n\t} else {\n'


def sums(n, v='place'):
    return ''.join(' (void)(%s + %d);' % (v, k) for k in range(n))


def top(n):
    return (TOP, '\tfloat angle = 0.0f;' + sums(n) + '\n\tPeek_RObj(); GalaxyMapLineRObjClass *line')


def at(pos, r):
    if pos == 'tx':
        return (ELSE_TX, '\t\t\ttm.Translate_X(2.0f); ' + r + '\n\t\t\tname->Set_Justify(0, 3);\n')
    if pos == 'co':
        return (COLOR, COLOR[:-1] + ' ' + r + '\n')
    if pos == 'en':
        return (END, '\t\tname->Unknown_6CE7D0(0); ' + r + '\n\t} else {\n')


SP = {'l2': 'd.Length2();', 'gt': 'tm.Get_Translation();', 'l2x2': 'd.Length2(); a.Length2();'}
FORMS = {}
for n in (3, 4):
    for pos in ('tx', 'co', 'en'):
        for k, r in SP.items():
            FORMS['s%d_%s_%s' % (n, pos, k)] = [top(n), at(pos, r)]
FORMS['s0_co_l2'] = [at('co', 'd.Length2();')]
FORMS['s0_en_gt'] = [at('en', 'tm.Get_Translation();')]
