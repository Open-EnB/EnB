"""Imports EA's Westwood libraries into the repository, file by file and function by function,
keeping whatever reproduces client.exe best.

Evidence comes from tools/vc6/bytematch_all.sh: for every EA copy (R Renegade, G Generals,
Z Zero Hour) and every source file, the client functions it reproduces byte for byte.

    python importer.py plan     --evidence DIR     choose a base copy per library and a copy per file
    python importer.py assemble --evidence DIR     copy the chosen files into the repository
    python importer.py verify                      compile the repository's libraries, byte-match them
    python importer.py refine   --evidence DIR     swap files / splice functions from other copies
                                                   where that adds matches and loses none
    python importer.py report                      write docs/imports.md

State (what came from where, what was merged, what matches) is kept in docs/data/imports.json.
Imported files are not edited except by splicing whole function definitions from another copy,
and by the patches in PATCHES (each documented in docs/imports.md).
"""
import argparse
import collections
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import annotations  # noqa: E402
import vc6  # noqa: E402
from clientimage import ClientImage  # noqa: E402
from easource import function_spans  # noqa: E402
from objmatch import match_objects  # noqa: E402

EA = r'C:\EnBDevelopment\EA'
SDK_INCLUDES = [r'C:\EnBDevelopment\SDK\directx8\include', r'C:\EnBDevelopment\SDK\radtools\include_enb']
STATE = os.path.join(REPO, 'docs', 'data', 'imports.json')
WORK = os.path.join(os.environ.get('TEMP', '.'), 'enb_import')

COPY_ROOT = {
    'R': EA + r'\CnC_Renegade\Code',
    'G': EA + r'\CnC_Generals_Zero_Hour\Generals\Code\Libraries\Source\WWVegas',
    'Z': EA + r'\CnC_Generals_Zero_Hour\GeneralsMD\Code\Libraries\Source\WWVegas',
}
COPY_NAME = {'R': 'Renegade', 'G': 'Generals', 'Z': 'Zero Hour', 'reconstructed': 'reconstructed'}
COPY_COMMIT = {'R': 'CnC_Renegade@3e00c3a', 'G': 'CnC_Generals_Zero_Hour@0a05454',
               'Z': 'CnC_Generals_Zero_Hour@0a05454'}
LIB_DIR = {
    'wwdebug': {'R': 'wwdebug', 'G': 'WWDebug', 'Z': 'WWDebug'},
    'wwlib': {'R': 'wwlib', 'G': 'WWLib', 'Z': 'WWLib'},
    'wwmath': {'R': 'WWMath', 'G': 'WWMath', 'Z': 'WWMath'},
    'wwsaveload': {'R': 'wwsaveload', 'G': 'WWSaveLoad', 'Z': 'WWSaveLoad'},
    'wwaudio': {'R': 'WWAudio', 'G': 'WWAudio', 'Z': 'WWAudio'},
    'ww3d2': {'R': 'ww3d2', 'G': 'WW3D2', 'Z': 'WW3D2'},
    'wwshade': {'Z': 'wwshade'},
}
LIBS = list(LIB_DIR)
HEADER_EXT = ('.h', '.hpp', '.inl')

