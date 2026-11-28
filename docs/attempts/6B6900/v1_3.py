HYPOTHESIS = ('B4 batch 3 (on split, res 28): top-level spend follows source order, so the case bodies written in '
              'another order than 1, 2, 3 change which Rotate_Z inlines (the client inlines 1 only); the translation '
              'written as element stores (Set_X/Y/Z_Translation or Row stores) for r40\'s ecx / edx swap')
FILE = 'app/client/bar2d.cpp'
SYMBOL = ''
ADDRESS = 0x6B6900

SD = '\tSet_Dirty();\n}\n'
SPLIT = '\tSet_Dirty_Bounds(); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals();\n}\n'
C = {
    '1': '\t\tcase 1:\n\t\t\ttm.Rotate_Z(WWMATH_PI);\n\t\t\ty -= Height;\n\t\t\tx += Width;\n\t\t\tbreak;\n',
    '2': '\t\tcase 2:\n\t\t\ttm.Rotate_Z(WWMATH_PI / 2);\n\t\t\ty -= Width;\n\t\t\tbreak;\n',
    '3': '\t\tcase 3:\n\t\t\ttm.Rotate_Z(WWMATH_PI * 1.5f);\n\t\t\tx += Height;\n\t\t\tbreak;\n',
}
OLD_SW = C['1'] + C['2'] + C['3']
TR = '\ttm.Set_Translation(Vector3(x, y, 0));\n'
TRS = {
    'base': None,
    'setxyz': '\ttm.Set_X_Translation(x); tm.Set_Y_Translation(y); tm.Set_Z_Translation(0);\n',
    'rows': '\ttm[0][3] = x; tm[1][3] = y; tm[2][3] = 0;\n',
}
FORMS = {}
for order in ('123', '132', '213', '231', '312', '321'):
    for t, tv in TRS.items():
        if order == '123' and t == 'base':
            continue
        h = [(SD, SPLIT)]
        if order != '123':
            h.append((OLD_SW, ''.join(C[k] for k in order)))
        if tv:
            h.append((TR, tv))
        FORMS['o%s_%s' % (order, t)] = h
