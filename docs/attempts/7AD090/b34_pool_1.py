HYPOTHESIS = ('0x7AD090 residue 49 (the client inlines name\'s ~RefCountPtr, pointer kept in esi, the two 0x6CE7D0 tails '
              'cross-jumped; ours calls ~RefCountPtr): B31 pool lever: dead (void)(x + k) sums grow the function pool so '
              'the starved ~RefCountPtr site expands; counts 1-8, at the top, after the name local, at the tail')
FILE = 'app/client/galaxy_map_render.cpp'
SYMBOL = 'GalaxyMapRenderGateClass::Unknown_7AD090'
ADDRESS = 0x7AD090

TOP = '\tfloat angle = 0.0f;\n\tPeek_RObj(); GalaxyMapLineRObjClass *line'
NAME = '\tRefCountPtr<FontSys> name = To->Name;\n'
TAIL = '\t\tname->Unknown_6CE7D0(2);\n\t}\n\treturn(angle);\n}\n\n\nfloat GalaxyMapRenderGateClass::Unknown_7AD5B0'


def sums(n, v='place'):
    return ''.join(' (void)(%s + %d);' % (v, k) for k in range(n))


FORMS = {}
for n in (1, 2, 3, 4, 6, 8):
    FORMS['top%d' % n] = [(TOP, '\tfloat angle = 0.0f;' + sums(n) + '\n\tPeek_RObj(); GalaxyMapLineRObjClass *line')]
for n in (2, 4, 8):
    FORMS['nm%d' % n] = [(NAME, NAME[:-1] + sums(n) + '\n')]
    FORMS['tl%d' % n] = [(TAIL, TAIL.replace('\treturn(angle);', '\t' + sums(n).strip() + ' return(angle);'))]
for n in (2, 4):
    FORMS['top%d_nopeek' % n] = [(TOP, '\tfloat angle = 0.0f;' + sums(n) + '\n\tGalaxyMapLineRObjClass *line')]
