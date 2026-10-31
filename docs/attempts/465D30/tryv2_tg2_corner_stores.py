F = 'app/shared/heightfield.cpp'
OLD = """			verts4[2].X = verts4[1].X;
			verts4[3].Y = verts4[2].Y = verts4[0].Y + YSpacing;
			verts4[2].Z = Get_Height(i + 1, j + 1);
			verts4[3].X = verts4[0].X;
			verts4[3].Z = Get_Height(i, j + 1);
"""

def form(lines):
    return [(F, OLD, ''.join('\t\t\t' + l + '\n' for l in lines))]
V = {
    'g5': form(['verts4[2].X = verts4[1].X;', 'verts4[2].Y = verts4[3].Y = verts4[0].Y + YSpacing;', 'verts4[2].Z = Get_Height(i + 1, j + 1);', 'verts4[3].X = verts4[0].X;', 'verts4[3].Z = Get_Height(i, j + 1);']),
    'g6': form(['verts4[2].X = verts4[1].X;', 'verts4[3].Y = verts4[2].Y = verts4[0].Y + YSpacing;', 'verts4[3].X = verts4[0].X;', 'verts4[2].Z = Get_Height(i + 1, j + 1);', 'verts4[3].Z = Get_Height(i, j + 1);']),
    'g7': form(['verts4[3].X = verts4[0].X;', 'verts4[2].X = verts4[1].X;', 'verts4[3].Y = verts4[2].Y = verts4[0].Y + YSpacing;', 'verts4[2].Z = Get_Height(i + 1, j + 1);', 'verts4[3].Z = Get_Height(i, j + 1);']),
    'g8': form(['verts4[2].X = verts4[1].X;', 'verts4[2].Y = verts4[0].Y + YSpacing;', 'verts4[2].Z = Get_Height(i + 1, j + 1);', 'verts4[3].Y = verts4[2].Y;', 'verts4[3].X = verts4[0].X;', 'verts4[3].Z = Get_Height(i, j + 1);']),
    'g9': form(['verts4[2].X = verts4[1].X;', 'verts4[3].X = verts4[0].X;', 'verts4[2].Y = verts4[0].Y + YSpacing;', 'verts4[2].Z = Get_Height(i + 1, j + 1);', 'verts4[3].Y = verts4[2].Y;', 'verts4[3].Z = Get_Height(i, j + 1);']),
}
COMPILE = [F]
WATCH = ['0x465D30']
