HYPOTHESIS = ('B4 batch 4 (on split, res 28: one Rotate_Z inline too many, the first Set_Flag cut): top-level budget is '
              'spent in source order, so some of the early clamps written as WWMath::Clamp / Min (inline, header bytes '
              'exact, the 2026-11-13 row cut too much with all of them) take exactly the one Rotate_Z; every subset')
FILE = 'app/client/bar2d.cpp'
SYMBOL = ''
ADDRESS = 0x6B6900

SD = '\tSet_Dirty();\n}\n'
SPLIT = '\tSet_Dirty_Bounds(); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals();\n}\n'
L = [
    ('\tif (percent < 0) percent = 0; else if (percent > 1) percent = 1;\n', '\tpercent = WWMath::Clamp(percent, 0, 1);\n'),
    ('\tif (x < 0) x = 0; else if (x > 1) x = 1;\n', '\tx = WWMath::Clamp(x, 0, 1);\n'),
    ('\tif (y < 0) y = 0; else if (y > 1) y = 1;\n', '\ty = WWMath::Clamp(y, 0, 1);\n'),
    ('\tHeight = height <= 1 ? height : 1;\n', '\tHeight = WWMath::Min(height, 1);\n'),
    ('\tWidth = width <= 1 ? width : 1;\n', '\tWidth = WWMath::Min(width, 1);\n'),
]
FORMS = {}
for m in range(1, 32):
    h = [(SD, SPLIT)]
    name = ''
    for i in range(5):
        if m & (1 << i):
            h.append(L[i])
            name += 'pxyhw'[i]
    FORMS['k_' + name] = h