# Local changes to imported files. Each keeps the file's line count, so __LINE__ values (which
# end up in the code) don't move.
PATCHES = {
    ('wwdebug', 'wwdebug.h'): [(
        '#include "..\\..\\..\\..\\gameengine\\include\\common\\debug.h"',
        '#include "gamedebug.h"',
        "Zero Hour's wwdebug.h includes Generals' GameEngine debug.h; gamedebug.h gives the release "
        "build's definitions of the macros it uses (all compile to nothing)")],
    ('ww3d2', 'meshbuild.h'): [(
        'MAX_STAGES = 2,',
        'MAX_STAGES = 8,',
        "MeshBuilderClass::MAX_STAGES is 8 in E&B, as in Zero Hour (Renegade and Generals: 2). Found "
        "from VertexMaterialClass: E&B's CRCDirty is at +0x6C, 0x30 bytes later than with 2 stages "
        "(Mapper[] and UVSource[] are sized by it)")],
    ('wwmath', 'matrix3d.h'): [
        ('\tvoid\tRe_Orthogonalize(void);',
         '\tvoid\tRe_Orthogonalize(void);\n\n\t// does "this = that * this"\n\tvoid preMul(const Matrix3D& that);\n\n\t// does "this = this * that"\n\tvoid postMul(const Matrix3D& that);\n\n\t// does "this = a * b"\n\tvoid mul(const Matrix3D& a, const Matrix3D& b);\n\n\tvoid mulVector3(const Vector3& in, Vector3& out) const;\n\tvoid mulVector3(Vector3& inout) const { mulVector3(inout, inout); }\n\n\n\tvoid mulVector3Array(const Vector3* in, Vector3* out, int count) const;\n\tvoid mulVector3Array(Vector3* inout, int count) const;',
         "Zero Hour's Matrix3D helpers (preMul, postMul, mul, mulVector3, mulVector3Array), without Zero Hour's ALLOW_TEMPORARIES rework of the operators (E&B's operator code is Renegade's). Zero Hour's streak.cpp, whose functions match the client, uses them"),
        ('#endif /* MATRIX3D_H */',
         'WWINLINE float submul(const Vector4& row, float tmp1, float tmp2, float tmp3)\n{\n  return row.X * tmp1 + row.Y * tmp2 + row.Z * tmp3;\n}\n\n// does "this = that * this"\nWWINLINE void Matrix3D::preMul(const Matrix3D& that)\n{\n\tassert(this != &that);\n\n\t// yes, this is ok. see the comments for \'mul\'\n\tthis->mul(that, *this);\n}\n\n// does "this = this * that"\nWWINLINE void Matrix3D::postMul(const Matrix3D& that)\n{\n\tassert(this != &that);\n\n#define AVOID_TEMP_IN_POSTMUL\n#ifdef AVOID_TEMP_IN_POSTMUL\n  float tmpX, tmpY, tmpZ, tmpW;\n\n\ttmpX = submul(this->Row[0], that.Row[0].X, that.Row[1].X, that.Row[2].X);\n\ttmpY = submul(this->Row[0], that.Row[0].Y, that.Row[1].Y, that.Row[2].Y);\n\ttmpZ = submul(this->Row[0], that.Row[0].Z, that.Row[1].Z, that.Row[2].Z);\n\ttmpW = submul(this->Row[0], that.Row[0].W, that.Row[1].W, that.Row[2].W);\n\n\tthis->Row[0].X = tmpX;\n\tthis->Row[0].Y = tmpY;\n\tthis->Row[0].Z = tmpZ;\n\tthis->Row[0].W += tmpW;\n\n\ttmpX = submul(this->Row[1], that.Row[0].X, that.Row[1].X, that.Row[2].X);\n\ttmpY = submul(this->Row[1], that.Row[0].Y, that.Row[1].Y, that.Row[2].Y);\n\ttmpZ = submul(this->Row[1], that.Row[0].Z, that.Row[1].Z, that.Row[2].Z);\n\ttmpW = submul(this->Row[1], that.Row[0].W, that.Row[1].W, that.Row[2].W);\n\n\tthis->Row[1].X = tmpX;\n\tthis->Row[1].Y = tmpY;\n\tthis->Row[1].Z = tmpZ;\n\tthis->Row[1].W += tmpW;\n\n\ttmpX = submul(this->Row[2], that.Row[0].X, that.Row[1].X, that.Row[2].X);\n\ttmpY = submul(this->Row[2], that.Row[0].Y, that.Row[1].Y, that.Row[2].Y);\n\ttmpZ = submul(this->Row[2], that.Row[0].Z, that.Row[1].Z, that.Row[2].Z);\n\ttmpW = submul(this->Row[2], that.Row[0].W, that.Row[1].W, that.Row[2].W);\n\n\tthis->Row[2].X = tmpX;\n\tthis->Row[2].Y = tmpY;\n\tthis->Row[2].Z = tmpZ;\n\tthis->Row[2].W += tmpW;\n#else\n\tMatrix3D tmp = *this;\n\tthis->mul(tmp, that);\n#endif\n}\n\n// does "this = A * B"\nWWINLINE void Matrix3D::mul(const Matrix3D& A, const Matrix3D& B)\n{\n\tassert(this != &A);\n\t//assert(this != &B);\n\t// nope, this is actually ok. (srj)\n\t//assert(this != &B);\n\n\tfloat tmp1,tmp2,tmp3;\n\n\ttmp1 = B.Row[0].X;\n\ttmp2 = B.Row[1].X;\n\ttmp3 = B.Row[2].X;\n\n\tthis->Row[0].X = submul(A.Row[0], tmp1, tmp2, tmp3);\n\tthis->Row[1].X = submul(A.Row[1], tmp1, tmp2, tmp3);\n\tthis->Row[2].X = submul(A.Row[2], tmp1, tmp2, tmp3);\n\n\ttmp1 = B.Row[0].Y;\n\ttmp2 = B.Row[1].Y;\n\ttmp3 = B.Row[2].Y;\n\n\tthis->Row[0].Y = submul(A.Row[0], tmp1, tmp2, tmp3);\n\tthis->Row[1].Y = submul(A.Row[1], tmp1, tmp2, tmp3);\n\tthis->Row[2].Y = submul(A.Row[2], tmp1, tmp2, tmp3);\n\n\ttmp1 = B.Row[0].Z;\n\ttmp2 = B.Row[1].Z;\n\ttmp3 = B.Row[2].Z;\n\n\tthis->Row[0].Z = submul(A.Row[0], tmp1, tmp2, tmp3);\n\tthis->Row[1].Z = submul(A.Row[1], tmp1, tmp2, tmp3);\n\tthis->Row[2].Z = submul(A.Row[2], tmp1, tmp2, tmp3);\n\n\ttmp1 = B.Row[0].W;\n\ttmp2 = B.Row[1].W;\n\ttmp3 = B.Row[2].W;\n\n\tthis->Row[0].W = submul(A.Row[0], tmp1, tmp2, tmp3) + A.Row[0].W;\n\tthis->Row[1].W = submul(A.Row[1], tmp1, tmp2, tmp3) + A.Row[1].W;\n\tthis->Row[2].W = submul(A.Row[2], tmp1, tmp2, tmp3) + A.Row[2].W;\n}\n\nWWINLINE void Matrix3D::mulVector3(const Vector3& in, Vector3& out) const\n{\n\t// note, it\'s ok for in==out here.\n\tfloat x = (Row[0].X * in.X + Row[0].Y * in.Y + Row[0].Z * in.Z + Row[0].W);\n\tfloat y = (Row[1].X * in.X + Row[1].Y * in.Y + Row[1].Z * in.Z + Row[1].W);\n\tfloat z = (Row[2].X * in.X + Row[2].Y * in.Y + Row[2].Z * in.Z + Row[2].W);\n\tout.Set(x, y, z);\n}\n\n\nWWINLINE void Matrix3D::mulVector3Array(const Vector3* in, Vector3* out, int count) const\n{\n\tassert(in != out);\n#ifdef ALLOW_TEMPORARIES\n\tfor (i=0; i<count; i++)\n\t{\n\t\tout[i] = (*this) * in[i];\n\t}\n#else\t\n\twhile (count--)\n\t{\n\t\t// we know that in != out, so no temps needed\n\t\tout->X = (Row[0].X * in->X + Row[0].Y * in->Y + Row[0].Z * in->Z + Row[0].W);\n\t\tout->Y = (Row[1].X * in->X + Row[1].Y * in->Y + Row[1].Z * in->Z + Row[1].W);\n\t\tout->Z = (Row[2].X * in->X + Row[2].Y * in->Y + Row[2].Z * in->Z + Row[2].W);\n\t\t++in;\n\t\t++out;\n\t}\n#endif\n}\n\nWWINLINE void Matrix3D::mulVector3Array(Vector3* inout, int count) const\n{\n#ifdef ALLOW_TEMPORARIES\n\tfor (i=0; i<count; i++)\n\t{\n\t\tinout[i] = (*this) * inout[i];\n\t}\n#else\t\n\twhile (count--)\n\t{\n\t\tfloat x = (Row[0].X * inout->X + Row[0].Y * inout->Y + Row[0].Z * inout->Z + Row[0].W);\n\t\tfloat y = (Row[1].X * inout->X + Row[1].Y * inout->Y + Row[1].Z * inout->Z + Row[1].W);\n\t\tfloat z = (Row[2].X * inout->X + Row[2].Y * inout->Y + Row[2].Z * inout->Z + Row[2].W);\n\t\tinout->Set(x, y, z);\n\t\t++inout;\n\t}\n#endif\n}\n\n#endif /* MATRIX3D_H */',
         'their definitions, verbatim from Zero Hour'),
    ],
    ('wwlib', 'sharebuf.h'): [
        ('\t\tShareBufferClass(int count);',
         '\t\tShareBufferClass(int count);\n\t\tShareBufferClass(int count, const char* msg);',
         "ShareBufferClass also has Zero Hour's constructor taking a memory-tag argument, ignored in a "
         "release build (Zero Hour only stores it in debug builds), so Zero Hour code that passes it "
         "compiles beside Renegade code that doesn't. A separate overload, not a default argument: a "
         "default would add a push to every existing call. Renegade's class otherwise (no W3DMPO base)"),
        ('template <class T> \nShareBufferClass<T>::ShareBufferClass(const ShareBufferClass<T> & that) :',
         'template <class T> \nShareBufferClass<T>::ShareBufferClass(int count, const char* /*msg*/) :\n'
         '\tCount(count)\n{\n\tassert(Count > 0);\n\tArray = new T[Count];\n}\n\n'
         'template <class T> \nShareBufferClass<T>::ShareBufferClass(const ShareBufferClass<T> & that) :',
         'its definition (the same body as the one-argument constructor)'),
    ],
    ('ww3d2', 'w3d_file.h'): [
        ('FutureStartTime; // Whether to start a particle in the future or not\n\tuint32\t\t\t\tPadding[9];',
         'FutureStartTime; // Whether to start a particle in the future or not\n\tuint8\t\t\t\t\tUnknownFlag;\t\t// E&B: see ParticleEmitterDefClass::Set_Unknown_Flag; meaning not known\n\tuint8\t\t\t\t\tUnknownPad[3];\n\tuint32\t\t\t\tPadding[8];',
         "W3dEmitterExtraInfoStruct (Zero Hour's) keeps its 40 bytes, but E&B uses the byte after FutureStartTime as a flag (set by ParticleEmitterDefClass's extra virtual, vtable slot 19 at 0x914520; passed to every emitter the definition creates); named UnknownFlag"),
    ],
    ('ww3d2', 'part_ldr.h'): [
        ('virtual void\t\t\tSet_Future_Start_Time (float value)\t\t\t{ m_ExtraInfo.FutureStartTime = value; }',
         'virtual void\t\t\tSet_Future_Start_Time (float value)\t\t\t{ m_ExtraInfo.FutureStartTime = value; }\n\t\tvirtual void\t\t\tSet_Unknown_Flag (bool value)\t\t\t\t{ m_ExtraInfo.UnknownFlag = value ? 1 : 0; }\t// E&B; name not known',
         "ParticleEmitterDefClass has an extra virtual after Set_Future_Start_Time (the client's vtable at 0xB10920, slot 19) setting the flag: named Set_Unknown_Flag. A bool argument stored as `? 1 : 0` into a uint8 reproduces the client's test/setne"),
        ('float\t\t\t\t\t\tGet_Future_Start_Time (void) const\t{ return m_ExtraInfo.FutureStartTime; }',
         'float\t\t\tGet_Future_Start_Time (void) const\t{ return m_ExtraInfo.FutureStartTime; }\n\t\tbool\t\t\tGet_Unknown_Flag (void) const\t\t{ return m_ExtraInfo.UnknownFlag != 0; }\t// E&B; name not known',
         'and a getter (ParticleEmitterClass::Create_From_Definition reads the byte and tests it)'),
    ],
    ('ww3d2', 'scene.h'): [
        ('\tvirtual ~SceneClass(void);',
         '\tvirtual ~SceneClass(void);\n\n\t///////////////////////////////////////////////////////////////////////////////////\n\t// RTTI information.\n\t///////////////////////////////////////////////////////////////////////////////////\n\tenum {\n\t\tSCENE_ID_UNKOWN = 0xFFFFFFFF,\n\t\tSCENE_ID_SCENE = 0,\n\t\tSCENE_ID_SIMPLE,\n\n\t\tSCENE_ID_LAST = 0x0000FFFF,\n\t};\n\tvirtual int\t\t\t\t\tGet_Scene_ID(void) const\t{\treturn SCENE_ID_SCENE;\t}',
         "SceneClass has Zero Hour's scene IDs: Get_Scene_ID() const (returns SCENE_ID_SCENE) right after the destructor, as in the client's vtable (0xB120FC, slot 2). Zero Hour's virtual Render is not in the client"),
        ('\tvirtual ~SimpleSceneClass(void);',
         '\tvirtual ~SimpleSceneClass(void);\n\n\tvirtual int\tGet_Scene_ID(void)\t{\treturn SCENE_ID_SIMPLE;\t}',
         "SimpleSceneClass::Get_Scene_ID() (not const, so a new virtual, not an override: as in Zero Hour) returns SCENE_ID_SIMPLE (the client's slot 23 returns 1)"),
    ],
    ('wwaudio', 'audiblesound.h'): [
        ('\t\tint\t\t\t\t\t\tm_LoopsLeft;',
         '\t\tint\t\t\t\t\t\tm_LoopsLeft;\n\t\tint\t\t\t\t\t\tm_Unknown7C;\t\t// E&B: 4 at construction; meaning not known\n\t\tint\t\t\t\t\t\tm_Unknown80;\t\t// E&B: -1 at construction; meaning not known',
         'AudibleSoundClass has two E&B ints after m_LoopsLeft (+0x7C, +0x80; every later member is 8 bytes later in the client), set to 4 and -1 by both constructors (client 0x9FCA50, 0x9FCE60) and not copied by operator=. Meaning not known: named m_Unknown7C, m_Unknown80'),
    ],
    ('wwaudio', 'audiblesound.cpp'): [
        ('\t\tm_LoopsLeft (0),',
         '\t\tm_LoopsLeft (0),\n\t\tm_Unknown7C (4),\n\t\tm_Unknown80 (-1),',
         'both constructors initialise them'),
    ],
    ('wwaudio', 'sound3d.h'): [
        ('\t\tVector3\t\t\t\t\tm_CurrentVelocity;',
         '\t\tVector3\t\t\t\t\tm_CurrentVelocity;\n\t\tVector3\t\t\t\t\tm_Unknown148;\t\t// E&B: zeroed at construction, not copied; meaning not known',
         'Sound3DClass has E&B fields: 12 bytes after m_CurrentVelocity (+0x148) and two ints after m_LastUpdate (+0x160, +0x164), all zeroed by the constructors (client 0x9F76A0, 0x9F7C90, 0x9F9220) and not copied by operator=. Meaning not known: m_Unknown148 (a Vector3 by size), m_Unknown160, m_Unknown164'),
        ('\t\tunsigned int\t\t\tm_LastUpdate;',
         '\t\tunsigned int\t\t\tm_LastUpdate;\n\t\tint\t\t\t\t\t\tm_Unknown160;\t\t// E&B: zeroed at construction, not copied; meaning not known\n\t\tint\t\t\t\t\t\tm_Unknown164;\t\t// E&B: zeroed at construction, not copied; meaning not known',
         'the two ints at the end'),
    ],
    ('wwaudio', 'sound3d.cpp'): [
        ('\t  m_CurrentVelocity (0, 0, 0),',
         '\t  m_CurrentVelocity (0, 0, 0),\n\t  m_Unknown148 (0, 0, 0),',
         'both constructors zero m_Unknown148'),
        ('\t  m_LastUpdate (0),',
         '\t  m_LastUpdate (0),\n\t  m_Unknown160 (0),\n\t  m_Unknown164 (0),',
         'and m_Unknown160, m_Unknown164'),
    ],
    ('wwmath', 'matrix4.h'): [
        ('#endif /*MATRIX4_H*/',
         '// Zero Hour calls this class Matrix4x4; code taken from Zero Hour (sphereobj.cpp, wwshade) uses\n'
         '// that name. The class is the same, so the name is an alias here.\n'
         'typedef Matrix4 Matrix4x4;\n\n#endif /*MATRIX4_H*/',
         "Matrix4x4 (Zero Hour's name for Matrix4) as an alias, for the Zero Hour code imported beside "
         "Renegade's WWMath; E&B's own name for the class is not known"),
    ],
    ('wwmath', 'matrix3.h'): [
        ('#endif /*Matrix3_H*/',
         '// Zero Hour calls this class Matrix3x3; code taken from Zero Hour uses that name. The class\n'
         '// is the same, so the name is an alias here.\n'
         'typedef Matrix3 Matrix3x3;\n\n#endif /*Matrix3_H*/',
         "Matrix3x3 (Zero Hour's name for Matrix3) as an alias, for the Zero Hour code imported beside "
         "Renegade's WWMath"),
    ],
    ('ww3d2', 'part_buf.h'): [
        ('\t\tShareBufferClass<unsigned int> *\tAPT;',
         '\t\tShareBufferClass<unsigned int> *\tAPT;\n\t\tShareBufferClass<unsigned char> * GroupID;\t// Only used for lines (as in Zero Hour)',
         "ParticleBufferClass has Zero Hour's line-group members: the GroupID buffer pointer after APT (every member from PingPongPosition, +0x204, is 4 bytes later in the client) and CurrentGroupID at the end; not Zero Hour's FutureStartTime (members before +0x204 don't move)"),
        ('\t\tfloat\t\t\t\t\t\t\t\t\tProjectedArea;',
         '\t\tfloat\t\t\t\t\t\t\t\t\tProjectedArea;\n\n\t\tunsigned char\t\t\t\t\t\tCurrentGroupID;\t// as in Zero Hour',
         'CurrentGroupID at the end'),
        ('\tunsigned int\tTimeStamp;\t// Millisecond time at creation.\n',
         '\tunsigned int\tTimeStamp;\t// Millisecond time at creation.\n\tunsigned char\tGroupID;\t\t// Group ID of the particle (as in Zero Hour)\n',
         "NewParticleStruct has Zero Hour's GroupID (E&B's emitter, Zero Hour's code, sets it)"),
        ('\t\tvoid Set_Emitter(ParticleEmitterClass *emitter);',
         '\t\tvoid Set_Emitter(ParticleEmitterClass *emitter);\n\t\tvoid Set_Current_GroupID(unsigned char grp) { CurrentGroupID = grp; }',
         "Zero Hour's Set_Current_GroupID"),
    ],
    ('ww3d2', 'part_emt.h'): [
        ('\t\t\tconst W3dEmitterLinePropertiesStruct * line_props = NULL);',
         '\t\t\tconst W3dEmitterLinePropertiesStruct * line_props = NULL,\n\t\t\tbool unknown_flag = false);',
         "ParticleEmitterClass (Zero Hour's) takes one more constructor argument, a bool after line_props (the client's constructor, 0x97D180, ends `ret 0x78`), kept in UnknownFlag"),
        ('Get_Future_Start_Time (void) const { return Buffer->Get_Future_Start_Time(); }',
         'Get_Future_Start_Time (void) const { return FutureStartTime / 1000.0f; }',
         'E&B keeps the future start time in the emitter (ms), not in the buffer as Zero Hour does'),
        ('GroupID;\t\t\t\t// The group ID of a particle. A start causes the group ID to increment.',
         'GroupID;\t\t\t\t// The group ID of a particle. A start causes the group ID to increment.\n\t\tunsigned int\t\t\t\tFutureStartTime;\t\t// E&B: future start time in milliseconds (Zero Hour keeps it in the buffer)',
         'FutureStartTime (+0xF4): the constructor stores future_start * 1000 there; Reset() starts EmitRemain from it'),
        ('bool\t\t\t\t\t\t\tIsInvisible;',
         "bool\t\t\t\t\t\t\tIsInvisible;\n\t\tbool\t\t\t\t\t\t\tUnknownFlag;\t\t\t// E&B: from the definition's flag; meaning not known",
         'UnknownFlag (+0xFD), copied by the copy constructor'),
    ],
    ('ww3d2', 'part_emt.cpp'): [
        ('\t\t\tconst W3dEmitterLinePropertiesStruct * line_props\n',
         '\t\t\tconst W3dEmitterLinePropertiesStruct * line_props,\n\t\t\tbool unknown_flag\n',
         'the extra constructor argument'),
        ('\tGroupID(0),\n\tBuffer(NULL),\n\tIsInvisible(false)',
         '\tGroupID(0),\n\tFutureStartTime((unsigned int)(future_start * 1000.0f)),\n\tBuffer(NULL),\n\tIsInvisible(false),\n\tUnknownFlag(unknown_flag)',
         'the constructor initialises FutureStartTime and UnknownFlag'),
        ('\tGroupID(0),\n\tBuffer(NULL),\n\tIsInvisible(src.IsInvisible)',
         '\tGroupID(0),\n\tFutureStartTime(src.FutureStartTime),\n\tBuffer(NULL),\n\tIsInvisible(src.IsInvisible),\n\tUnknownFlag(src.UnknownFlag)',
         'the copy constructor copies them'),
        ('accel/1000000.0f,max_age, future_start, tex, shader,',
         'accel/1000000.0f,max_age, tex, shader,',
         "the buffer is not given the future start time (E&B's buffer has no FutureStartTime)"),
        ('\tEmitRemain = 0;\t',
         '\tEmitRemain = FutureStartTime;\t',
         'Reset() starts EmitRemain from FutureStartTime (the client loads +0xF4 there)'),
        ('\t\t\tMIP_LEVELS_ALL,',
         '\t\t\tTextureClass::MIP_LEVELS_ALL,',
         "Renegade's texture API (MIP_LEVELS_ALL is TextureClass's)"),
        ('\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tdefinition.Get_Line_Properties ()) );',
         '\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tdefinition.Get_Line_Properties (),\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tdefinition.Get_Unknown_Flag ()) );',
         "Create_From_Definition passes the definition's flag"),
    ],
    ('ww3d2', 'rendobj.h'): [
        ('class RenderObjClass : public RefCountClass , public PersistClass, public MultiListObjectClass',
         """//////////////////////////////////////////////////////////////////////////////////
// RenderHookClass (as in Zero Hour)
//////////////////////////////////////////////////////////////////////////////////
class RenderHookClass
{
public:
	RenderHookClass(void) { }
	virtual ~RenderHookClass(void) { }
	virtual bool Pre_Render(RenderObjClass *robj, RenderInfoClass &rinfo) = 0;
	virtual void Post_Render(RenderObjClass *robj, RenderInfoClass &rinfo) = 0;
private:
	RenderHookClass(const RenderHookClass & src);
	RenderHookClass & operator=(const RenderHookClass & src);
};

class RenderObjClass : public RefCountClass , public PersistClass, public MultiListObjectClass""",
         "RenderObjClass has Zero Hour's render hook (RenderHookClass, the RenderHook member at the end "
         "of the class, Get/Set_Render_Hook, deleted in the destructor) but not Zero Hour's "
         "ObjectScale/ObjectColor. Found from the +4 shift of every member after 0x88 in HLodClass, "
         "SphereRenderObjClass, RingRenderObjClass, ... (tools/re/layoutdiff.py)"),
        ('	virtual ~RenderObjClass(void)																					{ }',
         '	virtual ~RenderObjClass(void)																					{ if (RenderHook) delete RenderHook; }',
         'the destructor deletes the render hook'),
        ('	virtual void *					Get_User_Data()																{ return User_Data; };',
         """	virtual void *					Get_User_Data()																{ return User_Data; };

	// Application-specific render hook:
	RenderHookClass *				Get_Render_Hook(void) { return RenderHook; }
	void								Set_Render_Hook(RenderHookClass *hook) { if (RenderHook) delete RenderHook; RenderHook = hook; }""",
         'render hook accessors'),
        ('	void *							User_Data;',
         """	void *							User_Data;

	RenderHookClass *				RenderHook;""",
         'the render hook member'),
        ('virtual HAnimClass *\t\t\tPeek_Animation( void )',
         '//\n\t// E&B: a virtual with no arguments and an empty body, in vtable slot 40 (between the\n\t// Set_Animation overloads and Peek_Animation). Its name is not known.\n\t//\n\tvirtual void\t\t\t\t\tUnknown_Virtual_40( void )\t\t\t\t\t\t\t\t\t\t\t{ }\n\tvirtual HAnimClass *\t\t\tPeek_Animation( void )',
         "RenderObjClass's vtable as in the client (113 slots, at 0xAE84F8; tools/re/vtables.py): one E&B virtual (void, no arguments, empty) before Peek_Animation, named Unknown_Virtual_40"),
        ('virtual int\t\t\t\t\t\tHas_User_Lighting(void) const',
         'int\t\t\t\t\t\t\t\tHas_User_Lighting(void) const',
         "Renegade's four user-lighting functions are not virtual (Zero Hour removed them; the client's vtable has no slots for them); kept as plain functions so Renegade's mesh and renderer code still compiles"),
        ('virtual void\t\t\t\t\tSet_Has_User_Lighting(bool onoff)',
         'void\t\t\t\t\t\t\t\tSet_Has_User_Lighting(bool onoff)',
         'Set_Has_User_Lighting not virtual'),
        ('virtual void\t\t\t\t\tSave_User_Lighting (ChunkSaveClass & csave);',
         'void\t\t\t\t\t\t\t\tSave_User_Lighting (ChunkSaveClass & csave);',
         'Save_User_Lighting not virtual'),
        ('virtual void\t\t\t\t\tLoad_User_Lighting (ChunkLoadClass & cload);',
         'void\t\t\t\t\t\t\t\tLoad_User_Lighting (ChunkLoadClass & cload);',
         'Load_User_Lighting not virtual'),
    ],
    ('ww3d2', 'rendobj.cpp'): [
        ('	User_Data(NULL),', """	User_Data(NULL),
	RenderHook(NULL),""", 'both constructors clear the render hook'),
    ],
}
# Files reconstructed by us from the client (no EA copy matches): `assemble` keeps the repository's
# version, and refine leaves them alone. Each entry: (library, file) -> what was done.
RECONSTRUCTED = {
    ('wwlib', 'rc4.cpp'): "E&B's RC4Class: a runtime-built initial key, the 8- and 16-byte key setups "
                          "inlined into Prepare_Key, inline assembly in the 16-byte setup and RC4(); "
                          "Print_State and operator= are Zero Hour's. All 5 functions byte-exact "
                          "(docs/library-matching.md §3.6)",
    ('wwlib', 'rc4.h'): "InitialKey / InitialKeyBuilt statics added; Prepare_Key_8bytes/16bytes removed",
}

