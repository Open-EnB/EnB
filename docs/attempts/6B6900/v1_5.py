HYPOTHESIS = ('B4 fn1 batch 5 (tree: split, masked 28): case 2\'s Rotate_Z inlines because the client spends more top-level '
              'budget between the 8th vertex and case 2; the matrix written as Matrix3D tm; tm.Make_Identity() (a top-level '
              'Make_Identity site where Matrix3D(true) nests it; same stores) or with the default ctor plus Set / identity forms')
FILE = 'app/client/bar2d.cpp'
SYMBOL = ''
ADDRESS = 0x6B6900
MT = '\tMatrix3D tm(true);\n'
FORMS = {
    'mkid': [(MT, '\tMatrix3D tm; tm.Make_Identity();\n')],
    'mkid_h': [(MT, '\tMatrix3D tm; tm.Make_Identity();\n'), ('\tHeight = height <= 1 ? height : 1;\n', '\tHeight = WWMath::Min(height, 1);\n')],
    'mkid_w': [(MT, '\tMatrix3D tm; tm.Make_Identity();\n'), ('\tWidth = width <= 1 ? width : 1;\n', '\tWidth = WWMath::Min(width, 1);\n')],
    'mkid_p': [(MT, '\tMatrix3D tm; tm.Make_Identity();\n'), ('\tif (percent < 0) percent = 0; else if (percent > 1) percent = 1;\n', '\tpercent = WWMath::Clamp(percent, 0, 1);\n')],
    'one': [(MT, '\tMatrix3D tm(1);\n')],
    'mkid_tr': [(MT, '\tMatrix3D tm; tm.Make_Identity();\n'), ('\ttm.Set_Translation(Vector3(x, y, 0));\n', '\ttm.Set_X_Translation(x); tm.Set_Y_Translation(y); tm.Set_Z_Translation(0);\n')],
}
