import runpy
_b = runpy.run_path(r'C:\EnBDevelopment\EnB\docs\attempts\4654E0\tryv2_tg2_block_forms.py')
F = _b['F']
OLD = _b['OLD']
CORNERS_BLOCK = _b['CORNERS_BLOCK']
MOVE_M = "\tfor (int m = 0; m < 8; m++) {\n\t\tcorners[m + 8] = corners[m] + boxtest.Move;\n\t}\n"
BOUNDS_B = """	Vector3 min = corners[0];
	Vector3 max = corners[0];
	for (int b = 1; b < 16; b++) {
		min.X = WWMath::Min(min.X, corners[b].X);
		max.X = WWMath::Max(max.X, corners[b].X);
		min.Y = WWMath::Min(min.Y, corners[b].Y);
		max.Y = WWMath::Max(max.Y, corners[b].Y);
	}
	float half = (float)HalfSize;
	int i0 = (int)(min.X / XSpacing + half - 1.0f);
	int i1 = (int)(max.X / XSpacing + half + 1.0f);
	int j1 = (int)(max.Y / YSpacing + half + 1.0f);
	int j0 = (int)(min.Y / YSpacing + half - 1.0f);
"""
CELL_D = "\tfor (int j = j0; j <= j1; j++) {\n\t\tfor (int i = i0; i <= i1; i++) {\n"
BOUNDS_BLK = "\tint i0, i1, j0, j1;\n\t{\n" + ''.join('\t' + l + '\n' for l in BOUNDS_B.replace('\tint i0', '\ti0').replace('\tint i1', '\ti1').replace('\tint j1', '\tj1').replace('\tint j0', '\tj0').splitlines()) + "\t}\n"
V = {
    'e1': [(F, OLD, CORNERS_BLOCK + MOVE_M + BOUNDS_B + CELL_D)],
    'e2': [(F, OLD, CORNERS_BLOCK + MOVE_M + BOUNDS_BLK + CELL_D)],
    'e3': [(F, OLD, CORNERS_BLOCK[:-3] + "\t\t" + MOVE_M.replace('\n\t', '\n\t\t') + "\t}\n" + BOUNDS_BLK + CELL_D)],
}
COMPILE = [F]
WATCH = ['0x4654E0']