# Text appended to imported files (after everything, so no existing line moves).
APPENDS = {
    ('wwlib', 'always.h'): ("""
/*
**	EnB import: Zero Hour's WW libraries use these macros from its always.h. In a release build
**	without the Generals memory pools they are plain operator new and an empty base class; these
**	are those definitions, for the Zero Hour files imported beside Renegade's always.h.
*/
#ifndef W3DNEW
#define MSGW3DNEW(MSG)					new
#define MSGW3DNEWARRAY(MSG)			new
#define W3DNEW							new
#define W3DNEWARRAY						new
#define W3DMPO_GLUE(ARGCLASS)
class W3DMPO { };
#endif
""", "Zero Hour's allocation macros (W3DNEW, W3DMPO_GLUE, W3DMPO) in their release form, appended "
     "for the Zero Hour files built against Renegade's always.h"),
}

NEW_FILES = {
    ('wwdebug', 'gamedebug.h'): '''/*
**	Earth & Beyond client (reconstruction)
**
**	Release-build definitions of the Generals debug macros that Zero Hour's wwdebug.h expects
**	from GameEngine\\Include\\Common\\debug.h. In Generals' release configuration (no
**	ALLOW_DEBUG_UTILS) every one of them compiles to nothing; these are those definitions.
**	E&B's own header is not known.
*/

#ifndef __GAMEDEBUG_H
#define __GAMEDEBUG_H

#define DEBUG_INIT(f)					((void)0)
#define DEBUG_SHUTDOWN()				((void)0)
#define DEBUG_LOG(m)					((void)0)
#define DEBUG_ASSERTLOG(c, m)			((void)0)
#define DEBUG_CRASH(m)					((void)0)
#define DEBUG_ASSERTCRASH(c, m)		((void)0)
#define BEGIN_PROFILE(uniqueid)
#define END_PROFILE(uniqueid, msg, howoftentolog, howoftentoreset)

#endif // __GAMEDEBUG_H
''',
}


