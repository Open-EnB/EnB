F = 'app/shared/heightfield.cpp'
CUR = ['verts4[2].X = verts4[1].X;', 'verts4[2].Y = verts4[0].Y + YSpacing;', 'verts4[2].Z = Get_Height(i + 1, j + 1);',
       'verts4[3].Y = verts4[2].Y;', 'verts4[3].X = verts4[0].X;', 'verts4[3].Z = Get_Height(i, j + 1);']


def blk(lines):
    return ''.join('\t\t\t' + l + '\n' for l in lines)


OLD = blk(CUR)
HEAD = CUR[:3]


def form(tail):
    return [(F, OLD, blk(HEAD + tail))]


V = {
    'h1': form(['verts4[3].Z = Get_Height(i, j + 1);', 'verts4[3].X = verts4[0].X;', 'verts4[3].Y = verts4[2].Y;']),
    'h2': form(['verts4[3] = Vector3(verts4[0].X, verts4[2].Y, Get_Height(i, j + 1));']),
    'h3': form(['verts4[3].Set(verts4[0].X, verts4[2].Y, Get_Height(i, j + 1));']),
    'h4': form(['verts4[3].X = verts4[0].X;', 'verts4[3].Y = verts4[0].Y + YSpacing;'.replace('verts4[0].Y + YSpacing', 'verts4[2].Y'), 'verts4[3].Z = Get_Height(i, j + 1);']),
    'h5': [(F, OLD, blk(['verts4[2].X = verts4[1].X;', 'float y = verts4[0].Y + YSpacing;', 'verts4[2].Y = y;', 'verts4[2].Z = Get_Height(i + 1, j + 1);',
                         'verts4[3].X = verts4[0].X;', 'verts4[3].Y = y;', 'verts4[3].Z = Get_Height(i, j + 1);']))],
}
COMPILE = [F]
WATCH = ['0x465D30']
