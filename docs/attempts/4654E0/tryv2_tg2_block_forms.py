F = 'app/shared/heightfield.cpp'
OLD = """	bool hit = false;
	Vector3 corners[16];
	int n = 0;
	int i, j, k;
	for (i = -1; i <= 1; i += 2) {
		for (j = -1; j <= 1; j += 2) {
			for (k = -1; k <= 1; k += 2) {
				float params[3];
				params[0] = (float)i;
				params[1] = (float)j;
				params[2] = (float)k;
				boxtest.Box.Compute_Point(params, &corners[n]);
				n++;
			}
		}
	}
	for (i = 0; i < 8; i++) {
		corners[i + 8] = corners[i] + boxtest.Move;
	}
	Vector3 min = corners[0];
	Vector3 max = corners[0];
	for (i = 1; i < 16; i++) {
		min.X = WWMath::Min(min.X, corners[i].X);
		max.X = WWMath::Max(max.X, corners[i].X);
		min.Y = WWMath::Min(min.Y, corners[i].Y);
		max.Y = WWMath::Max(max.Y, corners[i].Y);
	}
	float half = (float)HalfSize;
	int i0 = (int)(min.X / XSpacing + half - 1.0f);
	int i1 = (int)(max.X / XSpacing + half + 1.0f);
	int j1 = (int)(max.Y / YSpacing + half + 1.0f);
	int j0 = (int)(min.Y / YSpacing + half - 1.0f);
	for (j = j0; j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
"""
CORNERS_BLOCK = """	bool hit = false;
	Vector3 corners[16];
	{
		int n = 0;
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				for (int k = -1; k <= 1; k += 2) {
					float params[3];
					params[0] = (float)i;
					params[1] = (float)j;
					params[2] = (float)k;
					boxtest.Box.Compute_Point(params, &corners[n]);
					n++;
				}
			}
		}
	}
"""
MOVE = """	for (i = 0; i < 8; i++) {
		corners[i + 8] = corners[i] + boxtest.Move;
	}
"""
V3B = """	Vector3 min = corners[0];
	Vector3 max = corners[0];
	for (i = 1; i < 16; i++) {
		min.X = WWMath::Min(min.X, corners[i].X);
		max.X = WWMath::Max(max.X, corners[i].X);
		min.Y = WWMath::Min(min.Y, corners[i].Y);
		max.Y = WWMath::Max(max.Y, corners[i].Y);
	}
	float half = (float)HalfSize;
	int i0 = (int)(min.X / XSpacing + half - 1.0f);
	int i1 = (int)(max.X / XSpacing + half + 1.0f);
	int j1 = (int)(max.Y / YSpacing + half + 1.0f);
	int j0 = (int)(min.Y / YSpacing + half - 1.0f);
"""
SCB = """	float minx = corners[0].X;
	float maxx = corners[0].X;
	float miny = corners[0].Y;
	float maxy = corners[0].Y;
	for (i = 1; i < 16; i++) {
		minx = WWMath::Min(minx, corners[i].X);
		maxx = WWMath::Max(maxx, corners[i].X);
		miny = WWMath::Min(miny, corners[i].Y);
		maxy = WWMath::Max(maxy, corners[i].Y);
	}
	float half = (float)HalfSize;
	int i0 = (int)(minx / XSpacing + half - 1.0f);
	int i1 = (int)(maxx / XSpacing + half + 1.0f);
	int j1 = (int)(maxy / YSpacing + half + 1.0f);
	int j0 = (int)(miny / YSpacing + half - 1.0f);
"""
CELL = """	for (j = j0; j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
"""
IJ = "\tint i, j;\n"
V = {
    'blk_v3': [(F, OLD, CORNERS_BLOCK + IJ + MOVE + V3B + CELL)],
    'blk_sc': [(F, OLD, CORNERS_BLOCK + IJ + MOVE + SCB + CELL)],
    'blk_move_v3': [(F, OLD, CORNERS_BLOCK[:-3] + "\t\tfor (int m = 0; m < 8; m++) {\n\t\t\tcorners[m + 8] = corners[m] + boxtest.Move;\n\t\t}\n\t}\n" + IJ + V3B + CELL)],
}
COMPILE = [F]
WATCH = ['0x4654E0']