# ------------------------------------------------------------------------------------------ evidence

def key_of(filename):
    return os.path.splitext(os.path.basename(filename))[0].lower()


def load_evidence(evdir):
    """{lib: {copy: {file key: {address: (symbol, name)}}}} from bytematch_all.sh's JSONs."""
    ev = {}
    for lib in LIBS:
        ev[lib] = {}
        for c in LIB_DIR[lib]:
            path = os.path.join(evdir, f'{c}-{lib}.json')
            if not os.path.exists(path):
                continue
            per = collections.defaultdict(dict)
            for r in json.load(open(path)):
                if r['status'] == 'exact':
                    per[key_of(r['object'])][r['addresses'][0]] = (r['symbol'], r['name'])
            ev[lib][c] = per
    return ev


def source_files(lib, copy):
    """{file key: (filename, full path)} for a copy's .cpp files."""
    d = os.path.join(COPY_ROOT[copy], LIB_DIR[lib][copy])
    return {key_of(f): (f, os.path.join(d, f)) for f in os.listdir(d) if f.lower().endswith('.cpp')}


def headers(lib, copy):
    d = os.path.join(COPY_ROOT[copy], LIB_DIR[lib][copy])
    return {f.lower(): (f, os.path.join(d, f)) for f in os.listdir(d) if f.lower().endswith(HEADER_EXT)}


