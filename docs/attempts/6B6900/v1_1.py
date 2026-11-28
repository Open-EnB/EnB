HYPOTHESIS = ('B4 batch 1: the client has slightly less pool left at case 2 (Rotate_Z called in 2 and 3, the small Set_Dirty '
              'still inlined, the 8th UV cut): dead Percent.Get_Current() reads after the late sites (before Set_Dirty, '
              'before Set_Transform, inside case 1, after Set_Dirty) spend from the last site backwards; counts 1-4, 6')
FILE = 'app/client/bar2d.cpp'
SYMBOL = ''
ADDRESS = 0x6B6900

R = '(void)Percent.Get_Current(); '
ST = '\tSet_Transform(tm);\n'
SD = '\tSet_Dirty();\n}\n'
RZ = '\t\t\ttm.Rotate_Z(WWMATH_PI);\n'
TR = '\ttm.Set_Translation(Vector3(x, y, 0));\n'
FORMS = {}
for n in (1, 2, 3, 4, 6):
    FORMS['sd%d' % n] = [(ST, '\tSet_Transform(tm); ' + R * n + '\n')]
    FORMS['st%d' % n] = [(TR, '\ttm.Set_Translation(Vector3(x, y, 0)); ' + R * n + '\n')]
    FORMS['c1_%d' % n] = [(RZ, '\t\t\ttm.Rotate_Z(WWMATH_PI); ' + R * n + '\n')]
    FORMS['end%d' % n] = [(SD, '\tSet_Dirty(); ' + R * n + '\n}\n')]
SPLIT = '\tSet_Dirty_Bounds(); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals();\n}\n'
FORMS['split'] = [(SD, SPLIT)]
for n in (1, 2, 3):
    FORMS['split_sd%d' % n] = [(SD, SPLIT), (ST, '\tSet_Transform(tm); ' + R * n + '\n')]
