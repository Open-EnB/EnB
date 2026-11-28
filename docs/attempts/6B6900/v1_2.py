HYPOTHESIS = ('B4 batch 2 (on split, res 28: only the first Set_Flag out of line and case 2 Rotate_Z inlined): a read placed '
              'before the case 2 site or between the switch and the flags spends what case 2 takes; dead arithmetic '
              '(grows the pool) after the flags brings the first Set_Flag back inline')
FILE = 'app/client/bar2d.cpp'
SYMBOL = ''
ADDRESS = 0x6B6900

R = '(void)Percent.Get_Current(); '
A = '(void)(orientation + 1); '
ST = '\tSet_Transform(tm);\n'
SD = '\tSet_Dirty();\n}\n'
RZ = '\t\t\ttm.Rotate_Z(WWMATH_PI);\n'
RZ2 = '\t\t\ttm.Rotate_Z(WWMATH_PI / 2);\n'
MT = '\tMatrix3D tm(true);\n'
SW = '\t\t\tx += Height;\n\t\t\tbreak;\n\t}\n'
SPLIT = '\tSet_Dirty_Bounds(); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals();\n}\n'
FORMS = {}
for n in (1, 2, 3, 4, 5):
    FORMS['c1_%d' % n] = [(SD, SPLIT), (RZ, RZ[:-1] + ' ' + R * n + '\n')]
    FORMS['c2_%d' % n] = [(SD, SPLIT), (RZ2, '\t\t\t' + R * n + 'tm.Rotate_Z(WWMATH_PI / 2);\n')]
    FORMS['mt_%d' % n] = [(SD, SPLIT), (MT, '\tMatrix3D tm(true); ' + R * n + '\n')]
    FORMS['sw_%d' % n] = [(SD, SPLIT), (SW, '\t\t\tx += Height;\n\t\t\tbreak;\n\t} ' + R * n + '\n')]
    FORMS['ea_%d' % n] = [(SD, '\tSet_Dirty_Bounds(); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals(); ' + A * n + '\n}\n')]
    FORMS['ba_%d' % n] = [(SD, '\t' + A * n + 'Set_Dirty_Bounds(); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals();\n}\n')]