# ------------------------------------------------------------------------------------------ state

def load_state():
    return json.load(open(STATE)) if os.path.exists(STATE) else {}


def save_state(state):
    os.makedirs(os.path.dirname(STATE), exist_ok=True)
    json.dump(state, open(STATE, 'w'), indent=1, sort_keys=True)


# ------------------------------------------------------------------------------------------ plan

def plan(ev):
    state = {}
    for lib in LIBS:
        copies = ev[lib]
        if not copies:
            continue
        total = {c: len(set().union(*[set(v) for v in copies[c].values()]) if copies[c] else set())
                 for c in copies}
        base = max(total, key=lambda c: (total[c], c == 'R'))
        files = {}
        keys = set().union(*[set(copies[c]) for c in copies])
        for k in sorted(keys):
            best = max(copies, key=lambda c: (len(copies[c].get(k, {})), c == base))
            if not copies[best].get(k):
                continue
            files[k] = {'copy': best, 'expected': {c: len(copies[c].get(k, {})) for c in copies},
                        'merged': []}
        state[lib] = {'base': base, 'totals': total, 'files': files}
        swaps = sum(1 for f in files.values() if f['copy'] != base)
        print(f"{lib:11s} base {base} ({', '.join(f'{c}={n}' for c, n in total.items())}); "
              f"{len(files)} files with evidence, {swaps} from another copy")
    return state


