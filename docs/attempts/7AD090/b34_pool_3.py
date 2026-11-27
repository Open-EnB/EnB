HYPOTHESIS = ('0x7AD090: sums grow the pool in steps (2 sums: no change; 3: Set_Color 0x7A8350 inlines; 4: Translate_X '
              'too), the dtor never; if the per-site test is cost <= remaining, a growth between 2 and 3 sums lets the '
              'cheaper last site (name\'s ~RefCountPtr) expand while Set_Color stays called: finer growth statements')
FILE = 'app/client/galaxy_map_render.cpp'
SYMBOL = 'GalaxyMapRenderGateClass::Unknown_7AD090'
ADDRESS = 0x7AD090

TOP = '\tfloat angle = 0.0f;\n\tPeek_RObj(); GalaxyMapLineRObjClass *line'


def top(extra):
    return [(TOP, '\tfloat angle = 0.0f;' + extra + '\n\tPeek_RObj(); GalaxyMapLineRObjClass *line')]


S2 = ' (void)(place + 0); (void)(place + 1);'
FORMS = {
    's2_v1': top(S2 + ' (void)place;'),
    's2_v2': top(S2 + ' (void)place; (void)show_inter;'),
    's2_v3': top(S2 + ' (void)place; (void)show_inter; (void)show_intra;'),
    's2_f1': top(S2 + ' (void)(angle + 0.0f);'),
    's2_m1': top(S2 + ' (void)(place * 2);'),
    's2_c1': top(S2 + ' (void)(place + 0 + 1);'),
    's2_n1': top(S2 + ' (void)!place;'),
    's1_f1': top(' (void)(place + 0); (void)(angle + 0.0f);'),
    'f1': top(' (void)(angle + 0.0f);'),
    'f2': top(' (void)(angle + 0.0f); (void)(angle + 1.0f);'),
    'f3': top(' (void)(angle + 0.0f); (void)(angle + 1.0f); (void)(angle + 2.0f);'),
    's2_cmp': top(S2 + ' (void)(place == 0);'),
    's2_sh': top(S2 + ' (void)(place << 1);'),
    's2_and': top(S2 + ' (void)(place && show_inter);'),
}
