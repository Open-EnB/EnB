F = 'app/shared/heightfield.cpp'
JL = "\tfor (int j = j0; j <= j1; j++) {\n"
IL = "\t\tfor (int i = i0; i <= i1; i++) {\n"
VD = "\t\t\tVector3 verts4[4];\n"
OLD = JL + IL + VD
HEAD = "void HeightFieldClass::Get_Triangles(const OBBoxClass &box, std::vector<Vector3> &verts)\n{\n"
V = {
    'v_top': [(F, OLD, JL + IL), (F, HEAD, HEAD + "\tVector3 verts4[4];\n")],
    'v_in_j': [(F, OLD, JL + "\t\tVector3 verts4[4];\n" + IL)],
    'v_before_loop': [(F, OLD, "\tVector3 verts4[4];\n" + JL + IL)],
    'ij_top_v_in_i': [(F, OLD, "\tint i, j;\n\tfor (j = j0; j <= j1; j++) {\n\t\tfor (i = i0; i <= i1; i++) {\n" + VD)],
}
COMPILE = [F]
WATCH = ['0x465D30']