# ------------------------------------------------------------------------------------------ assemble

def dest(lib):
    return os.path.join(REPO, lib)


def apply_patches(lib, path, name):
    for (plib, pname), edits in PATCHES.items():
        if plib == lib and pname == name.lower():
            text = open(path, encoding='latin1', newline='').read()
            fnl = '\r\n' if '\r\n' in text else '\n'
            for old, new, _ in edits:
                old, new = old.replace('\n', fnl), new.replace('\n', fnl)
                if old not in text:
                    raise SystemExit(f'patch for {lib}/{name} no longer applies: {old}')
                text = text.replace(old, new)
            open(path, 'w', encoding='latin1', newline='').write(text)


def assemble(state):
    for lib, s in state.items():
        d = dest(lib)
        kept = {}
        for (rlib, rname) in RECONSTRUCTED:
            p = os.path.join(d, rname)
            if rlib == lib and os.path.exists(p):
                kept[rname] = open(p, 'rb').read()
        if os.path.isdir(d):
            shutil.rmtree(d)
        os.makedirs(d)
        base = s['base']
        hs = headers(lib, base)
        origin = {}
        for low, (name, path) in hs.items():
            shutil.copy2(path, os.path.join(d, name))
            origin[name] = base
        # Headers only another copy has (a file chosen from that copy may need them).
        for c in LIB_DIR[lib]:
            if c == base:
                continue
            for low, (name, path) in headers(lib, c).items():
                if low not in hs:
                    shutil.copy2(path, os.path.join(d, name))
                    origin[name] = c
                    hs[low] = (name, path)
        base_sources = source_files(lib, base)
        for k, f in s['files'].items():
            f['best'] = f.get('best', f['copy'])
            f['copy'] = base if k in base_sources else f['best']
            name, path = source_files(lib, f['copy'])[k]
            shutil.copy2(path, os.path.join(d, name))
            f['name'] = name
            f['merged'] = []
            f['headers_from'] = None
        # A source file the base copy lacks brings its own header(s) of the same name.
        for k, f in s['files'].items():
            if f['copy'] != base:
                for low, (name, path) in headers(lib, f['copy']).items():
                    if os.path.splitext(low)[0] == k:
                        for old in [n for n in os.listdir(d) if n.lower() == low]:
                            os.remove(os.path.join(d, old))
                        shutil.copy2(path, os.path.join(d, name))
                        origin[name] = f['copy']
        for name in os.listdir(d):
            apply_patches(lib, os.path.join(d, name), name)
            annotations.apply(lib, os.path.join(d, name), name)
            for (alib, aname), (text, _) in APPENDS.items():
                if alib == lib and aname == name.lower():
                    with open(os.path.join(d, name), 'a', newline='\r\n') as fp:
                        fp.write(text)
        for (nlib, nname), text in NEW_FILES.items():
            if nlib == lib:
                open(os.path.join(d, nname), 'w', newline='\r\n').write(text)
                origin[nname] = 'new'
        for rname, data in kept.items():
            for old in [n for n in os.listdir(d) if n.lower() == rname.lower()]:
                os.remove(os.path.join(d, old))
            open(os.path.join(d, rname), 'wb').write(data)
            origin[rname] = 'reconstructed'
            k = key_of(rname)
            if rname.lower().endswith('.cpp') and k in s['files']:
                s['files'][k]['copy'] = 'reconstructed'
                s['files'][k]['name'] = rname
        s['headers'] = origin
        print(f'{lib:11s} {len(s["files"])} sources, {len(origin)} headers')
    return state


