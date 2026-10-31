F = 'app/shared/land.cpp'
DECL = "\tint size = Field->Get_Size();\n\tunsigned char *extra = Unknown1214;\n\tunsigned int i, j, value;\n"
DECL_NO = "\tint size = Field->Get_Size();\n\tunsigned int i, j, value;\n"
ELSE = "\t} else {\n\t\tfor (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {\n\t\t\tfor (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {\n\t\t\t\tunsigned int n = j * size + i;\n\t\t\t\tunsigned char *color = &colors[n * 3];\n\t\t\t\tunsigned char *light = &lightmap[n * 4];\n\t\t\t\tunsigned char *add = &extra[n];\n"
ELSE_DECL = ELSE.replace("\t} else {\n", "\t} else {\n\t\tunsigned char *extra = Unknown1214;\n", 1)
ELSE_MEMBER = ELSE.replace("&extra[n]", "&Unknown1214[n]")
V = {
    'else_decl': [(F, DECL, DECL_NO), (F, ELSE, ELSE_DECL)],
    'member_add': [(F, DECL, DECL_NO), (F, ELSE, ELSE_MEMBER)],
    'extra_unused_member': [(F, ELSE, ELSE_MEMBER)],
}
COMPILE = [F]
WATCH = ['0x46D410']
