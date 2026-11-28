HYPOTHESIS = ('B4 fn1 batch 6 (tree: split, masked 28): VC6 cuts the deepest expansions first, yet the client keeps all three '
              'Set_Flag and cuts case 2\'s top-level Rotate_Z: the flags are not nested in the client (Model->Set_Flag at the '
              'top level, Model being DynamicMeshClass\'s protected member; Peek_Model() a layer), so they spend less')
FILE = 'app/client/bar2d.cpp'
SYMBOL = ''
ADDRESS = 0x6B6900
SPLIT = '\tSet_Dirty_Bounds(); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals();\n'
F = 'MeshGeometryClass::'
DIRECT = ('\tModel->Set_Flag(' + F + 'DIRTY_BOUNDS, true); Model->Set_Flag(' + F + 'DIRTY_PLANES, true); Model->Set_Flag('
          + F + 'DIRTY_VNORMALS, true);\n')
PEEK = DIRECT.replace('Model->', 'Peek_Model()->')
FORMS = {
    'direct': [(SPLIT, DIRECT)],
    'peek': [(SPLIT, PEEK)],
    'direct1': [(SPLIT, '\tModel->Set_Flag(' + F + 'DIRTY_BOUNDS, true); Set_Dirty_Planes(); Set_Dirty_Vertex_Normals();\n')],
    'sd_direct': [(SPLIT, '\tSet_Dirty_Bounds(); Model->Set_Flag(' + F + 'DIRTY_PLANES, true); Model->Set_Flag(' + F + 'DIRTY_VNORMALS, true);\n')],
}