# ------------------------------------------------------------------------------------------ verify

_img = None


def image():
    global _img
    if _img is None:
        _img = ClientImage()
    return _img


def tree_includes():
    return [dest(l) for l in LIBS if os.path.isdir(dest(l))] + SDK_INCLUDES


def compile_and_match(paths, tag):
    """Compile files from the repository; {file key: set(addresses)} plus compiler output."""
    out = os.path.join(WORK, tag)
    if os.path.isdir(out):
        shutil.rmtree(out)
    rc, text = vc6.compile_sources(paths, tree_includes(), out=out)
    objs = [os.path.join(out, f) for f in os.listdir(out)] if os.path.isdir(out) else []
    found = collections.defaultdict(set)
    for r in match_objects(image(), objs) if objs else []:
        if r['status'] == 'exact':
            found[key_of(r['object'])].add(r['addresses'][0])
    for p in paths:
        found.setdefault(key_of(p), set())
    return found, text


def verify(state, libs=None):
    """Compile and byte-match the libraries; the libraries are spread over the build VMs."""
    import concurrent.futures as cf
    import queue
    libs = [l for l in (libs or list(state)) if l in state]
    image()                                  # load the client once, before the threads
    free = queue.Queue()
    for h in vc6.hosts():
        free.put(h)

    def one(lib):
        host = free.get()
        try:
            vc6.use_host(host)
            s = state[lib]
            paths = [os.path.join(dest(lib), f['name']) for f in s['files'].values()]
            found, text = compile_and_match(paths, f'verify-{lib}')
            return lib, found, text
        finally:
            free.put(host)

    # biggest first, so the long WW3D2 compile overlaps with the rest
    order = sorted(libs, key=lambda l: -len(state[l]['files']))
    with cf.ThreadPoolExecutor(max(1, len(vc6.hosts()))) as ex:
        results = {lib: (found, text) for lib, found, text in ex.map(one, order)}
    for lib in libs:
        s = state[lib]
        found, text = results[lib]
        errors = [l for l in text.splitlines() if ' error ' in l]
        total = set()
        short = 0
        for k, f in s['files'].items():
            f['in_tree'] = sorted(found.get(k, set()))
            total |= found.get(k, set())
            if len(f['in_tree']) < f['expected'].get(f['copy'], 0):
                short += 1
        s['in_tree_total'] = len(total)
        s['compile_errors'] = errors[:50]
        print(f'{lib:11s} in tree: {len(total)} client functions (best copy alone: '
              f'{max(s["totals"].values())}); {short} files below their copy; {len(errors)} errors')
    return state


# ------------------------------------------------------------------------------------------ refine

def splice(text, donor_text, names):
    """Replace each named function definition in `text` with the donor's. Overloads are paired
    by order of definition. Returns (new text, names spliced)."""
    spans = function_spans(text)
    dspans = function_spans(donor_text)
    done = []
    edits = []
    for name in names:
        mine = [s for s in spans if s[0] == name]
        theirs = [s for s in dspans if s[0] == name]
        if not mine or not theirs or len(mine) != len(theirs):
            continue
        for (_, h, _, e), (_, dh, _, de) in zip(mine, theirs):
            edits.append((h, e + 1, donor_text[dh:de + 1]))
        done.append(name)
    for h, e, new in sorted(edits, reverse=True):
        text = text[:h] + new + text[e:]
    return text, done


def try_file(lib, f, text):
    """Write `text` as the file, compile it, and return the set of matched addresses."""
    path = os.path.join(dest(lib), f['name'])
    open(path, 'w', encoding='latin1', newline='').write(text)
    found, _ = compile_and_match([path], f'try-{lib}')
    return found.get(key_of(f['name']), set())


def tree_totals(state):
    return {lib: s.get('in_tree_total', 0) for lib, s in state.items()}


def try_with_headers(state, lib, f, c, k, donor):
    """Swap a file and its same-named header(s) to copy c; keep it only if no library's total
    drops. Returns the file's matched set, or None (and restores everything) if rejected."""
    d = dest(lib)
    pairs = [(n, p) for low, (n, p) in headers(lib, c).items() if os.path.splitext(low)[0] == k]
    if not pairs:
        return None
    saved = {}
    for n, p in pairs:
        for old in [x for x in os.listdir(d) if x.lower() == n.lower()]:
            saved[old] = open(os.path.join(d, old), 'rb').read()
            os.remove(os.path.join(d, old))
        shutil.copy2(p, os.path.join(d, n))
    path = os.path.join(d, f['name'])
    original = open(path, 'rb').read()
    open(path, 'w', encoding='latin1', newline='').write(donor)
    before = tree_totals(state)
    trial = json.loads(json.dumps(state))
    verify(trial)
    after = tree_totals(trial)
    got = set(trial[lib]['files'][k].get('in_tree', []))
    if all(after[l] >= before[l] for l in before) and sum(after.values()) > sum(before.values()):
        for l in state:
            state[l]['in_tree_total'] = trial[l]['in_tree_total']
            for kk, ff in state[l]['files'].items():
                ff['in_tree'] = trial[l]['files'][kk].get('in_tree', [])
        return got
    for n, _ in pairs:
        os.remove(os.path.join(d, n))
    for old, data in saved.items():
        open(os.path.join(d, old), 'wb').write(data)
    open(path, 'wb').write(original)
    return None


