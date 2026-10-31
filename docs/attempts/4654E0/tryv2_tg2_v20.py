F = 'app/shared/heightfield.cpp'
OLD = """			Get_Vertex(i, j, &verts[0]);
			verts[1].X = verts[0].X + XSpacing;
			verts[1].Y = verts[0].Y;
			verts[1].Z = Get_Height(i + 1, j);
			verts[2].X = verts[0].X + XSpacing;
			verts[2].Y = verts[0].Y + YSpacing;
			verts[2].Z = Get_Height(i + 1, j + 1);
			verts[3].X = verts[0].X;
			verts[3].Y = verts[0].Y + YSpacing;
			verts[3].Z = Get_Height(i, j + 1);
"""


def blk(lines):
    return ''.join('\t\t\t' + l + '\n' for l in lines)


V = {
    'c1': [(F, OLD, blk(['Get_Vertex(i, j, &verts[0]);', 'verts[1].X = verts[0].X + XSpacing;', 'verts[1].Y = verts[0].Y;', 'verts[1].Z = Get_Height(i + 1, j);',
                         'verts[2].X = verts[1].X;', 'float y = verts[0].Y + YSpacing;', 'verts[2].Y = y;', 'verts[2].Z = Get_Height(i + 1, j + 1);',
                         'verts[3].X = verts[0].X;', 'verts[3].Y = y;', 'verts[3].Z = Get_Height(i, j + 1);']))],
    'c2': [(F, OLD, blk(['Get_Vertex(i, j, &verts[0]);', 'verts[1].X = verts[0].X + XSpacing;', 'verts[1].Y = verts[0].Y;', 'verts[1].Z = Get_Height(i + 1, j);',
                         'verts[2].X = verts[0].X + XSpacing;', 'float y = verts[0].Y + YSpacing;', 'verts[2].Y = y;', 'verts[2].Z = Get_Height(i + 1, j + 1);',
                         'verts[3].X = verts[0].X;', 'verts[3].Y = y;', 'verts[3].Z = Get_Height(i, j + 1);']))],
    'c3': [(F, OLD, blk(['Get_Vertex(i, j, &verts[0]);', 'float x = verts[0].X + XSpacing;', 'verts[1].X = x;', 'verts[1].Y = verts[0].Y;', 'verts[1].Z = Get_Height(i + 1, j);',
                         'verts[2].X = x;', 'float y = verts[0].Y + YSpacing;', 'verts[2].Y = y;', 'verts[2].Z = Get_Height(i + 1, j + 1);',
                         'verts[3].X = verts[0].X;', 'verts[3].Y = y;', 'verts[3].Z = Get_Height(i, j + 1);']))],
    'c4': [(F, OLD, blk(['Get_Vertex(i, j, &verts[0]);', 'verts[1].X = verts[0].X + XSpacing;', 'verts[1].Y = verts[0].Y;', 'verts[1].Z = Get_Height(i + 1, j);',
                         'verts[2].X = verts[1].X;', 'verts[3].Y = verts[2].Y = verts[0].Y + YSpacing;', 'verts[2].Z = Get_Height(i + 1, j + 1);',
                         'verts[3].X = verts[0].X;', 'verts[3].Z = Get_Height(i, j + 1);']))],
}
COMPILE = [F]
WATCH = ['0x4654E0']
