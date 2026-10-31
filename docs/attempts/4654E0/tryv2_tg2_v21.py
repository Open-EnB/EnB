F = 'app/shared/heightfield.cpp'
OLD = """	float half = (float)HalfSize;
	int i0 = (int)(min.X / XSpacing + half - 1.0f);
	int i1 = (int)(max.X / XSpacing + half + 1.0f);
	int j1 = (int)(max.Y / YSpacing + half + 1.0f);
	int j0 = (int)(min.Y / YSpacing + half - 1.0f);
	for (j = j0; j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
			Vector3 verts[4];
			Vector3 normal;
			TriClass tri;
"""
TAIL = """			Vector3 verts[4];
			Vector3 normal;
			TriClass tri;
"""
V = {
    'd1': [(F, OLD, """	int i0 = (int)(min.X / XSpacing + (float)HalfSize - 1.0f);
	int i1 = (int)(max.X / XSpacing + (float)HalfSize + 1.0f);
	int j1 = (int)(max.Y / YSpacing + (float)HalfSize + 1.0f);
	int j0 = (int)(min.Y / YSpacing + (float)HalfSize - 1.0f);
	for (j = j0; j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
""" + TAIL)],
    'd2': [(F, OLD, """	float half = (float)HalfSize;
	int i0 = (int)(min.X / XSpacing + half - 1.0f);
	int i1 = (int)(max.X / XSpacing + half + 1.0f);
	int j1 = (int)(max.Y / YSpacing + half + 1.0f);
	for (j = (int)(min.Y / YSpacing + half - 1.0f); j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
""" + TAIL)],
    'd3': [(F, OLD, """	int i0 = (int)(min.X / XSpacing + (float)HalfSize - 1.0f);
	int i1 = (int)(max.X / XSpacing + (float)HalfSize + 1.0f);
	int j1 = (int)(max.Y / YSpacing + (float)HalfSize + 1.0f);
	for (j = (int)(min.Y / YSpacing + (float)HalfSize - 1.0f); j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
""" + TAIL)],
    'd4': [(F, OLD, """	float half = (float)HalfSize;
	int i0 = (int)(min.X / XSpacing + half - 1.0f);
	int i1 = (int)(max.X / XSpacing + half + 1.0f);
	int j1 = (int)(max.Y / YSpacing + half + 1.0f);
	int j0 = (int)(min.Y / YSpacing + half - 1.0f);
	Vector3 verts[4];
	Vector3 normal;
	TriClass tri;
	for (j = j0; j <= j1; j++) {
		for (i = i0; i <= i1; i++) {
""")],
}
COMPILE = [F]
WATCH = ['0x4654E0']