def refine(state, ev, libs=None, only=None):
    for lib in libs or state:
        s = state[lib]
        gained = 0
        for k, f in sorted(s['files'].items()):
            if (lib, f['name'].lower()) in RECONSTRUCTED:
                continue
            if only and k not in only:
                continue
            path = os.path.join(dest(lib), f['name'])
            cur = set(f.get('in_tree', []))
            text = open(path, encoding='latin1', newline='').read()
            improved = True
            while improved:
                improved = False
                for c in sorted(ev[lib], key=lambda c: -len(set(ev[lib][c].get(k, {})) - cur)):
                    wanted = {a: v for a, v in ev[lib][c].get(k, {}).items() if a not in cur}
                    if not wanted or k not in source_files(lib, c):
                        continue
                    donor_path = source_files(lib, c)[k][1]
                    donor = open(donor_path, encoding='latin1', newline='').read()
                    # 1. the whole file from the other copy, if the current one is clearly worse
                    if c != f['copy'] and len(ev[lib][c][k]) > len(cur):
                        got = try_file(lib, f, donor)
                        if len(got) > len(cur):
                            gained += len(got) - len(cur)
                            print(f'  {lib}/{f["name"]}: whole file from {c}: {len(cur)} -> {len(got)}')
                            f['copy'], f['merged'], cur, text = c, [], got, donor
                            improved = True
                            break
                        # with the other copy's header(s) of the same name, if nothing else loses
                        got = try_with_headers(state, lib, f, c, k, donor)
                        if got is not None and len(got) > len(cur):
                            gained += len(got) - len(cur)
                            print(f'  {lib}/{f["name"]}: whole file and header from {c}: {len(cur)} -> {len(got)}')
                            f['copy'], f['merged'], f['headers_from'], cur, text = c, [], c, got, donor
                            improved = True
                            break
                        open(path, 'w', encoding='latin1', newline='').write(text)
                    # 2. function by function
                    names = sorted({v[1] for v in wanted.values()})
                    new_text, done = splice(text, donor, names)
                    if not done:
                        continue
                    got = try_file(lib, f, new_text)
                    if cur <= got and len(got) > len(cur):
                        accepted = done
                    else:
                        # all together didn't work: one at a time
                        accepted, got = [], cur
                        new_text = text
                        for n in done:
                            t2, d2 = splice(new_text, donor, [n])
                            if not d2:
                                continue
                            g2 = try_file(lib, f, t2)
                            if got <= g2 and len(g2) > len(got):
                                new_text, got = t2, g2
                                accepted.append(n)
                    open(path, 'w', encoding='latin1', newline='').write(new_text if accepted else text)
                    if accepted:
                        gained += len(got) - len(cur)
                        print(f'  {lib}/{f["name"]}: {len(accepted)} function(s) from {c}: '
                              f'{len(cur)} -> {len(got)} ({", ".join(accepted)[:120]})')
                        f['merged'] += [{'copy': c, 'function': n} for n in accepted]
                        cur, text = got, new_text
                        improved = True
                        break
            f['in_tree'] = sorted(cur)
        print(f'{lib:11s} refine gained {gained}')
    return state


# ------------------------------------------------------------------------------------------ report

def report(state):
    lines = ['# Imported Westwood libraries', '',
             'Generated by `tools/vc6/importer.py report` from `docs/data/imports.json`. Each library '
             'was imported from EA\'s published copies (R Renegade, G Generals, Z Zero Hour; commits '
             + ', '.join(f'{COPY_NAME[c]} `{COPY_COMMIT[c]}`' for c in 'RGZ') + '), file by file, '
             'keeping the copy whose compiled code matches client.exe best, then splicing in whole '
             'function definitions from other copies where that adds byte-exact functions without '
             'losing any. "In tree": client functions our own source reproduces byte for byte '
             '(compiled with VC6 SP4, `/MT /W3 /O2 /Ob2`). See docs/library-matching.md Â§3.5.', '']
    lines += ['## Summary', '', '| Library | Base copy | Source files | From another copy | Functions merged | In tree | Best single copy |',
              '|---|---|---|---|---|---|---|']
    for lib, s in state.items():
        files = s['files']
        other = sum(1 for f in files.values() if f['copy'] != s['base'])
        merged = sum(len(f['merged']) for f in files.values())
        lines.append(f"| {lib} | {COPY_NAME[s['base']]} | {len(files)} | {other} | {merged} | "
                     f"{s.get('in_tree_total', '?')} | {max(s['totals'].values())} |")
    lines += ['', '## Local changes', '']
    for (lib, name), edits in PATCHES.items():
        for _, _, why in edits:
            lines.append(f'* `{lib}/{name}`: {why}.')
    for (lib, name), why in RECONSTRUCTED.items():
        lines.append(f'* `{lib}/{name}`: **reconstructed** from the client. {why}.')
    for (lib, name), (_, why) in APPENDS.items():
        lines.append(f'* `{lib}/{name}`: {why}.')
    lines.append('* Comments giving the C equivalent of each inline-assembly block are inserted into the '
                 'imported files listed in `tools/vc6/annotations.py` (docs/conventions.md, "Inline assembly").')
    for (lib, name) in NEW_FILES:
        lines.append(f'* `{lib}/{name}`: new file (see its header comment).')
    for lib, s in state.items():
        lines += ['', f'## {lib}', '', f"Base copy: {COPY_NAME[s['base']]}. Headers: "
                  + ', '.join(f'{n} from {COPY_NAME.get(c, c)}' for c, n in
                              collections.Counter(s.get('headers', {}).values()).items()) + '.', '']
        if s.get('compile_errors'):
            lines += ['Compile errors in the tree (first few):', '']
            lines += [f'    {e.strip()[:160]}' for e in s['compile_errors'][:8]] + ['']
        lines += ['| File | From | In tree | R | G | Z | Merged functions |', '|---|---|---|---|---|---|---|']
        for k, f in sorted(s['files'].items()):
            exp = f['expected']
            merged = '; '.join(f"{m['function']} ({m['copy']})" for m in f['merged'])
            lines.append(f"| {f.get('name', k)} | {f['copy']}{'+h' if f.get('headers_from') else ''} | {len(f.get('in_tree', []))} | "
                         + ' | '.join(str(exp.get(c, '-')) for c in 'RGZ') + f' | {merged} |')
    open(os.path.join(REPO, 'docs', 'imports.md'), 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('docs/imports.md written')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('command', choices=['plan', 'assemble', 'verify', 'refine', 'report', 'all'])
    ap.add_argument('--evidence', help="bytematch_all.sh's work directory")
    ap.add_argument('--lib', action='append', help='limit verify/refine to these libraries')
    ap.add_argument('--file', action='append', help='limit refine to these source files (e.g. lightenvironment.cpp)')
    a = ap.parse_args()
    if a.command in ('plan', 'assemble', 'refine', 'all') and not a.evidence:
        raise SystemExit('--evidence is required')
    ev = load_evidence(a.evidence) if a.evidence else None
    if a.command in ('plan', 'all'):
        state = plan(ev)
        save_state(state)
    state = load_state()
    if a.command in ('assemble', 'all'):
        save_state(assemble(state))
    if a.command in ('verify', 'all'):
        save_state(verify(state, a.lib))
    if a.command in ('refine', 'all'):
        save_state(refine(state, ev, a.lib, [key_of(f) for f in a.file] if a.file else None))
    if a.command in ('report', 'all'):
        report(state)


if __name__ == '__main__':
    main()
