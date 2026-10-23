"""Imports EA's Westwood libraries into the repository, file by file and function by function,
keeping whatever reproduces client.exe best.

Evidence comes from tools/vc6/bytematch_all.sh: for every EA copy (R Renegade, G Generals,
Z Zero Hour) and every source file, the client functions it reproduces byte for byte.

    python importer.py plan     --evidence DIR     choose a base copy per library and a copy per file
    python importer.py assemble --evidence DIR     copy the chosen files into the repository
                                                   (restarts every library from its base copy)
    python importer.py replay [--out DIR]          rebuild the libraries from the EA copies and the
                                                   decisions recorded in imports.json, into DIR
                                                   (default .work/replay), and compare with the
                                                   repository; exit status 1 if anything differs
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

# Compile flags where a library's differ from vc6.DEFAULT_FLAGS. wwshade (Zero Hour's newer
# library) was built with exception handling and without automatic inlining: its functions have
# EH frames (handlers at 0xAE0630+); /GX gains 16 of wwshade's own functions over no /GX (losing
# only out-of-line copies of other libraries' inline functions), and /Ob1 /GX 15 more over
# /Ob2 /GX, none lost (2026-10-23, docs/library-matching.md 3.5).
# wwshade and WWAudio were built with _MBCS: _tcsclen in StringClass(const TCHAR*) is a call to
# _mbslen (0xA2BCAB), whose only callers are ShdDefClass(classid) 0xA0EC30, the ShdMeshClass ctor
# 0xA08F80 and WWAudioClass 0x9F1BA0, 0x9F3700; WWAudio /D _MBCS gains those two, none lost.
# wwshade was also built with ENABLE_WWPROFILE: ShdMeshClass::Render 0xA09430 opens a
# WWProfileSampleClass ("ShdMeshClass::Render", Start_Profile 0x9EA930, dtor 0x444160).
LIB_FLAGS = {'wwshade': '/nologo /c /MT /W3 /O2 /Ob1 /GX /D _MBCS /D ENABLE_WWPROFILE',
             'wwaudio': '/nologo /c /MT /W3 /O2 /Ob2 /D _MBCS'}


# EA files the client does not link: not imported (plan, assemble) and dropped from the state.
# (library, file name in lower case) -> the evidence.
NOT_LINKED = {
    ('wwshade', 'shdlegacyw3d.cpp'):
        'no LegacyW3D shader in the client: no "LegacyW3D" string, SHD_Force_Links (0xA08C70) links 5 '
        'shaders, ShdRendererClass::Init_Shaders (0xA07E00) loops class ids 1..5, one prototype loader',
    ('wwshade', 'shdlegacyw3d.h'): 'its header (included by nothing else in E&B\'s wwshade)',
}


def not_linked(lib, name):
    return (lib, name.lower()) in NOT_LINKED


def drop_not_linked(state):
    for lib, s in state.items():
        for k in [k for k, f in s['files'].items() if not_linked(lib, f.get('name', k + '.cpp'))]:
            del s['files'][k]
        for name in [n for n in s.get('headers', {}) if not_linked(lib, n)]:
            del s['headers'][name]
    return state


def flags_for(paths):
    """The compile flags of the library the files belong to (all from one library)."""
    libs = {os.path.basename(os.path.dirname(os.path.abspath(p))).lower() for p in paths}
    flags = {LIB_FLAGS.get(l, vc6.DEFAULT_FLAGS) for l in libs}
    assert len(flags) == 1, f'files of libraries with different flags in one compile: {sorted(libs)}'
    return flags.pop()

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
         'Sound3DClass has E&B fields: 12 bytes after m_CurrentVelocity (+0x148) and four ints after m_LastUpdate (+0x160 to +0x16C), all zeroed by the constructors (client 0x9F76A0, 0x9F7C90, 0x9F9220) and not copied by operator=. Meaning not known: m_Unknown148 (a Vector3 by size), m_Unknown160 to m_Unknown16C'),
        ('\t\tunsigned int\t\t\tm_LastUpdate;',
         '\t\tunsigned int\t\t\tm_LastUpdate;\n\t\tint\t\t\t\t\t\tm_Unknown160;\t\t// E&B: zeroed at construction, not copied; meaning not known\n\t\tint\t\t\t\t\t\tm_Unknown164;\t\t// E&B: zeroed at construction, not copied; meaning not known\n\t\tint\t\t\t\t\t\tm_Unknown168;\t\t// E&B: zeroed at construction, not copied; meaning not known\n\t\tint\t\t\t\t\t\tm_Unknown16C;\t\t// E&B: zeroed at construction, not copied; meaning not known',
         'the four ints at the end'),
    ],
    ('wwaudio', 'sound3d.cpp'): [
        ('\t  m_CurrentVelocity (0, 0, 0),',
         '\t  m_CurrentVelocity (0, 0, 0),\n\t  m_Unknown148 (0, 0, 0),',
         'both constructors zero m_Unknown148'),
        ('\t  m_LastUpdate (0),',
         '\t  m_LastUpdate (0),\n\t  m_Unknown160 (0),\n\t  m_Unknown164 (0),\n\t  m_Unknown168 (0),\n\t  m_Unknown16C (0),',
         'and m_Unknown160 to m_Unknown16C'),
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
    ('ww3d2', 'meshmdl.h'): [
        ('\tMaterialInfoClass\t*\t\t\t\t\t\t\t\t\tMatInfo;',
         '\tMaterialInfoClass\t*\t\t\t\t\t\t\t\t\tMatInfo;\n\n\t// E&B (as Generals and Zero Hour): the polygon renderers of this model\n\tDX8PolygonRendererList\t\t\t\t\t\t\t\tPolygonRendererList;',
         "MeshModelClass keeps the PolygonRendererList member Renegade's later revision removed (Generals and Zero Hour have it after MatInfo): GapFiller is 24 bytes (a MultiListClass) later in the client"),
    ],
    ('wwlib', 'random.h'): [
        ('\tint magnitude = maxval - minval;',
         '\tunsigned int magnitude = (unsigned int)(maxval - minval);',
         "E&B's Pick_Random_Number works in unsigned arithmetic and guards the full-range mask: all four operator()(min, max) at 0x9D0000, 0x9D0180, 0x9D02B0, 0x9D0510 compare unsigned (jbe/ja) and reload an all-ones mask when the shift leaves none (variant A in .work/random)"),
        ('\tint mask = ~( (~0L) << (highbit+1));',
         '\tunsigned int mask = ~( (~0L) << (highbit+1));\n\tif (mask == 0) mask = ~0U;',
         'the mask, unsigned, all ones when the range needs every bit'),
        ('\tint pick = magnitude+1;',
         '\tunsigned int pick = magnitude+1;',
         'and the pick, compared unsigned with the magnitude'),
    ],
    ('ww3d2', 'shader.h'): [
        ('\t\tSSCAT_ADDITIVE,\n\t\tSSCAT_OTHER',
         '\t\tSSCAT_ADDITIVE,\n\t\tSSCAT_SCREEN,\n\t\tSSCAT_OTHER',
         "Zero Hour's SSCAT_SCREEN static sort category (src ONE, dest ONE_MINUS_SRC_COLOR): ShaderClass::Get_SS_Category (0x93CE00) and Guess_Sort_Level (0x93CE90) test for it"),
    ],
    ('ww3d2', 'mapper.h'): [
        ('\tGridTextureMapperClass(float fps, unsigned int gridwidth_log2, unsigned int stage);',
         '\tGridTextureMapperClass(float fps, unsigned int gridwidth_log2, unsigned int last_frame, unsigned int offset, unsigned int stage);',
         "GridTextureMapperClass has Zero Hour's Offset (the starting frame, after LastFrame) and its constructor taking last_frame and offset: every member from +0x20 is 4 bytes later in the client"),
        ('\tunsigned int\tLastFrame;\t\t\t\t// Last frame to use',
         '\tunsigned int\tLastFrame;\t\t\t\t// Last frame to use\n\tunsigned int\tOffset;\t\t\t\t// Only affects initialization, but need to store it for copy CTors to work',
         'the Offset member'),
        ('\tGridClassicEnvironmentMapperClass(float fps,unsigned int gridwidth, unsigned int stage):GridTextureMapperClass(fps,gridwidth,stage) { }',
         '\tGridClassicEnvironmentMapperClass(float fps, unsigned int gridwidth_log2, unsigned int last_frame, unsigned int offset, unsigned int stage) : GridTextureMapperClass(fps, gridwidth_log2, last_frame, offset, stage) { }',
         'the grid environment mappers pass them on, as in Zero Hour'),
        ('\tGridEnvironmentMapperClass(float fps,unsigned int gridwidth, unsigned int stage):GridTextureMapperClass(fps,gridwidth,stage) { }',
         '\tGridEnvironmentMapperClass(float fps, unsigned int gridwidth_log2, unsigned int last_frame, unsigned int offset, unsigned int stage) : GridTextureMapperClass(fps, gridwidth_log2, last_frame, offset, stage) { }',
         '(both)'),
    ],
    ('ww3d2', 'mapper.cpp'): [
        ('GridTextureMapperClass::GridTextureMapperClass(float fps, unsigned int gridwidth_log2, unsigned int stage) :\n\tTextureMapperClass(stage)\n{\n\tLastFrame = 0;\n\tinitialize(fps, gridwidth_log2);\n}',
         'GridTextureMapperClass::GridTextureMapperClass(float fps, unsigned int gridwidth_log2, unsigned int last_frame, unsigned int offset, unsigned int stage) :\n\tTextureMapperClass(stage),\n\tLastFrame(last_frame),\n\tOffset(offset)\n{\n\tinitialize(fps, gridwidth_log2);\n}',
         "and Zero Hour's constructor for it"),
    ],
    ('ww3d2', 'rinfo.h'): [
        ('class LightEnvironmentClass;',
         'class LightEnvironmentClass;\nclass TexProjectClass;',
         "RenderInfoClass has Zero Hour's Texture_Projector after light_environment (not its three alpha overrides): the client's constructor (0x930F80) zeroes one more member, and every member from +0x14 is 4 bytes later (SpecialRenderInfoClass's own from 0x124)"),
        ('\tLightEnvironmentClass*\t\tlight_environment;',
         '\tLightEnvironmentClass*\t\tlight_environment;\n\n\tTexProjectClass*\t\t\t\tTexture_Projector;',
         'the member'),
    ],
    ('ww3d2', 'rinfo.cpp'): [
        ('\tlight_environment(0),',
         '\tlight_environment(0),\n\tTexture_Projector(NULL),',
         'initialised to NULL, as in Zero Hour'),
    ],
    ('ww3d2', 'dx8caps.h'): [
        ('\tbool SupportRenderToTextureFormat[WW3D_FORMAT_COUNT];',
         '\tbool SupportRenderToTextureFormat[WW3D_FORMAT_COUNT];\n\tbool SupportDepthStencilFormat[WW3D_ZFORMAT_COUNT];',
         "DX8Caps has Zero Hour's twelve extra capability flags (SupportDepthStencilFormat[8] before SupportZBias; SupportModAlphaAddClr, SupportDot3, SupportPointSprites, SupportCubemaps after SupportAnisotropicFiltering): every member from VertexShaderVersion is 12 bytes later in the client, DeviceId too, so not Zero Hour's MaxSimultaneousTextures. Accessors as Zero Hour's"),
        ('\tbool SupportAnisotropicFiltering;',
         '\tbool SupportAnisotropicFiltering;\n\tbool SupportModAlphaAddClr;\n\tbool SupportDot3;\n\tbool SupportPointSprites;\n\tbool SupportCubemaps;',
         'the four flags after SupportAnisotropicFiltering'),
        ('\tbool Support_Anisotropic_Filtering() const { return SupportAnisotropicFiltering; }',
         '\tbool Support_Anisotropic_Filtering() const { return SupportAnisotropicFiltering; }\n\tbool Support_ModAlphaAddClr() const { return SupportModAlphaAddClr; }\n\tbool Support_Dot3() const { return SupportDot3; }\n\tbool Support_PointSprites() const { return SupportPointSprites; }\n\tbool Support_Cubemaps() const { return SupportCubemaps; }',
         "Zero Hour's accessors for them"),
        ('\tint Get_Pixel_Shader_Minor_Version() const { return 0xff&(VertexShaderVersion); }',
         '\tint Get_Pixel_Shader_Minor_Version() const { return 0xff&(VertexShaderVersion); }\n\tint Get_Pixel_Shader_Major_Version() const { return 0xff&(PixelShaderVersion>>8); }\n\tbool Support_Depth_Stencil_Format(WW3DZFormat format) const { return SupportDepthStencilFormat[format]; }',
         "and Zero Hour's Get_Pixel_Shader_Major_Version (Renegade's is misspelt Majon) and Support_Depth_Stencil_Format"),
    ],
    ('ww3d2', 'rendobj.h'): [
        ('\t\tCLASSID_RENEGADE_TERRAIN,',
         '\t\tCLASSID_SHDMESH,\t\t\t\t// mesh class that uses the scaleable shader system',
         "E&B's class IDs end as Zero Hour's: ShdMeshClass::Class_ID (0xA0A7E0, in the table with ShdMeshClass::Render) returns 0x24, CLASSID_LAND + 1, where Renegade has CLASSID_RENEGADE_TERRAIN (unused in the libraries)"),
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
        ('\tvirtual int\t\t\t\t\t\tRemove_Sub_Objects_From_Bone(const char * bname);',
         '\tvirtual int\t\t\t\t\t\tRemove_Sub_Objects_From_Bone(int boneindex);\n\tvirtual int\t\t\t\t\t\tRemove_Sub_Objects_From_Bone(const char * bname);',
         "RenderObjClass's vtable as in the client (113 slots, at 0xAE84F8; tools/re/vtables.py): Zero Hour's Remove_Sub_Objects_From_Bone(int) overload (slot 35, 0x941F90; VC6 lists overloads in reverse declaration order)"),
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
        ('int RenderObjClass::Remove_Sub_Objects_From_Bone(const char * bname)\n{\n\tint boneidx = Get_Bone_Index(bname);\n\tint count = Get_Num_Sub_Objects_On_Bone(boneidx);\n\tint remove_count = 0;\n\tfor (int i = count-1; i >= 0; i--) {\n\t\tRenderObjClass *robj = Get_Sub_Object_On_Bone(i, boneidx);\n',
         'int RenderObjClass::Remove_Sub_Objects_From_Bone(int boneindex)\n{\n\tint count = Get_Num_Sub_Objects_On_Bone(boneindex);\n\tint remove_count = 0;\n\tfor (int i = count-1; i >= 0; i--) {\n\t\tRenderObjClass *robj = Get_Sub_Object_On_Bone(i, boneindex);\n',
         'and the overload itself, from Zero Hour'),
        ('\treturn remove_count;\n}\n\n\n/****',
         '\treturn remove_count;\n}\n\n\n/***********************************************************************************************\n * RenderObjClass::Remove_Sub_Objects_From_Bone -- remove all objects from a named bone        *\n *=============================================================================================*/\nint RenderObjClass::Remove_Sub_Objects_From_Bone(const char * bname)\n{\n\treturn Remove_Sub_Objects_From_Bone(Get_Bone_Index(bname));\n}\n\n\n/****',
         'the named version calls it (Zero Hour)'),
        ('	User_Data(NULL),', """	User_Data(NULL),
	RenderHook(NULL),""", 'both constructors clear the render hook'),
    ],
}
# D9 (docs/progress.md): E&B's texture classes and DX8Wrapper are Zero Hour's. Files that stay
# Renegade's are written against its texture API; these edits give them Zero Hour's, as
# Zero Hour's own copies of those files have it. Applied after the file's PATCHES entry.
TEXTURE_API_PATCHES = {
    ('ww3d2', 'assetmgr.cpp'): [
        ('\t\tIDirect3DTexture8* d3d_texture=tex->Peek_DX8_Texture();',
         '\t\tIDirect3DTexture8* d3d_texture=tex->Peek_D3D_Texture();',
         "Zero Hour's TextureBaseClass::Peek_D3D_Texture (Renegade: Peek_DX8_Texture)"),
        ('\tTextureClass::MipCountType mip_level_count,',
         '\tMipCountType mip_level_count,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\t\tmip_level_count=TextureClass::MIP_LEVELS_1;',
         '\t\tmip_level_count=MIP_LEVELS_1;',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
    ],
    ('ww3d2', 'bmp2d.cpp'): [
        ('\t\t\tTextureClass *piece_texture =NEW_REF(TextureClass,(piece_surface,TextureClass::MIP_LEVELS_1));\t\t\t\n\t\t\tpiece_texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);\n\t\t\tpiece_texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);',
         '\t\t\tTextureClass *piece_texture =NEW_REF(TextureClass,(piece_surface,MIP_LEVELS_1));\t\t\t\n\t\t\tpiece_texture->Get_Filter().Set_U_Addr_Mode(TextureFilterClass::TEXTURE_ADDRESS_CLAMP);\n\t\t\tpiece_texture->Get_Filter().Set_V_Addr_Mode(TextureFilterClass::TEXTURE_ADDRESS_CLAMP);',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file; addressing modes are set on the texture's TextureFilterClass in Zero Hour's texture API"),
    ],
    ('ww3d2', 'dx8wrapper.cpp'): [
        ('\tTextureClass::MipCountType mip_level_count,',
         '\tMipCountType mip_level_count,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tconst char *filename,\n\tTextureClass::MipCountType mip_level_count)\n{',
         '\tconst char *filename,\n\tMipCountType mip_level_count)\n{',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tTextureClass::MipCountType mip_level_count)',
         '\tMipCountType mip_level_count)',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tif (mip_level_count!=TextureClass::MIP_LEVELS_1) {',
         '\tif (mip_level_count!=MIP_LEVELS_1) {',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tTextureClass * tex = NEW_REF(TextureClass,(width,height,format,TextureClass::MIP_LEVELS_1,TextureClass::POOL_DEFAULT,true));',
         '\tTextureClass * tex = NEW_REF(TextureClass,(width,height,format,MIP_LEVELS_1,TextureClass::POOL_DEFAULT,true));',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tif (tex->Peek_DX8_Texture() == NULL) {',
         '\tif (tex->Peek_D3D_Texture() == NULL) {',
         "Zero Hour's TextureBaseClass::Peek_D3D_Texture (Renegade: Peek_DX8_Texture)"),
        ('void\nDX8Wrapper::Set_Render_Target(IDirect3DSurface8 *render_target, bool use_default_depth_buffer)\n{\n\tDX8_THREAD_ASSERT();\n\tDX8_Assert();\n\n\t//\n\t//\tShould we restore the default render target set a new one?\n\t//\n\tif (render_target == NULL || render_target == DefaultRenderTarget) {\n\t\t// If there is currently a custom render target, default must NOT be NULL.\n\t\tif (CurrentRenderTarget) {\n\t\t\tWWASSERT(DefaultRenderTarget!=NULL);\n\t\t}\n\n\t\t//\n\t\t//\tRestore the default render target\n\t\t//\n\t\tif (DefaultRenderTarget != NULL) {\n\t\t\tDX8CALL(SetRenderTarget (DefaultRenderTarget, DefaultDepthBuffer));\n\t\t\tDefaultRenderTarget->Release ();\n\t\t\tDefaultRenderTarget = NULL;\n\t\t\tif (DefaultDepthBuffer) {\n\t\t\t\tDefaultDepthBuffer->Release ();\n\t\t\t\tDefaultDepthBuffer = NULL;\n\t\t\t}\n\t\t}\n\n\t\t//\n\t\t//\tRelease our hold on the "current" render target\n\t\t//\n\t\tif (CurrentRenderTarget != NULL) {\n\t\t\tCurrentRenderTarget->Release ();\n\t\t\tCurrentRenderTarget = NULL;\n\t\t}\n\n\t} else if (render_target != CurrentRenderTarget) {\n\t\tWWASSERT(DefaultRenderTarget==NULL);\n\n\t\t//\n\t\t//\tWe\'ll need the depth buffer later...\n\t\t//\n\t\tif (DefaultDepthBuffer == NULL) {\n//\t\tIDirect3DSurface8 *depth_buffer = NULL;\n\t\t\tDX8CALL(GetDepthStencilSurface (&DefaultDepthBuffer));\n\t\t}\n\n\t\t//\n\t\t//\tGet a pointer to the default render target (if necessary)\n\t\t//\n\t\tif (DefaultRenderTarget == NULL) {\n\t\t\tDX8CALL(GetRenderTarget (&DefaultRenderTarget));\n\t\t}\n\n\t\t//\n\t\t//\tRelease our hold on the old "current" render target\n\t\t//\n\t\tif (CurrentRenderTarget != NULL) {\n\t\t\tCurrentRenderTarget->Release ();\n\t\t\tCurrentRenderTarget = NULL;\n\t\t}\n\n\t\t//\n\t\t//\tKeep a copy of the current render target (for housekeeping)\n\t\t//\n\t\tCurrentRenderTarget = render_target;\n\t\tWWASSERT (CurrentRenderTarget != NULL);\n\t\tif (CurrentRenderTarget != NULL) {\n\t\t\tCurrentRenderTarget->AddRef ();\n\n\t\t\t//\n\t\t\t//\tSwitch render targets\n\t\t\t//\n\t\t\tif (use_default_depth_buffer) {\n\t\t\t\tDX8CALL(SetRenderTarget (CurrentRenderTarget, DefaultDepthBuffer));\n\t\t\t} else {\n\t\t\t\tDX8CALL(SetRenderTarget (CurrentRenderTarget, NULL));\n\t\t\t}\n\t\t}\n\t}\n\n\t//\n\t//\tFree our hold on the depth buffer\n\t//\n//\tif (depth_buffer != NULL) {\n//\t\tdepth_buffer->Release ();\n//\t\tdepth_buffer = NULL;\n//\t}\n\n\tIsRenderToTexture = false;\n\treturn ;\n}',
         'void\nDX8Wrapper::Set_Render_Target(IDirect3DSurface8 *render_target, bool use_default_depth_buffer)\n{\n//#ifndef _XBOX\n\tDX8_THREAD_ASSERT();\n\tDX8_Assert();\n\n\t//\n\t//\tShould we restore the default render target set a new one?\n\t//\n\tif (render_target == NULL || render_target == DefaultRenderTarget) \n\t{\n\t\t// If there is currently a custom render target, default must NOT be NULL.\n\t\tif (CurrentRenderTarget) \n\t\t{\n\t\t\tWWASSERT(DefaultRenderTarget!=NULL);\n\t\t}\n\n\t\t//\n\t\t//\tRestore the default render target\n\t\t//\n\t\tif (DefaultRenderTarget != NULL) \n\t\t{\n\t\t\tDX8CALL(SetRenderTarget (DefaultRenderTarget, DefaultDepthBuffer));\n\t\t\tDefaultRenderTarget->Release ();\n\t\t\tDefaultRenderTarget = NULL;\n\t\t\tif (DefaultDepthBuffer) \n\t\t\t{\n\t\t\t\tDefaultDepthBuffer->Release ();\n\t\t\t\tDefaultDepthBuffer = NULL;\n\t\t\t}\n\t\t}\n\n\t\t//\n\t\t//\tRelease our hold on the "current" render target\n\t\t//\n\t\tif (CurrentRenderTarget != NULL) \n\t\t{\n\t\t\tCurrentRenderTarget->Release ();\n\t\t\tCurrentRenderTarget = NULL;\n\t\t}\n\n\t\tif (CurrentDepthBuffer!=NULL)\n\t\t{\n\t\t\tCurrentDepthBuffer->Release();\n\t\t\tCurrentDepthBuffer=NULL;\n\t\t}\n\n\t} \n\telse if (render_target != CurrentRenderTarget) \n\t{\n\t\tWWASSERT(DefaultRenderTarget==NULL);\n\n\t\t//\n\t\t//\tWe\'ll need the depth buffer later...\n\t\t//\n\t\tif (DefaultDepthBuffer == NULL) \n\t\t{\n//\t\tIDirect3DSurface8 *depth_buffer = NULL;\n\t\t\tDX8CALL(GetDepthStencilSurface (&DefaultDepthBuffer));\n\t\t}\n\n\t\t//\n\t\t//\tGet a pointer to the default render target (if necessary)\n\t\t//\n\t\tif (DefaultRenderTarget == NULL) \n\t\t{\n\t\t\tDX8CALL(GetRenderTarget (&DefaultRenderTarget));\n\t\t}\n\n\t\t//\n\t\t//\tRelease our hold on the old "current" render target\n\t\t//\n\t\tif (CurrentRenderTarget != NULL) \n\t\t{\n\t\t\tCurrentRenderTarget->Release ();\n\t\t\tCurrentRenderTarget = NULL;\n\t\t}\n\n\t\tif (CurrentDepthBuffer!=NULL)\n\t\t{\n\t\t\tCurrentDepthBuffer->Release();\n\t\t\tCurrentDepthBuffer=NULL;\n\t\t}\n\n\t\t//\n\t\t//\tKeep a copy of the current render target (for housekeeping)\n\t\t//\n\t\tCurrentRenderTarget = render_target;\n\t\tWWASSERT (CurrentRenderTarget != NULL);\n\t\tif (CurrentRenderTarget != NULL) \n\t\t{\n\t\t\tCurrentRenderTarget->AddRef ();\n\n\t\t\t//\n\t\t\t//\tSwitch render targets\n\t\t\t//\n\t\t\tif (use_default_depth_buffer) \n\t\t\t{\n\t\t\t\tDX8CALL(SetRenderTarget (CurrentRenderTarget, DefaultDepthBuffer));\n\t\t\t}\n\t\t\telse \n\t\t\t{\n\t\t\t\tDX8CALL(SetRenderTarget (CurrentRenderTarget, NULL));\n\t\t\t}\n\t\t}\n\t}\n\n\t//\n\t//\tFree our hold on the depth buffer\n\t//\n//\tif (depth_buffer != NULL) {\n//\t\tdepth_buffer->Release ();\n//\t\tdepth_buffer = NULL;\n//\t}\n\n\tIsRenderToTexture = false;\n\treturn ;\n//#endif // XBOX\n}\n\n\nvoid DX8Wrapper::Set_Render_Target\n(\n\tIDirect3DSurface8* render_target, \n\tIDirect3DSurface8* depth_buffer \n)\n{\n//#ifndef _XBOX\n\tDX8_THREAD_ASSERT();\n\tDX8_Assert();\n\n\t//\n\t//\tShould we restore the default render target set a new one?\n\t//\n\tif (render_target == NULL || render_target == DefaultRenderTarget) \n\t{\n\t\t// If there is currently a custom render target, default must NOT be NULL.\n\t\tif (CurrentRenderTarget) \n\t\t{\n\t\t\tWWASSERT(DefaultRenderTarget!=NULL);\n\t\t}\n\n\t\t//\n\t\t//\tRestore the default render target\n\t\t//\n\t\tif (DefaultRenderTarget != NULL) \n\t\t{\n\t\t\tDX8CALL(SetRenderTarget (DefaultRenderTarget, DefaultDepthBuffer));\n\t\t\tDefaultRenderTarget->Release ();\n\t\t\tDefaultRenderTarget = NULL;\n\t\t\tif (DefaultDepthBuffer) \n\t\t\t{\n\t\t\t\tDefaultDepthBuffer->Release ();\n\t\t\t\tDefaultDepthBuffer = NULL;\n\t\t\t}\n\t\t}\n\n\t\t//\n\t\t//\tRelease our hold on the "current" render target\n\t\t//\n\t\tif (CurrentRenderTarget != NULL) \n\t\t{\n\t\t\tCurrentRenderTarget->Release ();\n\t\t\tCurrentRenderTarget = NULL;\n\t\t}\n\n\t\tif (CurrentDepthBuffer!=NULL)\n\t\t{\n\t\t\tCurrentDepthBuffer->Release();\n\t\t\tCurrentDepthBuffer=NULL;\n\t\t}\n\t}\n\telse if (render_target != CurrentRenderTarget) \n\t{\n\t\tWWASSERT(DefaultRenderTarget==NULL);\n\n\t\t//\n\t\t//\tWe\'ll need the depth buffer later...\n\t\t//\n\t\tif (DefaultDepthBuffer == NULL) \n\t\t{\n//\t\tIDirect3DSurface8 *depth_buffer = NULL;\n\t\t\tDX8CALL(GetDepthStencilSurface (&DefaultDepthBuffer));\n\t\t}\n\n\t\t//\n\t\t//\tGet a pointer to the default render target (if necessary)\n\t\t//\n\t\tif (DefaultRenderTarget == NULL) \n\t\t{\n\t\t\tDX8CALL(GetRenderTarget (&DefaultRenderTarget));\n\t\t}\n\n\t\t//\n\t\t//\tRelease our hold on the old "current" render target\n\t\t//\n\t\tif (CurrentRenderTarget != NULL) \n\t\t{\n\t\t\tCurrentRenderTarget->Release ();\n\t\t\tCurrentRenderTarget = NULL;\n\t\t}\n\n\t\tif (CurrentDepthBuffer!=NULL)\n\t\t{\n\t\t\tCurrentDepthBuffer->Release();\n\t\t\tCurrentDepthBuffer=NULL;\n\t\t}\n\n\t\t//\n\t\t//\tKeep a copy of the current render target (for housekeeping)\n\t\t//\n\t\tCurrentRenderTarget = render_target;\n\t\tCurrentDepthBuffer = depth_buffer;\n\t\tWWASSERT (CurrentRenderTarget != NULL);\n\t\tif (CurrentRenderTarget != NULL) \n\t\t{\n\t\t\tCurrentRenderTarget->AddRef ();\n\t\t\tCurrentDepthBuffer->AddRef();\n\n\t\t\t//\n\t\t\t//\tSwitch render targets\n\t\t\t//\n\t\t\tDX8CALL(SetRenderTarget (CurrentRenderTarget, CurrentDepthBuffer));\n\t\t}\n\t}\n\n\tIsRenderToTexture=true;\n//#endif // XBOX\n}',
         "Zero Hour's Set_Render_Target(IDirect3DSurface8 *, bool), which also releases the current depth buffer (byte-exact at 0x926AD0), and its overload taking a depth buffer (0x926C40)"),
        ('void\nDX8Wrapper::Set_Render_Target(IDirect3DSwapChain8 *swap_chain)\n{\n\tDX8_THREAD_ASSERT();\n\tWWASSERT (swap_chain != NULL);\n\n\t//\n\t//\tGet the back buffer for the swap chain\n\t//\n\tLPDIRECT3DSURFACE8 render_target = NULL;\n\tswap_chain->GetBackBuffer (0, D3DBACKBUFFER_TYPE_MONO, &render_target);\n\n\t//\n\t//\tSet this back buffer as the render targer\n\t//\n\tSet_Render_Target (render_target, true);\n\n\t//\n\t//\tRelease our hold on the back buffer\n\t//\n\tif (render_target != NULL) {\n\t\trender_target->Release ();\n\t\trender_target = NULL;\n\t}\n\n\tIsRenderToTexture = false;\n\n\treturn ;\n}',
         'void\nDX8Wrapper::Set_Render_Target(IDirect3DSwapChain8 *swap_chain)\n{\n\tDX8_THREAD_ASSERT();\n\tWWASSERT (swap_chain != NULL);\n\n\t//\n\t//\tGet the back buffer for the swap chain\n\t//\n\tLPDIRECT3DSURFACE8 render_target = NULL;\n\tswap_chain->GetBackBuffer (0, D3DBACKBUFFER_TYPE_MONO, &render_target);\n\n\t//\n\t//\tSet this back buffer as the render targer\n\t//\n\tSet_Render_Target (render_target, true);\n\n\t//\n\t//\tRelease our hold on the back buffer\n\t//\n\tif (render_target != NULL) {\n\t\trender_target->Release ();\n\t\trender_target = NULL;\n\t}\n\n\tIsRenderToTexture = false;\n\n\treturn ;\n}',
         "Zero Hour's Set_Render_Target(IDirect3DSwapChain8 *)"),
        ('\tIsRenderToTexture = true;\n}\n',
         '\tIsRenderToTexture = true;\n}\n\nvoid DX8Wrapper::Create_Render_Target\n(\n\tint width, \n\tint height, \n\tWW3DFormat format,\n\tWW3DZFormat zformat,\n\tTextureClass** target,\n\tZTextureClass** depth_buffer\n)\n{\n\tDX8_THREAD_ASSERT();\n\tDX8_Assert();\n\tnumber_of_DX8_calls++;\n\n\t// Use the current display format if format isn\'t specified\n\tif (format==WW3D_FORMAT_UNKNOWN) \n\t{\n\t\t*target=NULL;\n\t\t*depth_buffer=NULL;\n\t\treturn;\n/*\t\tD3DDISPLAYMODE mode;\n\t\tDX8CALL(GetDisplayMode(&mode));\n\t\tformat=D3DFormat_To_WW3DFormat(mode.Format);*/\n\t}\n\n\t// If render target format isn\'t supported return NULL\n\tif (!Get_Current_Caps()->Support_Render_To_Texture_Format(format) ||\n\t\t !Get_Current_Caps()->Support_Depth_Stencil_Format(zformat)) \n\t{\n\t\tWWDEBUG_SAY(("DX8Wrapper - Render target with depth format is not supported\\r\\n"));\n\t\treturn;\n\t}\n\n\t//\tNote: We\'re going to force the width and height to be powers of two and equal\n\tconst D3DCAPS8& dx8caps=Get_Current_Caps()->Get_DX8_Caps();\n\tfloat poweroftwosize = width;\n\tif (height > 0 && height < width) \n\t{\n\t\tpoweroftwosize = height;\n\t}\n\tpoweroftwosize = ::Find_POT (poweroftwosize);\n\n\tif (poweroftwosize>dx8caps.MaxTextureWidth) \n\t{\n\t\tpoweroftwosize=dx8caps.MaxTextureWidth;\n\t}\n\n\tif (poweroftwosize>dx8caps.MaxTextureHeight) \n\t{\n\t\tpoweroftwosize=dx8caps.MaxTextureHeight;\n\t}\n\n\twidth = height = poweroftwosize;\n\n\t//\tAttempt to create the render target\n\tTextureClass* tex=NEW_REF(TextureClass,(width,height,format,MIP_LEVELS_1,TextureClass::POOL_DEFAULT,true));\n\n\t// 3dfx drivers are lying in the CheckDeviceFormat call and claiming\n\t// that they support render targets!\n\tif (tex->Peek_D3D_Base_Texture() == NULL) \n\t{\n\t\tWWDEBUG_SAY(("DX8Wrapper - Render target creation failed!\\r\\n"));\n\t\tREF_PTR_RELEASE(tex);\n\t}\n\n\t*target=tex;\n\n\t// attempt to create the depth stencil buffer\n\t*depth_buffer=NEW_REF\n\t(\n\t\tZTextureClass,\n\t\t(\n\t\t\twidth,\n\t\t\theight,\n\t\t\tzformat,\n\t\t\tMIP_LEVELS_1,\n\t\t\tTextureClass::POOL_DEFAULT\n\t\t)\n\t);\n}\n\nvoid DX8Wrapper::Set_Render_Target_With_Z\n(\n\tTextureClass* texture,\n\tZTextureClass* ztexture\n)\n{\n\tWWASSERT(texture!=NULL);\n\tIDirect3DSurface8 * d3d_surf = texture->Get_D3D_Surface_Level();\n\tWWASSERT(d3d_surf != NULL);\n\n\tIDirect3DSurface8* d3d_zbuf=NULL;\n\tif (ztexture!=NULL)\n\t{\n\n\t\td3d_zbuf=ztexture->Get_D3D_Surface_Level();\n\t\tWWASSERT(d3d_zbuf!=NULL);\n\t\tSet_Render_Target(d3d_surf,d3d_zbuf);\n\t\td3d_zbuf->Release();\n\t}\n\telse\n\t{\n\t\tSet_Render_Target(d3d_surf,true);\n\t}\n\td3d_surf->Release();\n\n\tIsRenderToTexture = true;\n}\n',
         "Zero Hour's 06/27/02 additions (see dx8wrapper.h): a render target with a depth buffer (Create_Render_Target, Set_Render_Target_With_Z), verbatim"),
        ('\tDX8CALL(UpdateTexture(system->D3DTexture,video->D3DTexture));',
         '\tDX8CALL(UpdateTexture(system->Peek_D3D_Base_Texture(),video->Peek_D3D_Base_Texture()));',
         "the D3D texture is TextureBaseClass's private member; Zero Hour's copy uses Peek_D3D_Base_Texture"),
        ('}\n\nIDirect3DSurface8 * DX8Wrapper::_Create_DX8_Surface(unsigned int width, unsigned int height, WW3DFormat format)',
         '}\n\nIDirect3DTexture8 * DX8Wrapper::_Create_DX8_ZTexture\n(\n\tunsigned int width,\n\tunsigned int height,\n\tWW3DZFormat zformat,\n\tMipCountType mip_level_count,\n\tD3DPOOL pool\n)\n{\n\tDX8_THREAD_ASSERT();\n\tDX8_Assert();\n\tIDirect3DTexture8* texture = NULL;\n\n\tD3DFORMAT zfmt=WW3DZFormat_To_D3DFormat(zformat);\n\n\tunsigned ret=DX8Wrapper::_Get_D3D_Device8()->CreateTexture\n\t(\n\t\twidth,\n\t\theight,\n\t\tmip_level_count,\n\t\tD3DUSAGE_DEPTHSTENCIL, \n\t\tzfmt, \n\t\tpool,\n\t\t&texture\n\t);\n\n\tif (ret==D3DERR_NOTAVAILABLE) \n\t{\n\t\tNon_Fatal_Log_DX8_ErrorCode(ret,__FILE__,__LINE__);\n\t\treturn NULL;\n\t}\n\n\t// If ran out of texture ram, try invalidating some textures and mesh cache.\n\tif (ret==D3DERR_OUTOFVIDEOMEMORY) \n\t{\n\t\tWWDEBUG_SAY(("Error: Out of memory while creating render target. Trying to release assets...\\n"));\n\t\t// Free all textures that haven\'t been used in the last 5 seconds\n\t\tTextureClass::Invalidate_Old_Unused_Textures(5000);\n\n\t\t// Invalidate the mesh cache\n\t\tWW3D::_Invalidate_Mesh_Cache();\n\n\t\tret=DX8Wrapper::_Get_D3D_Device8()->CreateTexture\n\t\t(\n\t\t\twidth,\n\t\t\theight,\n\t\t\tmip_level_count,\n\t\t\tD3DUSAGE_DEPTHSTENCIL, \n\t\t\tzfmt, \n\t\t\tpool,\n\t\t\t&texture\n\t\t);\n\n\t\tif (SUCCEEDED(ret)) \n\t\t{\n\t\t\tWWDEBUG_SAY(("...Render target creation succesful.\\n"));\n\t\t}\n\t\telse \n\t\t{\n\t\t\tWWDEBUG_SAY(("...Render target creation failed.\\n"));\n\t\t}\n\t\tif (ret==D3DERR_OUTOFVIDEOMEMORY) \n\t\t{\n\t\t\tNon_Fatal_Log_DX8_ErrorCode(ret,__FILE__,__LINE__);\n\t\t\treturn NULL;\n\t\t}\n\t}\n\n\tDX8_ErrorCode(ret);\n\n\ttexture->AddRef(); // don\'t release this texture\n\n\t// Just return the texture, no reduction\n\t// allowed for render targets.\n\n\treturn texture;\n}\n\nIDirect3DSurface8 * DX8Wrapper::_Create_DX8_Surface(unsigned int width, unsigned int height, WW3DFormat format)',
         "Zero Hour's _Create_DX8_ZTexture, verbatim (byte-exact at 0x925DD0)"),
        ('unsigned DX8Wrapper::Get_Last_Frame_DX8_Calls()\t\t\t\t\t{ return last_frame_number_of_DX8_calls; }',
         'unsigned DX8Wrapper::Get_Last_Frame_DX8_Calls()\t\t\t\t\t{ return last_frame_number_of_DX8_calls; }\nunsigned DX8Wrapper::Get_Last_Frame_Draw_Calls()\t\t\t\t{ return last_frame_draw_calls; }',
         '(its accessor, Zero Hour)'),
        ('void DX8Wrapper::End_Statistics()\n{\n\tlast_frame_matrix_changes=matrix_changes;\n\tlast_frame_material_changes=material_changes;\n\tlast_frame_vertex_buffer_changes=vertex_buffer_changes;\n\tlast_frame_index_buffer_changes=index_buffer_changes;\n\tlast_frame_light_changes=light_changes;\n\tlast_frame_texture_changes = texture_changes;\n\tlast_frame_render_state_changes = render_state_changes;\n\tlast_frame_texture_stage_state_changes = texture_stage_state_changes;\n\tlast_frame_number_of_DX8_calls=number_of_DX8_calls;\n}',
         'void DX8Wrapper::End_Statistics()\n{\n\tlast_frame_matrix_changes=matrix_changes;\n\tlast_frame_material_changes=material_changes;\n\tlast_frame_vertex_buffer_changes=vertex_buffer_changes;\n\tlast_frame_index_buffer_changes=index_buffer_changes;\n\tlast_frame_light_changes=light_changes;\n\tlast_frame_texture_changes = texture_changes;\n\tlast_frame_render_state_changes = render_state_changes;\n\tlast_frame_texture_stage_state_changes = texture_stage_state_changes;\n\tlast_frame_number_of_DX8_calls=number_of_DX8_calls;\n\tlast_frame_draw_calls=draw_calls;\n}',
         "Zero Hour's End_Statistics, which counts draw calls too (byte-exact; Renegade's isn't)"),
        ('void DX8Wrapper::Begin_Statistics()\n{\n\tmatrix_changes=0;\n\tmaterial_changes=0;\n\tvertex_buffer_changes=0;\n\tindex_buffer_changes=0;\n\tlight_changes=0;\n\ttexture_changes = 0;\n\trender_state_changes =0;\n\ttexture_stage_state_changes =0;\n\tnumber_of_DX8_calls=0;\n}',
         'void DX8Wrapper::Begin_Statistics()\n{\n\tmatrix_changes=0;\n\tmaterial_changes=0;\n\tvertex_buffer_changes=0;\n\tindex_buffer_changes=0;\n\tlight_changes=0;\n\ttexture_changes = 0;\n\trender_state_changes =0;\n\ttexture_stage_state_changes =0;\n\tnumber_of_DX8_calls=0;\n\tdraw_calls=0;\n}',
         "Zero Hour's Begin_Statistics, which counts draw calls too (byte-exact; Renegade's isn't)"),
        ('void DX8Wrapper::Reset_Statistics()\n{\n\tmatrix_changes\t= 0;\n\tmaterial_changes = 0;\n\tvertex_buffer_changes = 0;\n\tindex_buffer_changes = 0;\n\tlight_changes = 0;\n\ttexture_changes = 0;\n\trender_state_changes =0;\n\ttexture_stage_state_changes =0;\n\n\tnumber_of_DX8_calls = 0;\n\tlast_frame_matrix_changes = 0;\n\tlast_frame_material_changes = 0;\n\tlast_frame_vertex_buffer_changes = 0;\n\tlast_frame_index_buffer_changes = 0;\n\tlast_frame_light_changes = 0;\n\tlast_frame_texture_changes = 0;\n\tlast_frame_render_state_changes = 0;\n\tlast_frame_texture_stage_state_changes = 0;\n\tlast_frame_number_of_DX8_calls = 0;\n}',
         'void DX8Wrapper::Reset_Statistics()\n{\n\tmatrix_changes\t= 0;\n\tmaterial_changes = 0;\n\tvertex_buffer_changes = 0;\n\tindex_buffer_changes = 0;\n\tlight_changes = 0;\n\ttexture_changes = 0;\n\trender_state_changes =0;\n\ttexture_stage_state_changes =0;\n\tdraw_calls =0;\n\n\tnumber_of_DX8_calls = 0;\n\tlast_frame_matrix_changes = 0;\n\tlast_frame_material_changes = 0;\n\tlast_frame_vertex_buffer_changes = 0;\n\tlast_frame_index_buffer_changes = 0;\n\tlast_frame_light_changes = 0;\n\tlast_frame_texture_changes = 0;\n\tlast_frame_render_state_changes = 0;\n\tlast_frame_texture_stage_state_changes = 0;\n\tlast_frame_number_of_DX8_calls = 0;\n\tlast_frame_draw_calls =0;\n}',
         "Zero Hour's Reset_Statistics, which counts draw calls too (byte-exact; Renegade's isn't)"),
        ('\tTextureClass::_Init_Filters((TextureClass::TextureFilterMode)WW3D::Get_Texture_Filter());',
         '\tTextureFilterClass::_Init_Filters((TextureFilterClass::TextureFilterMode)WW3D::Get_Texture_Filter());',
         "the default filters belong to TextureFilterClass in Zero Hour's texture API (as in Zero Hour's copy)"),
        ('static unsigned\t\t\t\tlast_frame_number_of_DX8_calls\t\t\t\t\t\t= 0;',
         'static unsigned\t\t\t\tlast_frame_number_of_DX8_calls\t\t\t\t\t\t= 0;\nstatic unsigned\t\t\t\tlast_frame_draw_calls\t\t\t\t\t\t\t\t\t= 0;',
         '(and its last-frame copy)'),
        ('unsigned\t\t\t\t\t\t\tDX8Wrapper::texture_stage_state_changes\t\t\t= 0;',
         'unsigned\t\t\t\t\t\t\tDX8Wrapper::texture_stage_state_changes\t\t\t= 0;\nunsigned\t\t\t\t\t\t\tDX8Wrapper::draw_calls\t\t\t\t\t\t\t\t\t= 0;',
         '(the draw-call counter)'),
        ('IDirect3DSurface8 *\t\t\tDX8Wrapper::CurrentRenderTarget\t\t\t\t\t\t= NULL;',
         'IDirect3DSurface8 *\t\t\tDX8Wrapper::CurrentRenderTarget\t\t\t\t\t\t= NULL;\nIDirect3DSurface8 *\t\t\tDX8Wrapper::CurrentDepthBuffer\t\t\t\t\t\t= NULL;',
         '(the current depth buffer)'),
        ('D3DMATRIX\t\t\t\t\t\tDX8Wrapper::old_prj;',
         'D3DMATRIX\t\t\t\t\t\tDX8Wrapper::old_prj;\n\n// shader system additions KJM v\nDWORD\t\t\t\t\t\t\t\tDX8Wrapper::Vertex_Shader\t\t\t\t\t\t\t\t= 0;\nDWORD\t\t\t\t\t\t\t\tDX8Wrapper::Pixel_Shader\t\t\t\t\t\t\t\t= 0;\n\nVector4\t\t\t\t\t\t\tDX8Wrapper::Vertex_Shader_Constants[MAX_VERTEX_SHADER_CONSTANTS];\nVector4\t\t\t\t\t\t\tDX8Wrapper::Pixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];\n\nDWORD\t\t\t\t\t\t\t\tDX8Wrapper::Vertex_Processing_Behavior\t\t\t\t= 0;\nZTextureClass*\t\t\t\t\tDX8Wrapper::Shadow_Map[MAX_SHADOW_MAPS];\n// shader system additions KJM ^',
         "Zero Hour's 06/27/02 additions (see dx8wrapper.h): their statics (the two Vector4 arrays' initialisers are byte-exact, 0x920D70 and 0x920E70)"),
    ],
    ('ww3d2', 'font3d.cpp'): [
        ('\t\tTexture = NEW_REF(TextureClass,(_surface,TextureClass::MIP_LEVELS_1));',
         '\t\tTexture = NEW_REF(TextureClass,(_surface,MIP_LEVELS_1));',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
    ],
    ('ww3d2', 'metalmap.cpp'): [
        ('\t\tTextures[i]=NEW_REF(TextureClass,(METALMAP_SIZE,METALMAP_SIZE,format,TextureClass::MIP_LEVELS_1));\n\t\tTextures[i]->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);\n\t\tTextures[i]->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);',
         '\t\tTextures[i]=NEW_REF(TextureClass,(METALMAP_SIZE,METALMAP_SIZE,format,MIP_LEVELS_1));\n\t\tTextures[i]->Get_Filter().Set_U_Addr_Mode(TextureFilterClass::TEXTURE_ADDRESS_CLAMP);\n\t\tTextures[i]->Get_Filter().Set_V_Addr_Mode(TextureFilterClass::TEXTURE_ADDRESS_CLAMP);',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file; addressing modes are set on the texture's TextureFilterClass in Zero Hour's texture API"),
    ],
    ('ww3d2', 'missingtexture.cpp'): [
        ('\t\tTextureClass::MIP_LEVELS_ALL);',
         '\t\tMIP_LEVELS_ALL);',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
    ],
    ('ww3d2', 'part_emt.cpp'): [
        ('\t\t\tTextureClass::MIP_LEVELS_ALL,',
         '\t\t\tMIP_LEVELS_ALL,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
    ],
    ('ww3d2', 'render2d.cpp'): [
        ('\tTextureClass * tex = WW3DAssetManager::Get_Instance()->Get_Texture( filename, TextureClass::MIP_LEVELS_1 );',
         '\tTextureClass * tex = WW3DAssetManager::Get_Instance()->Get_Texture( filename, MIP_LEVELS_1 );',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
    ],
    ('ww3d2', 'render2dsentence.cpp'): [
        ('\t\tTextureClass *new_texture = new TextureClass (desc.Width, desc.Width, WW3D_FORMAT_A4R4G4B4, TextureClass::MIP_LEVELS_1);',
         '\t\tTextureClass *new_texture = new TextureClass (desc.Width, desc.Width, WW3D_FORMAT_A4R4G4B4, MIP_LEVELS_1);',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
    ],
    ('ww3d2', 'ww3d.cpp'): [
        ('\tif (texture_filter>TextureClass::TEXTURE_FILTER_ANISOTROPIC) texture_filter=TextureClass::TEXTURE_FILTER_ANISOTROPIC;',
         '\tif (texture_filter>TextureFilterClass::TEXTURE_FILTER_ANISOTROPIC) texture_filter=TextureFilterClass::TEXTURE_FILTER_ANISOTROPIC;',
         "filter enums belong to TextureFilterClass in Zero Hour's texture API"),
        ('\tTextureClass::_Init_Filters((TextureClass::TextureFilterMode)TextureFilter);',
         '\tTextureFilterClass::_Init_Filters((TextureFilterClass::TextureFilterMode)TextureFilter);',
         "filter enums and the default filters belong to TextureFilterClass in Zero Hour's texture API"),
    ],
    ('ww3d2', 'assetmgr.h'): [
        ('\t\tTextureClass::MipCountType mip_level_count=TextureClass::MIP_LEVELS_ALL,',
         '\t\tMipCountType mip_level_count=MIP_LEVELS_ALL,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
    ],
    ('ww3d2', 'dx8wrapper.h'): [
        ('\t\tTextureClass::MipCountType mip_level_count,',
         '\t\tMipCountType mip_level_count,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tstatic IDirect3DTexture8 * _Create_DX8_Texture(const char *filename, TextureClass::MipCountType mip_level_count);\n\tstatic IDirect3DTexture8 * _Create_DX8_Texture(IDirect3DSurface8 *surface, TextureClass::MipCountType mip_level_count);',
         '\tstatic IDirect3DTexture8 * _Create_DX8_Texture(const char *filename, MipCountType mip_level_count);\n\tstatic IDirect3DTexture8 * _Create_DX8_Texture(IDirect3DSurface8 *surface, MipCountType mip_level_count);',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tfriend class DX8VertexBufferClass;\n};\n',
         "\tfriend class DX8VertexBufferClass;\n};\n\n// shader system updates KJM v\nWWINLINE void DX8Wrapper::Set_Vertex_Shader(DWORD vertex_shader)\n{\n#if 0 //(gth) some code is bypassing this acessor function so we can't count on this variable...\n\t// may be incorrect if shaders are created and destroyed dynamically\n\tif (Vertex_Shader==vertex_shader) return;\n#endif\n\n\tVertex_Shader=vertex_shader;\n\tDX8CALL(SetVertexShader(Vertex_Shader));\n}\n\nWWINLINE void DX8Wrapper::Set_Pixel_Shader(DWORD pixel_shader)\n{\n\t// may be incorrect if shaders are created and destroyed dynamically\n\tif (Pixel_Shader==pixel_shader) return;\n\n\tPixel_Shader=pixel_shader;\n\tDX8CALL(SetPixelShader(Pixel_Shader));\n}\n\nWWINLINE void DX8Wrapper::Set_Vertex_Shader_Constant(int reg, const void* data, int count)\n{\n\tint memsize=sizeof(Vector4)*count;\n\n\t// may be incorrect if shaders are created and destroyed dynamically\n\tif (memcmp(data, &Vertex_Shader_Constants[reg],memsize)==0) return;\n\n\tmemcpy(&Vertex_Shader_Constants[reg],data,memsize);\n\tDX8CALL(SetVertexShaderConstant(reg,data,count));\n}\n\nWWINLINE void DX8Wrapper::Set_Pixel_Shader_Constant(int reg, const void* data, int count)\n{\n\tint memsize=sizeof(Vector4)*count;\n\n\t// may be incorrect if shaders are created and destroyed dynamically\n\tif (memcmp(data, &Pixel_Shader_Constants[reg],memsize)==0) return;\n\n\tmemcpy(&Pixel_Shader_Constants[reg],data,memsize);\n\tDX8CALL(SetPixelShaderConstant(reg,data,count));\n}\n// shader system updates KJM ^\n",
         "Zero Hour's inline shader setters, verbatim"),
        ('\tstatic IDirect3DSurface8 *\t\t\tCurrentRenderTarget;',
         '\tstatic IDirect3DSurface8 *\t\t\tCurrentRenderTarget;\n\tstatic IDirect3DSurface8 *\t\t\tCurrentDepthBuffer;',
         "the current depth buffer, released by Zero Hour's Set_Render_Target (0x926AD0, 0x926C40)"),
        ('\tstatic unsigned\t\t\t\t\t\ttexture_stage_state_changes;',
         '\tstatic unsigned\t\t\t\t\t\ttexture_stage_state_changes;\n\tstatic unsigned\t\t\t\t\t\tdraw_calls;',
         '(the draw-call counter)'),
        ('\tstatic IDirect3DBaseTexture8 *\tTextures[MAX_TEXTURE_STAGES];',
         '\tstatic IDirect3DBaseTexture8 *\tTextures[MAX_TEXTURE_STAGES];\n\n\t// shader system updates KJM v\n\tstatic DWORD\t\t\t\t\t\t\tVertex_Shader;\n\tstatic DWORD\t\t\t\t\t\t\tPixel_Shader;\n\n\tstatic Vector4\t\t\t\t\t\t\tVertex_Shader_Constants[MAX_VERTEX_SHADER_CONSTANTS];\n\tstatic Vector4\t\t\t\t\t\t\tPixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];\n\n\tstatic DWORD\t\t\t\t\t\t\tVertex_Processing_Behavior;\n\n\tstatic ZTextureClass*\t\t\t\tShadow_Map[MAX_SHADOW_MAPS];\n\t// shader system updates KJM ^',
         '(their state)'),
        ('\tstatic bool\t\t\t\t\tIs_Render_To_Texture(void) { return IsRenderToTexture; }',
         '\tstatic bool\t\t\t\t\tIs_Render_To_Texture(void) { return IsRenderToTexture; }\n\n\t// for depth map support KJM V\n\tstatic void Create_Render_Target\n\t(\n\t\tint width, \n\t\tint height, \n\t\tWW3DFormat format,\n\t\tWW3DZFormat zformat,\n\t\tTextureClass** target,\n\t\tZTextureClass** depth_buffer\n\t);\n\tstatic void\t\t\t\t\tSet_Render_Target_With_Z (TextureClass * texture, ZTextureClass* ztexture=NULL);\n\n\tstatic void Set_Shadow_Map(int idx, ZTextureClass* ztex) { Shadow_Map[idx]=ztex; }\n\tstatic ZTextureClass* Get_Shadow_Map(int idx) { return Shadow_Map[idx]; }\n\t// for depth map support KJM ^\n\n\t// shader system udpates KJM v\n\tstatic void Set_Vertex_Shader(DWORD vertex_shader);\n\tstatic void Set_Pixel_Shader(DWORD pixel_shader);\n\n\tstatic void Set_Vertex_Shader_Constant(int reg, const void* data, int count);\n\tstatic void Set_Pixel_Shader_Constant(int reg, const void* data, int count);\n\n\tstatic DWORD Get_Vertex_Processing_Behavior() { return Vertex_Processing_Behavior; }\n\t// shader system updates KJM ^',
         "Zero Hour's 06/27/02 additions (its 'KJM' blocks: render to shadow-buffer texture, shader system updates), which E&B has; the rest of E&B's DX8Wrapper is Renegade's (its render state, device and index-buffer code match Renegade's bytes): the depth-map and shader interface (Set_Pixel_Shader, Set_Vertex_Shader_Constant, Set_Pixel_Shader_Constant byte-exact at 0x927610..0x9276A0)"),
        ('\tstatic void\t\t\t\t\tSet_Render_Target (IDirect3DSurface8 *render_target, bool use_default_depth_buffer = false);',
         '\tstatic void\t\t\t\t\tSet_Render_Target (IDirect3DSurface8 *render_target, bool use_default_depth_buffer = false);\n\tstatic void\t\t\t\t\tSet_Render_Target (IDirect3DSurface8* render_target, IDirect3DSurface8* dpeth_buffer);',
         "Zero Hour's render target with a depth buffer (0x926C40)"),
        ('\tstatic unsigned Get_Last_Frame_DX8_Calls();',
         '\tstatic unsigned Get_Last_Frame_DX8_Calls();\n\tstatic unsigned Get_Last_Frame_Draw_Calls();',
         "Zero Hour's draw-call statistics (Reset/Begin/End_Statistics byte-exact at 0x924680..0x924730)"),
        ('\tstatic IDirect3DTexture8 * _Create_DX8_Texture(IDirect3DSurface8 *surface, MipCountType mip_level_count);',
         '\tstatic IDirect3DTexture8 * _Create_DX8_Texture(IDirect3DSurface8 *surface, MipCountType mip_level_count);\n\tstatic IDirect3DTexture8 * _Create_DX8_ZTexture\n\t(\n\t\tunsigned int width,\n\t\tunsigned int height,\n\t\tWW3DZFormat zformat,\n\t\tMipCountType mip_level_count,\n\t\tD3DPOOL pool=D3DPOOL_MANAGED\n\t);',
         "Zero Hour's _Create_DX8_ZTexture (byte-exact at 0x925DD0)"),
        ('class TextureClass;',
         'class TextureClass;\nclass ZTextureClass;',
         '(ZTextureClass, Zero Hour)'),
        ('const unsigned MAX_TEXTURE_STAGES=2;',
         'const unsigned MAX_TEXTURE_STAGES=2;\nconst unsigned MAX_VERTEX_SHADER_CONSTANTS=96;\nconst unsigned MAX_PIXEL_SHADER_CONSTANTS=8;\nconst unsigned MAX_SHADOW_MAPS=1;',
         "Zero Hour's 06/27/02 additions (its 'KJM' blocks: render to shadow-buffer texture, shader system updates), which E&B has; the rest of E&B's DX8Wrapper is Renegade's (its render state, device and index-buffer code match Renegade's bytes). Its constant limits (Zero Hour's values; MAX_TEXTURE_STAGES is set to Zero Hour's 8 in WWSHADE_API_PATCHES)"),
    ],
    ('ww3d2', 'texture.h'): [
        ("};\n\nclass CubeTextureClass : public TextureClass\n{\npublic:\n\t// Create texture with desired height, width and format.\n\tCubeTextureClass\n\t(\n\t\tunsigned width, \n\t\tunsigned height, \n\t\tWW3DFormat format,\n\t\tMipCountType mip_level_count=MIP_LEVELS_ALL,\n\t\tPoolType pool=POOL_MANAGED,\n\t\tbool rendertarget=false,\n\t\tbool allow_reduction=true\n\t);\n\n\t// Create texture from a file. If format is specified the texture is converted to that format.\n\t// Note that the format must be supported by the current device and that a texture can't exist\n\t// in the system with the same name in multiple formats.\n\tCubeTextureClass\n\t(\n\t\tconst char *name,\n\t\tconst char *full_path=NULL,\n\t\tMipCountType mip_level_count=MIP_LEVELS_ALL,\n\t\tWW3DFormat texture_format=WW3D_FORMAT_UNKNOWN,\n\t\tbool allow_compression=true,\n\t\tbool allow_reduction=true\n\t);\n\n\t// Create texture from a surface.\n\tCubeTextureClass\n\t(\n\t\tSurfaceClass *surface, \n\t\tMipCountType mip_level_count=MIP_LEVELS_ALL\n\t);\t\t\n\n\tCubeTextureClass(IDirect3DBaseTexture8* d3d_texture);\n\n\tvirtual void Apply_New_Surface(IDirect3DBaseTexture8* tex, bool initialized, bool disable_auto_invalidation = false);\t// If the parameter is true, the texture will be flagged as initialised\n\n\tvirtual TexAssetType Get_Asset_Type() const { return TEX_CUBEMAP; }\n\n\tvirtual CubeTextureClass* As_CubeTextureClass() { return this; }\n\n};\n\nclass VolumeTextureClass : public TextureClass\n{\npublic:\n\t// Create texture with desired height, width and format.\n\tVolumeTextureClass\n\t(\n\t\tunsigned width, \n\t\tunsigned height, \n\t\tunsigned depth,\n\t\tWW3DFormat format,\n\t\tMipCountType mip_level_count=MIP_LEVELS_ALL,\n\t\tPoolType pool=POOL_MANAGED,\n\t\tbool rendertarget=false,\n\t\tbool allow_reduction=true\n\t);\n\n\t// Create texture from a file. If format is specified the texture is converted to that format.\n\t// Note that the format must be supported by the current device and that a texture can't exist\n\t// in the system with the same name in multiple formats.\n\tVolumeTextureClass\n\t(\n\t\tconst char *name,\n\t\tconst char *full_path=NULL,\n\t\tMipCountType mip_level_count=MIP_LEVELS_ALL,\n\t\tWW3DFormat texture_format=WW3D_FORMAT_UNKNOWN,\n\t\tbool allow_compression=true,\n\t\tbool allow_reduction=true\n\t);\n\n\t// Create texture from a surface.\n\tVolumeTextureClass\n\t(\n\t\tSurfaceClass *surface, \n\t\tMipCountType mip_level_count=MIP_LEVELS_ALL\n\t);\t\t\n\n\tVolumeTextureClass(IDirect3DBaseTexture8* d3d_texture);\n\n\tvirtual void Apply_New_Surface(IDirect3DBaseTexture8* tex, bool initialized, bool disable_auto_invalidation = false);\t// If the parameter is true, the texture will be flagged as initialised\n\n\tvirtual TexAssetType Get_Asset_Type() const { return TEX_VOLUME; }\n\n\tvirtual VolumeTextureClass* As_VolumeTextureClass() { return this; }\n\nprotected:\n\n\tint Depth;\n};",
         '};',
         'no CubeTextureClass or VolumeTextureClass (08/05/02 redesign; see above)'),
        ('\tIDirect3DSurface8 *Get_D3D_Surface_Level(unsigned int level = 0);\n\tvirtual unsigned Get_Texture_Memory_Usage() const;\n',
         '\tIDirect3DSurface8 *Get_D3D_Surface_Level(unsigned int level = 0);\n\tunsigned Get_Texture_Memory_Usage() const;\n',
         "not virtual: ZTextureClass's vtable (0xB11A10) has 6 slots, the last the base's As_TextureClass (0x94B190)"),
        ('\tWW3DZFormat Get_Texture_Format() const { return DepthStencilTextureFormat; }\n\n\tvirtual TexAssetType Get_Asset_Type() const { return TEX_REGULAR; }',
         '\tWW3DZFormat Get_Texture_Format() const { return DepthStencilTextureFormat; }',
         'ZTextureClass: no Get_Asset_Type either'),
        ('\tvirtual unsigned Get_Texture_Memory_Usage() const;',
         '\tunsigned Get_Texture_Memory_Usage() const;',
         "not virtual: TextureClass's vtable (0xB119F0) is Delete_This, destructor, Init, Apply_New_Surface, Apply, As_TextureClass"),
        ('\tTextureClass(IDirect3DBaseTexture8* d3d_texture);\n\n\t// defualt constructors for derived classes (cube & vol)\n\tTextureClass\n\t(\n\t\tunsigned width,\n\t\tunsigned height,\n\t\tMipCountType mip_level_count=MIP_LEVELS_ALL,\n\t\tPoolType pool=POOL_MANAGED,\n\t\tbool rendertarget=false,\n\t\tWW3DFormat format=WW3D_FORMAT_UNKNOWN,\n\t\tbool allow_reduction=true\n\t)\n\t: TextureBaseClass(width,height,mip_level_count,pool,rendertarget,allow_reduction), TextureFormat(format), Filter(mip_level_count) { }\n\n\tvirtual TexAssetType Get_Asset_Type() const { return TEX_REGULAR; }',
         '\tTextureClass(IDirect3DBaseTexture8* d3d_texture);',
         'no constructor for cube and volume textures, and no virtual Get_Asset_Type'),
        ('\t\tbool allow_compression=true,\n\t\tbool allow_reduction=true',
         '\t\tbool allow_compression=true',
         'TextureClass(name, full_path, mip, format, allow_compression): 5 arguments, no allow_reduction (0x94B3D0 ends ret 0x14)'),
        ('\t\tbool rendertarget=false,\n\t\tbool allow_reduction=true',
         '\t\tbool rendertarget=false',
         'TextureClass(width, height, format, mip, pool, rendertarget): 6 arguments, no allow_reduction (0x94B240 ends ret 0x18)'),
        ('\tfriend class TextureLoadTaskClass;\n\tfriend class CubeTextureLoadTaskClass;\n\tfriend class VolumeTextureLoadTaskClass;',
         '\tfriend class TextureLoadTaskClass;',
         '(no cube or volume load tasks)'),
        ('\tbool IsProcedural;\n\tbool IsReducible;',
         '\tbool IsProcedural;',
         'no IsReducible: the four bools fill +0xC..+0xF and InactivationTime is at +0x10 (TextureBaseClass constructor 0x94B020; D3DTexture at +0x34, Zero Hour +0x38)'),
        ('\tIDirect3DTexture8* Peek_D3D_Texture() const { return (IDirect3DTexture8*)Peek_D3D_Base_Texture(); }\n\tIDirect3DVolumeTexture8* Peek_D3D_VolumeTexture() const { return (IDirect3DVolumeTexture8*)Peek_D3D_Base_Texture(); }\n\tIDirect3DCubeTexture8* Peek_D3D_CubeTexture() const { return (IDirect3DCubeTexture8*)Peek_D3D_Base_Texture(); }',
         '\tIDirect3DTexture8* Peek_D3D_Texture() const { return (IDirect3DTexture8*)Peek_D3D_Base_Texture(); }',
         '(no cube or volume textures)'),
        ('\tvirtual TextureClass* As_TextureClass() { return NULL; }\n\tvirtual CubeTextureClass* As_CubeTextureClass() { return NULL; }\n\tvirtual VolumeTextureClass* As_VolumeTextureClass() { return NULL; }',
         '\tvirtual TextureClass* As_TextureClass() { return NULL; }',
         "no cube or volume casts: As_TextureClass is the base's last slot (0xB119E4)"),
        ('\tbool Is_Procedural() const { return IsProcedural; }\n\tbool Is_Reducible() const { return IsReducible; } //can texture be reduced in resolution for LOD purposes?',
         '\tbool Is_Procedural() const { return IsProcedural; }',
         'no IsReducible (LOD reduction came with the 08/05/02 redesign)'),
        ('\n\t// Debug utility functions for returning the texture memory usage\n\tvirtual unsigned Get_Texture_Memory_Usage() const=0;',
         '',
         'Get_Texture_Memory_Usage is not a virtual of the base class (the vtable has no slot for it)'),
        ('\tvirtual ~TextureBaseClass();\n\n\tvirtual TexAssetType Get_Asset_Type() const=0;',
         '\tvirtual ~TextureBaseClass();',
         '(no Get_Asset_Type)'),
        ('\t\tbool rendertarget=false,\n\t\tbool reducible=true',
         '\t\tbool rendertarget=false',
         "the constructor takes 5 arguments, no 'reducible' (0x94B020 ends ret 0x14)"),
        ('\t};\n\n\tenum TexAssetType\n\t{\n\t\tTEX_REGULAR,\n\t\tTEX_CUBEMAP,\n\t\tTEX_VOLUME\n\t};',
         '\t};',
         'no TexAssetType: Get_Asset_Type is not virtual in E&B (TextureBaseClass vtable at 0xB119D0 has 6 slots: Delete_This, the destructor, Init, Apply_New_Surface, Apply, As_TextureClass)'),
        ('class TextureLoadTaskClass;\nclass CubeTextureClass;\nclass VolumeTextureClass;',
         'class TextureLoadTaskClass;',
         'no cube or volume texture classes: no vtables for them in the client (0xB119A0: TextureBaseClass, DX8TextureTrackerClass, TextureClass, DX8ZTextureTrackerClass, ZTextureClass, then SurfaceClass), and nothing calls D3DXCreateCubeTexture/D3DXCreateVolumeTexture'),
    ],
    ('ww3d2', 'texture.cpp'): [
        ('}\n\n\n\n/*************************************************************************\n**                             CubeTextureClass\n*************************************************************************/\nCubeTextureClass::CubeTextureClass\n(\n\tunsigned width, \n\tunsigned height, \n\tWW3DFormat format, \n\tMipCountType mip_level_count, \n\tPoolType pool,\n\tbool rendertarget,\n\tbool allow_reduction\n)\n: TextureClass(width, height, format, mip_level_count, pool, rendertarget)\n{\n\tInitialized=true;\n\tIsProcedural=true;\n\tIsReducible=false;\n\n\tswitch (format) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tdefault : break;\n\t}\n\t\t\n\tD3DPOOL d3dpool=(D3DPOOL)0;\n\tswitch(pool)\n\t{\n\tcase POOL_DEFAULT\t\t: d3dpool=D3DPOOL_DEFAULT; break;\n\tcase POOL_MANAGED\t\t: d3dpool=D3DPOOL_MANAGED; break;\n\tcase POOL_SYSTEMMEM\t: d3dpool=D3DPOOL_SYSTEMMEM; break;\n\tdefault: WWASSERT(0);\n\t}\n\n\tPoke_Texture\n\t(\n\t\tDX8Wrapper::_Create_DX8_Cube_Texture\n\t\t(\n\t\t\twidth, \n\t\t\theight, \n\t\t\tformat, \n\t\t\tmip_level_count,\n\t\t\td3dpool,\n\t\t\trendertarget\n\t\t)\n\t);\n\n\tif (pool==POOL_DEFAULT)\n\t{\n\t\tSet_Dirty();\n\t\tDX8TextureTrackerClass *track=new DX8TextureTrackerClass\n\t\t(\n\t\t\twidth, \n\t\t\theight, \n\t\t\tformat, \n\t\t\tmip_level_count,\n\t\t\tthis,\n\t\t\trendertarget\n\t\t);\n\t\tDX8TextureManagerClass::Add(track);\n\t}\n\tLastAccessed=WW3D::Get_Sync_Time();\n}\n\n\n\n// ----------------------------------------------------------------------------\nCubeTextureClass::CubeTextureClass\n(\n\tconst char *name,\n\tconst char *full_path,\n\tMipCountType mip_level_count,\n\tWW3DFormat texture_format,\n\tbool allow_compression,\n\tbool allow_reduction\n)\n:\tTextureClass(0,0,mip_level_count, POOL_MANAGED, false, texture_format)\n{\n\tIsCompressionAllowed=allow_compression;\n\tInactivationTime=DEFAULT_INACTIVATION_TIME;\t\t// Default inactivation time 30 seconds\n\n\tswitch (TextureFormat) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tcase WW3D_FORMAT_U8V8:\t\t// Bumpmap\n\tcase WW3D_FORMAT_L6V5U5:\t// Bumpmap\n\tcase WW3D_FORMAT_X8L8V8U8:\t// Bumpmap\n\t\t// If requesting bumpmap format that isn\'t available we\'ll just return the surface in whatever color\n\t\t// format the texture file is in. (This is illegal case, the format support should always be queried\n\t\t// before creating a bump texture!)\n\t\tif (!DX8Wrapper::Is_Initted() || !DX8Wrapper::Get_Current_Caps()->Support_Texture_Format(TextureFormat)) \n\t\t{\n\t\t\tTextureFormat=WW3D_FORMAT_UNKNOWN;\n\t\t}\n\t\t// If bump format is valid, make sure compression is not allowed so that we don\'t even attempt to load\n\t\t// from a compressed file (quality isn\'t good enough for bump map). Also disable mipmapping.\n\t\telse \n\t\t{\n\t\t\tIsCompressionAllowed=false;\n\t\t\tMipLevelCount=MIP_LEVELS_1;\n\t\t\tFilter.Set_Mip_Mapping(TextureFilterClass::FILTER_TYPE_NONE);\n\t\t}\n\t\tbreak;\n\tdefault:\tbreak;\n\t}\n\n\tWWASSERT_PRINT(name && name[0], "TextureClass CTor: NULL or empty texture name\\n");\n\tint len=strlen(name);\n\tfor (int i=0;i<len;++i) \n\t{\n\t\tif (name[i]==\'+\') \n\t\t{\n\t\t\tIsLightmap=true;\n\n\t\t\t// Set bilinear filtering for lightmaps (they are very stretched and\n\t\t\t// low detail so we don\'t care for anisotropic or trilinear filtering...)\n\t\t\tFilter.Set_Min_Filter(TextureFilterClass::FILTER_TYPE_FAST);\n\t\t\tFilter.Set_Mag_Filter(TextureFilterClass::FILTER_TYPE_FAST);\n\t\t\tif (mip_level_count!=MIP_LEVELS_1) Filter.Set_Mip_Mapping(TextureFilterClass::FILTER_TYPE_FAST);\n\t\t\tbreak;\n\t\t}\n\t}\n\tSet_Texture_Name(name);\n\tSet_Full_Path(full_path);\n\tWWASSERT(name[0]!=\'\\0\');\n\tif (!WW3D::Is_Texturing_Enabled()) \n\t{\n\t\tInitialized=true;\n\t\tPoke_Texture(NULL);\n\t}\n\n\t// Find original size from the thumbnail (but don\'t create thumbnail texture yet!)\n\tThumbnailClass* thumb=ThumbnailManagerClass::Peek_Thumbnail_Instance_From_Any_Manager(Get_Full_Path());\n\tif (thumb) \n\t{\n\t\tWidth=thumb->Get_Original_Texture_Width();\n\t\tHeight=thumb->Get_Original_Texture_Height();\n \t\tif (MipLevelCount!=MIP_LEVELS_1) {\n \t\t\tMipLevelCount=(MipCountType)thumb->Get_Original_Texture_Mip_Level_Count();\n \t\t}\n\t}\n\n\tLastAccessed=WW3D::Get_Sync_Time();\n\n\t// If the thumbnails are not enabled, init the texture at this point to avoid stalling when the\n\t// mesh is rendered.\n\tif (!WW3D::Get_Thumbnail_Enabled()) \n\t{\n\t\tif (TextureLoader::Is_DX8_Thread()) \n\t\t{\n\t\t\tInit();\n\t\t}\n\t}\n}\n\n// don\'t know if these are needed\n#if 0\n// ----------------------------------------------------------------------------\nCubeTextureClass::CubeTextureClass\n(\n\tSurfaceClass *surface, \n\tMipCountType mip_level_count\n)\n:\tTextureClass(0,0,mip_level_count, POOL_MANAGED, false, surface->Get_Surface_Format())\n{\n\tIsProcedural=true;\n\tInitialized=true;\n\tIsReducible=false;\n\n\tSurfaceClass::SurfaceDescription sd;\n\tsurface->Get_Description(sd);\n\tWidth=sd.Width;\n\tHeight=sd.Height;\n\tswitch (sd.Format) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tdefault: break;\n\t}\n\n\tPoke_Texture\n\t(\n\t\tDX8Wrapper::_Create_DX8_Cube_Texture\n\t\t(\n\t\t\tsurface->Peek_D3D_Surface(), \n\t\t\tmip_level_count\n\t\t)\n\t);\n\tLastAccessed=WW3D::Get_Sync_Time();\n}\n\n// ----------------------------------------------------------------------------\nCubeTextureClass::CubeTextureClass(IDirect3DBaseTexture8* d3d_texture)\n:\tTextureBaseClass\n\t(\n\t\t0,\n\t\t0,\n\t\t((MipCountType)d3d_texture->GetLevelCount())\n\t),\n\tFilter((MipCountType)d3d_texture->GetLevelCount())\n{\n\tInitialized=true;\n\tIsProcedural=true;\n\tIsReducible=false;\n\n\tPeek_Texture()->AddRef();\n\tIDirect3DSurface8* surface;\n\tDX8_ErrorCode(Peek_D3D_Texture()->GetSurfaceLevel(0,&surface));\n\tD3DSURFACE_DESC d3d_desc;\n\t::ZeroMemory(&d3d_desc, sizeof(D3DSURFACE_DESC));\n\tDX8_ErrorCode(surface->GetDesc(&d3d_desc));\n\tWidth=d3d_desc.Width;\n\tHeight=d3d_desc.Height;\n\tTextureFormat=D3DFormat_To_WW3DFormat(d3d_desc.Format);\n\tswitch (TextureFormat) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tdefault: break;\n\t}\n\n\tLastAccessed=WW3D::Get_Sync_Time();\n}\n#endif\n\n//**********************************************************************************************\n//! Apply new surface to texture\n/*! \n*/\nvoid CubeTextureClass::Apply_New_Surface\n(\n\tIDirect3DBaseTexture8* d3d_texture,\n\tbool initialized,\n\tbool disable_auto_invalidation\n)\n{\n\tIDirect3DBaseTexture8* d3d_tex=Peek_D3D_Base_Texture();\n\n\tif (d3d_tex) d3d_tex->Release();\n\n\tPoke_Texture(d3d_texture);//TextureLoadTask->Peek_D3D_Texture();\n\td3d_texture->AddRef();\n\n\tif (initialized) Initialized=true;\n\tif (disable_auto_invalidation) InactivationTime = 0;\n\n\tWWASSERT(d3d_texture);\n\tD3DSURFACE_DESC d3d_desc;\n\t::ZeroMemory(&d3d_desc, sizeof(D3DSURFACE_DESC));\n\tDX8_ErrorCode(Peek_D3D_CubeTexture()->GetLevelDesc(0,&d3d_desc));\n\n\tif (initialized) \n\t{\n\t\tTextureFormat=D3DFormat_To_WW3DFormat(d3d_desc.Format);\n\t\tWidth=d3d_desc.Width;\n\t\tHeight=d3d_desc.Height;\n\t}\n}\n\n\n/*************************************************************************\n**                             VolumeTextureClass\n*************************************************************************/\nVolumeTextureClass::VolumeTextureClass\n(\n\tunsigned width, \n\tunsigned height, \n\tunsigned depth,\n\tWW3DFormat format, \n\tMipCountType mip_level_count, \n\tPoolType pool,\n\tbool rendertarget,\n\tbool allow_reduction\n)\n: TextureClass(width, height, format, mip_level_count, pool, rendertarget),\n  Depth(depth)\n{\n\tInitialized=true;\n\tIsProcedural=true;\n\tIsReducible=false;\n\n\tswitch (format) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tdefault : break;\n\t}\n\t\t\n\tD3DPOOL d3dpool=(D3DPOOL)0;\n\tswitch(pool)\n\t{\n\tcase POOL_DEFAULT\t\t: d3dpool=D3DPOOL_DEFAULT; break;\n\tcase POOL_MANAGED\t\t: d3dpool=D3DPOOL_MANAGED; break;\n\tcase POOL_SYSTEMMEM\t: d3dpool=D3DPOOL_SYSTEMMEM; break;\n\tdefault: WWASSERT(0);\n\t}\n\n\tPoke_Texture\n\t(\n\t\tDX8Wrapper::_Create_DX8_Volume_Texture\n\t\t(\n\t\t\twidth, \n\t\t\theight, \n\t\t\tdepth,\n\t\t\tformat, \n\t\t\tmip_level_count,\n\t\t\td3dpool\n\t\t)\n\t);\n\n\tif (pool==POOL_DEFAULT)\n\t{\n\t\tSet_Dirty();\n\t\tDX8TextureTrackerClass *track=new DX8TextureTrackerClass\n\t\t(\n\t\t\twidth, \n\t\t\theight, \n\t\t\tformat, \n\t\t\tmip_level_count,\n\t\t\tthis,\n\t\t\trendertarget\n\t\t);\n\t\tDX8TextureManagerClass::Add(track);\n\t}\n\tLastAccessed=WW3D::Get_Sync_Time();\n}\n\n\n\n// ----------------------------------------------------------------------------\nVolumeTextureClass::VolumeTextureClass\n(\n\tconst char *name,\n\tconst char *full_path,\n\tMipCountType mip_level_count,\n\tWW3DFormat texture_format,\n\tbool allow_compression,\n\tbool allow_reduction\n)\n:\tTextureClass(0,0,mip_level_count, POOL_MANAGED, false, texture_format),\n\tDepth(0)\n{\n\tIsCompressionAllowed=allow_compression;\n\tInactivationTime=DEFAULT_INACTIVATION_TIME;\t\t// Default inactivation time 30 seconds\n\n\tswitch (TextureFormat) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tcase WW3D_FORMAT_U8V8:\t\t// Bumpmap\n\tcase WW3D_FORMAT_L6V5U5:\t// Bumpmap\n\tcase WW3D_FORMAT_X8L8V8U8:\t// Bumpmap\n\t\t// If requesting bumpmap format that isn\'t available we\'ll just return the surface in whatever color\n\t\t// format the texture file is in. (This is illegal case, the format support should always be queried\n\t\t// before creating a bump texture!)\n\t\tif (!DX8Wrapper::Is_Initted() || !DX8Wrapper::Get_Current_Caps()->Support_Texture_Format(TextureFormat)) \n\t\t{\n\t\t\tTextureFormat=WW3D_FORMAT_UNKNOWN;\n\t\t}\n\t\t// If bump format is valid, make sure compression is not allowed so that we don\'t even attempt to load\n\t\t// from a compressed file (quality isn\'t good enough for bump map). Also disable mipmapping.\n\t\telse \n\t\t{\n\t\t\tIsCompressionAllowed=false;\n\t\t\tMipLevelCount=MIP_LEVELS_1;\n\t\t\tFilter.Set_Mip_Mapping(TextureFilterClass::FILTER_TYPE_NONE);\n\t\t}\n\t\tbreak;\n\tdefault:\tbreak;\n\t}\n\n\tWWASSERT_PRINT(name && name[0], "TextureClass CTor: NULL or empty texture name\\n");\n\tint len=strlen(name);\n\tfor (int i=0;i<len;++i) \n\t{\n\t\tif (name[i]==\'+\') \n\t\t{\n\t\t\tIsLightmap=true;\n\n\t\t\t// Set bilinear filtering for lightmaps (they are very stretched and\n\t\t\t// low detail so we don\'t care for anisotropic or trilinear filtering...)\n\t\t\tFilter.Set_Min_Filter(TextureFilterClass::FILTER_TYPE_FAST);\n\t\t\tFilter.Set_Mag_Filter(TextureFilterClass::FILTER_TYPE_FAST);\n\t\t\tif (mip_level_count!=MIP_LEVELS_1) Filter.Set_Mip_Mapping(TextureFilterClass::FILTER_TYPE_FAST);\n\t\t\tbreak;\n\t\t}\n\t}\n\tSet_Texture_Name(name);\n\tSet_Full_Path(full_path);\n\tWWASSERT(name[0]!=\'\\0\');\n\tif (!WW3D::Is_Texturing_Enabled()) \n\t{\n\t\tInitialized=true;\n\t\tPoke_Texture(NULL);\n\t}\n\n\t// Find original size from the thumbnail (but don\'t create thumbnail texture yet!)\n\tThumbnailClass* thumb=ThumbnailManagerClass::Peek_Thumbnail_Instance_From_Any_Manager(Get_Full_Path());\n\tif (thumb) \n\t{\n\t\tWidth=thumb->Get_Original_Texture_Width();\n\t\tHeight=thumb->Get_Original_Texture_Height();\n \t\tif (MipLevelCount!=MIP_LEVELS_1) {\n \t\t\tMipLevelCount=(MipCountType)thumb->Get_Original_Texture_Mip_Level_Count();\n \t\t}\n\t}\n\n\tLastAccessed=WW3D::Get_Sync_Time();\n\n\t// If the thumbnails are not enabled, init the texture at this point to avoid stalling when the\n\t// mesh is rendered.\n\tif (!WW3D::Get_Thumbnail_Enabled()) \n\t{\n\t\tif (TextureLoader::Is_DX8_Thread()) \n\t\t{\n\t\t\tInit();\n\t\t}\n\t}\n}\n\n// don\'t know if these are needed\n#if 0\n// ----------------------------------------------------------------------------\nCubeTextureClass::CubeTextureClass\n(\n\tSurfaceClass *surface, \n\tMipCountType mip_level_count\n)\n:\tTextureClass(0,0,mip_level_count, POOL_MANAGED, false, surface->Get_Surface_Format())\n{\n\tIsProcedural=true;\n\tInitialized=true;\n\tIsReducible=false;\n\n\tSurfaceClass::SurfaceDescription sd;\n\tsurface->Get_Description(sd);\n\tWidth=sd.Width;\n\tHeight=sd.Height;\n\tswitch (sd.Format) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tdefault: break;\n\t}\n\n\tPoke_Texture\n\t(\n\t\tDX8Wrapper::_Create_DX8_Cube_Texture\n\t\t(\n\t\t\tsurface->Peek_D3D_Surface(), \n\t\t\tmip_level_count\n\t\t)\n\t);\n\tLastAccessed=WW3D::Get_Sync_Time();\n}\n\n// ----------------------------------------------------------------------------\nCubeTextureClass::CubeTextureClass(IDirect3DBaseTexture8* d3d_texture)\n:\tTextureBaseClass\n\t(\n\t\t0,\n\t\t0,\n\t\t((MipCountType)d3d_texture->GetLevelCount())\n\t),\n\tFilter((MipCountType)d3d_texture->GetLevelCount())\n{\n\tInitialized=true;\n\tIsProcedural=true;\n\tIsReducible=false;\n\n\tPeek_Texture()->AddRef();\n\tIDirect3DSurface8* surface;\n\tDX8_ErrorCode(Peek_D3D_Texture()->GetSurfaceLevel(0,&surface));\n\tD3DSURFACE_DESC d3d_desc;\n\t::ZeroMemory(&d3d_desc, sizeof(D3DSURFACE_DESC));\n\tDX8_ErrorCode(surface->GetDesc(&d3d_desc));\n\tWidth=d3d_desc.Width;\n\tHeight=d3d_desc.Height;\n\tTextureFormat=D3DFormat_To_WW3DFormat(d3d_desc.Format);\n\tswitch (TextureFormat) \n\t{\n\tcase WW3D_FORMAT_DXT1:\n\tcase WW3D_FORMAT_DXT2:\n\tcase WW3D_FORMAT_DXT3:\n\tcase WW3D_FORMAT_DXT4:\n\tcase WW3D_FORMAT_DXT5:\n\t\tIsCompressionAllowed=true;\n\t\tbreak;\n\tdefault: break;\n\t}\n\n\tLastAccessed=WW3D::Get_Sync_Time();\n}\n#endif\n\n\n\n\n//**********************************************************************************************\n//! Apply new surface to texture\n/*! \n*/\nvoid VolumeTextureClass::Apply_New_Surface\n(\n\tIDirect3DBaseTexture8* d3d_texture,\n\tbool initialized,\n\tbool disable_auto_invalidation\n)\n{\n\tIDirect3DBaseTexture8* d3d_tex=Peek_D3D_Base_Texture();\n\n\tif (d3d_tex) d3d_tex->Release();\n\n\tPoke_Texture(d3d_texture);//TextureLoadTask->Peek_D3D_Texture();\n\td3d_texture->AddRef();\n\n\tif (initialized) Initialized=true;\n\tif (disable_auto_invalidation) InactivationTime = 0;\n\n\tWWASSERT(d3d_texture);\n\tD3DVOLUME_DESC d3d_desc;\n\t::ZeroMemory(&d3d_desc, sizeof(D3DVOLUME_DESC));\n\n\tDX8_ErrorCode(Peek_D3D_VolumeTexture()->GetLevelDesc(0,&d3d_desc));\n\n\tif (initialized)\n\t{\n\t\tTextureFormat=D3DFormat_To_WW3DFormat(d3d_desc.Format);\n\t\tWidth=d3d_desc.Width;\n\t\tHeight=d3d_desc.Height;\n\t\tDepth=d3d_desc.Depth;\n\t}\n}',
         '}',
         'no CubeTextureClass or VolumeTextureClass (see texture.h)'),
        ('\t}\n\tInitialized=true;\n\tIsProcedural=true;\n\tIsReducible=false;\n\n\tLastAccessed=WW3D::Get_Sync_Time();',
         '\t}\n\tInitialized=true;\n\tIsProcedural=true;\n\n\tLastAccessed=WW3D::Get_Sync_Time();',
         'no IsReducible (see texture.h)'),
        ('\tInitialized=true;\n\tIsProcedural=true;\n\tIsReducible=false;\n\t',
         '\tInitialized=true;\n\tIsProcedural=true;\n\t',
         'no IsReducible (see texture.h)'),
        ('\tInitialized=true;\n\tIsReducible=false;',
         '\tInitialized=true;',
         'no IsReducible (see texture.h)'),
        ('\tInactivationTime=DEFAULT_INACTIVATION_TIME;\t\t// Default inactivation time 30 seconds\n\tIsReducible=allow_reduction;',
         '\tInactivationTime=DEFAULT_INACTIVATION_TIME;\t\t// Default inactivation time 30 seconds',
         'no IsReducible (see texture.h)'),
        ('\tbool allow_compression,\n\tbool allow_reduction',
         '\tbool allow_compression',
         'no allow_reduction (0x94B3D0 ends ret 0x14)'),
        ('\tIsProcedural=true;\n\tIsReducible=false;',
         '\tIsProcedural=true;',
         'no IsReducible (see texture.h): the constructor (0x94B240) sets only Initialized (+0xC) and IsProcedural (+0xF)'),
        ('\tbool rendertarget,\n\tbool allow_reduction\n)\n:\tTextureBaseClass(width, height, mip_level_count, pool, rendertarget,allow_reduction),',
         '\tbool rendertarget\n)\n:\tTextureBaseClass(width, height, mip_level_count, pool, rendertarget),',
         'no allow_reduction (0x94B240 ends ret 0x18)'),
        ("\tif (MipLevelCount==MIP_LEVELS_1) return 0;\n\tif (Width <= 32 || Height <= 32) return 0;\n\n\tint reduction=WW3D::Get_Texture_Reduction();\n\n\t// 'large texture extra reduction' causes textures above 256x256 to be reduced one more step.\n\tif (WW3D::Is_Large_Texture_Extra_Reduction_Enabled() && (Width > 256 || Height > 256)) {\n\t\treduction++;\n\t}\n\tif (MipLevelCount && reduction>MipLevelCount) {\n\t\treduction=MipLevelCount;\n\t}\n\treturn reduction;",
         '\tif (MipLevelCount==MIP_LEVELS_1) return 0;\n\n\tint reduction=WW3D::Get_Texture_Reduction();\n\tif (MipLevelCount && reduction>MipLevelCount) {\n\t\treduction=MipLevelCount;\n\t}\n\treturn reduction;',
         "Renegade's TextureClass::Get_Reduction body: no minimum size and no 'large texture extra reduction' (WW3D::Is_Large_Texture_Extra_Reduction_Enabled is Zero Hour's; E&B's WW3D is Renegade's)"),
        ('\tIsProcedural(false),\n\tIsReducible(reducible),',
         '\tIsProcedural(false),',
         'no IsReducible (see texture.h)'),
        ('\tbool rendertarget,\n\tbool reducible',
         '\tbool rendertarget',
         "the base constructor has no 'reducible' argument (0x94B020 ends ret 0x14)"),
        ('\t\t// Get the current texture\n\t\tTextureBaseClass* tex=ite.Peek_Value();\n\t\tif (tex->Is_Procedural()) ',
         '\t\t// Get the current texture\n\t\tTextureClass* tex=ite.Peek_Value();\n\t\tif (tex->Is_Procedural()) ',
         "TextureClass* (Renegade's form): Get_Texture_Memory_Usage is TextureClass's, not a virtual of the base"),
        ('\t\t// Get the current texture\n\t\tTextureBaseClass* tex=ite.Peek_Value();\n\t\tif (tex->Is_Lightmap()) ',
         '\t\t// Get the current texture\n\t\tTextureClass* tex=ite.Peek_Value();\n\t\tif (tex->Is_Lightmap()) ',
         "TextureClass* (Renegade's form): Get_Texture_Memory_Usage is TextureClass's, not a virtual of the base"),
        ('\t\t// Get the current texture\n\t\tTextureBaseClass* tex=ite.Peek_Value();\n\t\ttotal_texture_size+=tex->Get_Texture_Memory_Usage();',
         '\t\t// Get the current texture\n\t\tTextureClass* tex=ite.Peek_Value();\n\t\ttotal_texture_size+=tex->Get_Texture_Memory_Usage();',
         "TextureClass* (Renegade's form): Get_Texture_Memory_Usage is TextureClass's, not a virtual of the base"),
        ('\tfor (ite.First ();!ite.Is_Done();ite.Next ()) \n\t{\n\t\t// Get the current texture\n\t\tTextureBaseClass* tex=ite.Peek_Value();\n\t\tif (!tex->Initialized) \n\t\t{\n\t\t\ttotal_locked_surface_size+=tex->Get_Texture_Memory_Usage();',
         '\tfor (ite.First ();!ite.Is_Done();ite.Next ()) \n\t{\n\t\t// Get the current texture\n\t\tTextureClass* tex=ite.Peek_Value();\n\t\tif (!tex->Initialized) \n\t\t{\n\t\t\ttotal_locked_surface_size+=tex->Get_Texture_Memory_Usage();',
         "TextureClass* (Renegade's form): Get_Texture_Memory_Usage is TextureClass's, not a virtual of the base"),
    ],
    ('ww3d2', 'textureloader.h'): [
        ('\t\tunsigned char *\t\tLockedSurfacePtr[TextureClass::MIP_LEVELS_MAX];\n\t\tunsigned\tint\t\t\tLockedSurfacePitch[TextureClass::MIP_LEVELS_MAX];',
         '\t\tunsigned char *\t\tLockedSurfacePtr[MIP_LEVELS_MAX];\n\t\tunsigned\tint\t\t\tLockedSurfacePitch[MIP_LEVELS_MAX];',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\t\tTextureClass*\t\t\tTexture;',
         '\t\tTextureBaseClass*\t\t\tTexture;',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\t\tTextureClass *\t\t\tPeek_Texture\t\t\t\t(void)\t\t\t\t{ return Texture;\t\t\t}',
         '\t\tTextureBaseClass *\t\t\tPeek_Texture\t\t\t\t(void)\t\t\t\t{ return Texture;\t\t\t}',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\t\tvoid\t\t\t\t\t\tInit\t\t\t\t\t\t\t(TextureClass *tc, TaskType type, PriorityType priority);',
         '\t\tvoid\t\t\t\t\t\tInit\t\t\t\t\t\t\t(TextureBaseClass *tc, TaskType type, PriorityType priority);',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\t\tstatic TextureLoadTaskClass *\tCreate\t\t\t(TextureClass *tc, TaskType type, PriorityType priority);',
         '\t\tstatic TextureLoadTaskClass *\tCreate\t\t\t(TextureBaseClass *tc, TaskType type, PriorityType priority);',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\tstatic void Load_Thumbnail\t\t\t\t\t\t(TextureClass *tc);',
         '\tstatic void Load_Thumbnail\t\t\t\t\t\t(TextureBaseClass *tc);',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\tstatic void Request_Foreground_Loading(TextureClass* tc);',
         '\tstatic void Request_Foreground_Loading(TextureBaseClass* tc);',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\tstatic void Request_Background_Loading(TextureClass* tc);',
         '\tstatic void Request_Background_Loading(TextureBaseClass* tc);',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\tstatic void\tRequest_Thumbnail(TextureClass* tc);',
         '\tstatic void\tRequest_Thumbnail(TextureBaseClass* tc);',
         "Zero Hour's 06/27/02 'texture class abstraction': the loader works on TextureBaseClass (the rest of E&B's loader is Renegade's: its task lists and Validate_Texture_Size match Renegade's bytes, and its load task has no vtable, unlike Zero Hour's 08/05/02 cube and volume tasks)"),
        ('\t\tconst StringClass& filename);',
         '\t\tconst StringClass& filename,const Vector3& hsv_shift);',
         "Load_Thumbnail takes the HSV shift, as Zero Hour's (byte-exact at 0x948DC0)"),
        ('\t\tunsigned\tint\t\t\tReduction;',
         '\t\tunsigned\tint\t\t\tReduction;\n\t\tVector3\t\t\t\t\tHSVShift;',
         "the load task carries the texture's HSV shift, as Zero Hour's (TextureBaseClass has HSVShift at +0x20, and Load_Thumbnail takes it)"),
    ],
    ('ww3d2', 'textureloader.cpp'): [
        ('\tTextureClass::MipCountType mip_level_count,',
         '\tMipCountType mip_level_count,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\t\t(TextureClass::MipCountType)mips);',
         '\t\t(MipCountType)mips);',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\t\tdest_format,\n\t\tTextureClass::MIP_LEVELS_ALL,\n#ifdef USE_MANAGED_TEXTURES',
         '\t\tdest_format,\n\t\tMIP_LEVELS_ALL,\n#ifdef USE_MANAGED_TEXTURES',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\t\tTextureClass::MIP_LEVELS_ALL,',
         '\t\tMIP_LEVELS_ALL,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\t\tIDirect3DTexture8* comp_tex=Load_Compressed_Texture(filename,0,TextureClass::MIP_LEVELS_1,WW3D_FORMAT_UNKNOWN);',
         '\t\tIDirect3DTexture8* comp_tex=Load_Compressed_Texture(filename,0,MIP_LEVELS_1,WW3D_FORMAT_UNKNOWN);',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tif (tc->Peek_DX8_Texture()) {',
         '\tif (tc->Peek_D3D_Texture()) {',
         "Zero Hour's TextureBaseClass::Peek_D3D_Texture (Renegade: Peek_DX8_Texture)"),
        ('\t// is done by Init() and Deinit().\n\n\tfor (int i = 0; i < TextureClass::MIP_LEVELS_MAX; ++i) {\n\t\tLockedSurfacePtr[i]\t\t= NULL;\n\t\tLockedSurfacePitch[i]\t= 0;',
         '\t// is done by Init() and Deinit().\n\n\tfor (int i = 0; i < MIP_LEVELS_MAX; ++i) {\n\t\tLockedSurfacePtr[i]\t\t= NULL;\n\t\tLockedSurfacePitch[i]\t= 0;',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\n\tfor (int i = 0; i < TextureClass::MIP_LEVELS_MAX; ++i) {\n\t\tLockedSurfacePtr[i]\t\t= NULL;',
         '\n\tfor (int i = 0; i < MIP_LEVELS_MAX; ++i) {\n\t\tLockedSurfacePtr[i]\t\t= NULL;',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\tfor (int i = 0; i < TextureClass::MIP_LEVELS_MAX; ++i) {',
         '\tfor (int i = 0; i < MIP_LEVELS_MAX; ++i) {',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('\t\t(TextureClass::MipCountType)mip_level_count,',
         '\t\t(MipCountType)mip_level_count,',
         "MipCountType and MIP_LEVELS_* are global in Zero Hour's texture API (ww3dformat.h), as in Zero Hour's copy of this file"),
        ('{\n\tThumbnailClass* thumb=NULL;\n\tThumbnailManagerClass* thumb_man=ThumbnailManagerClass::Peek_List().Head();\n\twhile (thumb_man) {\n\t\tthumb=thumb_man->Peek_Thumbnail_Instance(filename);\n\t\tif (thumb) break;\n\t\tthumb_man=thumb_man->Succ();\n\t}\n\tif (!thumb) {',
         '{\n\tThumbnailClass* thumb=ThumbnailManagerClass::Peek_Thumbnail_Instance_From_Any_Manager(filename);\n\tif (!thumb) {',
         "E&B's thumbnails are Zero Hour's: one lookup across all managers (the client calls it, 0x91AD40, from Load_Thumbnail, the TextureClass constructor and here)"),
        ('void TextureLoadTaskClass::Init(TextureClass* tc, TaskType type, PriorityType priority)',
         'void TextureLoadTaskClass::Init(TextureBaseClass* tc, TaskType type, PriorityType priority)',
         "the loader works on TextureBaseClass (Zero Hour's 06/27/02 texture class abstraction; see textureloader.h)"),
        ('TextureLoadTaskClass *TextureLoadTaskClass::Create(TextureClass *tc, TaskType type, PriorityType priority)',
         'TextureLoadTaskClass *TextureLoadTaskClass::Create(TextureBaseClass *tc, TaskType type, PriorityType priority)',
         "the loader works on TextureBaseClass (Zero Hour's 06/27/02 texture class abstraction; see textureloader.h)"),
        ('void TextureLoader::Load_Thumbnail(TextureClass *tc)',
         'void TextureLoader::Load_Thumbnail(TextureBaseClass *tc)',
         "the loader works on TextureBaseClass (Zero Hour's 06/27/02 texture class abstraction; see textureloader.h)"),
        ('void TextureLoader::Request_Foreground_Loading(TextureClass *tc)',
         'void TextureLoader::Request_Foreground_Loading(TextureBaseClass *tc)',
         "the loader works on TextureBaseClass (Zero Hour's 06/27/02 texture class abstraction; see textureloader.h)"),
        ('void TextureLoader::Request_Background_Loading(TextureClass *tc)',
         'void TextureLoader::Request_Background_Loading(TextureBaseClass *tc)',
         "the loader works on TextureBaseClass (Zero Hour's 06/27/02 texture class abstraction; see textureloader.h)"),
        ('void TextureLoader::Request_Thumbnail(TextureClass *tc)',
         'void TextureLoader::Request_Thumbnail(TextureBaseClass *tc)',
         "the loader works on TextureBaseClass (Zero Hour's 06/27/02 texture class abstraction; see textureloader.h)"),
        ('IDirect3DTexture8* TextureLoader::Load_Thumbnail(const StringClass& filename)//,WW3DFormat texture_format)\n{\n\tWWASSERT(Is_DX8_Thread());\n\n\tThumbnailClass* thumb=NULL;\n\tThumbnailManagerClass* thumb_man=ThumbnailManagerClass::Peek_List().Head();\n\twhile (thumb_man) {\n\t\tthumb=thumb_man->Peek_Thumbnail_Instance(filename);\n\t\tif (thumb) break;\n\t\tthumb_man=thumb_man->Succ();\n\t}\n\n\t// If no thumb is found return a missing texture\n\tif (!thumb) {\n\t\treturn MissingTexture::_Get_Missing_Texture();\n\t}\n\n\tWWASSERT(thumb->Get_Format()==WW3D_FORMAT_A4R4G4B4);\n\tunsigned src_pitch=thumb->Get_Width()*2;\t// Thumbs are always 16 bits\n\tWW3DFormat dest_format;\n\tWW3DFormat texture_format=WW3D_FORMAT_UNKNOWN;\n\tif (texture_format==WW3D_FORMAT_UNKNOWN) {\n\t\tdest_format=Get_Valid_Texture_Format(WW3D_FORMAT_A4R4G4B4,false); // no compressed formats please\n\t}\n\telse {\n\t\tdest_format=Get_Valid_Texture_Format(texture_format,false);\t// no compressed formats please\n\t\tWWASSERT(dest_format==texture_format);\n\t}\n\n\tIDirect3DTexture8* sysmem_texture = DX8Wrapper::_Create_DX8_Texture(\n\t\tthumb->Get_Width(),\n\t\tthumb->Get_Height(),\n\t\tdest_format,\n\t\tMIP_LEVELS_ALL,\n#ifdef USE_MANAGED_TEXTURES\n\t\tD3DPOOL_MANAGED);\n#else\n\t\tD3DPOOL_SYSTEMMEM);\n#endif\n\n\tunsigned level=0;\n\tD3DLOCKED_RECT locked_rects[12];\n\tWWASSERT(sysmem_texture->GetLevelCount()<=12);\n\n\t// Lock all surfaces\n\tfor (level=0;level<sysmem_texture->GetLevelCount();++level) {\n\t\tDX8_ErrorCode(\n\t\t\tsysmem_texture->LockRect(\n\t\t\t\tlevel,\n\t\t\t\t&locked_rects[level],\n\t\t\t\tNULL,\n\t\t\t\t0));\n\t}\n\n\tunsigned char* src_surface=thumb->Peek_Bitmap();\n\tWW3DFormat src_format=thumb->Get_Format();\n\tunsigned width=thumb->Get_Width();\n\tunsigned height=thumb->Get_Height();\n\n\tfor (level=0;level<sysmem_texture->GetLevelCount()-1;++level) {\n\t\tBitmapHandlerClass::Copy_Image_Generate_Mipmap(\n\t\t\twidth,\n\t\t\theight,\n\t\t\t(unsigned char*)locked_rects[level].pBits,\n\t\t\tlocked_rects[level].Pitch,\n\t\t\tdest_format,\n\t\t\tsrc_surface,\n\t\t\tsrc_pitch,\n\t\t\tsrc_format,\n\t\t\t(unsigned char*)locked_rects[level+1].pBits,\t// mipmap\n\t\t\tlocked_rects[level+1].Pitch);// mipmap\n\n\t\tsrc_format=dest_format;\n\t\tsrc_surface=(unsigned char*)locked_rects[level].pBits;\n\t\tsrc_pitch=locked_rects[level].Pitch;\n\t\twidth>>=1;\n\t\theight>>=1;\n\t}\n\n\t// Unlock all surfaces\n\tfor (level=0;level<sysmem_texture->GetLevelCount();++level) {\n\t\tDX8_ErrorCode(sysmem_texture->UnlockRect(level));\n\t}\n#ifdef USE_MANAGED_TEXTURES\n\treturn sysmem_texture;\n#else\n\tIDirect3DTexture8* d3d_texture = DX8Wrapper::_Create_DX8_Texture(\n\t\tthumb->Get_Width(),\n\t\tthumb->Get_Height(),\n\t\tdest_format,\n\t\tMIP_LEVELS_ALL,\n\t\tD3DPOOL_DEFAULT);\n\tDX8CALL(UpdateTexture(sysmem_texture,d3d_texture));\n\tsysmem_texture->Release();\n\n\tWWDEBUG_SAY(("Created non-managed texture (%s)\\n",filename));\n\treturn d3d_texture;\n#endif\n}',
         'IDirect3DTexture8* TextureLoader::Load_Thumbnail(const StringClass& filename, const Vector3& hsv_shift)//,WW3DFormat texture_format)\n{\n\tWWASSERT(Is_DX8_Thread());\n\n\tThumbnailClass* thumb=NULL;\n\tthumb=ThumbnailManagerClass::Peek_Thumbnail_Instance_From_Any_Manager(filename);\n\n\t// If no thumb is found return a missing texture\n\tif (!thumb) {\n\t\treturn MissingTexture::_Get_Missing_Texture();\n\t}\n\n\tWWASSERT(thumb->Get_Format()==WW3D_FORMAT_A4R4G4B4);\n\tunsigned src_pitch=thumb->Get_Width()*2;\t// Thumbs are always 16 bits\n\tWW3DFormat dest_format;\n\tWW3DFormat texture_format=WW3D_FORMAT_UNKNOWN;\n\tif (texture_format==WW3D_FORMAT_UNKNOWN) {\n\t\tdest_format=Get_Valid_Texture_Format(WW3D_FORMAT_A4R4G4B4,false); // no compressed formats please\n\t}\n\telse {\n\t\tdest_format=Get_Valid_Texture_Format(texture_format,false);\t// no compressed formats please\n\t\tWWASSERT(dest_format==texture_format);\n\t}\n\n\tIDirect3DTexture8* sysmem_texture = DX8Wrapper::_Create_DX8_Texture(\n\t\tthumb->Get_Width(),\n\t\tthumb->Get_Height(),\n\t\tdest_format,\n\t\tMIP_LEVELS_ALL,\n#ifdef USE_MANAGED_TEXTURES\n\t\tD3DPOOL_MANAGED);\n#else\n\t\tD3DPOOL_SYSTEMMEM);\n#endif\n\n\tunsigned level=0;\n\tD3DLOCKED_RECT locked_rects[12];\n\tWWASSERT(sysmem_texture->GetLevelCount()<=12);\n\n\t// Lock all surfaces\n\tfor (level=0;level<sysmem_texture->GetLevelCount();++level) {\n\t\tDX8_ErrorCode(\n\t\t\tsysmem_texture->LockRect(\n\t\t\t\tlevel,\n\t\t\t\t&locked_rects[level],\n\t\t\t\tNULL,\n\t\t\t\t0));\n\t}\n\n\tunsigned char* src_surface=thumb->Peek_Bitmap();\n\tWW3DFormat src_format=thumb->Get_Format();\n\tunsigned width=thumb->Get_Width();\n\tunsigned height=thumb->Get_Height();\n\n\tVector3 hsv=hsv_shift;\n\tfor (level=0;level<sysmem_texture->GetLevelCount()-1;++level) {\n\t\tBitmapHandlerClass::Copy_Image_Generate_Mipmap(\n\t\t\twidth,\n\t\t\theight,\n\t\t\t(unsigned char*)locked_rects[level].pBits,\n\t\t\tlocked_rects[level].Pitch,\n\t\t\tdest_format,\n\t\t\tsrc_surface,\n\t\t\tsrc_pitch,\n\t\t\tsrc_format,\n\t\t\t(unsigned char*)locked_rects[level+1].pBits,\t// mipmap\n\t\t\tlocked_rects[level+1].Pitch,\n\t\t\thsv);\n\t\thsv=Vector3(0.0f,0.0f,0.0f);\t// Only do the shift for the first level, as the mipmaps are based on it.\n\n\t\tsrc_format=dest_format;\n\t\tsrc_surface=(unsigned char*)locked_rects[level].pBits;\n\t\tsrc_pitch=locked_rects[level].Pitch;\n\t\twidth>>=1;\n\t\theight>>=1;\n\t}\n\n\t// Unlock all surfaces\n\tfor (level=0;level<sysmem_texture->GetLevelCount();++level) {\n\t\tDX8_ErrorCode(sysmem_texture->UnlockRect(level));\n\t}\n#ifdef USE_MANAGED_TEXTURES\n\treturn sysmem_texture;\n#else\n\tIDirect3DTexture8* d3d_texture = DX8Wrapper::_Create_DX8_Texture(\n\t\tthumb->Get_Width(),\n\t\tthumb->Get_Height(),\n\t\tdest_format,\n\t\tTextureBaseClass::MIP_LEVELS_ALL,\n\t\tD3DPOOL_DEFAULT);\n\tDX8CALL(UpdateTexture(sysmem_texture,d3d_texture));\n\tsysmem_texture->Release();\n\n\tWWDEBUG_SAY(("Created non-managed texture (%s)\\n",filename));\n\treturn d3d_texture;\n#endif\n}',
         "Zero Hour's Load_Thumbnail, verbatim: the HSV shift applied to the first level, and the lookup through Peek_Thumbnail_Instance_From_Any_Manager (byte-exact at 0x948DC0)"),
        ('\tReduction\t\t= Texture->Get_Reduction();',
         '\tReduction\t\t= Texture->Get_Reduction();\n\tHSVShift\t\t\t= Texture->Get_HSV_Shift();',
         "the load task carries the texture's HSV shift, as Zero Hour's (TextureBaseClass has HSVShift at +0x20, and Load_Thumbnail takes it)"),
        ('\tFormat\t\t\t= Texture->Get_Texture_Format();',
         "\tTextureClass* tex=Texture->As_TextureClass();\n\n\tif (tex)\n\t{\n\t\tFormat\t\t\t= tex->Get_Texture_Format(); // don't assume format yet KM\n\t}\n\telse\n\t{\n\t\tFormat\t\t\t= WW3D_FORMAT_UNKNOWN;\n\t}\n",
         "the format comes from the TextureClass behind the base (Zero Hour's lines)"),
        ('\tIDirect3DTexture8 *d3d_texture = Load_Thumbnail(tc->Get_Full_Path());',
         '\tIDirect3DTexture8 *d3d_texture = Load_Thumbnail(tc->Get_Full_Path(),tc->Get_HSV_Shift());',
         "passes the HSV shift (Zero Hour's line; Zero Hour's Get_Asset_Type check below it is from the 08/05/02 redesign)"),
    ],
    ('ww3d2', 'ddsfile.h'): [
        ('\t\tunsigned dest_pitch);',
         '\t\tunsigned dest_pitch,\n\t\tconst Vector3& hsv_shift=Vector3(0.0f,0.0f,0.0f));',
         "Zero Hour's HSV shift when copying a DDS level (Copy_Level_To_Surface byte-exact at 0x9A0CA0); the rest of E&B's DDSFileClass is Renegade's (no volume or cube members: Renegade's destructor, Get_Memory_Pointer, Get_Level_Size and Get_Pixel match)"),
        ('\tvoid Copy_Level_To_Surface(unsigned level,IDirect3DSurface8* d3d_surface);',
         '\tvoid Copy_Level_To_Surface(unsigned level,IDirect3DSurface8* d3d_surface,const Vector3& hsv_shift=Vector3(0.0f,0.0f,0.0f));',
         "Zero Hour's HSV shift when copying a DDS level (Copy_Level_To_Surface byte-exact at 0x9A0CA0); the rest of E&B's DDSFileClass is Renegade's (no volume or cube members: Renegade's destructor, Get_Memory_Pointer, Get_Level_Size and Get_Pixel match)"),
        ('#include "wwstring.h"',
         '#include "wwstring.h"\n#include "vector3.h"',
         "Zero Hour's HSV shift when copying a DDS level (Copy_Level_To_Surface byte-exact at 0x9A0CA0); the rest of E&B's DDSFileClass is Renegade's (no volume or cube members: Renegade's destructor, Get_Memory_Pointer, Get_Level_Size and Get_Pixel match)"),
        ('\t\tunsigned source_y) const;\t\t\t// DDS y offset to copy from, must be aligned by 4!',
         '\t\tunsigned source_y,\t\t\t\t\t// DDS y offset to copy from, must be aligned by 4!\n\t\tconst Vector3& hsv_shift=Vector3(0.0f,0.0f,0.0f)) const;',
         "Zero Hour's Get_4x4_Block, which recolours the block by the HSV shift (Copy_Level_To_Surface passes it)"),
    ],
    ('ww3d2', 'ddsfile.cpp'): [
        ('void DDSFileClass::Copy_Level_To_Surface(\n\tunsigned level,\n\tWW3DFormat dest_format, \n\tunsigned dest_width, \n\tunsigned dest_height, \n\tunsigned char* dest_surface, \n\tunsigned dest_pitch)\n{\n\tWWASSERT(DDSMemory);\n\tWWASSERT(dest_surface);\n\n\t// If the format and size is a match just copy the contents\n\tif (dest_format==Format && dest_width==Get_Width(level) && dest_height==Get_Height(level)) {\n\t\tunsigned compressed_size=Get_Level_Size(level);\n\t\tmemcpy(dest_surface,Get_Memory_Pointer(level),compressed_size);\n\t}\n\telse {\n\t\t// If size matches, copy each pixel linearly with color space conversion\n\t\tif (dest_width==Get_Width(level) && dest_height==Get_Height(level)) {\n\t\t\t// An exception here - if the source format is DXT1 and the destination\n\t\t\t// is DXT2, just copy the contents and create an empty alpha channel.\n\t\t\t// This is needed on NVidia cards that have problems with DXT1 compression.\n\t\t\tif (Format==WW3D_FORMAT_DXT1 && dest_format==WW3D_FORMAT_DXT2) {\n\t\t\t\tconst unsigned* src_ptr=reinterpret_cast<const unsigned*>(Get_Memory_Pointer(level));\n\t\t\t\tunsigned* dest_ptr=reinterpret_cast<unsigned*>(dest_surface);\n\t\t\t\tfor (unsigned y=0;y<dest_height;y+=4) {\n\t\t\t\t\tfor (unsigned x=0;x<dest_width;x+=4) {\n\t\t\t\t\t\t*dest_ptr++=0xffffffff;\t\t// Bytes 1-4 of alpha block\n\t\t\t\t\t\t*dest_ptr++=0xffffffff;\t\t// Bytes 5-8 of alpha block\n\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 1-4 of color block\n\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 5-8 of color block\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t\telse {\n\t\t\t\tunsigned dest_bpp=Get_Bytes_Per_Pixel(dest_format);\n\n\t\t\t\t// Copy 4x4 block at a time\n\t\t\t\tbool contains_alpha=false;\n\t\t\t\tfor (unsigned y=0;y<dest_height;y+=4) {\n\t\t\t\t\tunsigned char* dest_ptr=dest_surface;\n\t\t\t\t\tdest_ptr+=y*dest_pitch;\n\t\t\t\t\tfor (unsigned x=0;x<dest_width;x+=4,dest_ptr+=dest_bpp*4) {\n\t\t\t\t\t\tcontains_alpha|=Get_4x4_Block(dest_ptr,dest_pitch,dest_format,level,x,y);\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\tif (Format==WW3D_FORMAT_DXT1 && contains_alpha) {\n\t\t\t\t\tWWDEBUG_SAY(("Warning: DXT1 format should not contain alpha information - file %s\\n",Name));\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t}\n}',
         'void DDSFileClass::Copy_Level_To_Surface\n(\n\tunsigned level,\n\tWW3DFormat dest_format, \n\tunsigned dest_width, \n\tunsigned dest_height, \n\tunsigned char* dest_surface, \n\tunsigned dest_pitch,\n\tconst Vector3& hsv_shift\n)\n{\n\tWWASSERT(DDSMemory);\n\tWWASSERT(dest_surface);\n\n\tif (!DDSMemory || !Get_Memory_Pointer(level))\n\t{\n\t\tWWASSERT_PRINT(DDSMemory,"Surface mip level pointer is missing\\n");\n\t\treturn;\n\t}\n\n\t// If the format and size is a match just copy the contents\n\tbool has_hsv_shift = hsv_shift[0]!=0.0f || hsv_shift[1]!=0.0f || hsv_shift[2]!=0.0f;\n\tif (dest_format==Format && dest_width==Get_Width(level) && dest_height==Get_Height(level)) {\n\t\t// If hue shift, we can\'t just copy...\n\t\tif (has_hsv_shift) {\n\t\t\tif (Format==WW3D_FORMAT_DXT1) {\n\t\t\t\tconst unsigned* src_ptr=reinterpret_cast<const unsigned*>(Get_Memory_Pointer(level));\n\t\t\t\tunsigned* dest_ptr=reinterpret_cast<unsigned*>(dest_surface);\n\t\t\t\tfor (unsigned y=0;y<dest_height;y+=4) {\n\t\t\t\t\tfor (unsigned x=0;x<dest_width;x+=4) {\n\t\t\t\t\t\tunsigned cols=*src_ptr++;\t\t// Bytes 1-4 of color block\n\t\t\t\t\t\tunsigned col0=RGB565_To_ARGB8888(unsigned short(cols>>16));\n\t\t\t\t\t\tunsigned col1=RGB565_To_ARGB8888(unsigned short(cols&0xffff));\n\t\t\t\t\t\tRecolor(col0,hsv_shift);\n\t\t\t\t\t\tRecolor(col1,hsv_shift);\n\t\t\t\t\t\tcol0=ARGB8888_To_RGB565(col0);\n\t\t\t\t\t\tcol1=ARGB8888_To_RGB565(col1);\n\t\t\t\t\t\tcols=unsigned(col0)<<16|col1;\n\t\t\t\t\t\t*dest_ptr++=cols;\n\n\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 5-8 of color block\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t\telse if (Format==WW3D_FORMAT_DXT5) {\n\t\t\t\tconst unsigned* src_ptr=reinterpret_cast<const unsigned*>(Get_Memory_Pointer(level));\n\t\t\t\tunsigned* dest_ptr=reinterpret_cast<unsigned*>(dest_surface);\n\t\t\t\tfor (unsigned y=0;y<dest_height;y+=4) {\n\t\t\t\t\tfor (unsigned x=0;x<dest_width;x+=4) {\n\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 1-4 of alpha block\n\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 5-8 of alpha block\n\t\t\t\t\t\tunsigned cols=*src_ptr++;\t\t// Bytes 1-4 of color block\n\t\t\t\t\t\tunsigned col0=RGB565_To_ARGB8888(unsigned short(cols>>16));\n\t\t\t\t\t\tunsigned col1=RGB565_To_ARGB8888(unsigned short(cols&0xffff));\n\t\t\t\t\t\tRecolor(col0,hsv_shift);\n\t\t\t\t\t\tRecolor(col1,hsv_shift);\n\t\t\t\t\t\tcol0=ARGB8888_To_RGB565(col0);\n\t\t\t\t\t\tcol1=ARGB8888_To_RGB565(col1);\n\t\t\t\t\t\tcols=unsigned(col0)<<16|col1;\n\t\t\t\t\t\t*dest_ptr++=cols;\n\n\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 5-8 of color block\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t\telse {\n\t\t\t\tWWASSERT(0);\n\t\t\t}\n\n\t\t}\n\t\telse {\n\t\t\tunsigned compressed_size=Get_Level_Size(level);\n\t\t\tmemcpy(dest_surface,Get_Memory_Pointer(level),compressed_size);\n\t\t}\n\t}\n\telse {\n\t\t// If size matches, copy each pixel linearly with color space conversion\n\t\tif (dest_width==Get_Width(level) && dest_height==Get_Height(level)) {\n\t\t\t// An exception here - if the source format is DXT1 and the destination\n\t\t\t// is DXT2, just copy the contents and create an empty alpha channel.\n\t\t\t// This is needed on NVidia cards that have problems with DXT1 compression.\n\t\t\tif (Format==WW3D_FORMAT_DXT1 && dest_format==WW3D_FORMAT_DXT2) {\n\t\t\t\t// If hue shift, we can\'t just copy...\n\t\t\t\tif (has_hsv_shift) {\n\t\t\t\t\tconst unsigned* src_ptr=reinterpret_cast<const unsigned*>(Get_Memory_Pointer(level));\n\t\t\t\t\tunsigned* dest_ptr=reinterpret_cast<unsigned*>(dest_surface);\n\t\t\t\t\tfor (unsigned y=0;y<dest_height;y+=4) {\n\t\t\t\t\t\tfor (unsigned x=0;x<dest_width;x+=4) {\n\t\t\t\t\t\t\t*dest_ptr++=0xffffffff;\t\t// Bytes 1-4 of alpha block\n\t\t\t\t\t\t\t*dest_ptr++=0xffffffff;\t\t// Bytes 5-8 of alpha block\n//\t\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 1-4 of color block\n\n\t\t\t\t\t\t\tunsigned cols=*src_ptr++;\t// Bytes 1-4 of color block\n\t\t\t\t\t\t\tunsigned col0=RGB565_To_ARGB8888(unsigned short(cols>>16));\n\t\t\t\t\t\t\tunsigned col1=RGB565_To_ARGB8888(unsigned short(cols&0xffff));\n\t\t\t\t\t\t\tRecolor(col0,hsv_shift);\n\t\t\t\t\t\t\tRecolor(col1,hsv_shift);\n\t\t\t\t\t\t\tcol0=ARGB8888_To_RGB565(col0);\n\t\t\t\t\t\t\tcol1=ARGB8888_To_RGB565(col1);\n\t\t\t\t\t\t\tcols=unsigned(col0)<<16|col1;\n\t\t\t\t\t\t\t*dest_ptr++=cols;\n\n\t\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 5-8 of color block\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\telse {\n\t\t\t\t\tconst unsigned* src_ptr=reinterpret_cast<const unsigned*>(Get_Memory_Pointer(level));\n\t\t\t\t\tunsigned* dest_ptr=reinterpret_cast<unsigned*>(dest_surface);\n\t\t\t\t\tfor (unsigned y=0;y<dest_height;y+=4) {\n\t\t\t\t\t\tfor (unsigned x=0;x<dest_width;x+=4) {\n\t\t\t\t\t\t\t*dest_ptr++=0xffffffff;\t\t// Bytes 1-4 of alpha block\n\t\t\t\t\t\t\t*dest_ptr++=0xffffffff;\t\t// Bytes 5-8 of alpha block\n\t\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 1-4 of color block\n\t\t\t\t\t\t\t*dest_ptr++=*src_ptr++;\t\t// Bytes 5-8 of color block\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t\telse {\n\t\t\t\tunsigned dest_bpp=Get_Bytes_Per_Pixel(dest_format);\n\n\t\t\t\t// Copy 4x4 block at a time\n\t\t\t\tbool contains_alpha=false;\n\t\t\t\tfor (unsigned y=0;y<dest_height;y+=4) {\n\t\t\t\t\tunsigned char* dest_ptr=dest_surface;\n\t\t\t\t\tdest_ptr+=y*dest_pitch;\n\t\t\t\t\tfor (unsigned x=0;x<dest_width;x+=4,dest_ptr+=dest_bpp*4) {\n\t\t\t\t\t\tcontains_alpha|=Get_4x4_Block(dest_ptr,dest_pitch,dest_format,level,x,y,hsv_shift);\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\tif (Format==WW3D_FORMAT_DXT1 && contains_alpha) {\n\t\t\t\t\tWWDEBUG_SAY(("Warning: DXT1 format should not contain alpha information - file %s\\n",Name));\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t}\n}',
         "Zero Hour's HSV shift when copying a DDS level (Copy_Level_To_Surface byte-exact at 0x9A0CA0); the rest of E&B's DDSFileClass is Renegade's (no volume or cube members: Renegade's destructor, Get_Memory_Pointer, Get_Level_Size and Get_Pixel match): Zero Hour's definition, verbatim"),
        ('void DDSFileClass::Copy_Level_To_Surface(unsigned level,IDirect3DSurface8* d3d_surface)\n{\n\tWWASSERT(d3d_surface);\n\t// Verify that the destination surface size matches the source surface size\n\tD3DSURFACE_DESC surface_desc;\n\tDX8_ErrorCode(d3d_surface->GetDesc(&surface_desc));\n\n\t// First lock the surface\n\tD3DLOCKED_RECT locked_rect;\n\tDX8_ErrorCode(d3d_surface->LockRect(&locked_rect,NULL,0));\n\n\tCopy_Level_To_Surface(\n\t\tlevel,\n\t\tD3DFormat_To_WW3DFormat(surface_desc.Format),\n\t\tsurface_desc.Width,\n\t\tsurface_desc.Height,\n\t\treinterpret_cast<unsigned char*>(locked_rect.pBits),\n\t\tlocked_rect.Pitch);\n\n\t// Finally, unlock the surface\n\tDX8_ErrorCode(d3d_surface->UnlockRect());\n}',
         'void DDSFileClass::Copy_Level_To_Surface(unsigned level,IDirect3DSurface8* d3d_surface,const Vector3& hsv_shift)\n{\n\tWWASSERT(d3d_surface);\n\t// Verify that the destination surface size matches the source surface size\n\tD3DSURFACE_DESC surface_desc;\n\tDX8_ErrorCode(d3d_surface->GetDesc(&surface_desc));\n\n\t// First lock the surface\n\tD3DLOCKED_RECT locked_rect;\n\tDX8_ErrorCode(d3d_surface->LockRect(&locked_rect,NULL,0));\n\n\tCopy_Level_To_Surface(\n\t\tlevel,\n\t\tD3DFormat_To_WW3DFormat(surface_desc.Format),\n\t\tsurface_desc.Width,\n\t\tsurface_desc.Height,\n\t\treinterpret_cast<unsigned char*>(locked_rect.pBits),\n\t\tlocked_rect.Pitch,\n\t\thsv_shift);\n\n\t// Finally, unlock the surface\n\tDX8_ErrorCode(d3d_surface->UnlockRect());\n}',
         "Zero Hour's HSV shift when copying a DDS level (Copy_Level_To_Surface byte-exact at 0x9A0CA0); the rest of E&B's DDSFileClass is Renegade's (no volume or cube members: Renegade's destructor, Get_Memory_Pointer, Get_Level_Size and Get_Pixel match): Zero Hour's definition, verbatim"),
        ('\n// ----------------------------------------------------------------------------\n\nWWINLINE static unsigned RGB565_To_ARGB8888(unsigned short rgb)\n{\n\tunsigned rgba=0;\n\trgba|=unsigned(rgb&0x001f)<<3;\n\trgba|=unsigned(rgb&0x07e0)<<5;\n\trgba|=unsigned(rgb&0xf800)<<8;\n\treturn rgba;\n}\n',
         '',
         "moved above Copy_Level_To_Surface, which now uses it (as in Zero Hour's file)"),
        ('\treturn true;\n}\n\n// ----------------------------------------------------------------------------\n//',
         '\treturn true;\n}\n\n// ----------------------------------------------------------------------------\n\nWWINLINE static unsigned RGB565_To_ARGB8888(unsigned short rgb)\n{\n\tunsigned rgba=0;\n\trgba|=unsigned(rgb&0x001f)<<3;\n\trgba|=unsigned(rgb&0x07e0)<<5;\n\trgba|=unsigned(rgb&0xf800)<<8;\n\treturn rgba;\n}\n\nWWINLINE static unsigned short ARGB8888_To_RGB565(unsigned argb_)\n{\n\tunsigned char* argb=(unsigned char*)&argb_;\n\tunsigned short rgb;\n\trgb=((argb[2])&0xf8)<<8;\n\trgb|=((argb[1])&0xfc)<<3;\n\trgb|=((argb[0])&0xf8)>>3;\n\treturn rgb;\n}\n\n\n// ----------------------------------------------------------------------------\n//',
         "Zero Hour's colour helpers for recolouring DXT blocks, placed as in Zero Hour's file"),
        ("bool DDSFileClass::Get_4x4_Block(\n\tunsigned char* dest_ptr,\t\t\t// Destination surface pointer\n\tunsigned dest_pitch,\t\t\t\t\t// Destination surface pitch, in bytes\n\tWW3DFormat dest_format,\t\t\t\t// Destination surface format, A8R8G8B8 is fastest\n\tunsigned level,\t\t\t\t\t\t// DDS mipmap level to copy from\n\tunsigned source_x,\t\t\t\t\t// DDS x offset to copy from, must be aligned by 4!\n\tunsigned source_y) const\t\t\t// DDS y offset to copy from, must be aligned by 4!\n{\n\t// Verify the block alignment\n\tWWASSERT((source_x&3)==0);\n\tWWASSERT((source_y&3)==0);\n\t// Verify level\n\tWWASSERT(level<MipLevels);\n\t// Verify coordinate bounds\n\tWWASSERT(source_x<Get_Width(level));\n\tWWASSERT(source_y<Get_Height(level));\n\n\tunsigned dest_bpp=Get_Bytes_Per_Pixel(dest_format);\n\n\tswitch (Format) {\n\t// Note that we don't currently really support alpha on DXT1 - all alpha textures should use DXT5.\n\t// The reason for this is that when converting from DXT1 to 16 bit uncompressed texture we want\n\t// to be able to use RGB565 format instead of ARGB4444. As the alpha is encoded in DXT1 per-block\n\t// basis there isn't really a way to tell if the surface has an alpha or not so either we use alpha\n\t// or we don't.\n\tcase WW3D_FORMAT_DXT1:\n\t\t{\n\t\t\tconst unsigned char* block_memory=Get_Memory_Pointer(level)+(source_x/4)*8+((source_y/4)*(Get_Width(level)/4))*8;\n\n\t\t\tunsigned col0=RGB565_To_ARGB8888(*(unsigned short*)&block_memory[0]);\n\t\t\tunsigned col1=RGB565_To_ARGB8888(*(unsigned short*)&block_memory[2]);\n\t\t\t// Even if we don't support alpha, decompression is different if source has alpha\n\t\t\tunsigned dest_pixel=0;\n\t\t\tif (col0>col1) {\n\t\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\t\tunsigned char line=block_memory[4+y];\n\t\t\t\t\tfor (int x=0;x<4;++x) {\n\t\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\t\tcase 0: dest_pixel=col0|0xff000000; break;\n\t\t\t\t\t\tcase 1: dest_pixel=col1|0xff000000; break;\n\t\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,85)|0xff000000; break;\n\t\t\t\t\t\tcase 3: dest_pixel=Combine_Colors(col0,col1,85)|0xff000000; break;\n\t\t\t\t\t\t}\n\t\t\t\t\t\tline>>=2;\n\n\t\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\treturn false;\t// No alpha found in the block\n\t\t\t}\n\t\t\telse {\n\t\t\t\tbool contains_alpha=false;\n\t\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\t\tunsigned char line=block_memory[4+y];\n\t\t\t\t\tfor (int x=0;x<4;++x) {\n\t\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\t\tcase 0: dest_pixel=col0|0xff000000; break;\n\t\t\t\t\t\tcase 1: dest_pixel=col1|0xff000000; break;\n\t\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,128)|0xff000000; break;\n\t\t\t\t\t\tcase 3: dest_pixel=0x00000000; contains_alpha=true; break;\n\t\t\t\t\t\t}\n\t\t\t\t\t\tline>>=2;\n\n\t\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\treturn contains_alpha;\t// Alpha block...?\n\t\t\t}\n\t\t}\n\t\tbreak;\n\tcase WW3D_FORMAT_DXT2:\n\t\treturn false;\n\tcase WW3D_FORMAT_DXT3:\n\t\treturn false;\n\tcase WW3D_FORMAT_DXT4:\n\t\treturn false;\n\tcase WW3D_FORMAT_DXT5:\n\t\t{\n\t\t\t// Init alphas\n\t\t\tconst unsigned char* alpha_block=Get_Memory_Pointer(level)+(source_x/4)*16+((source_y/4)*(Get_Width(level)/4))*16;\n\n\t\t\tunsigned alphas[8];\n\t\t\talphas[0]=alpha_block[0];\n\t\t\talphas[1]=alpha_block[1];\n\n\t\t\t// 8-alpha or 6-alpha block?    \n\t\t\tif (alphas[0]>alphas[1]) {    \n\t\t\t\talphas[2]=(6*alphas[0]+1*alphas[1]+3) / 7;   // bit code 010\n\t\t\t\talphas[3]=(5*alphas[0]+2*alphas[1]+3) / 7;   // bit code 011\n\t\t\t\talphas[4]=(4*alphas[0]+3*alphas[1]+3) / 7;   // bit code 100\n\t\t\t\talphas[5]=(3*alphas[0]+4*alphas[1]+3) / 7;   // bit code 101\n\t\t\t\talphas[6]=(2*alphas[0]+5*alphas[1]+3) / 7;   // bit code 110\n\t\t\t\talphas[7]=(1*alphas[0]+6*alphas[1]+3) / 7;   // bit code 111  \n\t\t\t}\n\t\t\telse {\n\t\t\t\talphas[2]=(4*alphas[0]+1*alphas[1]+2) / 5;   // Bit code 010\n\t\t\t\talphas[3]=(3*alphas[0]+2*alphas[1]+2) / 5;   // Bit code 011\n\t\t\t\talphas[4]=(2*alphas[0]+3*alphas[1]+2) / 5;   // Bit code 100\n\t\t\t\talphas[5]=(1*alphas[0]+4*alphas[1]+2) / 5;   // Bit code 101\n\t\t\t\talphas[6]=0; \t\t\t\t\t\t\t\t\t\t   // Bit code 110\n\t\t\t\talphas[7]=255; \t\t\t\t\t\t\t\t\t   // Bit code 111\n\t\t\t}\n\n\t\t\t// Init colors\n\t\t\tconst unsigned char* color_block=alpha_block+8;\n\t\t\tunsigned col0=RGB565_To_ARGB8888(*(unsigned short*)&color_block[0]);\n\t\t\tunsigned col1=RGB565_To_ARGB8888(*(unsigned short*)&color_block[2]);\n\n\t\t\tunsigned dest_pixel=0;\n\t\t\tunsigned bit_idx=0;\n\t\t\tunsigned contains_alpha=0xff;\n\n\t\t\tunsigned alpha_indices[16];\n\t\t\tunsigned* ai_ptr=alpha_indices;\n\t\t\tfor (int a=0;a<2;++a) {\n\t\t\t\tai_ptr[0]=alpha_block[2]&0x7;\n\t\t\t\tai_ptr[1]=(alpha_block[2]>>3)&0x7;\n\t\t\t\tai_ptr[2]=(alpha_block[2]>>6)|((alpha_block[3]&1)<<2);\n\t\t\t\tai_ptr[3]=(alpha_block[3]>>1)&0x7;\n\t\t\t\tai_ptr[4]=(alpha_block[3]>>4)&0x7;\n\t\t\t\tai_ptr[5]=(alpha_block[3]>>7)|((alpha_block[4]&3)<<1);\n\t\t\t\tai_ptr[6]=(alpha_block[4]>>2)&0x7;\n\t\t\t\tai_ptr[7]=(alpha_block[4]>>5);\n\t\t\t\tai_ptr+=8;\n\t\t\t\talpha_block+=3;\n\t\t\t}\n\n\t\t\tunsigned aii=0;\n\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\tunsigned char line=color_block[4+y];\n\t\t\t\tfor (int x=0;x<4;++x,bit_idx+=3) {\n\t\t\t\t\tunsigned alpha_value=alphas[alpha_indices[aii++]];\n\t\t\t\t\tcontains_alpha&=alpha_value;\n\t\t\t\t\talpha_value<<=24;\n\n\t\t\t\t\t// Extract color\n\n\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\tcase 0: dest_pixel=col0|alpha_value; break;\n\t\t\t\t\tcase 1: dest_pixel=col1|alpha_value; break;\n\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,85)|alpha_value; break;\n\t\t\t\t\tcase 3: dest_pixel=Combine_Colors(col0,col1,85)|alpha_value; break;\n\t\t\t\t\t}\n\t\t\t\t\tline>>=2;\n\n\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t}\n\t\t\t}\n\n\n\n\n/*\n\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\tunsigned char line=color_block[4+y];\n\t\t\t\tfor (int x=0;x<4;++x,bit_idx+=3) {\n\t\t\t\t\tunsigned byte_idx=bit_idx/8;\n\t\t\t\t\tunsigned tmp_bit_idx=bit_idx&7;\n\t\t\t\t\tunsigned alpha_index=0;\n\t\t\t\t\tfor (int i=0;i<3;++i) {\n\t\t\t\t\t\tWWASSERT(byte_idx<6);\n\t\t\t\t\t\tunsigned alpha_bit=(alpha_block[2+byte_idx]>>(tmp_bit_idx))&1;\n\t\t\t\t\t\talpha_index|=alpha_bit<<(i);\n\t\t\t\t\t\ttmp_bit_idx++;\n\t\t\t\t\t\tif (tmp_bit_idx>=8) {\n\t\t\t\t\t\t\ttmp_bit_idx=0;\n\t\t\t\t\t\t\tbyte_idx++;\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t\tWWASSERT(alpha_index<8);\n\t\t\t\t\tunsigned alpha_value=alphas[alpha_index];\n\t\t\t\t\tcontains_alpha&=alpha_value;\n\t\t\t\t\talpha_value<<=24;\n\n\t\t\t\t\t// Extract color\n\n\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\tcase 0: dest_pixel=col0|alpha_value; break;\n\t\t\t\t\tcase 1: dest_pixel=col1|alpha_value; break;\n\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,85)|alpha_value; break;\n\t\t\t\t\tcase 3: dest_pixel=Combine_Colors(col0,col1,85)|alpha_value; break;\n\t\t\t\t\t}\n\t\t\t\t\tline>>=2;\n\n\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t}\n\t\t\t}\n*/\n\t\t\treturn contains_alpha!=0xff;\t// Alpha block... DXT5 should only be used when the image needs alpha\n\t\t\t\t\t\t\t\t\t\t\t\t\t// but for now check anyway...\n\t\t}\n\t}\n\treturn false;\n\n}",
         "bool DDSFileClass::Get_4x4_Block(\n\tunsigned char* dest_ptr,\t\t\t// Destination surface pointer\n\tunsigned dest_pitch,\t\t\t\t\t// Destination surface pitch, in bytes\n\tWW3DFormat dest_format,\t\t\t\t// Destination surface format, A8R8G8B8 is fastest\n\tunsigned level,\t\t\t\t\t\t// DDS mipmap level to copy from\n\tunsigned source_x,\t\t\t\t\t// DDS x offset to copy from, must be aligned by 4!\n\tunsigned source_y,\t\t\t\t\t// DDS y offset to copy from, must be aligned by 4!\n\tconst Vector3& hsv_shift) const\n{\n\t// Verify the block alignment\n\tWWASSERT((source_x&3)==0);\n\tWWASSERT((source_y&3)==0);\n\t// Verify level\n\tWWASSERT(level<MipLevels);\n\t// Verify coordinate bounds\n\tWWASSERT(source_x<Get_Width(level));\n\tWWASSERT(source_y<Get_Height(level));\n\n\tunsigned dest_bpp=Get_Bytes_Per_Pixel(dest_format);\n\n\tbool has_hsv_shift = hsv_shift[0]!=0.0f || hsv_shift[1]!=0.0f || hsv_shift[2]!=0.0f;\n\tswitch (Format) {\n\t// Note that we don't currently really support alpha on DXT1 - all alpha textures should use DXT5.\n\t// The reason for this is that when converting from DXT1 to 16 bit uncompressed texture we want\n\t// to be able to use RGB565 format instead of ARGB4444. As the alpha is encoded in DXT1 per-block\n\t// basis there isn't really a way to tell if the surface has an alpha or not so either we use alpha\n\t// or we don't.\n\tcase WW3D_FORMAT_DXT1:\n\t\t{\n\t\t\tconst unsigned char* block_memory=Get_Memory_Pointer(level)+(source_x/4)*8+((source_y/4)*(Get_Width(level)/4))*8;\n\n\t\t\tunsigned col0=RGB565_To_ARGB8888(*(unsigned short*)&block_memory[0]);\n\t\t\tunsigned col1=RGB565_To_ARGB8888(*(unsigned short*)&block_memory[2]);\n\t\t\t// Even if we don't support alpha, decompression is different if source has alpha\n\t\t\tunsigned dest_pixel=0;\n\t\t\tif (col0>col1) {\n\t\t\t\tif (has_hsv_shift) {\n\t\t\t\t\tRecolor(col0,hsv_shift);\n\t\t\t\t\tRecolor(col1,hsv_shift);\n\t\t\t\t}\n\n\t\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\t\tunsigned char line=block_memory[4+y];\n\t\t\t\t\tfor (int x=0;x<4;++x) {\n\t\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\t\tcase 0: dest_pixel=col0|0xff000000; break;\n\t\t\t\t\t\tcase 1: dest_pixel=col1|0xff000000; break;\n\t\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,85)|0xff000000; break;\n\t\t\t\t\t\tcase 3: dest_pixel=Combine_Colors(col0,col1,85)|0xff000000; break;\n\t\t\t\t\t\t}\n\t\t\t\t\t\tline>>=2;\n\n\t\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\treturn false;\t// No alpha found in the block\n\t\t\t}\n\t\t\telse {\n\t\t\t\tif (has_hsv_shift) {\n\t\t\t\t\tRecolor(col0,hsv_shift);\n\t\t\t\t\tRecolor(col1,hsv_shift);\n\t\t\t\t}\n\t\t\t\tbool contains_alpha=false;\n\t\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\t\tunsigned char line=block_memory[4+y];\n\t\t\t\t\tfor (int x=0;x<4;++x) {\n\t\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\t\tcase 0: dest_pixel=col0|0xff000000; break;\n\t\t\t\t\t\tcase 1: dest_pixel=col1|0xff000000; break;\n\t\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,128)|0xff000000; break;\n\t\t\t\t\t\tcase 3: dest_pixel=0x00000000; contains_alpha=true; break;\n\t\t\t\t\t\t}\n\t\t\t\t\t\tline>>=2;\n\n\t\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\treturn contains_alpha;\t// Alpha block...?\n\t\t\t}\n\t\t}\n\t\tbreak;\n\tcase WW3D_FORMAT_DXT2:\n\t\treturn false;\n\tcase WW3D_FORMAT_DXT3:\n\t\treturn false;\n\tcase WW3D_FORMAT_DXT4:\n\t\treturn false;\n\tcase WW3D_FORMAT_DXT5:\n\t\t{\n\t\t\t// Init alphas\n\t\t\tconst unsigned char* alpha_block=Get_Memory_Pointer(level)+(source_x/4)*16+((source_y/4)*(Get_Width(level)/4))*16;\n\n\t\t\tunsigned alphas[8];\n\t\t\talphas[0]=alpha_block[0];\n\t\t\talphas[1]=alpha_block[1];\n\n\t\t\t// 8-alpha or 6-alpha block?    \n\t\t\tif (alphas[0]>alphas[1]) {    \n\t\t\t\talphas[2]=(6*alphas[0]+1*alphas[1]+3) / 7;   // bit code 010\n\t\t\t\talphas[3]=(5*alphas[0]+2*alphas[1]+3) / 7;   // bit code 011\n\t\t\t\talphas[4]=(4*alphas[0]+3*alphas[1]+3) / 7;   // bit code 100\n\t\t\t\talphas[5]=(3*alphas[0]+4*alphas[1]+3) / 7;   // bit code 101\n\t\t\t\talphas[6]=(2*alphas[0]+5*alphas[1]+3) / 7;   // bit code 110\n\t\t\t\talphas[7]=(1*alphas[0]+6*alphas[1]+3) / 7;   // bit code 111  \n\t\t\t}\n\t\t\telse {\n\t\t\t\talphas[2]=(4*alphas[0]+1*alphas[1]+2) / 5;   // Bit code 010\n\t\t\t\talphas[3]=(3*alphas[0]+2*alphas[1]+2) / 5;   // Bit code 011\n\t\t\t\talphas[4]=(2*alphas[0]+3*alphas[1]+2) / 5;   // Bit code 100\n\t\t\t\talphas[5]=(1*alphas[0]+4*alphas[1]+2) / 5;   // Bit code 101\n\t\t\t\talphas[6]=0; \t\t\t\t\t\t\t\t\t\t   // Bit code 110\n\t\t\t\talphas[7]=255; \t\t\t\t\t\t\t\t\t   // Bit code 111\n\t\t\t}\n\n\t\t\t// Init colors\n\t\t\tconst unsigned char* color_block=alpha_block+8;\n\t\t\tunsigned col0=RGB565_To_ARGB8888(*(unsigned short*)&color_block[0]);\n\t\t\tunsigned col1=RGB565_To_ARGB8888(*(unsigned short*)&color_block[2]);\n\t\t\tif (has_hsv_shift) {\n\t\t\t\tRecolor(col0,hsv_shift);\n\t\t\t\tRecolor(col1,hsv_shift);\n\t\t\t}\n\n\t\t\tunsigned dest_pixel=0;\n\t\t\tunsigned bit_idx=0;\n\t\t\tunsigned contains_alpha=0xff;\n\n\t\t\tunsigned alpha_indices[16];\n\t\t\tunsigned* ai_ptr=alpha_indices;\n\t\t\tfor (int a=0;a<2;++a) {\n\t\t\t\tai_ptr[0]=alpha_block[2]&0x7;\n\t\t\t\tai_ptr[1]=(alpha_block[2]>>3)&0x7;\n\t\t\t\tai_ptr[2]=(alpha_block[2]>>6)|((alpha_block[3]&1)<<2);\n\t\t\t\tai_ptr[3]=(alpha_block[3]>>1)&0x7;\n\t\t\t\tai_ptr[4]=(alpha_block[3]>>4)&0x7;\n\t\t\t\tai_ptr[5]=(alpha_block[3]>>7)|((alpha_block[4]&3)<<1);\n\t\t\t\tai_ptr[6]=(alpha_block[4]>>2)&0x7;\n\t\t\t\tai_ptr[7]=(alpha_block[4]>>5);\n\t\t\t\tai_ptr+=8;\n\t\t\t\talpha_block+=3;\n\t\t\t}\n\n\t\t\tunsigned aii=0;\n\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\tunsigned char line=color_block[4+y];\n\t\t\t\tfor (int x=0;x<4;++x,bit_idx+=3) {\n\t\t\t\t\tunsigned alpha_value=alphas[alpha_indices[aii++]];\n\t\t\t\t\tcontains_alpha&=alpha_value;\n\t\t\t\t\talpha_value<<=24;\n\n\t\t\t\t\t// Extract color\n\n\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\tcase 0: dest_pixel=col0|alpha_value; break;\n\t\t\t\t\tcase 1: dest_pixel=col1|alpha_value; break;\n\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,85)|alpha_value; break;\n\t\t\t\t\tcase 3: dest_pixel=Combine_Colors(col0,col1,85)|alpha_value; break;\n\t\t\t\t\t}\n\t\t\t\t\tline>>=2;\n\n\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t}\n\t\t\t}\n\n\n\n\n/*\n\t\t\tfor (int y=0;y<4;++y) {\n\t\t\t\tunsigned char* tmp_dest_ptr=dest_ptr;\n\t\t\t\tdest_ptr+=dest_pitch;\n\t\t\t\tunsigned char line=color_block[4+y];\n\t\t\t\tfor (int x=0;x<4;++x,bit_idx+=3) {\n\t\t\t\t\tunsigned byte_idx=bit_idx/8;\n\t\t\t\t\tunsigned tmp_bit_idx=bit_idx&7;\n\t\t\t\t\tunsigned alpha_index=0;\n\t\t\t\t\tfor (int i=0;i<3;++i) {\n\t\t\t\t\t\tWWASSERT(byte_idx<6);\n\t\t\t\t\t\tunsigned alpha_bit=(alpha_block[2+byte_idx]>>(tmp_bit_idx))&1;\n\t\t\t\t\t\talpha_index|=alpha_bit<<(i);\n\t\t\t\t\t\ttmp_bit_idx++;\n\t\t\t\t\t\tif (tmp_bit_idx>=8) {\n\t\t\t\t\t\t\ttmp_bit_idx=0;\n\t\t\t\t\t\t\tbyte_idx++;\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t\tWWASSERT(alpha_index<8);\n\t\t\t\t\tunsigned alpha_value=alphas[alpha_index];\n\t\t\t\t\tcontains_alpha&=alpha_value;\n\t\t\t\t\talpha_value<<=24;\n\n\t\t\t\t\t// Extract color\n\n\t\t\t\t\tswitch (line&3) {\n\t\t\t\t\tcase 0: dest_pixel=col0|alpha_value; break;\n\t\t\t\t\tcase 1: dest_pixel=col1|alpha_value; break;\n\t\t\t\t\tcase 2: dest_pixel=Combine_Colors(col1,col0,85)|alpha_value; break;\n\t\t\t\t\tcase 3: dest_pixel=Combine_Colors(col0,col1,85)|alpha_value; break;\n\t\t\t\t\t}\n\t\t\t\t\tline>>=2;\n\n\t\t\t\t\tBitmapHandlerClass::Write_B8G8R8A8(tmp_dest_ptr,dest_format,dest_pixel);\n\t\t\t\t\ttmp_dest_ptr+=dest_bpp;\n\t\t\t\t}\n\t\t\t}\n*/\n\t\t\treturn contains_alpha!=0xff;\t// Alpha block... DXT5 should only be used when the image needs alpha\n\t\t\t\t\t\t\t\t\t\t\t\t\t// but for now check anyway...\n\t\t}\n\t}\n\treturn false;\n\n}",
         "Zero Hour's Get_4x4_Block, which recolours the block by the HSV shift (Copy_Level_To_Surface passes it), verbatim"),
    ],
    ('ww3d2', 'texturethumbnail.cpp'): [
        ('\tthumb_file->Close();\n\tff.Return_File(thumb_file);\n#endif\n}',
         '\tthumb_file->Close();\n\tff.Return_File(thumb_file);\n}',
         "E&B loads and writes its thumbnail files ('#if 0' is Generals' change, jba): the client's destructor calls Save (0x91A730, byte-exact once Save isn't empty and inlined)"),
        ("\t// If the thumbnail hash table was modified, save it to disk\n#if 0 // don't write thumbnails.  jba.",
         '\t// If the thumbnail hash table was modified, save it to disk',
         "E&B loads and writes its thumbnail files ('#if 0' is Generals' change, jba): the client's destructor calls Save (0x91A730, byte-exact once Save isn't empty and inlined)"),
        ('\tff.Return_File(thumb_file);\n#endif',
         '\tff.Return_File(thumb_file);',
         '(Load: the same)'),
        ("\t// If the thumbnail hash table file is available, init hash table\n#if 0 // don't do thumbnail file.",
         '\t// If the thumbnail hash table file is available, init hash table',
         '(Load: the same)'),
    ],
}
for _key, _edits in TEXTURE_API_PATCHES.items():
    PATCHES.setdefault(_key, []).extend(_edits)
# end of TEXTURE_API_PATCHES
# wwshade (D9, docs/progress.md): Zero Hour WW3D2 features the client's wwshade uses. Each edit
# gives a file Zero Hour's code where the client shows it (address in the reason).
# Applied after the file's PATCHES and TEXTURE_API_PATCHES entries.
WWSHADE_API_PATCHES = {
    ('ww3d2', 'rendobj.h'): [
        ('\t\tHAS_USER_LIGHTING =\t\t\t0x00040000,\t\t\t// the user has installed a static lighting solve.\n',
         '\t\tHAS_USER_LIGHTING =\t\t\t0x00040000,\t\t\t// the user has installed a static lighting solve.\n\t\tIS_SELF_SHADOWED =\t\t\t0x00080000,\t\t\t// the mesh is self shadowed\n',
         "Zero Hour's IS_SELF_SHADOWED bit: ShdMeshClass::Render (0xA09430) sets and clears 0x80000 in Bits (+0x18)"),
        ('\tbool\t\t\t\t\t\t\t\tAre_Sub_Object_Transforms_Dirty(void)\t\t\t\t\t\t\t\t\t{ return (Bits & SUBOBJ_TRANSFORMS_DIRTY) != 0; }\n',
         '\tbool\t\t\t\t\t\t\t\tAre_Sub_Object_Transforms_Dirty(void)\t\t\t\t\t\t\t\t\t{ return (Bits & SUBOBJ_TRANSFORMS_DIRTY) != 0; }\n\n\tvoid\t\t\t\t\t\t\t\tSet_Is_Self_Shadowed()\t\t\t\t\t\t\t\t\t\t\t\t\t\t{ Bits|=IS_SELF_SHADOWED; }\n\tvoid\t\t\t\t\t\t\t\tUnset_Is_Self_Shadowed()\t\t\t\t\t\t\t\t\t\t\t\t\t{ Bits&=~IS_SELF_SHADOWED; }\n\tint\t\t\t\t\t\t\t\tIs_Self_Shadowed() const\t\t\t\t\t\t\t\t\t\t\t\t\t{ return (Bits&IS_SELF_SHADOWED); }\n',
         "Zero Hour's self-shadow accessors, used by ShdMeshClass::Render (0xA09430)"),
    ],
    ('ww3d2', 'lightenvironment.h'): [
        ('\tstatic float\t\tGet_Lighting_LOD_Cutoff(void);\n',
         '\tstatic float\t\tGet_Lighting_LOD_Cutoff(void);\n\n\tstatic int\t\t\tGet_Max_Lights() { return MAX_LIGHTS; }\n',
         "Zero Hour's Get_Max_Lights, used by ShdHWVertexShader::Light (0xA0F610; its other overload 0xA0FC20 is Zero Hour's byte for byte)"),
    ],
    ('ww3d2', 'dx8wrapper.h'): [
        ('\t// shader system udpates KJM v\n\tstatic void Set_Vertex_Shader(DWORD vertex_shader);\n',
         '\t// shader system udpates KJM v\n\tstatic void Apply_Default_State();\n\n\tstatic void Set_Vertex_Shader(DWORD vertex_shader);\n',
         "Zero Hour's Apply_Default_State: the client's at 0x9270D0 (ZENABLE, SHADEMODE GOURAUD, ZWRITEENABLE, ALPHATESTENABLE 0, SRCBLEND ONE..., 0x680 bytes of shader constants), called by ShdDX8RendererClass::Flush (0xA08060)"),
        ('\tstatic DWORD Get_Vertex_Processing_Behavior() { return Vertex_Processing_Behavior; }\n',
         '\tstatic DWORD Get_Vertex_Processing_Behavior() { return Vertex_Processing_Behavior; }\n\n\t// Needed by scene lighting class\n\tstatic void\t\t\t\t\t\tSet_Ambient(const Vector3& color);\n\tstatic const Vector3&\t\tGet_Ambient() { return Ambient_Color; }\n',
         "Zero Hour's Set_Ambient/Get_Ambient: WW3D::Render (0x917E2F) stores the scene ambient in Ambient_Color (0xC00530) before D3DRS_AMBIENT; ShdHWVertexShader::Light (0xA0F7D4) reads it"),
        ('\tstatic void Invalidate_Cached_Render_States(void);\n',
         '',
         'Invalidate_Cached_Render_States is public in Zero Hour (ShdDX8RendererClass::Flush, 0xA08060, calls it); moved below'),
        ('\tstatic void Apply_Default_State();\n\n',
         '\tstatic void Apply_Default_State();\n\tstatic void Invalidate_Cached_Render_States(void);\n\n',
         'Invalidate_Cached_Render_States public, as in Zero Hour (called by ShdDX8RendererClass::Flush, 0xA08060)'),
        ('\tstatic ZTextureClass*\t\t\t\tShadow_Map[MAX_SHADOW_MAPS];\n',
         '\tstatic ZTextureClass*\t\t\t\tShadow_Map[MAX_SHADOW_MAPS];\n\n\tstatic Vector3\t\t\t\t\t\t\tAmbient_Color;\n',
         "Zero Hour's Ambient_Color (client 0xC00530)"),
        ('\tDX8CALL(SetPixelShaderConstant(reg,data,count));\n}\n// shader system updates KJM ^\n',
         '\tDX8CALL(SetPixelShaderConstant(reg,data,count));\n}\n\nWWINLINE void DX8Wrapper::Set_Ambient(const Vector3& color)\n{\n\tAmbient_Color=color;\n\tSet_DX8_Render_State(D3DRS_AMBIENT, DX8Wrapper::Convert_Color(color,0.0f));\n}\n// shader system updates KJM ^\n',
         "Zero Hour's Set_Ambient (inlined in WW3D::Render at 0x917E2F: Ambient_Color 0xC00530, then D3DRS_AMBIENT)"),
        ("#if 0 //(gth) some code is bypassing this acessor function so we can't count on this variable...\n\t// may be incorrect if shaders are created and destroyed dynamically\n\tif (Vertex_Shader==vertex_shader) return;\n#endif\n",
         '\t// may be incorrect if shaders are created and destroyed dynamically\n\tif (Vertex_Shader==vertex_shader) return;\n',
         "Set_Vertex_Shader keeps its redundancy check in the client (out-of-line copy 0x9275E0: compares Vertex_Shader 0xC00640 before SetVertexShader), before Zero Hour's (gth) #if 0"),
        ('const unsigned MAX_PIXEL_SHADER_CONSTANTS=8;\n',
         'const unsigned MAX_PIXEL_SHADER_CONSTANTS=8;\nconst unsigned MAX_VERTEX_STREAMS=2;\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\tunsigned vertex_buffer_type;\n',
         '\tunsigned vertex_buffer_types[MAX_VERTEX_STREAMS];\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\tVertexBufferClass* vertex_buffer;\n',
         '\tVertexBufferClass* vertex_buffers[MAX_VERTEX_STREAMS];\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\tstatic void Set_Vertex_Buffer(const VertexBufferClass* vb);\n',
         '\tstatic void Set_Vertex_Buffer(const VertexBufferClass* vb, unsigned stream=0);\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('WWINLINE void DX8Wrapper::Set_Render_State(const RenderStateStruct& state)\n{\n\tif (render_state.index_buffer) {\n\t\trender_state.index_buffer->Release_Engine_Ref();\n\t}\n\n\tif (render_state.vertex_buffer) {\n\t\trender_state.vertex_buffer->Release_Engine_Ref();\n\t}\n\n\trender_state=state;\n\trender_state_changed=0xffffffff;\n\n\tif (render_state.index_buffer) {\n\t\trender_state.index_buffer->Add_Engine_Ref();\n\t}\n\n\tif (render_state.vertex_buffer) {\n\t\trender_state.vertex_buffer->Add_Engine_Ref();\n\t}\n}\n\nWWINLINE void DX8Wrapper::Release_Render_State()\n{\n\tif (render_state.index_buffer) {\n\t\trender_state.index_buffer->Release_Engine_Ref();\n\t}\n\n\tif (render_state.vertex_buffer) {\n\t\trender_state.vertex_buffer->Release_Engine_Ref();\n\t}\n\n\tREF_PTR_RELEASE(render_state.vertex_buffer);\n\tREF_PTR_RELEASE(render_state.index_buffer);\n\tREF_PTR_RELEASE(render_state.material);\n\tfor (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_RELEASE(render_state.Textures[i]);\n}\n\n\nWWINLINE RenderStateStruct::RenderStateStruct()\n\t:\n\tmaterial(0),\n\tvertex_buffer(0),\n\tindex_buffer(0)\n{\n\tfor (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) Textures[i]=0;\n}\n\nWWINLINE RenderStateStruct::~RenderStateStruct()\n{\n\tREF_PTR_RELEASE(material);\n\tREF_PTR_RELEASE(vertex_buffer);\n\tREF_PTR_RELEASE(index_buffer);\n\tfor (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_RELEASE(Textures[i]);\n}\n\n\nWWINLINE RenderStateStruct& RenderStateStruct::operator= (const RenderStateStruct& src)\n{\n\tREF_PTR_SET(material,src.material);\n\tREF_PTR_SET(vertex_buffer,src.vertex_buffer);\n\tREF_PTR_SET(index_buffer,src.index_buffer);\n\tfor (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_SET(Textures[i],src.Textures[i]);\n',
         'WWINLINE void DX8Wrapper::Set_Render_State(const RenderStateStruct& state)\n{\n\tint i;\n\n\tif (render_state.index_buffer) {\n\t\trender_state.index_buffer->Release_Engine_Ref();\n\t}\n\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) \n\t{\n\t\tif (render_state.vertex_buffers[i]) \n\t\t{\n\t\t\trender_state.vertex_buffers[i]->Release_Engine_Ref();\n\t\t}\n\t}\n\n\trender_state=state;\n\trender_state_changed=0xffffffff;\n\n\tif (render_state.index_buffer) {\n\t\trender_state.index_buffer->Add_Engine_Ref();\n\t}\n\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) \n\t{\n\t\tif (render_state.vertex_buffers[i]) \n\t\t{\n\t\t\trender_state.vertex_buffers[i]->Add_Engine_Ref();\n\t\t}\n\t}\n}\n\nWWINLINE void DX8Wrapper::Release_Render_State()\n{\n\tint i;\n\n\tif (render_state.index_buffer) {\n\t\trender_state.index_buffer->Release_Engine_Ref();\n\t}\n\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\tif (render_state.vertex_buffers[i]) {\n\t\t\trender_state.vertex_buffers[i]->Release_Engine_Ref();\n\t\t}\n\t}\n\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\tREF_PTR_RELEASE(render_state.vertex_buffers[i]);\n\t}\n\tREF_PTR_RELEASE(render_state.index_buffer);\n\tREF_PTR_RELEASE(render_state.material);\n\n\t\n\tfor (i=0;i<MAX_TEXTURE_STAGES;++i) \n\t{\n\t\tREF_PTR_RELEASE(render_state.Textures[i]);\n\t}\n}\n\n\nWWINLINE RenderStateStruct::RenderStateStruct()\n\t:\n\tmaterial(0),\n\tindex_buffer(0)\n{\n\tunsigned i;\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) vertex_buffers[i]=0;\n\tfor (i=0;i<MAX_TEXTURE_STAGES;++i) Textures[i]=0;\n}\n\nWWINLINE RenderStateStruct::~RenderStateStruct()\n{\n\tunsigned i;\n\tREF_PTR_RELEASE(material);\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\tREF_PTR_RELEASE(vertex_buffers[i]);\n\t}\n\tREF_PTR_RELEASE(index_buffer);\n\n\tfor (i=0;i<MAX_TEXTURE_STAGES;++i) \n\t{\n\t\tREF_PTR_RELEASE(Textures[i]);\n\t}\n}\n\n\nWWINLINE RenderStateStruct& RenderStateStruct::operator= (const RenderStateStruct& src)\n{\n\tunsigned i;\n\tREF_PTR_SET(material,src.material);\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\tREF_PTR_SET(vertex_buffers[i],src.vertex_buffers[i]);\n\t}\n\tREF_PTR_SET(index_buffer,src.index_buffer);\n\n\tfor (i=0;i<MAX_TEXTURE_STAGES;++i) \n\t{\n\t\tREF_PTR_SET(Textures[i],src.Textures[i]);\n\t}\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream (Set_Render_State, Release_Render_State and RenderStateStruct as Zero Hour's)"),
        ('\tvertex_buffer_type=src.vertex_buffer_type;\n',
         '\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\tvertex_buffer_types[i]=src.vertex_buffer_types[i];\n\t}\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('class LightEnvironmentClass;\n',
         'class LightEnvironmentClass;\nclass RenderInfoClass;\n',
         "Forward declaration for Zero Hour's Render_Info"),
        ('\tstatic void Set_Light_Environment(LightEnvironmentClass* light_env);\n',
         '\tstatic void Set_Light_Environment(LightEnvironmentClass* light_env, bool unknown_set_lights=true);\n',
         "The client's Set_Light_Environment (0x926460) takes a second argument (E&B's; no EA copy has it): it always stores light_env in Zero Hour's Light_Environment (0xC00648) and sets the device lights only when the flag is set; mesh (0x93F639) and dx8renderer (0x97A32D) pass 1, ShdDX8RendererNodeClass::Flush (0xA089C2) passes 0. The body is otherwise Renegade's. Parameter name unknown (D10)"),
        ('\tstatic DWORD Get_Vertex_Processing_Behavior() { return Vertex_Processing_Behavior; }\n',
         "\tstatic DWORD Get_Vertex_Processing_Behavior() { return Vertex_Processing_Behavior; }\n\n\t// E&B: sets Zero Hour's otherwise unused Render_Info (ShdDX8RendererNodeClass::Flush, 0xA089D9)\n\tstatic void Unknown_Set_Render_Info(RenderInfoClass* rinfo) { Render_Info=rinfo; }\n",
         "Zero Hour's Render_Info (0xC0064C), set by ShdDX8RendererNodeClass::Flush (0xA089D9) before Apply_Instance; the setter is E&B's (no EA copy has one), name unknown (D10)"),
        ('\tstatic Vector4\t\t\t\t\t\t\tPixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];\n\n\tstatic DWORD\t\t\t\t\t\t\tVertex_Processing_Behavior;\n',
         '\tstatic Vector4\t\t\t\t\t\t\tPixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];\n\n\tstatic LightEnvironmentClass*\t\tLight_Environment;\n\tstatic RenderInfoClass*\t\t\t\tRender_Info;\n\n\tstatic DWORD\t\t\t\t\t\t\tVertex_Processing_Behavior;\n',
         "Zero Hour's Light_Environment (0xC00648) and Render_Info (0xC0064C)"),
        ('const unsigned MAX_TEXTURE_STAGES=2;\n',
         'const unsigned MAX_TEXTURE_STAGES=8;\n',
         "Zero Hour's MAX_TEXTURE_STAGES=8: the client's render_state.Textures runs 0xBFFC88-0xBFFCA8 (8 pointers, Lights follow at 0xBFFCA8, Set_Light_Environment 0x9265CB), Invalidate_Cached_Render_States (0x921C20) fills 0x100 dwords of TextureStageStates (8x32), ShdSubMeshClass keeps UV[8] at +0x74 (ShdDX8RendererNodeClass constructor, 0xA083BE)"),
    ],
    ('ww3d2', 'dx8wrapper.cpp'): [
        ('ZTextureClass*\t\t\t\t\tDX8Wrapper::Shadow_Map[MAX_SHADOW_MAPS];\n',
         'ZTextureClass*\t\t\t\t\tDX8Wrapper::Shadow_Map[MAX_SHADOW_MAPS];\n\nVector3\t\t\t\t\t\t\tDX8Wrapper::Ambient_Color;\n',
         "Zero Hour's Ambient_Color (client 0xC00530)"),
        ('const char* DX8Wrapper::Get_DX8_Render_State_Name(D3DRENDERSTATETYPE state)\n',
         '//**********************************************************************************************\n//! Resets render device to default state\n/*!\n*/\nvoid DX8Wrapper::Apply_Default_State()\n{\n\tSNAPSHOT_SAY(("DX8Wrapper::Apply_Default_State()\\n"));\n\t\n\t// only set states used in game\n\tSet_DX8_Render_State(D3DRS_ZENABLE, TRUE);\n//\tSet_DX8_Render_State(D3DRS_FILLMODE, D3DFILL_SOLID);\n\tSet_DX8_Render_State(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);\n\t//Set_DX8_Render_State(D3DRS_LINEPATTERN, 0);\n\tSet_DX8_Render_State(D3DRS_ZWRITEENABLE, TRUE);\n\tSet_DX8_Render_State(D3DRS_ALPHATESTENABLE, FALSE);\n\t//Set_DX8_Render_State(D3DRS_LASTPIXEL, FALSE);\n\tSet_DX8_Render_State(D3DRS_SRCBLEND, D3DBLEND_ONE);\n\tSet_DX8_Render_State(D3DRS_DESTBLEND, D3DBLEND_ZERO);\n\tSet_DX8_Render_State(D3DRS_CULLMODE, D3DCULL_CW);\n\tSet_DX8_Render_State(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);\n\tSet_DX8_Render_State(D3DRS_ALPHAREF, 0);\n\tSet_DX8_Render_State(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);\n\tSet_DX8_Render_State(D3DRS_DITHERENABLE, FALSE);\n\tSet_DX8_Render_State(D3DRS_ALPHABLENDENABLE, FALSE);\n\tSet_DX8_Render_State(D3DRS_FOGENABLE, FALSE);\n\tSet_DX8_Render_State(D3DRS_SPECULARENABLE, FALSE);\n//\tSet_DX8_Render_State(D3DRS_ZVISIBLE, FALSE);\n//\tSet_DX8_Render_State(D3DRS_FOGCOLOR, 0);\n//\tSet_DX8_Render_State(D3DRS_FOGTABLEMODE, D3DFOG_NONE);\n//\tSet_DX8_Render_State(D3DRS_FOGSTART, 0);\n\n//\tSet_DX8_Render_State(D3DRS_FOGEND, WWMath::Float_As_Int(1.0f));\n//\tSet_DX8_Render_State(D3DRS_FOGDENSITY, WWMath::Float_As_Int(1.0f));\n\n\t//Set_DX8_Render_State(D3DRS_EDGEANTIALIAS, FALSE);\n\tSet_DX8_Render_State(D3DRS_ZBIAS, 0);\n//\tSet_DX8_Render_State(D3DRS_RANGEFOGENABLE, FALSE);\n\tSet_DX8_Render_State(D3DRS_STENCILENABLE, FALSE);\n\tSet_DX8_Render_State(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);\n\tSet_DX8_Render_State(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);\n\tSet_DX8_Render_State(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);\n\tSet_DX8_Render_State(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);\n\tSet_DX8_Render_State(D3DRS_STENCILREF, 0);\n\tSet_DX8_Render_State(D3DRS_STENCILMASK, 0xffffffff);\n\tSet_DX8_Render_State(D3DRS_STENCILWRITEMASK, 0xffffffff);\n\tSet_DX8_Render_State(D3DRS_TEXTUREFACTOR, 0);\n/*\tSet_DX8_Render_State(D3DRS_WRAP0, D3DWRAP_U| D3DWRAP_V);\n\tSet_DX8_Render_State(D3DRS_WRAP1, D3DWRAP_U| D3DWRAP_V);\n\tSet_DX8_Render_State(D3DRS_WRAP2, D3DWRAP_U| D3DWRAP_V);\n\tSet_DX8_Render_State(D3DRS_WRAP3, D3DWRAP_U| D3DWRAP_V);\n\tSet_DX8_Render_State(D3DRS_WRAP4, D3DWRAP_U| D3DWRAP_V);\n\tSet_DX8_Render_State(D3DRS_WRAP5, D3DWRAP_U| D3DWRAP_V);\n\tSet_DX8_Render_State(D3DRS_WRAP6, D3DWRAP_U| D3DWRAP_V);\n\tSet_DX8_Render_State(D3DRS_WRAP7, D3DWRAP_U| D3DWRAP_V);*/\n\tSet_DX8_Render_State(D3DRS_CLIPPING, TRUE);\n\tSet_DX8_Render_State(D3DRS_LIGHTING, FALSE);\n\t//Set_DX8_Render_State(D3DRS_AMBIENT, 0);\n//\tSet_DX8_Render_State(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);\n\tSet_DX8_Render_State(D3DRS_COLORVERTEX, TRUE);\n/*\tSet_DX8_Render_State(D3DRS_LOCALVIEWER, TRUE);\n\tSet_DX8_Render_State(D3DRS_NORMALIZENORMALS, FALSE);\n\tSet_DX8_Render_State(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);\n\tSet_DX8_Render_State(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);\n\tSet_DX8_Render_State(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);\n\tSet_DX8_Render_State(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);\n\tSet_DX8_Render_State(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);*/\n\t//Set_DX8_Render_State(D3DRS_CLIPPLANEENABLE, 0);\n\tSet_DX8_Render_State(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);\n\t//Set_DX8_Render_State(D3DRS_POINTSIZE, 0x3f800000);\n\t//Set_DX8_Render_State(D3DRS_POINTSIZE_MIN, 0);\n\t//Set_DX8_Render_State(D3DRS_POINTSPRITEENABLE, FALSE);\n\t//Set_DX8_Render_State(D3DRS_POINTSCALEENABLE, FALSE);\n\t//Set_DX8_Render_State(D3DRS_POINTSCALE_A, 0);\n\t//Set_DX8_Render_State(D3DRS_POINTSCALE_B, 0);\n\t//Set_DX8_Render_State(D3DRS_POINTSCALE_C, 0);\n\t//Set_DX8_Render_State(D3DRS_MULTISAMPLEANTIALIAS, TRUE);\n\t//Set_DX8_Render_State(D3DRS_MULTISAMPLEMASK, 0xffffffff);\n\t//Set_DX8_Render_State(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);\n\t//Set_DX8_Render_State(D3DRS_PATCHSEGMENTS, 0x3f800000);\n\t//Set_DX8_Render_State(D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE);\n\t//Set_DX8_Render_State(D3DRS_POINTSIZE_MAX, Float_At_Int(64.0f));\n\t//Set_DX8_Render_State(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);\n\tSet_DX8_Render_State(D3DRS_COLORWRITEENABLE, 0x0000000f);\n\t//Set_DX8_Render_State(D3DRS_TWEENFACTOR, 0);\n\tSet_DX8_Render_State(D3DRS_BLENDOP, D3DBLENDOP_ADD);\n\t//Set_DX8_Render_State(D3DRS_POSITIONORDER, D3DORDER_CUBIC);\n\t//Set_DX8_Render_State(D3DRS_NORMALORDER, D3DORDER_LINEAR);\n\n\t// disable TSS stages\n\tint i;\n\tfor (i=0; i<CurrentCaps->Get_Max_Textures_Per_Pass(); i++)\n\t{\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_COLOROP, D3DTOP_DISABLE);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_COLORARG2, D3DTA_DIFFUSE);\n\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);\n\t\n\t\t/*Set_DX8_Texture_Stage_State(i, D3DTSS_BUMPENVMAT00, 0);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_BUMPENVMAT01, 0);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_BUMPENVMAT10, 0);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_BUMPENVMAT11, 0);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_BUMPENVLSCALE, 0);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_BUMPENVLOFFSET, 0);*/\n\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_TEXCOORDINDEX, i);\n\t\t\n\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_BORDERCOLOR, 0);\n//\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);\n//\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);\n//\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);\n//\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_MIPMAPLODBIAS, 0);\n//\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_MAXMIPLEVEL, 0);\n//\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_MAXANISOTROPY, 1);\n\t\t//Set_DX8_Texture_Stage_State(i, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);\n\t\t//Set_DX8_Texture_Stage_State(i, D3DTSS_COLORARG0, D3DTA_CURRENT);\n\t\t//Set_DX8_Texture_Stage_State(i, D3DTSS_ALPHAARG0, D3DTA_CURRENT);\n\t\t//Set_DX8_Texture_Stage_State(i, D3DTSS_RESULTARG, D3DTA_CURRENT);\n\n\t\tSet_DX8_Texture_Stage_State(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);\n\t\tSet_Texture(i,NULL);\n\t}\n\n//\tDX8Wrapper::Set_Material(NULL);\n\tVertexMaterialClass::Apply_Null();\n\n\tfor (unsigned index=0;index<4;++index) {\n\t\tSNAPSHOT_SAY(("Clearing light %d to NULL\\n",index));\n\t\tSet_DX8_Light(index,NULL);\n\t}\n\n\t// set up simple default TSS \n\tVector4 vconst[MAX_VERTEX_SHADER_CONSTANTS];\n\tmemset(vconst,0,sizeof(Vector4)*MAX_VERTEX_SHADER_CONSTANTS);\n\tSet_Vertex_Shader_Constant(0, vconst, MAX_VERTEX_SHADER_CONSTANTS);\n\n\tVector4 pconst[MAX_PIXEL_SHADER_CONSTANTS];\n\tmemset(pconst,0,sizeof(Vector4)*MAX_PIXEL_SHADER_CONSTANTS);\n\tSet_Pixel_Shader_Constant(0, pconst, MAX_PIXEL_SHADER_CONSTANTS);\n\n\tSet_Vertex_Shader(DX8_FVF_XYZNDUV2);\n\tSet_Pixel_Shader(0);\n\n\tShaderClass::Invalidate();\n}\n\nconst char* DX8Wrapper::Get_DX8_Render_State_Name(D3DRENDERSTATETYPE state)\n',
         "Zero Hour's Apply_Default_State, the client's 0x9270D0"),
        ('//\tDX8Wrapper::Set_Material(NULL);\n\tVertexMaterialClass::Apply_Null();\n\n\tfor (unsigned index=0;index<4;++index) {\n\t\tSNAPSHOT_SAY(("Clearing light %d to NULL\\n",index));\n\t\tSet_DX8_Light(index,NULL);\n\t}\n\n\t// set up simple default TSS \n',
         '\tDX8Wrapper::Set_Material(NULL);\n\n\t// set up simple default TSS \n',
         "Apply_Default_State as the client has it (0x927546): Set_Material(NULL) (its out-of-line copy 0x47F470), no VertexMaterialClass::Apply_Null and no loop clearing four lights (Zero Hour's later form)"),
        ('\tSet_Pixel_Shader(0);\n\n\tShaderClass::Invalidate();\n}\n',
         '\tSet_Pixel_Shader(0);\n}\n',
         'Apply_Default_State as the client has it: it ends after Set_Pixel_Shader (0x9275C2), without ShaderClass::Invalidate'),
        ('void DX8Wrapper::Apply_Default_State()\n{\n\tSNAPSHOT_SAY(("DX8Wrapper::Apply_Default_State()\\n"));\n\t\n',
         'void DX8Wrapper::Apply_Default_State()\n{\n',
         "Apply_Default_State without Zero Hour's opening SNAPSHOT_SAY: with it VC6 inlines one more Set_DX8_Render_State (ZFUNC) and one fewer Set_Texture than the client's 0x9270D0; without it the function is byte-exact"),
        ('\t** Shutdown ww3d systems\n\t*/\n\tif (render_state.vertex_buffer) render_state.vertex_buffer->Release_Engine_Ref();\n\tREF_PTR_RELEASE(render_state.vertex_buffer);\n',
         '\t** Shutdown ww3d systems\n\t*/\n\tint i;\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\tif (render_state.vertex_buffers[i]) render_state.vertex_buffers[i]->Release_Engine_Ref();\n\t\tREF_PTR_RELEASE(render_state.vertex_buffers[i]);\n\t}\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\t\tif (render_state.vertex_buffer) render_state.vertex_buffer->Release_Engine_Ref();\n\t\tREF_PTR_RELEASE(render_state.vertex_buffer);\n',
         '\t\tfor (unsigned i=0;i<MAX_VERTEX_STREAMS;++i) \n\t\t{\n\t\t\tif (render_state.vertex_buffers[i]) render_state.vertex_buffers[i]->Release_Engine_Ref();\n\t\t\tREF_PTR_RELEASE(render_state.vertex_buffers[i]);\n\t\t}\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('void DX8Wrapper::Set_Vertex_Buffer(const VertexBufferClass* vb)\n{\n\trender_state.vba_offset=0;\n\trender_state.vba_count=0;\n\tif (render_state.vertex_buffer) {\n\t\trender_state.vertex_buffer->Release_Engine_Ref();\n\t}\n\tREF_PTR_SET(render_state.vertex_buffer,const_cast<VertexBufferClass*>(vb));\n\tif (vb) {\n\t\tvb->Add_Engine_Ref();\n\t\trender_state.vertex_buffer_type=vb->Type();\n\t}\n\telse {\n\t\trender_state.index_buffer_type=BUFFER_TYPE_INVALID;\n\t}\n',
         'void DX8Wrapper::Set_Vertex_Buffer(const VertexBufferClass* vb, unsigned stream)\n{\n\trender_state.vba_offset=0;\n\trender_state.vba_count=0;\n\tif (render_state.vertex_buffers[stream]) {\n\t\trender_state.vertex_buffers[stream]->Release_Engine_Ref();\n\t}\n\tREF_PTR_SET(render_state.vertex_buffers[stream],const_cast<VertexBufferClass*>(vb));\n\tif (vb) {\n\t\tvb->Add_Engine_Ref();\n\t\trender_state.vertex_buffer_types[stream]=vb->Type();\n\t}\n\telse {\n\t\trender_state.vertex_buffer_types[stream]=BUFFER_TYPE_INVALID;\n\t}\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream; a NULL buffer marks its own stream's type invalid (0x924E99), not the index buffer's as in Renegade"),
        ('void DX8Wrapper::Set_Vertex_Buffer(const DynamicVBAccessClass& vba_)\n{\n\tif (render_state.vertex_buffer) render_state.vertex_buffer->Release_Engine_Ref();\n\n\tDynamicVBAccessClass& vba=const_cast<DynamicVBAccessClass&>(vba_);\n\trender_state.vertex_buffer_type=vba.Get_Type();\n\trender_state.vba_offset=vba.VertexBufferOffset;\n\trender_state.vba_count=vba.Get_Vertex_Count();\n\tREF_PTR_SET(render_state.vertex_buffer,vba.VertexBuffer);\n\trender_state.vertex_buffer->Add_Engine_Ref();\n',
         'void DX8Wrapper::Set_Vertex_Buffer(const DynamicVBAccessClass& vba_)\n{\n\t// Release all streams (only one stream allowed in the legacy pipeline)\n\tfor (int i=1;i<MAX_VERTEX_STREAMS;++i) {\n\t\tDX8Wrapper::Set_Vertex_Buffer(NULL, i);\n\t}\n\n\tif (render_state.vertex_buffers[0]) render_state.vertex_buffers[0]->Release_Engine_Ref();\n\tDynamicVBAccessClass& vba=const_cast<DynamicVBAccessClass&>(vba_);\n\trender_state.vertex_buffer_types[0]=vba.Get_Type();\n\trender_state.vba_offset=vba.VertexBufferOffset;\n\trender_state.vba_count=vba.Get_Vertex_Count();\n\tREF_PTR_SET(render_state.vertex_buffers[0],vba.VertexBuffer);\n\trender_state.vertex_buffers[0]->Add_Engine_Ref();\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream; the client's Set_Vertex_Buffer(DynamicVBAccessClass) (0x924F60) first releases stream 1 with Set_Vertex_Buffer(NULL, 1)"),
        ('\tWWASSERT(render_state.vertex_buffer_type==BUFFER_TYPE_SORTING || render_state.vertex_buffer_type==BUFFER_TYPE_DYNAMIC_SORTING);\n',
         '\tWWASSERT(render_state.vertex_buffer_types[0]==BUFFER_TYPE_SORTING || render_state.vertex_buffer_types[0]==BUFFER_TYPE_DYNAMIC_SORTING);\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\t\tVertexFormatXYZNDUV2* src = static_cast<SortingVertexBufferClass*>(render_state.vertex_buffer)->VertexBuffer;\n',
         '\t\tVertexFormatXYZNDUV2* src = static_cast<SortingVertexBufferClass*>(render_state.vertex_buffers[0])->VertexBuffer;\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\t\tswitch (render_state.vertex_buffer_type) {\n',
         '\t\tswitch (render_state.vertex_buffer_types[0]) {\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\t\t\tvertex_count=render_state.vertex_buffer->Get_Vertex_Count()-render_state.index_base_offset-render_state.vba_offset-min_vertex_index;\n',
         '\t\t\tvertex_count=render_state.vertex_buffers[0]->Get_Vertex_Count()-render_state.index_base_offset-render_state.vba_offset-min_vertex_index;\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\tswitch (render_state.vertex_buffer_type) {\n',
         '\tswitch (render_state.vertex_buffer_types[0]) {\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\t\tif (render_state.vertex_buffer) {\n\t\t\tswitch (render_state.vertex_buffer_type) {//->Type()) {\n\t\t\tcase BUFFER_TYPE_DX8:\n\t\t\tcase BUFFER_TYPE_DYNAMIC_DX8:\n\t\t\t\tDX8CALL(SetStreamSource(\n\t\t\t\t\t0,\n\t\t\t\t\tstatic_cast<DX8VertexBufferClass*>(render_state.vertex_buffer)->Get_DX8_Vertex_Buffer(),\n\t\t\t\t\trender_state.vertex_buffer->FVF_Info().Get_FVF_Size()));\n\t\t\t\tDX8_RECORD_VERTEX_BUFFER_CHANGE();\n\t\t\t\tDX8CALL(SetVertexShader(render_state.vertex_buffer->FVF_Info().Get_FVF()));\n\t\t\t\tbreak;\n\t\t\tcase BUFFER_TYPE_SORTING:\n\t\t\tcase BUFFER_TYPE_DYNAMIC_SORTING:\n\t\t\t\tbreak;\n\t\t\tdefault:\n\t\t\t\tWWASSERT(0);\n\t\t\t}\n\t\t} else {\n\t\t\tDX8CALL(SetStreamSource(0,NULL,0));\n\t\t\tDX8_RECORD_VERTEX_BUFFER_CHANGE();\n\t\t}\n',
         '\t\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\t\tif (render_state.vertex_buffers[i]) {\n\t\t\t\tswitch (render_state.vertex_buffer_types[i]) {//->Type()) {\n\t\t\t\tcase BUFFER_TYPE_DX8:\n\t\t\t\tcase BUFFER_TYPE_DYNAMIC_DX8:\n\t\t\t\t\tDX8CALL(SetStreamSource(\n\t\t\t\t\t\ti,\n\t\t\t\t\t\tstatic_cast<DX8VertexBufferClass*>(render_state.vertex_buffers[i])->Get_DX8_Vertex_Buffer(),\n\t\t\t\t\t\trender_state.vertex_buffers[i]->FVF_Info().Get_FVF_Size()));\n\t\t\t\t\tDX8_RECORD_VERTEX_BUFFER_CHANGE();\n\t\t\t\t\t{\n\t\t\t\t\t\t// If the VB format is FVF, set the FVF as a vertex shader\n\t\t\t\t\t\tunsigned fvf=render_state.vertex_buffers[i]->FVF_Info().Get_FVF();\n\t\t\t\t\t\tif (fvf!=0) {\n\t\t\t\t\t\t\tSet_Vertex_Shader(fvf);\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t\tbreak;\n\t\t\t\tcase BUFFER_TYPE_SORTING:\n\t\t\t\tcase BUFFER_TYPE_DYNAMIC_SORTING:\n\t\t\t\t\tbreak;\n\t\t\t\tdefault:\n\t\t\t\t\tWWASSERT(0);\n\t\t\t\t}\n\t\t\t} else {\n\t\t\t\tDX8CALL(SetStreamSource(i,NULL,0));\n\t\t\t\tDX8_RECORD_VERTEX_BUFFER_CHANGE();\n\t\t\t}\n\t\t}\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream (Apply_Render_State_Changes sets each stream; an FVF buffer sets its FVF through Set_Vertex_Shader)"),
        ('\tREF_PTR_RELEASE(render_state.material);\n\tfor (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_RELEASE(render_state.Textures[i]);\n\n\tTextureLoader::Deinit();\n',
         '\tREF_PTR_RELEASE(render_state.material);\n\tfor (i=0;i<MAX_TEXTURE_STAGES;++i) REF_PTR_RELEASE(render_state.Textures[i]);\n\n\tTextureLoader::Deinit();\n',
         "The stream loop's int i (Zero Hour's) is reused by the texture loop, as in Zero Hour; VC6 leaks for-loop variables, so a second declaration does not compile"),
        ('Vector4\t\t\t\t\t\t\tDX8Wrapper::Pixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];\n\n',
         'Vector4\t\t\t\t\t\t\tDX8Wrapper::Pixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];\n\nLightEnvironmentClass*\t\tDX8Wrapper::Light_Environment\t\t\t\t\t\t\t= NULL;\nRenderInfoClass*\t\t\t\tDX8Wrapper::Render_Info\t\t\t\t\t\t\t\t\t= NULL;\n\n',
         "Zero Hour's Light_Environment (0xC00648) and Render_Info (0xC0064C)"),
        ('void DX8Wrapper::Set_Light_Environment(LightEnvironmentClass* light_env)\n{\n\tif (light_env) {\n',
         'void DX8Wrapper::Set_Light_Environment(LightEnvironmentClass* light_env, bool unknown_set_lights)\n{\n\tLight_Environment=light_env;\n\n\tif (unknown_set_lights && light_env) {\n',
         "The client's Set_Light_Environment (0x926460) takes a second argument (E&B's; no EA copy has it): it always stores light_env in Zero Hour's Light_Environment (0xC00648) and sets the device lights only when the flag is set; mesh (0x93F639) and dx8renderer (0x97A32D) pass 1, ShdDX8RendererNodeClass::Flush (0xA089C2) passes 0. The body is otherwise Renegade's. Parameter name unknown (D10)"),
    ],
    ('ww3d2', 'ww3d.cpp'): [
        ('\tVector3 ambient = scene->Get_Ambient_Light();\n\tDX8Wrapper::Set_DX8_Render_State(D3DRS_AMBIENT, DX8Wrapper::Convert_Color(ambient,0.0f));\n',
         '\tDX8Wrapper::Set_Ambient(scene->Get_Ambient_Light());\n',
         "Zero Hour's WW3D::Render: the client (0x917E2F) stores the ambient in Ambient_Color (0xC00530) before D3DRS_AMBIENT"),
    ],
    ('wwshade', 'shdrenderer.cpp'): [
        ('\tDX8Wrapper::Apply_Render_State_Changes();\n\tDX8Wrapper::Invalidate_Cached_Render_States();\n\tDX8Wrapper::Apply_Default_State();\n',
         '\tDX8Wrapper::Invalidate_Cached_Render_States();\n\tDX8Wrapper::Apply_Default_State();\n\tDX8Wrapper::Apply_Render_State_Changes();\n',
         "The client's ShdDX8RendererClass::Flush (0xA08060) calls Invalidate_Cached_Render_States (0x921C20), Apply_Default_State (0x9270D0), then Apply_Render_State_Changes (0x9254A0)"),
    ],
    ('wwshade', 'shdmesh.cpp'): [
        ('\tif ((Is_Translucent()!=0) && (!raytest.CheckTranslucent)) return false;\n',
         '\tif (raytest.IgnoreTranslucentMeshes && Is_Translucent()!=0) return false;\n',
         "ShdMeshClass::Cast_Ray as the client has it (0xA09580): tests the ray's byte at +0x40 (Renegade's IgnoreTranslucentMeshes) before Is_Translucent; the client has no Zero Hour CheckTranslucent"),
    ],
    ('ww3d2', 'sortingrenderer.cpp'): [
        ('(state->sorting_state.vertex_buffer_type==BUFFER_TYPE_SORTING || state->sorting_state.vertex_buffer_type==BUFFER_TYPE_DYNAMIC_SORTING)));\n',
         '(state->sorting_state.vertex_buffer_types[0]==BUFFER_TYPE_SORTING || state->sorting_state.vertex_buffer_types[0]==BUFFER_TYPE_DYNAMIC_SORTING)));\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\n\tSortingVertexBufferClass* vertex_buffer=static_cast<SortingVertexBufferClass*>(state->sorting_state.vertex_buffer);\n',
         '\n\tSortingVertexBufferClass* vertex_buffer=static_cast<SortingVertexBufferClass*>(state->sorting_state.vertex_buffers[0]);\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('void Release_Refs(SortingNodeStruct* state)\n{\n\tREF_PTR_RELEASE(state->sorting_state.vertex_buffer);\n',
         'void Release_Refs(SortingNodeStruct* state)\n{\n\tint i;\n\tfor (i=0;i<MAX_VERTEX_STREAMS;++i) {\n\t\tREF_PTR_RELEASE(state->sorting_state.vertex_buffers[i]);\n\t}\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\t\t\tSortingVertexBufferClass* vertex_buffer=static_cast<SortingVertexBufferClass*>(state->sorting_state.vertex_buffer);\n',
         '\t\t\tSortingVertexBufferClass* vertex_buffer=static_cast<SortingVertexBufferClass*>(state->sorting_state.vertex_buffers[0]);\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\t\t\t(state->sorting_state.vertex_buffer_type==BUFFER_TYPE_SORTING || state->sorting_state.vertex_buffer_type==BUFFER_TYPE_DYNAMIC_SORTING)) {\n',
         '\t\t\t(state->sorting_state.vertex_buffer_types[0]==BUFFER_TYPE_SORTING || state->sorting_state.vertex_buffer_types[0]==BUFFER_TYPE_DYNAMIC_SORTING)) {\n',
         "Zero Hour's two vertex streams: the client's Set_Vertex_Buffer(vb, stream) at 0x924E20 keeps vertex_buffers[2] at 0xBFFEE0 and vertex_buffer_types[2] at 0xBFFECC (vba_offset 0xBFFED8, vba_count 0xBFFEDA); ShdDX8RendererNodeClass::Flush (0xA08790) sets one buffer per stream"),
        ('\tREF_PTR_RELEASE(state->sorting_state.material);\n\tfor (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) {\n',
         '\tREF_PTR_RELEASE(state->sorting_state.material);\n\tfor (i=0;i<MAX_TEXTURE_STAGES;++i) {\n',
         "The stream loop's int i (Zero Hour's) is reused by the texture loop, as in Zero Hour; VC6 leaks for-loop variables, so a second declaration does not compile"),
    ],
    ('ww3d2', 'dx8vertexbuffer.h'): [
        ('\tVertexBufferClass(unsigned type, unsigned FVF, unsigned short VertexCount);\n',
         '\tVertexBufferClass(unsigned type, unsigned FVF, unsigned short VertexCount, unsigned vertex_size=0);\n',
         "Zero Hour's 06/26/02 vertex size for non-FVF buffers: the client's DX8VertexBufferClass(FVF, count, usage, vertex_size) at 0x94F330 (ret 0x10) builds new FVFInfoClass(FVF, vertex_size) (0x9BA5D0); ShdDX8RendererNodeClass's constructor (0xA084BE) calls it with FVF 0. Without Zero Hour's W3DMPO base and W3DNEW"),
        ('\tDX8VertexBufferClass(unsigned FVF, unsigned short VertexCount, UsageType usage=USAGE_DEFAULT);\n',
         '\tDX8VertexBufferClass(unsigned FVF, unsigned short VertexCount, UsageType usage=USAGE_DEFAULT, unsigned vertex_size=0); // Vertex size not used with FVF formats\n',
         "Zero Hour's 06/26/02 vertex size for non-FVF buffers: the client's DX8VertexBufferClass(FVF, count, usage, vertex_size) at 0x94F330 (ret 0x10) builds new FVFInfoClass(FVF, vertex_size) (0x9BA5D0); ShdDX8RendererNodeClass's constructor (0xA084BE) calls it with FVF 0. Without Zero Hour's W3DMPO base and W3DNEW"),
    ],
    ('ww3d2', 'dx8vertexbuffer.cpp'): [
        ('VertexBufferClass::VertexBufferClass(unsigned type_, unsigned FVF, unsigned short vertex_count_)\n',
         'VertexBufferClass::VertexBufferClass(unsigned type_, unsigned FVF, unsigned short vertex_count_, unsigned vertex_size)\n',
         "Zero Hour's 06/26/02 vertex size for non-FVF buffers: the client's DX8VertexBufferClass(FVF, count, usage, vertex_size) at 0x94F330 (ret 0x10) builds new FVFInfoClass(FVF, vertex_size) (0x9BA5D0); ShdDX8RendererNodeClass's constructor (0xA084BE) calls it with FVF 0. Without Zero Hour's W3DMPO base and W3DNEW"),
        ('\tfvf_info=new FVFInfoClass(FVF);\n',
         '\tWWASSERT((FVF!=0 && vertex_size==0) || (FVF==0 && vertex_size!=0));\n\tfvf_info=new FVFInfoClass(FVF,vertex_size);\n',
         "Zero Hour's 06/26/02 vertex size for non-FVF buffers: the client's DX8VertexBufferClass(FVF, count, usage, vertex_size) at 0x94F330 (ret 0x10) builds new FVFInfoClass(FVF, vertex_size) (0x9BA5D0); ShdDX8RendererNodeClass's constructor (0xA084BE) calls it with FVF 0. Without Zero Hour's W3DMPO base and W3DNEW"),
        ('DX8VertexBufferClass::DX8VertexBufferClass(unsigned FVF, unsigned short vertex_count_, UsageType usage)\n',
         'DX8VertexBufferClass::DX8VertexBufferClass(unsigned FVF, unsigned short vertex_count_, UsageType usage, unsigned vertex_size)\n',
         "Zero Hour's 06/26/02 vertex size for non-FVF buffers: the client's DX8VertexBufferClass(FVF, count, usage, vertex_size) at 0x94F330 (ret 0x10) builds new FVFInfoClass(FVF, vertex_size) (0x9BA5D0); ShdDX8RendererNodeClass's constructor (0xA084BE) calls it with FVF 0. Without Zero Hour's W3DMPO base and W3DNEW"),
        ('\tVertexBufferClass(BUFFER_TYPE_DX8, FVF, vertex_count_),\n',
         '\tVertexBufferClass(BUFFER_TYPE_DX8, FVF, vertex_count_, vertex_size),\n',
         "Zero Hour's 06/26/02 vertex size for non-FVF buffers: the client's DX8VertexBufferClass(FVF, count, usage, vertex_size) at 0x94F330 (ret 0x10) builds new FVFInfoClass(FVF, vertex_size) (0x9BA5D0); ShdDX8RendererNodeClass's constructor (0xA084BE) calls it with FVF 0. Without Zero Hour's W3DMPO base and W3DNEW"),
    ],
    ('ww3d2', 'dx8renderer.h'): [
        ('#include "dx8wrapper.h"\n',
         '#include "dx8wrapper.h"\n#include "meshmatdesc.h"\n',
         "dx8renderer keeps 2 texture stages through MeshMatDescClass::MAX_TEX_STAGES, as Zero Hour changed it when MAX_TEXTURE_STAGES became 8 (its own copy's comment); with MAX_TEXTURE_STAGES there, 11 dx8renderer functions stop matching"),
        ('\tTextureClass *\t\t\t\t\t\t\t\t\ttextures[MAX_TEXTURE_STAGES];\n',
         '\tTextureClass *\t\t\t\t\t\t\t\t\ttextures[MeshMatDescClass::MAX_TEX_STAGES];\n',
         "dx8renderer keeps 2 texture stages through MeshMatDescClass::MAX_TEX_STAGES, as Zero Hour changed it when MAX_TEXTURE_STAGES became 8 (its own copy's comment); with MAX_TEXTURE_STAGES there, 11 dx8renderer functions stop matching"),
    ],
    ('ww3d2', 'dx8renderer.cpp'): [
        ('\tfor (int a=0;a<MAX_TEXTURE_STAGES;++a) {\n\t\ttextures[a]=NULL;\n',
         '\tfor (int a=0;a<MeshMatDescClass::MAX_TEX_STAGES;++a) \n\t{\n\t\ttextures[a]=NULL;\n',
         "dx8renderer keeps 2 texture stages through MeshMatDescClass::MAX_TEX_STAGES, as Zero Hour changed it when MAX_TEXTURE_STAGES became 8 (its own copy's comment); with MAX_TEXTURE_STAGES there, 11 dx8renderer functions stop matching"),
        ('\tfor (int a=0;a<MAX_TEXTURE_STAGES;++a) {\n\t\tREF_PTR_RELEASE(textures[a]);\n',
         '\tfor (int a=0;a<MeshMatDescClass::MAX_TEX_STAGES;++a) \n\t{\n\t\tREF_PTR_RELEASE(textures[a]);\n',
         "dx8renderer keeps 2 texture stages through MeshMatDescClass::MAX_TEX_STAGES, as Zero Hour changed it when MAX_TEXTURE_STAGES became 8 (its own copy's comment); with MAX_TEXTURE_STAGES there, 11 dx8renderer functions stop matching"),
        ('\tfor (int stage=0;stage<MAX_TEXTURE_STAGES;++stage) {\n\t\twork2.Format(',
         '\tfor (int stage=0;stage<MeshMatDescClass::MAX_TEX_STAGES;++stage) {\n\t\twork2.Format(',
         "dx8renderer keeps 2 texture stages through MeshMatDescClass::MAX_TEX_STAGES, as Zero Hour changed it when MAX_TEXTURE_STAGES became 8 (its own copy's comment); with MAX_TEXTURE_STAGES there, 11 dx8renderer functions stop matching"),
        ('\t\t\tWWASSERT(MAX_TEXTURE_STAGES==MeshMatDescClass::MAX_TEX_STAGES);\n',
         '\t\t\t// disabled this assert as MAX_TEXTURE_STAGES is now 8, but legacy MeshMat::MAX_TEX_STAGES is still 2\n\t//\t\tWWASSERT(MAX_TEXTURE_STAGES==MeshMatDescClass::MAX_TEX_STAGES);\n',
         "dx8renderer keeps 2 texture stages through MeshMatDescClass::MAX_TEX_STAGES, as Zero Hour changed it when MAX_TEXTURE_STAGES became 8 (its own copy's comment); with MAX_TEXTURE_STAGES there, 11 dx8renderer functions stop matching"),
        ('\t\tfor (unsigned i=0;i<MAX_TEXTURE_STAGES;++i) {\n\t\t\tSNAPSHOT_SAY(("Set_Texture(%d,%s)\\n"',
         '\t\tfor (unsigned i=0;i<MeshMatDescClass::MAX_TEX_STAGES;++i) \n\t\t{\n\t\t\tSNAPSHOT_SAY(("Set_Texture(%d,%s)\\n"',
         "dx8renderer keeps 2 texture stages through MeshMatDescClass::MAX_TEX_STAGES, as Zero Hour changed it when MAX_TEXTURE_STAGES became 8 (its own copy's comment); with MAX_TEXTURE_STAGES there, 11 dx8renderer functions stop matching"),
    ],
    ('ww3d2', 'dx8caps.h'): [
        ('\tint Get_Pixel_Shader_Minor_Version() const { return 0xff&(VertexShaderVersion); }',
         '\tint Get_Pixel_Shader_Minor_Version() const { return 0xff&(PixelShaderVersion); }',
         "Zero Hour's Get_Pixel_Shader_Minor_Version reads PixelShaderVersion (Renegade's reads the vertex shader's): ShdBumpDiffDefClass::Init 0xA0B9B0 and ShdBumpSpecDefClass::Init take major and minor from the same field, +0x12C"),
    ],
}
for _key, _edits in WWSHADE_API_PATCHES.items():
    PATCHES.setdefault(_key, []).extend(_edits)
# end of WWSHADE_API_PATCHES
# E&B's wwshade is earlier than Zero Hour's (D9, docs/progress.md): renderer without list containers or sorting, shader interface without class id or comparisons, sub-mesh without legacy conversion or skins, an E&B cube map shader; and the WW3D2/WWLib parts it needs (fill light, const reference counts). Each reason gives the client address. Applied after WWSHADE_API_PATCHES.
WWSHADE_EARLY_PATCHES = {
    ('ww3d2', 'lightenvironment.cpp'): [
        ('#include "light.h"\n\n',
         '#include "light.h"\n#include "colorspace.h"\n\n',
         "Zero Hour's fill light: Add_Fill_Light 0x985790 (without the later near-black test), Calculate_Fill_Light 0x985840, Pre_Render_Update 0x985660 (calls it first, no ambient clamp; OutputLightStruct::Init's direction guard inlined), Add_Light 0x985430 (sorted insertion, FillIntensity scaling; no near-black test, no point lights)"),
        ('\tMatrix3D::Inverse_Rotate_Vector(camera_tm,input.Direction,&Direction);\n}\t\n',
         '\tMatrix3D::Inverse_Rotate_Vector(camera_tm,input.Direction,&Direction);\n\t\n\t// Guard against a direction that is invalid\n\tif(Direction.Length2() == 0.0f) {\n\t\tDirection.X = 1.0f;\n\t}\n}\t\n',
         "Zero Hour's fill light: Add_Fill_Light 0x985790 (without the later near-black test), Calculate_Fill_Light 0x985840, Pre_Render_Update 0x985660 (calls it first, no ambient clamp; OutputLightStruct::Init's direction guard inlined), Add_Light 0x985430 (sorted insertion, FillIntensity scaling; no near-black test, no point lights)"),
        ('\tOutputAmbient(0,0,0)\n',
         '\tOutputAmbient(0,0,0),\n\tFillLight(),\n\tFillIntensity(0.0f)\n',
         "Zero Hour's fill light: Add_Fill_Light 0x985790 (without the later near-black test), Calculate_Fill_Light 0x985840, Pre_Render_Update 0x985660 (calls it first, no ambient clamp; OutputLightStruct::Init's direction guard inlined), Add_Light 0x985430 (sorted insertion, FillIntensity scaling; no near-black test, no point lights)"),
        ('\tnew_light.Init(light,ObjectCenter);\n',
         '\tnew_light.Init(light, ObjectCenter);\n\n\t// If we have the fill light set, we also want to the diffuse light to be modified by the intensity of the light source\n\tif(FillIntensity) new_light.Diffuse *= light.Get_Intensity();\n',
         "Zero Hour's fill light: Add_Fill_Light 0x985790 (without the later near-black test), Calculate_Fill_Light 0x985840, Pre_Render_Update 0x985660 (calls it first, no ambient clamp; OutputLightStruct::Init's direction guard inlined), Add_Light 0x985430 (sorted insertion, FillIntensity scaling; no near-black test, no point lights)"),
        ('\t\tif (LightCount < MAX_LIGHTS) {\n\t\t\tInputLights[LightCount] = new_light;\n\t\t\tLightCount++;\n\t\t} else {\n\t\t\tfor (int light_index=0; light_index<LightCount; light_index++) {\n\t\t\t\tif (new_light.Contribution() > InputLights[light_index].Contribution()) {\n\t\t\t\t\tInputLights[light_index] = new_light;\n\t\t\t\t\tlight_index = MAX_LIGHTS;\n\t\t\t\t}\n\t\t\t}\n',
         "\n\t\t// Insert the light into the sorted list of InputLights if it's contribution is greater than the any of the current number of lights\n\t\tfor (int light_index=0; light_index < LightCount; light_index++) {\n\t\t\tif (new_light.Contribution() > InputLights[light_index].Contribution()) {\n\t\t\t\t\n\t\t\t\t// Move back the lights in the InputLights Array to make space for the new light.\n\t\t\t\t// The last light might be discarded if it moves off the array as it is the weakest light in the list.\n\t\t\t\tfor (int i = LightCount; i > light_index; --i) {\n\t\t\t\t\tif (i < MAX_LIGHTS) {\n\t\t\t\t\t\tInputLights[i] = InputLights[i - 1];\n\t\t\t\t\t}\n\t\t\t\t}\n\n\t\t\t\t// Add the new light into the InputLights List where it belongs\n\t\t\t\tInputLights[light_index] = new_light;\n\n\t\t\t\t// Increment the light count if we have not reach the maximum lights limit yet\n\t\t\t\tLightCount = min(LightCount + 1, (int)MAX_LIGHTS);\n\n\t\t\t\t// Since we have inserted a new light, we are done for this function\n\t\t\t\treturn;\n\t\t\t}\n\t\t}\n\n\t\t// If the light was not inserted but there are still spots empty in the InputLights list, insert the lights at the end of the list\n\t\tif (LightCount < MAX_LIGHTS) {\n\t\t\tInputLights[LightCount] = new_light;\n\t\t\t++LightCount;\n",
         "Zero Hour's fill light: Add_Fill_Light 0x985790 (without the later near-black test), Calculate_Fill_Light 0x985840, Pre_Render_Update 0x985660 (calls it first, no ambient clamp; OutputLightStruct::Init's direction guard inlined), Add_Light 0x985430 (sorted insertion, FillIntensity scaling; no near-black test, no point lights)"),
        ('\tfor (int light_index=0; light_index<LightCount; light_index++) {\n\t\tOutputLights[light_index].Init(InputLights[light_index],camera_tm);\n\t}\n\n\tOutputAmbient.X = WWMath::Clamp(OutputAmbient.X);\n\tOutputAmbient.Y = WWMath::Clamp(OutputAmbient.Y);\n\tOutputAmbient.Z = WWMath::Clamp(OutputAmbient.Z);\n',
         '\t/*\n\t** Calculate and set up the fill light for the object\n\t*/\n\tCalculate_Fill_Light();\n\n\tfor (int light_index=0; light_index<LightCount; light_index++) {\n\t\tOutputLights[light_index].Init(InputLights[light_index],camera_tm);\n\t}\n',
         "Zero Hour's fill light: Add_Fill_Light 0x985790 (without the later near-black test), Calculate_Fill_Light 0x985840, Pre_Render_Update 0x985660 (calls it first, no ambient clamp; OutputLightStruct::Init's direction guard inlined), Add_Light 0x985430 (sorted insertion, FillIntensity scaling; no near-black test, no point lights)"),
        ('\treturn _LightingLODCutoff;\n}\n\n',
         "\treturn _LightingLODCutoff;\n}\n\n\n/************************************************************************************************\n**\n** LightEnvironmentClass::Add_Fill_Light Implementation\n** The fill light is inserted in the InputLights list as ont of the lights, and if the \n** list is already full, it preempts the last and weakest light in that list.\n**\n************************************************************************************************/\nvoid LightEnvironmentClass::Add_Fill_Light(void)\n{\n\t// Get the 1st empty light slot or the very last slot regardless of whether the last slot is empty or not\n\tint slot = 0;\n\tif (LightCount == MAX_LIGHTS) {\n\t\tslot = MAX_LIGHTS - 1;\n\t} else {\n\t\tslot = LightCount;\n\t\t++LightCount;\n\t}\n\n\t/*\n\t** Add in the ambient component\n\t*/\n\tOutputAmbient += FillLight.Ambient;\n\n\t/*\n\t** Insert the fill light into the calculated slot of the InputLights\n\t*/\n\tInputLights[slot] = FillLight;\n}\n\n/************************************************************************************************\n**\n** LightEnvironmentClass::Calculate_Fill_Light Implementation\n** The fill light takes up to the top 3 lights in the InputList and averages them into 1 light source.\n** The averaged light source is then flipped in direction and location as well as in HUE of the color.\n** This final light is used to support the top 3 lights by providing a calulated fill to augment the lights.\n**\n************************************************************************************************/\nvoid LightEnvironmentClass::Calculate_Fill_Light(void)\n{\t\n\t// Early exit if we have no lights at all or if the fill light intensity is zero\n\tif (LightCount == 0 || FillIntensity == 0.0f) return;\n\n\t// Initialize the averaged light to the primary light source (light with the most contribution)\n\tfloat primary_contribution = InputLights[0].Contribution();\n\tInputLightStruct average_light = InputLights[0]; \n\n\t// Loop through the remaining lights on the list (up to 2) and add their contributions to the averaged light\n\tint num_lights = min(LightCount, MAX_LIGHTS - 1); \n\tfor (int i = 1; i < num_lights; ++i) {\n\t\t\n\t\t// The ratio is the percentage of the remaining light's contribution compared to the primary light source\n\t\tfloat ratio = InputLights[i].Contribution() / primary_contribution;\n\t\t\n\t\taverage_light.Direction += (InputLights[i].Direction * ratio);\n\t\taverage_light.Ambient\t+= (InputLights[i].Ambient * ratio);\n\t\taverage_light.Diffuse\t+= (InputLights[i].Diffuse * ratio);\n\t}\n\t\n\t// Normalize the averaged light direction\n\taverage_light.Direction.Normalize();\n\t\n\t// Now we have the averaged light, we should derive the fill light \n\t// Convert from RGB to HSV to get the reserve hue and with the value modified by the fill intensity\n\tVector3 temp;\n\tRGB_To_HSV(temp, average_light.Diffuse);\n\ttemp.X += 180.0f;\t\t\t\t// Get the opposite hue from the averaged light\n\tif(temp.X > 360.0f) {\n\t\ttemp.X -= 360.0f;\n\t}\n\ttemp.Z *= FillIntensity;\t// fraction of the intensity\n\tHSV_To_RGB(FillLight.Diffuse, temp);\t\n\n\t// Zero out the fill ambient\n\tFillLight.Ambient.Set(0.0f, 0.0f, 0.0f);\n\n\t// now we set the fill light direction to be opposite the average light\n\tFillLight.Direction = average_light.Direction * (-1.0f);\t\t\n\tFillLight.DiffuseRejected = false;\n\n\t// Add the fill light into the InputLights list\n\tAdd_Fill_Light();\n}\n\n",
         "Zero Hour's fill light: Add_Fill_Light 0x985790 (without the later near-black test), Calculate_Fill_Light 0x985840, Pre_Render_Update 0x985660 (calls it first, no ambient clamp; OutputLightStruct::Init's direction guard inlined), Add_Light 0x985430 (sorted insertion, FillIntensity scaling; no near-black test, no point lights)"),
    ],
    ('ww3d2', 'lightenvironment.h'): [
        ('\tvoid\t\t\t\t\tPre_Render_Update(const Matrix3D & camera_tm);\n\n',
         '\tvoid\t\t\t\t\tPre_Render_Update(const Matrix3D & camera_tm);\n\tvoid\t\t\t\t\tAdd_Fill_Light(void);\n\tvoid\t\t\t\t\tCalculate_Fill_Light(void);\n\tvoid\t\t\t\t\tSet_Fill_Intensity(float intensity)\t\t\t{ FillIntensity = intensity; }\n\n',
         "Zero Hour's fill light (06/27/02 KM), without Generals' point-light fields: LightEnvironmentClass is 0x148 bytes (ShdDX8RendererNodeClass::Render 0xA08769 copies 0x52 dwords), the ctor 0x9853B0 zeroes FillIntensity at +0x144"),
        ('\n};\n',
         '\n\tInputLightStruct \tFillLight;\t\t\t\t\t\t// Used to store the calculated fill light\n\tfloat\t\t\t\t\tFillIntensity;\t\t\t\t\t// Used to determine how strong the fill light should be\n\n};\n',
         "Zero Hour's fill light (06/27/02 KM), without Generals' point-light fields: LightEnvironmentClass is 0x148 bytes (ShdDX8RendererNodeClass::Render 0xA08769 copies 0x52 dwords), the ctor 0x9853B0 zeroes FillIntensity at +0x144"),
    ],
    ('wwlib', 'refcount.h'): [
        ('\tWWINLINE void Add_Ref(void)\t\t\t\t\t\t\t\t\t\t{ NumRefs++; }\n#else\n\tvoid Add_Ref(void);\n',
         '\tWWINLINE void Add_Ref(void) const\t\t\t\t\t\t\t{ NumRefs++; }\n#else\n\tvoid Add_Ref(void) const;\n',
         "Zero Hour's const Add_Ref/Release_Ref/Num_Refs (mutable NumRefs): wwshade keeps const ShdDefClass pointers (ShdInterfaceClass ctor 0xA08E70 adds a reference to its const def); no code change elsewhere (all libraries verified)"),
        ('\tWWINLINE void\t\tRelease_Ref(void)\t\t\t\t\t\t\t{ \n',
         '\tWWINLINE void\t\tRelease_Ref(void) const\t\t\t\t\t{ \n',
         "Zero Hour's const Add_Ref/Release_Ref/Num_Refs (mutable NumRefs): wwshade keeps const ShdDefClass pointers (ShdInterfaceClass ctor 0xA08E70 adds a reference to its const def); no code change elsewhere (all libraries verified)"),
        ('\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tif (NumRefs == 0) Delete_This(); \n',
         '\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tif (NumRefs == 0) const_cast<RefCountClass*>(this)->Delete_This(); \n',
         "Zero Hour's const Add_Ref/Release_Ref/Num_Refs (mutable NumRefs): wwshade keeps const ShdDefClass pointers (ShdInterfaceClass ctor 0xA08E70 adds a reference to its const def); no code change elsewhere (all libraries verified)"),
        ('\tint\t\t\t\t\tNum_Refs(void)\t\t\t\t\t\t\t\t{ return NumRefs; }\n',
         '\tint\t\t\t\t\tNum_Refs(void) const\t\t\t\t\t\t{ return NumRefs; }\n',
         "Zero Hour's const Add_Ref/Release_Ref/Num_Refs (mutable NumRefs): wwshade keeps const ShdDefClass pointers (ShdInterfaceClass ctor 0xA08E70 adds a reference to its const def); no code change elsewhere (all libraries verified)"),
        ('\tint\t\t\t\t\tNumRefs;\n',
         '\tmutable int\t\t\tNumRefs;\n',
         "Zero Hour's const Add_Ref/Release_Ref/Num_Refs (mutable NumRefs): wwshade keeps const ShdDefClass pointers (ShdInterfaceClass ctor 0xA08E70 adds a reference to its const def); no code change elsewhere (all libraries verified)"),
        ('\tstatic void\t\t\tInc_Total_Refs(RefCountClass *);\n',
         '\tstatic void\t\t\tInc_Total_Refs(const RefCountClass *);\n',
         "Zero Hour's const Add_Ref/Release_Ref/Num_Refs (mutable NumRefs): wwshade keeps const ShdDefClass pointers (ShdInterfaceClass ctor 0xA08E70 adds a reference to its const def); no code change elsewhere (all libraries verified)"),
        ('\tstatic void\t\t\tDec_Total_Refs(RefCountClass *);\n',
         '\tstatic void\t\t\tDec_Total_Refs(const RefCountClass *);\n',
         "Zero Hour's const Add_Ref/Release_Ref/Num_Refs (mutable NumRefs): wwshade keeps const ShdDefClass pointers (ShdInterfaceClass ctor 0xA08E70 adds a reference to its const def); no code change elsewhere (all libraries verified)"),
    ],
    ('wwshade', 'shdclassids.h'): [
        ('\tSHDDEF_CLASSID_LEGACYW3D,\n',
         "\t// E&B: no LEGACYW3D (ShdRendererClass::Init_Shaders 0xA07E00 loops 1..5; MeshCategories\n\t// has 6 entries, 0xA07EC2), as shddump.h's Shader_ClassIDs\n",
         "no LegacyW3D shader: ShdRendererClass::Init_Shaders 0xA07E00 loops class ids 1..5, MeshCategories has 6 entries (0xA07EC2), SHD_Force_Links 0xA08C70 links 5 shaders; shddump.h's Shader_ClassIDs ends at CUBEMAP"),
    ],
    ('wwshade', 'shdcubemap.cpp'): [
        ('#include <d3dx8math.h>\n#include "dx8fvf.h"\n',
         '#include <d3dx8math.h>\n#include <d3dx8tex.h>\n#include "dx8fvf.h"\n',
         'E&B\'s cube map shader: registered as "Cube Map (unsupported)" (0xBCB814, 0xA0B1B0), texture from D3DXCreateCubeTextureFromFileA (ctor 0xA0AD80), released in the dtor (0xA0AEC0), set with SetTexture (0xA0B040); base ctor without class id'),
        ('REGISTER_SHDDEF(ShdCubeMapDefClass,SHDDEF_CLASSID_CUBEMAP,"Cube Map");\n',
         'REGISTER_SHDDEF(ShdCubeMapDefClass,SHDDEF_CLASSID_CUBEMAP,"Cube Map (unsupported)");\n',
         'E&B\'s cube map shader: registered as "Cube Map (unsupported)" (0xBCB814, 0xA0B1B0), texture from D3DXCreateCubeTextureFromFileA (ctor 0xA0AD80), released in the dtor (0xA0AEC0), set with SetTexture (0xA0B040); base ctor without class id'),
        (':\tShdInterfaceClass(def,SHDDEF_CLASSID_CUBEMAP),\n',
         ':\tShdInterfaceClass(def),\n',
         'E&B\'s cube map shader: registered as "Cube Map (unsupported)" (0xBCB814, 0xA0B1B0), texture from D3DXCreateCubeTextureFromFileA (ctor 0xA0AD80), released in the dtor (0xA0AEC0), set with SetTexture (0xA0B040); base ctor without class id'),
        ('\tTexture=WW3DAssetManager::Get_Instance()->Get_Texture\n\t(\n\t\tDefinition->Get_Texture_Name(),\n\t\tMIP_LEVELS_ALL,\n\t\tWW3D_FORMAT_UNKNOWN,\n\t\ttrue,\n\t\tTextureBaseClass::TEX_CUBEMAP\n',
         '\tD3DXCreateCubeTextureFromFile\n\t(\n\t\tDX8Wrapper::_Get_D3D_Device8(),\n\t\tDefinition->Get_Texture_Name(),\n\t\t&Texture\n',
         'E&B\'s cube map shader: registered as "Cube Map (unsupported)" (0xBCB814, 0xA0B1B0), texture from D3DXCreateCubeTextureFromFileA (ctor 0xA0AD80), released in the dtor (0xA0AEC0), set with SetTexture (0xA0B040); base ctor without class id'),
        ('\tREF_PTR_RELEASE(Texture);\n',
         '\tTexture->Release();\n',
         'E&B\'s cube map shader: registered as "Cube Map (unsupported)" (0xBCB814, 0xA0B1B0), texture from D3DXCreateCubeTextureFromFileA (ctor 0xA0AD80), released in the dtor (0xA0AEC0), set with SetTexture (0xA0B040); base ctor without class id'),
        ('\tDX8Wrapper::Set_Texture(0, Texture);\n',
         '\tDX8CALL(SetTexture(0, Texture));\n',
         'E&B\'s cube map shader: registered as "Cube Map (unsupported)" (0xBCB814, 0xA0B1B0), texture from D3DXCreateCubeTextureFromFileA (ctor 0xA0AD80), released in the dtor (0xA0AEC0), set with SetTexture (0xA0B040); base ctor without class id'),
        ('\tDX8Wrapper::Set_DX8_Material(Material);\n}\n',
         '\tDX8Wrapper::Set_DX8_Material(Material);\n\tShdHWVertexShader::Unknown_Set_Lights();\n}\n',
         "E&B's light setup for the fixed function shaders (0xA0FC60, after Light in shdhwshader.obj; name not known), called at the end of Shd6CubeMapClass::Apply_Shared 0xA0AF40, Shd6SimpleClass and Shd6GlossMaskClass::Apply_Shared (0xA0CD90, 0xA0D910)"),
        ('\tAmbient(1,1,1),\n\tDiffuse(1,1,1),\n\tSpecular(0,0,0)\n{\n\tNAMED_TEXTURE_FILENAME_PARAM',
         '\tAmbient(1,1,1),\n\tDiffuse(0,0,0),\n\tSpecular(0,0,0)\n{\n\tNAMED_TEXTURE_FILENAME_PARAM',
         "E&B's cube map definition: ShdCubeMapDefClass() 0xA0A840 sets Diffuse (+0x28) to 0,0,0"),
        ('\tAmbient(that.Ambient),\n\tDiffuse(that.Diffuse),\n\tSpecular(that.Specular)\n{\n\tTextureName=that.TextureName;\n}\n',
         '\tAmbient(1,1,1),\n\tDiffuse(0,0,0),\n\tSpecular(0,0,0)\n{\n\tTextureName=that.TextureName;\n\n\tAmbient=that.Ambient;\n\tDiffuse=that.Diffuse;\n\tSpecular=that.Specular;\n}\n',
         "ShdCubeMapDefClass(const&) 0xA0A900 initialises Ambient 1,1,1, Diffuse and Specular 0,0,0, then assigns from that (as ShdSimpleDefClass's copy ctor)"),
    ],
    ('wwshade', 'shdcubemap.h'): [
        ('\tTextureClass*\t\t\t\t\tTexture;\n',
         '\tIDirect3DCubeTexture8*\t\t\tTexture;\n',
         "E&B's cube map shader keeps a D3D cube texture (Shd6CubeMapClass dtor 0xA0AEC0 calls Texture->Release(); Apply_Instance 0xA0B040 sets it on the device)"),
    ],
    ('wwshade', 'shdglossmask.cpp'): [
        (':\tShdInterfaceClass(def,SHDDEF_CLASSID_GLOSSMASK),\n',
         ':\tShdInterfaceClass(def),\n',
         'base ctor without class id (ShdInterfaceClass ctor 0xA08E70 takes only def)'),
        ('\tDX8Wrapper::Set_DX8_Material(Material);\n\n}\n',
         '\tDX8Wrapper::Set_DX8_Material(Material);\n\tShdHWVertexShader::Unknown_Set_Lights();\n\n}\n',
         "E&B's light setup for the fixed function shaders (0xA0FC60, after Light in shdhwshader.obj; name not known), called at the end of Shd6CubeMapClass::Apply_Shared 0xA0AF40, Shd6SimpleClass and Shd6GlossMaskClass::Apply_Shared (0xA0CD90, 0xA0D910)"),
        (':\tShdDefClass(SHDDEF_CLASSID_GLOSSMASK),\n\tAmbient(1,1,1),\n',
         ':\tShdDefClass(SHDDEF_CLASSID_GLOSSMASK),\n\tAmbient(0,0,0),\n',
         "E&B's definitions start Ambient at 0,0,0: ShdGlossMaskDefClass() 0xA0C680 (+0x1C..0x24 zero)"),
        ('\tAmbient(that.Ambient),\n\tDiffuse(that.Diffuse),\n\tSpecular(that.Specular)\n{\n\tTextureName=that.TextureName;\n}\n',
         '\tAmbient(0,0,0),\n\tDiffuse(1,1,1),\n\tSpecular(1,1,1)\n{\n\tTextureName=that.TextureName;\n\n\tAmbient=that.Ambient;\n\tDiffuse=that.Diffuse;\n\tSpecular=that.Specular;\n}\n',
         "ShdGlossMaskDefClass(const&) 0xA0C740: copy constructor initialises the members to the defaults and assigns them from that in its body (as ShdSimpleDefClass's)"),
        ('\t\tDX8Wrapper::Set_Texture(1, Texture);\n\t}\n\n\tDX8Wrapper::Set_DX8_Material(Material);\n}\n',
         '\t\tDX8Wrapper::Set_Texture(1, Texture);\n\t}\n}\n',
         'Shd6GlossMaskClass::Apply_Instance 0xA0CF10 sets only the textures; the material is set in Apply_Shared 0xA0CD90'),
    ],
    ('wwshade', 'shdinterface.cpp'): [
        ('ShdInterfaceClass::ShdInterfaceClass(const ShdDefClass * def, int class_id) : \n\tDefinition(NULL),\n\tClassID(class_id)\n{ \n',
         'ShdInterfaceClass::ShdInterfaceClass(const ShdDefClass * def) :\n\tDefinition(NULL)\n{\n',
         'ShdInterfaceClass ctor without class id (0xA08E70)'),
    ],
    ('wwshade', 'shdinterface.h'): [
        ('\tShdInterfaceClass(const ShdDefClass * def,int class_id);\n\tvirtual ~ShdInterfaceClass(void);\n\t\n\tconst ShdDefClass *\t\tPeek_Definition(void);\t\n\n\tWWINLINE int\t\t\tGet_Class_ID() const { return ClassID; }\n\n\t// For rendering efficiency, all shaders should implement comparison operator that the renderer\n\t// can use to sort the meshes.\n\tvirtual bool Greater_Than\t(const ShdInterfaceClass& s,int pass) const { return true; }\n\tvirtual bool Similar_Enough\t(const ShdInterfaceClass& s,int pass) const { return true; }\n',
         "\t// E&B: no class id; the shader's type is its definition's (ShdInterfaceClass ctor 0xA08E70\n\t// takes only def, ret 4; Shd6CubeMapClass's Material at +0xC follows Definition at +8)\n\tShdInterfaceClass(const ShdDefClass * def);\n\tvirtual ~ShdInterfaceClass(void);\n\n\tconst ShdDefClass *\t\tPeek_Definition(void);\n\n\t// E&B: no Greater_Than/Similar_Enough (Shd6SimpleClass vtable 0xB16F0C: Get_Pass_Count in slot 2)\n",
         "E&B's earlier ShdInterfaceClass: ctor takes only def (0xA08E70, ret 4), no ClassID (Shd6CubeMapClass's Material at +0xC); no Greater_Than/Similar_Enough/Is_Opaque (Shd6SimpleClass vtable 0xB16F0C: Get_Pass_Count in slot 2, 12 slots)"),
        ('\t// Return whether this shader is opaque.  This property is used to determine whether\n\t// geometric shadows should be cast from an object for example.  Alpha-Test shaders should\n\t// return false to Is_Opaque.\n\tvirtual bool\t\t\t\tIs_Opaque(void) const { return true; }\n',
         "\t// E&B: no Is_Opaque (Shd6SimpleClass's table ends with Copy_Vertex_Stream, slot 11)\n",
         "E&B's earlier ShdInterfaceClass: ctor takes only def (0xA08E70, ret 4), no ClassID (Shd6CubeMapClass's Material at +0xC); no Greater_Than/Similar_Enough/Is_Opaque (Shd6SimpleClass vtable 0xB16F0C: Get_Pass_Count in slot 2, 12 slots)"),
        ('\tint\tClassID;\n',
         '',
         "E&B's earlier ShdInterfaceClass: ctor takes only def (0xA08E70, ret 4), no ClassID (Shd6CubeMapClass's Material at +0xC); no Greater_Than/Similar_Enough/Is_Opaque (Shd6SimpleClass vtable 0xB16F0C: Get_Pass_Count in slot 2, 12 slots)"),
    ],
    ('wwshade', 'shdlib.cpp'): [
        ('\tWW3DAssetManager::Get_Instance()->Register_Prototype_Loader(&_MeshLoader);\n',
         '',
         'SHD_Register_Loader registers only _ShdMeshLoader (0xA07D30: one Register_Prototype_Loader call)'),
    ],
    ('wwshade', 'shdloader.cpp'): [
        ('ShdMeshLegacyLoaderClass\t_ShdMeshLegacyLoader;\n',
         '',
         'no ShdMeshLegacyLoaderClass (one loader static, 0xC406C8, initialised at 0xA08B10)'),
        ('\n\nPrototypeClass * ShdMeshLegacyLoaderClass::Load_W3D(ChunkLoadClass & cload)\n{\n\tMeshClass * mesh = NEW_REF( MeshClass, () );\n\n\tif (mesh == NULL) {\n\t\treturn NULL;\n\t}\n\n\tif (mesh->Load_W3D(cload) != WW3D_ERROR_OK) {\n\n\t\t// if the load failed, delete the mesh\n\t\tassert(mesh->Num_Refs() == 1);\n\t\tmesh->Release_Ref();\n\t\treturn NULL;\n\n\t} else {\n\t\tShdMeshClass* shdmesh=NEW_REF( ShdMeshClass, () );\n\t\tshdmesh->Init_From_Legacy_Mesh(mesh);\n\t\tmesh->Release_Ref();\n\n\t\t// create the prototype and add it to the lists\n\t\tPrimitivePrototypeClass * newproto = W3DNEW PrimitivePrototypeClass(shdmesh);\n\t\tshdmesh->Release_Ref();\n\t\treturn newproto;\n\t\n\t}\n}\n',
         '',
         'no ShdMeshLegacyLoaderClass (one loader static, 0xC406C8, initialised at 0xA08B10)'),
    ],
    ('wwshade', 'shdloader.h'): [
        ('class ShdMeshLegacyLoaderClass : public PrototypeLoaderClass\n{\npublic:\n\n\tvirtual int\t\t\t\t\t\tChunk_Type(void) { return W3D_CHUNK_MESH; }\n\tvirtual PrototypeClass *\tLoad_W3D(ChunkLoadClass & cload);\n};\n\n\nextern ShdMeshLoaderClass\t\t\t_ShdMeshLoader;\nextern ShdMeshLegacyLoaderClass\t_ShdMeshLegacyLoader;\n',
         'extern ShdMeshLoaderClass\t\t\t_ShdMeshLoader;\n',
         'one prototype loader (vtable 0xB16ABC: Chunk_Type 0xA08C60, Load_W3D 0xA08B70; one static, 0xC406C8): no ShdMeshLegacyLoaderClass'),
    ],
    ('wwshade', 'shdmesh.cpp'): [
        ('void ShdMeshClass::Init_From_Legacy_Mesh(MeshClass* mesh)\n{\n\tSet_Name(mesh->Get_Name());\n\tMeshModelClass* model=mesh->Peek_Model();\n\tif (model) \n\t{\n\t\tint first_poly=0;\n\t\tint sub_mesh_count=0;\n\t\twhile (first_poly<model->Get_Polygon_Count()) {\n\t\t\tsub_mesh_count++;\n\t\t\tShdSubMeshClass * sub_mesh = NEW_REF( ShdSubMeshClass, () );\n\t\t\tsub_mesh->Init_From_Legacy_Mesh_Model(model,first_poly);\n\t\t\tSubMeshes.Resize(sub_mesh_count);\n\t\t\tSubMeshes[sub_mesh_count-1].Mesh=sub_mesh;\n\t\t\tSubMeshes[sub_mesh_count-1].Renderer=NULL;\n\t\t\tif (sub_mesh->Get_Visible_Polygon_Count()==0) break;\n\t\t\tfirst_poly+=sub_mesh->Get_Visible_Polygon_Count();\n\t\t}\n\t}\n\n\t// Pull interesting stuff out of the w3d attributes bits\n\tSet_Collision_Type(mesh->Get_Collision_Type());\n\tSet_Hidden(mesh->Is_Hidden());\n\tSet_Translucent(mesh->Is_Translucent());\t\n}\n\n\n\n',
         '',
         'ShdMeshClass::Load_W3D ends with the collision type and hidden bit (0xA0A380: no cast-shadow or translucency loops); no Init_From_Legacy_Mesh'),
        ('\t\n\tfor (i=0;i<SubMeshes.Length(); i++) { \n\t\tbool shadow = (hdr.Attributes & W3D_MESH_FLAG_CAST_SHADOW) == W3D_MESH_FLAG_CAST_SHADOW;\n\t\tSubMeshes[i].Mesh->Set_Flag(MeshGeometryClass::CAST_SHADOW,shadow);\n\t}\n\t\n\t// Indicate whether this mesh is translucent.  The mesh is considered translucent\n\t// if sorting has been enabled (alpha blending on pass 0) or if pass0 contains alpha-test.\n\t// This flag is used to determine if the mesh can cast a geometric shadow.\n\tbool is_translucent = false;\n\tfor (i=0;i<SubMeshes.Length(); i++) { \n\t\tif (SubMeshes[i].Mesh) {\n\t\t\tif (SubMeshes[i].Mesh->Is_Sorting()) {\n\t\t\t\tSet_Translucent(true);\n\t\t\t}\n\t\t\tShdInterfaceClass * shader = SubMeshes[i].Mesh->Peek_Shader();\n\t\t\tif ((shader) && (!shader->Is_Opaque())) {\n\t\t\t\tSet_Translucent(true);\n\t\t\t}\n\t\t}\n\t}\n',
         '',
         'ShdMeshClass::Load_W3D ends with the collision type and hidden bit (0xA0A380: no cast-shadow or translucency loops); no Init_From_Legacy_Mesh'),
    ],
    ('wwshade', 'shdmesh.h'): [
        ('\tvoid\t\t\t\t\t\t\t\tInit_From_Legacy_Mesh(MeshClass* mesh);\n',
         '',
         'no legacy mesh conversion (no Init_From_Legacy_Mesh in the client; one prototype loader)'),
    ],
    ('wwshade', 'shdrenderer.cpp'): [
        ('static DynamicVectorClass<Vector3>\t\t\t\t_TempVertexBuffer;\nstatic DynamicVectorClass<Vector3>\t\t\t\t_TempNormalBuffer;\n\n',
         '',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\t\tif (def != NULL) {\n\t\t\tdef->Init();\n\t\t\tREF_PTR_RELEASE(def);\n\t\t}\n',
         '\t\tdef->Init();\n\t\tREF_PTR_RELEASE(def);\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\t\tif (def) {\n\t\t\tdef->Shutdown();\n\t\t\tREF_PTR_RELEASE(def);\n\t\t}\n\t}\n}\n\n/////////////////////////////////////////////////////////////////////////////////////\n// DirectX8 renderer utility class declaration\n/////////////////////////////////////////////////////////////////////////////////////\n\nRendererListContainerClass::RendererListContainerClass(int pass) : Pass(pass)\n{\n}\n\nRendererListContainerClass::~RendererListContainerClass()\n{\n\tUnregister_All();\n}\n\nvoid RendererListContainerClass::Register_Renderer(ShdRendererNodeClass* node)\n{\n\tWWASSERT(node);\n\tnode->Add_Ref();\n\tnode->Set_Renderer_List_Container(this,Pass);\n\tLinkedNodes.Add_Tail(node);\n}\n\nvoid RendererListContainerClass::Unregister_All()\n{\n\twhile (!LinkedNodes.Is_Empty()) {\n\t\tShdRendererNodeClass* node=LinkedNodes.Remove_Head();\n//\t\tREF_PTR_RELEASE(node);\n\t\tdelete node;\n\t}\n}\n\nvoid RendererListContainerClass::Flush()\n{\n\tShdRendererNodeClass* prev_node=NULL;\n\twhile (!VisibleNodes.Is_Empty()) \n\t{\n\t\tShdRendererNodeClass* node=VisibleNodes.Remove_Head();\n\t\tnode->Apply_Shared_Shader_Settings(prev_node,Pass);\n\t\tnode->Flush(Pass);\n\t\tprev_node=node;\n\t}\n\n}\n\n',
         '\t\tdef->Shutdown();\n\t\tREF_PTR_RELEASE(def);\n\t}\n}\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('//\tvoid Add_Visible_Node(ShdRendererNodeClass* node,int pass);\n',
         '\tvoid Add_Visible_Node(ShdRendererNodeClass* node,int pass_count);\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('//\tShdRendererNodeList VisibleNodeList[SHD_MAX_PASSES];\n\tRendererListContainerList RendererListContainers[SHD_MAX_PASSES];\n',
         '\t// E&B: one visible list per pass (MeshContainerClass is 0x68 bytes, 0xA08005; the lists\n\t// at +8, 0x18 bytes each, 0xA080D9)\n\tShdRendererNodeList VisibleNodeList[SHD_MAX_PASSES];\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\tvirtual void Apply_Shared_Shader_Settings(ShdRendererNodeClass* prev,int pass);\n\n\tvirtual bool Greater_Than(const ShdRendererNodeClass&, int pass) const;\n\tvirtual bool Similar_Enough(const ShdRendererNodeClass&, int pass) const;\n',
         '\tvirtual void Apply_Shared_Shader_Settings(int pass);\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('}\n\nbool ShdDX8RendererNodeClass::Greater_Than(const ShdRendererNodeClass& s, int pass) const\n{\n\t// Danger! Assuming ShdDX8RendererNodeClass!\n\tconst ShdDX8RendererNodeClass* src=static_cast<const ShdDX8RendererNodeClass*>(&s);\n\n\tWWASSERT(SubMesh && src->SubMesh);\n\treturn SubMesh->Peek_Shader()->Greater_Than(*src->SubMesh->Peek_Shader(),pass);\n}\n\nbool ShdDX8RendererNodeClass::Similar_Enough(const ShdRendererNodeClass& s, int pass) const\n{\n\t// Danger! Assuming ShdDX8RendererNodeClass!\n\tconst ShdDX8RendererNodeClass* src=static_cast<const ShdDX8RendererNodeClass*>(&s);\n\n\tWWASSERT(SubMesh && src->SubMesh);\n\treturn SubMesh->Peek_Shader()->Similar_Enough(*src->SubMesh->Peek_Shader(),pass);\n',
         '',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\t\twhile (!RendererListContainers[pass].Is_Empty()) {\n\t\t\tRendererListContainerClass* cont=RendererListContainers[pass].Remove_Head();\n\t\t\tcont->Unregister_All();\n\t\t\tREF_PTR_RELEASE(cont);\n',
         '\t\twhile (!VisibleNodeList[pass].Is_Empty()) {\n\t\t\tShdRendererNodeClass* node=VisibleNodeList[pass].Remove_Head();\n\t\t\tdelete node;\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\tShdDX8RendererNodeClass* node=new ShdDX8RendererNodeClass(this,mesh,sub_mesh);\n\tShdInterfaceClass* shdi=sub_mesh->Peek_Shader();\n\tWWASSERT(shdi);\n\tfor (int pass=0; pass<shdi->Get_Pass_Count(); pass++) \n\t{\n\t\tRendererListContainerIterator ite(&RendererListContainers[pass]);\n\t\tite.Last();\n\t\twhile (!ite.Is_Done()) {\n\t\t\tRendererListContainerClass* cont=ite.Peek_Obj();\n\t\t\tif (cont) {\n\t\t\t\tShdRendererNodeListIterator ite2(&cont->Peek_Linked_Nodes());\n\t\t\t\tite2.First();\n\t\t\t\t// Container should delete itself when the last node is removed, so we must\n\t\t\t\t// never have an empty node.\n\t\t\t\tWWASSERT(!ite2.Is_Done());\n\t\t\t\t\n\t\t\t\tShdRendererNodeClass* obj=ite2.Peek_Obj();\n\t\t\t\tif (obj) {\n\t\t\t\t\tif (node->Greater_Than(*obj,pass)) {\n\t\t\t\t\t\t// If similar enough, add to the same renderer container, otherwise create a new one\n\t\t\t\t\t\tif (node->Similar_Enough(*obj,pass)) {\n\t\t\t\t\t\t\tcont->Register_Renderer(node);\n\t\t\t\t\t\t}\n\t\t\t\t\t\telse {\n\t\t\t\t\t\t\tRendererListContainerClass* new_cont=new RendererListContainerClass(pass);\n\t\t\t\t\t\t\tnew_cont->Register_Renderer(node);\n\t\t\t\t\t\t\tRendererListContainers[pass].Add_After(new_cont,new_cont);\n\t\t\t\t\t\t}\n\t\t\t\t\t\tbreak;\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t\tite.Prev();\n\t\t}\n\t\tif (ite.Is_Done()) {\n\t\t\tRendererListContainerClass* new_cont=new RendererListContainerClass(pass);\n\t\t\tnew_cont->Register_Renderer(node);\n\t\t\tRendererListContainers[pass].Add(new_cont);\n\t\t}\n\t}\n\n\treturn node;\n',
         '\treturn new ShdDX8RendererNodeClass(this,mesh,sub_mesh);\n}\n\n/////////////////////////////////////////////////////////////////////////////////////\n// Link the node to the visible list of each of its passes.\n/////////////////////////////////////////////////////////////////////////////////////\nvoid ShdDX8RendererClass::MeshContainerClass::Add_Visible_Node(ShdRendererNodeClass* node,int pass_count)\n{\n\tfor (int pass=0; pass<pass_count; pass++) \n\t{\n\t\tVisibleNodeList[pass].Add_Tail(node);\n\t}\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\t\tRendererListContainerIterator ite(&RendererListContainers[pass]);\n\t\tite.First();\n\t\twhile (!ite.Is_Done()) {\n\t\t\tRendererListContainerClass* cont=ite.Peek_Obj();\n\t\t\tcont->Flush();\n\t\t\tite.Next();\n',
         '\t\tif (!VisibleNodeList[pass].Is_Empty()) {\n\t\t\t// the first node sets the states the pass shares\n\t\t\tShdRendererNodeClass* node=VisibleNodeList[pass].Remove_Head();\n\t\t\tnode->Apply_Shared_Shader_Settings(pass);\n\t\t\tnode->Flush(pass);\n\t\t\twhile (!VisibleNodeList[pass].Is_Empty()) {\n\t\t\t\tnode=VisibleNodeList[pass].Remove_Head();\n\t\t\t\tnode->Flush(pass);\n\t\t\t}\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ("\t// create usage depending on associated shader's processing behaviour KM\n\tDX8IndexBufferClass::UsageType ib_usage=DX8IndexBufferClass::USAGE_DEFAULT;\n\n\tif (!SubMesh->Peek_Shader()->Use_HW_Vertex_Processing())\n\t{\n\t\tib_usage=DX8IndexBufferClass::USAGE_SOFTWAREPROCESSING;\n\t}\n\n\tint count=SubMesh->Get_Visible_Polygon_Count();\n\tif (!count) count=SubMesh->Get_Polygon_Count();\n\tunsigned index_count=count*3;\n\tif (SubMesh->Is_Sorting()) {\n\t\tIndexBuffer=new SortingIndexBufferClass(index_count);\n\t}\n\telse {\n\t\tIndexBuffer=new DX8IndexBufferClass(index_count,ib_usage);\n\t}\n\tIndexBufferClass::WriteLockClass ilock(IndexBuffer);\n\tconst TriIndex* indices=SubMesh->Get_Polygon_Array();\n\tint i;\n\tint j=0;\n\tfor (i=SubMesh->Get_First_Visible_Polygon();i<count+SubMesh->Get_First_Visible_Polygon();++i) \n\t{\n\t\tilock.Get_Index_Array()[j++]=indices[i][0];\n\t\tilock.Get_Index_Array()[j++]=indices[i][1];\n\t\tilock.Get_Index_Array()[j++]=indices[i][2];\n\t}\n\n\t// Don't use static vertex buffers if skin\n\tif (SubMesh->Get_Flag(MeshGeometryClass::SKIN)) {\n\t\treturn;\n\t}\n\n",
         '',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\t\tif (SubMesh->Is_Sorting()) {\n\t\t\tVertexBuffers[n]=\n\t\t\t\tnew SortingVertexBufferClass(vertex_count);\n\t\t}\n\t\telse {\n\t\t\tVertexBuffers[n]=\n\t\t\t\tnew DX8VertexBufferClass\n\t\t\t\t(\n\t\t\t\t\t0,\n\t\t\t\t\tvertex_count,\n\t\t\t\t\tvb_usage,\n\t\t\t\t\tvertex_size\n\t\t\t\t);\n\t\t}\n',
         '\t\tVertexBuffers[n]=\n\t\t\tnew DX8VertexBufferClass\n\t\t\t(\n\t\t\t\t0,\n\t\t\t\tvertex_count,\n\t\t\t\tvb_usage,\n\t\t\t\tvertex_size\n\t\t\t);\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\t\t\tSubMesh->Get_Vertex_Count()\n\t\t);\n\t}\n}\n',
         "\t\t\tSubMesh->Get_Vertex_Count()\n\t\t);\n\t}\n\n\t// create usage depending on associated shader's processing behaviour KM\n\tDX8IndexBufferClass::UsageType ib_usage=DX8IndexBufferClass::USAGE_DEFAULT;\n\n\tif (!SubMesh->Peek_Shader()->Use_HW_Vertex_Processing())\n\t{\n\t\tib_usage=DX8IndexBufferClass::USAGE_SOFTWAREPROCESSING;\n\t}\n\n\tunsigned index_count=SubMesh->Get_Polygon_Count()*3;\n\tIndexBuffer=new DX8IndexBufferClass(index_count,ib_usage);\n\tIndexBufferClass::WriteLockClass ilock(IndexBuffer);\n\tconst TriIndex* indices=SubMesh->Get_Polygon_Array();\n\tint i;\n\tfor (i=0;i<SubMesh->Get_Polygon_Count();++i) \n\t{\n\t\tilock.Get_Index_Array()[i*3]=indices[i][0];\n\t\tilock.Get_Index_Array()[i*3+1]=indices[i][1];\n\t\tilock.Get_Index_Array()[i*3+2]=indices[i][2];\n\t}\n}\n",
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\telse {\n\t\tLightEnvironment.Reset(Vector3(0.0f,0.0f,0.0f),Vector3(0.0f,0.0f,0.0f));\n\t}\n\n\tConnect();\n}\n\nbool enable_rnd=true;\n',
         '\n\tContainer->Add_Visible_Node(this,SubMesh->Peek_Shader()->Get_Pass_Count());\n}\n\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\t// BEGIN OF SKIN CODE\n\tif (SubMesh->Get_Flag(MeshGeometryClass::SKIN)) {\n\t\tDX8Wrapper::Set_Vertex_Buffer(NULL);\t// Free up the reference to the current vertex buffer\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t// (in case it is the dynamic, which may have to be resized)\n\n\t\tunsigned vertex_count=SubMesh->Get_Vertex_Count();\n\t\tunsigned stream_count=SubMesh->Peek_Shader()->Get_Vertex_Stream_Count();\n\t\tWWASSERT(stream_count>0);\n\n\t\t// For now there are restrictions in vertex buffer format with skinning\n\t\tFVFInfoClass fi(dynamic_fvf_type);\n\t\tWWASSERT(SubMesh->Peek_Shader()->Get_Vertex_Size(0)==fi.Get_FVF_Size());\n\n\t\tDynamicVBAccessClass vb(\n\t\t\tSubMesh->Is_Sorting() ? BUFFER_TYPE_DYNAMIC_SORTING : BUFFER_TYPE_DYNAMIC_DX8,\n\t\t\tdynamic_fvf_type,\n\t\t\tvertex_count);\n\t\tSNAPSHOT_SAY(("DynamicVBAccess - %s - %d vertices\\n",SubMesh->Is_Sorting() ? "sorting" : "non-sorting",vertex_count));\n\n\t\tif (_TempVertexBuffer.Length() < (int)vertex_count) _TempVertexBuffer.Resize(vertex_count); \n\t\tif (_TempNormalBuffer.Length() < (int)vertex_count) _TempNormalBuffer.Resize(vertex_count);\n\t\tVector3* loc=&(_TempVertexBuffer[0]);\n\t\tVector3* norm=&(_TempNormalBuffer[0]);\n\t\tWWASSERT(Mesh->Get_Container());\n\t\tSubMesh->Get_Deformed_Vertices(loc,norm,Mesh->Get_Container()->Get_HTree());\n\n\t\t// Compose the vertex and index buffers\n\t\tVertexStreamStruct vss;\n\t\tvss.Locations=loc;\n\t\tvss.Normals=norm;\n\t\tfor (unsigned stage=0;stage<MAX_TEXTURE_STAGES;++stage) \n\t\t{\n\t\t\tvss.UV[stage]=SubMesh->Get_UV_Array(stage);\n\t\t}\n\t\tvss.DiffuseInt=SubMesh->Get_Diffuse_Array();\n\t\tvss.S=SubMesh->Get_Tangent_Basis_S_Array();\n\t\tvss.T=SubMesh->Get_Tangent_Basis_T_Array();\n\t\tvss.SxT=SubMesh->Get_Tangent_Basis_SxT_Array();\n\n\t\tWWASSERT(stream_count==1);\n\t\t{\n\t\t\tDynamicVBAccessClass::WriteLockClass vlock(&vb);\n\t\t\tSubMesh->Peek_Shader()->Copy_Vertex_Stream\n\t\t\t(\n\t\t\t\t0,//stream\n\t\t\t\tvlock.Get_Formatted_Vertex_Array(),\n\t\t\t\tvss,\n\t\t\t\tSubMesh->Get_Vertex_Count()\n\t\t\t);\n\t\t}\n\t\tDX8Wrapper::Set_Vertex_Buffer(vb);//stream 0\n\n\t\tDX8Wrapper::Set_Index_Buffer(IndexBuffer,0);\n\t\tDX8Wrapper::Set_Transform(D3DTS_WORLD,Matrix3D(true));//Mesh->Get_Transform());\n\n\t\tDX8Wrapper::Set_Light_Environment(&LightEnvironment);\n\t\tSubMesh->Peek_Shader()->Apply_Instance(cur_pass, *RenderInfo);\n\n\t\tif (SubMesh->Is_Sorting()) {\n\t\t\tSortingRendererClass::Insert_Triangles\n\t\t\t(\n\t\t\t\t0,\n\t\t\t\tSubMesh->Get_Visible_Polygon_Count() ? SubMesh->Get_Visible_Polygon_Count() : SubMesh->Get_Polygon_Count(),\n\t\t\t\t0,\n\t\t\t\tSubMesh->Get_Vertex_Count()\n\t\t\t);\n\t\t}\n\t\telse {\n\t\t\tDX8Wrapper::Draw_Triangles\n\t\t\t(\n\t\t\t\t0,\n\t\t\t\tSubMesh->Get_Visible_Polygon_Count() ? SubMesh->Get_Visible_Polygon_Count() : SubMesh->Get_Polygon_Count(),\n\t\t\t\t0,\n\t\t\t\tSubMesh->Get_Vertex_Count()\n\t\t\t);\n\t\t}\n\n\t\treturn;\n\t}\n\n\t// END OF SKIN CODE\n\n\n',
         '',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('\tDX8Wrapper::Set_Light_Environment(&LightEnvironment);\n\tSubMesh->Peek_Shader()->Apply_Instance(cur_pass, *RenderInfo);\n\n\tif (SubMesh->Is_Sorting()) {\n\t\tSortingRendererClass::Insert_Triangles\n\t\t(\n\t\t\t0,\n\t\t\tSubMesh->Get_Visible_Polygon_Count() ? SubMesh->Get_Visible_Polygon_Count() : SubMesh->Get_Polygon_Count(),\n\t\t\t0,\n\t\t\tSubMesh->Get_Vertex_Count()\n\t\t);\n\t}\n\telse {\n\t\tDX8Wrapper::Draw_Triangles\n\t\t(\n\t\t\t0,\n\t\t\tSubMesh->Get_Visible_Polygon_Count() ? SubMesh->Get_Visible_Polygon_Count() : SubMesh->Get_Polygon_Count(),\n\t\t\t0,\n\t\t\tSubMesh->Get_Vertex_Count()\n\t\t);\n\t}\n\n}\n\nvoid ShdDX8RendererNodeClass::Apply_Shared_Shader_Settings(ShdRendererNodeClass* prev_node, int pass)\n{\n\t// If the previously set shader is not the same type as this, we need to set\n\t// all render states to default state.\n\tif (prev_node) {\n\t\tShdDX8RendererNodeClass* prev_dx8_node=static_cast<ShdDX8RendererNodeClass*>(prev_node); // Illegal to pass another type!\n\t\tShdInterfaceClass* shd=prev_dx8_node->SubMesh->Peek_Shader();\n\t\tif (shd->Get_Class_ID()!=SubMesh->Peek_Shader()->Get_Class_ID()) {\n\t\t\tDX8Wrapper::Apply_Default_State();\n\t\t}\n\t}\n',
         '\t// E&B: the environment is only recorded (the flag), and the render info is handed to\n\t// DX8Wrapper with it (0xA089C2-0xA089D9)\n\tDX8Wrapper::Set_Light_Environment(&LightEnvironment,false);\n\tRenderInfo->light_environment=&LightEnvironment;\n\tDX8Wrapper::Unknown_Set_Render_Info(RenderInfo);\n\tSubMesh->Peek_Shader()->Apply_Instance(cur_pass, *RenderInfo);\n\n\tDX8Wrapper::Draw_Triangles\n\t(\n\t\t0,\n\t\tSubMesh->Get_Polygon_Count(),\n\t\t0,\n\t\tSubMesh->Get_Vertex_Count()\n\t);\n}\n\nvoid ShdDX8RendererNodeClass::Apply_Shared_Shader_Settings(int pass)\n{\n\t// E&B: always from the default state (0xA08A20)\n\tDX8Wrapper::Apply_Default_State();\n',
         "E&B's earlier renderer: no RendererListContainerClass; MeshContainerClass keeps a visible list per pass (0x68 bytes, 0xA08005; dtor 0xA08130 deletes the nodes), Register_Mesh only creates the node (0xA081D0), Add_Visible_Node (0xA08240) and Flush (0xA08270); the node ctor builds the vertex streams, then the index buffer of all polygons (0xA082D0); Render links the node (0xA08750); Flush without skin or sorting, light environment only recorded, render info handed to DX8Wrapper (0xA08790); Apply_Shared_Shader_Settings(pass) from the default state (0xA08A20); no skin temp buffers (the file's only static is in shdloader, 0xA08B10)"),
        ('{\n\tWWPROFILE("ShdDX8RendererClass::Flush");\n\n\tDX8Wrapper::Invalidate_Cached_Render_States();\n',
         '{\n\tDX8Wrapper::Invalidate_Cached_Render_States();\n',
         "wwshade is built with ENABLE_WWPROFILE (ShdMeshClass::Render 0xA09430 opens a WWProfileSampleClass), but E&B's ShdDX8RendererClass::Flush 0xA08060 has no sampler: it starts with Invalidate_Cached_Render_States"),
    ],
    ('wwshade', 'shdrenderer.h'): [
        ('class RendererListContainerClass;\n\ntypedef MultiListClass<ShdRendererNodeClass> ShdRendererNodeList;\ntypedef MultiListIterator<ShdRendererNodeClass> ShdRendererNodeListIterator;\ntypedef MultiListClass<RendererListContainerClass> RendererListContainerList;\ntypedef MultiListIterator<RendererListContainerClass> RendererListContainerIterator;\n\nclass RendererListContainerClass : public MultiListObjectClass, public RefCountClass\n{\n\tint Pass;\n\tShdRendererNodeList LinkedNodes;\n\tShdRendererNodeList VisibleNodes;\npublic:\n\tRendererListContainerClass(int pass);\n\tvirtual ~RendererListContainerClass();\n\n\tvoid Add_Visible_Node(ShdRendererNodeClass* node) {\n\t\tVisibleNodes.Add_Tail(node);\n\t}\n\n\tShdRendererNodeList& Peek_Linked_Nodes() { return LinkedNodes; }\n\n\tvoid Flush();\n\n\tvoid Register_Renderer(ShdRendererNodeClass* node);\n\tvoid Unregister_All();\n\n};\n',
         '\ntypedef MultiListClass<ShdRendererNodeClass> ShdRendererNodeList;\ntypedef MultiListIterator<ShdRendererNodeClass> ShdRendererNodeListIterator;\n',
         "E&B's earlier renderer node: MultiListObjectClass only (node ctor 0xA082D0 sets the list node at +4 and vtable 0xB16AA8; no reference count, list containers or sorting), virtuals Render/Flush/Apply_Shared_Shader_Settings(pass) (vtable 0xB16A98)"),
        ('*/\nclass ShdRendererNodeClass : public MultiListObjectClass, public RefCountClass\n{\n\tRendererListContainerClass* RendererListContainer[SHD_MAX_PASSES];\n\tint MaxPass;\npublic:\n\tShdRendererNodeClass() : MaxPass(0)\n\t{\n\t\tfor (int a=0;a<SHD_MAX_PASSES;++a) {\n\t\t\tRendererListContainer[a]=0;\n\t\t}\n\t}\n\tvirtual ~ShdRendererNodeClass()\n\t{\n\t\tfor (int a=0;a<SHD_MAX_PASSES;++a) {\n\t\t\tREF_PTR_RELEASE(RendererListContainer[a]);\n\t\t}\n',
         "**\n** E&B: an earlier form, without Zero Hour's renderer list containers, reference count or\n** sorting (vtable 0xB16AA8: destructor and three slots; the node constructor 0xA082D0 sets\n** only the list node at +4).\n*/\nclass ShdRendererNodeClass : public MultiListObjectClass\n{\npublic:\n\tShdRendererNodeClass()\n\t{\n\t}\n\tvirtual ~ShdRendererNodeClass()\n\t{\n",
         "E&B's earlier renderer node: MultiListObjectClass only (node ctor 0xA082D0 sets the list node at +4 and vtable 0xB16AA8; no reference count, list containers or sorting), virtuals Render/Flush/Apply_Shared_Shader_Settings(pass) (vtable 0xB16A98)"),
        ('\tvirtual void Apply_Shared_Shader_Settings(ShdRendererNodeClass* prev_node, int pass)=0;\n\n\tvirtual bool Greater_Than(const ShdRendererNodeClass&, int pass) const =0;\n\n\tvoid Connect()\n\t{\n\t\tfor (int pass=0;pass<=MaxPass;++pass) {\n\t\t\tWWASSERT(RendererListContainer[pass]);\n\t\t\tif (RendererListContainer[pass]) RendererListContainer[pass]->Add_Visible_Node(this);\n\t\t}\n\t}\n\n\tvoid Set_Renderer_List_Container(RendererListContainerClass* cont,int pass)\n\t{\n\t\tREF_PTR_SET(RendererListContainer[pass],cont);\n\t\tif (cont) MaxPass=max(MaxPass,pass);\n\t}\n\tRendererListContainerClass* Peek_Renderer_List_Container(int pass) { return RendererListContainer[pass]; }\n',
         '\tvirtual void Apply_Shared_Shader_Settings(int pass)=0;\n',
         "E&B's earlier renderer node: MultiListObjectClass only (node ctor 0xA082D0 sets the list node at +4 and vtable 0xB16AA8; no reference count, list containers or sorting), virtuals Render/Flush/Apply_Shared_Shader_Settings(pass) (vtable 0xB16A98)"),
    ],
    ('wwshade', 'shdsimple.cpp'): [
        (':\tShdInterfaceClass(def,SHDDEF_CLASSID_SIMPLE),\n',
         ':\tShdInterfaceClass(def),\n',
         'base ctor without class id (Shd6SimpleClass ctor 0xA0D780)'),
        ('\tDX8Wrapper::Set_DX8_Render_State(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL);\n\n}\n',
         '\tDX8Wrapper::Set_DX8_Render_State(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL);\n\n\tDX8Wrapper::Set_DX8_Material(Material);\n\tShdHWVertexShader::Unknown_Set_Lights();\n}\n',
         "E&B's light setup for the fixed function shaders (0xA0FC60, after Light in shdhwshader.obj; name not known), called at the end of Shd6CubeMapClass::Apply_Shared 0xA0AF40, Shd6SimpleClass and Shd6GlossMaskClass::Apply_Shared (0xA0CD90, 0xA0D910); E&B sets the material here, not in Apply_Instance (0xA0DA00 only sets the texture)"),
        ('\tDX8Wrapper::Set_DX8_Material(Material);\n\tDX8Wrapper::Set_Texture(0, Texture);\n',
         '\tDX8Wrapper::Set_Texture(0, Texture);\n',
         'Shd6SimpleClass::Apply_Instance 0xA0DA00 only sets the texture; the material is set in Apply_Shared 0xA0D910'),
        (':\tShdDefClass(SHDDEF_CLASSID_SIMPLE),\n\tAmbient(1,1,1),\n',
         ':\tShdDefClass(SHDDEF_CLASSID_SIMPLE),\n\tAmbient(0,0,0),\n',
         "E&B's definitions start Ambient at 0,0,0: ShdSimpleDefClass() 0xA0D290 (+0x1C..0x24 zero)"),
    ],
    ('wwshade', 'shdsubmesh.cpp'): [
        ('#include "shdlegacyw3d.h"\n',
         '#include "shddef.h"\n',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        ('\tDiffuse(NULL),\n\tFirstVisiblePolygon(0),\n\tVisiblePolygonCount(0),\n\tSorting(false)\n',
         '\tDiffuse(NULL)\n',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        ('\tDiffuse(NULL),\n\tFirstVisiblePolygon(that.FirstVisiblePolygon),\n\tVisiblePolygonCount(that.VisiblePolygonCount),\n\tSorting(false)\n',
         '\tDiffuse(NULL)\n',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        ('\t\tFirstVisiblePolygon=that.FirstVisiblePolygon;\n\t\tVisiblePolygonCount=that.VisiblePolygonCount;\n\t\tSorting=that.Sorting;\n',
         '',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        ('\n// The legacy renderer is FVF-based so we need to figure out what FVF format to use\nstatic unsigned Define_FVF(MeshModelClass* mmc,bool enable_lighting)\n{\n\tif (mmc->Get_Flag(MeshGeometryClass::SKIN)) {\n\t\treturn dynamic_fvf_type;\n\t}\n\tif ((!!mmc->Get_Flag(MeshGeometryClass::SORT)) && WW3D::Is_Sorting_Enabled()) {\n\t\treturn dynamic_fvf_type;\n\t}\n\n\tunsigned fvf=D3DFVF_XYZ;\n\n\tint tex_coord_count=mmc->Get_UV_Array_Count();\n\n\tif (mmc->Get_Color_Array(0,false)) {\n\t\tfvf|=D3DFVF_DIFFUSE;\n\t}\n\tif (mmc->Get_Color_Array(1,false)) {\n\t\tfvf|=D3DFVF_SPECULAR;\n\t}\n\t\n\tswitch (tex_coord_count) {\n\tdefault:\n\tcase 0:\n\t\tbreak;\n\tcase 1: fvf|=D3DFVF_TEX1; break;\n\tcase 2: fvf|=D3DFVF_TEX2; break;\n\tcase 3: fvf|=D3DFVF_TEX3; break;\n\tcase 4: fvf|=D3DFVF_TEX4; break;\n\tcase 5: fvf|=D3DFVF_TEX5; break;\n\tcase 6: fvf|=D3DFVF_TEX6; break;\n\tcase 7: fvf|=D3DFVF_TEX7; break;\n\tcase 8: fvf|=D3DFVF_TEX8; break;\n\t}\n\n\tif (!mmc->Needs_Vertex_Normals()) {  //enable_lighting || mmc->Get_Flag(MeshModelClass::PRELIT_MASK)) {\n\t\treturn fvf;\n\t}\n\n\tfvf|=D3DFVF_NORMAL;\t// Realtime-lit\n\treturn fvf;\n}\n\n\n// The legacy renderer is FVF-based so we need to figure out what FVF format to use\nstatic unsigned Define_FVF(ShdSubMeshClass* ssm,bool enable_lighting)\n{\n\tif (ssm->Get_Flag(MeshGeometryClass::SKIN)) {\n\t\treturn dynamic_fvf_type;\n\t}\n\tif ((!!ssm->Get_Flag(MeshGeometryClass::SORT)) && WW3D::Is_Sorting_Enabled()) {\n\t\treturn dynamic_fvf_type;\n\t}\n\n\tunsigned fvf=D3DFVF_XYZ;\n\n\tint tex_coord_count=0;\n\tfor (tex_coord_count=0;tex_coord_count < MeshMatDescClass::MAX_TEX_STAGES; tex_coord_count++) {\n\t\tif (ssm->Get_UV_Array(tex_coord_count) == NULL) {\n\t\t\tbreak;\n\t\t}\n\t}\n\n\tif (ssm->Get_Diffuse_Array() != NULL) {\n\t\tfvf|=D3DFVF_DIFFUSE;\n\t}\n\t\n\tswitch (tex_coord_count) {\n\tdefault:\n\tcase 0:\n\t\tbreak;\n\tcase 1: fvf|=D3DFVF_TEX1; break;\n\tcase 2: fvf|=D3DFVF_TEX2; break;\n\tcase 3: fvf|=D3DFVF_TEX3; break;\n\tcase 4: fvf|=D3DFVF_TEX4; break;\n\tcase 5: fvf|=D3DFVF_TEX5; break;\n\tcase 6: fvf|=D3DFVF_TEX6; break;\n\tcase 7: fvf|=D3DFVF_TEX7; break;\n\tcase 8: fvf|=D3DFVF_TEX8; break;\n\t}\n\n\tif (ssm->Get_Vertex_Normal_Array() != NULL) {\n\t\tfvf|=D3DFVF_NORMAL;\t// Realtime-lit\n\t}\n\n\treturn fvf;\n}\n\n\nvoid ShdSubMeshClass::Init_From_Legacy_Mesh_Model(MeshModelClass* model,int first_polygon)\n{\n\tWWASSERT(first_polygon<model->Get_Polygon_Count());\n\n\tSorting=!!model->Get_Flag(MeshGeometryClass::SORT) && WW3D::Is_Sorting_Enabled();\n\n\tint pass;\n\n\t// Figure out how many polygons we can use\n\tint active_polygon_count=0;\n\tbool arrays_used=false;\n\tfor (pass=0;pass<model->Get_Pass_Count() && !arrays_used;++pass) {\n\t\tif (model->Has_Shader_Array(pass) || model->Has_Material_Array(pass)) {\n\t\t\tarrays_used=true;\n\t\t\tbreak;\n\t\t}\n\t\tfor (int stage=0;stage<MeshMatDescClass::MAX_TEX_STAGES;++stage) {\n\t\t\tTextureClass* texture=NULL;\n\t\t\tif (model->Has_Texture_Array(pass,stage)) {\n\t\t\t\tarrays_used=true;\n\t\t\t\tbreak;\n\t\t\t}\n\t\t}\n\t}\n\n\t// If any arrays are used by the mesh, count how many polygons we have that share properties.\n\tif (arrays_used) {\n\t\tbool stop=false;\n\t\tactive_polygon_count=0;\n\t\tfor (int poly=first_polygon;poly<model->Get_Polygon_Count() && !stop;++poly) {\n\t\t\tfor (pass=0;pass<model->Get_Pass_Count();++pass) {\n\t\t\t\tif (model->Has_Shader_Array(pass)) {\n\t\t\t\t\tif (model->Get_Shader(first_polygon,pass)!=model->Get_Shader(poly,pass)) {\n\t\t\t\t\t\tstop=true;\n\t\t\t\t\t\tbreak;\n\t\t\t\t\t}\n\t\t\t\t}\n\n\t\t\t\tif (model->Has_Material_Array(pass)) {\n\t\t\t\t\tTriIndex first_tri=model->Get_Polygon_Array()[first_polygon];\n\t\t\t\t\tTriIndex cur_tri=model->Get_Polygon_Array()[poly];\n\t\t\t\t\tif (model->Peek_Material(first_tri[0],pass)!=model->Peek_Material(cur_tri[0],pass) ||\n\t\t\t\t\t\tmodel->Peek_Material(first_tri[1],pass)!=model->Peek_Material(cur_tri[1],pass) ||\n\t\t\t\t\t\tmodel->Peek_Material(first_tri[2],pass)!=model->Peek_Material(cur_tri[2],pass)) {\n\t\t\t\t\t\tstop=true;\n\t\t\t\t\t\tbreak;\n\t\t\t\t\t}\n\t\t\t\t}\n\n\t\t\t\tfor (int stage=0;stage<MeshMatDescClass::MAX_TEX_STAGES;++stage) {\n\t\t\t\t\tif (model->Has_Texture_Array(pass,stage)) {\n\t\t\t\t\t\tif (model->Peek_Texture(first_polygon,pass,stage)!=model->Peek_Texture(poly,pass,stage)) {\n\t\t\t\t\t\t\tstop=true;\n\t\t\t\t\t\t\tbreak;\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t}\n\t\t\tactive_polygon_count++;\n\t\t}\n\t}\n\tFirstVisiblePolygon=first_polygon;\n\tVisiblePolygonCount=active_polygon_count;\n\tif (FirstVisiblePolygon!=0 && VisiblePolygonCount==0) {\n\t\tVisiblePolygonCount=model->Get_Polygon_Count()-FirstVisiblePolygon;\n\t}\n\tWWASSERT(FirstVisiblePolygon+VisiblePolygonCount<=model->Get_Polygon_Count());\n\n\t// Now that the definition is initialized, create a shader instance (there\'s really\n\t// only one def/instance pair with the legacy meshes, no optimization planned currently)\n\tShdLegacyW3DDefClass* shader_def=NEW_REF( ShdLegacyW3DDefClass, () );\n\tShader=shader_def->Create();\n\tREF_PTR_RELEASE(shader_def);\n\n\t// Danger! We\'re assuming Shd6LegacyW3DClass is the only type created by legacy shader!!!\n\t((Shd6LegacyW3DClass*)Shader)->Set_FVF(Define_FVF(model,true));\n\t((Shd6LegacyW3DClass*)Shader)->Set_Pass_Count(model->Get_Pass_Count());\n\n\tfor (pass=0;pass<model->Get_Pass_Count();++pass) {\n\t\tShaderClass legacy_shader=ShaderClass::_PresetOpaqueShader;\n\t\tif (model->Has_Shader_Array(pass)) {\n\t\t\tlegacy_shader=model->Get_Shader(first_polygon,pass);\n\t\t}\n\t\telse {\n\t\t\tlegacy_shader=model->Get_Single_Shader(pass);\n\t\t}\n\n\t\tVertexMaterialClass* legacy_material=NULL;\n\t\tif (model->Has_Material_Array(pass)) {\n\t\t\tTriIndex first_tri=model->Get_Polygon_Array()[first_polygon];\n\t\t\tlegacy_material=model->Get_Material(first_tri[0],pass);\n\t\t}\n\t\telse {\n\t\t\tlegacy_material=model->Get_Single_Material(pass);\n\t\t}\n\n\t\tfor (int stage=0;stage<MeshMatDescClass::MAX_TEX_STAGES;++stage) {\n\t\t\tTextureClass* texture=NULL;\n\t\t\tif (model->Has_Texture_Array(pass,stage)) {\n\t\t\t\ttexture=model->Get_Texture(first_polygon,pass,stage);\n\t\t\t}\n\t\t\telse {\n\t\t\t\ttexture=model->Get_Single_Texture(pass,stage);\n\t\t\t}\n\n\t\t\tif (texture) {\n//\t\t\t\tStringClass texture_name=texture->Get_Full_Path();\n//\t\t\t\ttexture->Release_Ref();\n//\t\t\t\tshader_def->Set_Texture_Name(pass,stage,texture_name);\n\t\t\t\t((Shd6LegacyW3DClass*)Shader)->Set_Texture(pass,stage,texture);\n\t\t\t\ttexture->Release_Ref();\n\t\t\t}\n\t\t}\n\n\n\t\t((Shd6LegacyW3DClass*)Shader)->Set_Shader(pass,legacy_shader);\n\t\t((Shd6LegacyW3DClass*)Shader)->Set_Material(pass,legacy_material);\n\t}\n\n\t*(MeshGeometryClass*)this = *(MeshGeometryClass*)model;\n\tif (CullTree)\n\t\tCullTree->Set_Mesh(this);\n\n\t// Copy texture coordinates\n\tint uv_array_count=model->Get_UV_Array_Count();\n\tWWASSERT(uv_array_count<=MAX_TEXTURE_STAGES);\n\tfor (int uvi=0;uvi<uv_array_count;++uvi) {\n\t\tconst Vector2 *uv=model->Get_UV_Array_By_Index(uvi);\n\t\tif (uv) {\n\t\t\tUV[uvi]=NEW_REF(ShareBufferClass<Vector2>,(model->Get_Vertex_Count(),"ShdSubMeshClass::UVCoords"));\n\t\t\tmemcpy(UV[uvi]->Get_Array(),uv,sizeof(Vector2)*model->Get_Vertex_Count());\n\t\t}\n\t}\n\n\t// Copy vertex color array\n\tconst unsigned* diffuse=model->Get_DCG_Array(0);\n\tif (diffuse) {\n\t\tDiffuse=NEW_REF(ShareBufferClass<unsigned>,(model->Get_Vertex_Count(),"ShdSubMeshClass::Diffuse"));\n\t\tmemcpy(Diffuse->Get_Array(),diffuse,sizeof(unsigned)*model->Get_Vertex_Count());\n\t}\n\n}\n\n',
         '',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        ("\tif ((Shader->Peek_Definition() != NULL) && (Shader->Peek_Definition()->Get_Class_ID() == SHDDEF_CLASSID_LEGACYW3D)) {\n\t\t// Danger! We're assuming Shd6LegacyW3DClass is the only type created by legacy shader!!!\n\t\tShd6LegacyW3DClass * legacy_shader = (Shd6LegacyW3DClass*)Shader;\n\t\tlegacy_shader->Set_FVF(Define_FVF(this,true));\n\t\tif (!legacy_shader->Is_Opaque()) {\n\t\t\tSet_Flag(MeshGeometryClass::SORT,true);\n\t\t}\n\t}\n\n",
         '',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        ('\t\tcase W3D_CHUNK_SHDSUBMESH_VERTEX_INFLUENCES\t\t: error=read_vertex_influences(cload); break;\n',
         '',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        (' * ShdSubMeshClass::read_vertex_influences -- read the vertex bone links from a w3d file       *\n *                                                                                             *\n * This is for WWSKin support, presumably when we implement HW skining this may become obsolete*\n *                                                                                             *\n * INPUT:                                                                                      *\n *                                                                                             *\n * OUTPUT:                                                                                     *\n *                                                                                             *\n * WARNINGS:                                                                                   *\n *                                                                                             *\n * HISTORY:                                                                                    *\n *   11/07/2002  gth: Created                                                                  *\n *=============================================================================================*/\nWW3DErrorType ShdSubMeshClass::read_vertex_influences(ChunkLoadClass& cload)\n{\n\tuint16 * bone_links = get_bone_links();\n\n\tfor (int i=0; i<VertexCount; i++) \n\t{\n\t\tif \n\t\t(\n\t\t\tcload.Read\n\t\t\t(\n\t\t\t\tbone_links++,\n\t\t\t\tsizeof(uint8)\n\t\t\t)!=sizeof(uint8)\n\t\t) \n\t\t{\n\t\t\treturn WW3D_ERROR_LOAD_FAILED;\n\t\t}\n\t}\n\n\tSet_Flag(SKIN,true);\n\n\treturn WW3D_ERROR_OK;\t\n}\n\n/***********************************************************************************************\n',
         '',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
        ('\tREF_PTR_RELEASE(shddef);\n\treturn WW3D_ERROR_OK;\n}\n\n\n/***********************************************************************************************\n * ShdSubMesh::Get_Deformed_Vertices -- Gets the deformed vertices for a skin                   *\n *                                                                                             *\n * INPUT:                                                                                      *\n *                                                                                             *\n * OUTPUT:                                                                                     *\n *                                                                                             *\n * WARNINGS:                                                                                   *\n *                                                                                             *\n * HISTORY:                                                                                    *\n *   3/4/2001   gth : Created.                                                                 *\n *=============================================================================================*/\nvoid\tShdSubMeshClass::Get_Deformed_Vertices(Vector3 *dst_vert, Vector3 *dst_norm, const HTreeClass* htree)\n{\n\tWWASSERT(Get_Flag(MeshGeometryClass::SKIN));\n\tWWASSERT(htree);\n\tget_deformed_vertices(dst_vert,dst_norm,htree);\n}\n\n\n/***********************************************************************************************\n * ShdSubMeshClass::Get_Deformed_Vertices -- Gets the deformed vertices for a skin                   *\n *                                                                                             *\n * INPUT:                                                                                      *\n *                                                                                             *\n * OUTPUT:                                                                                     *\n *                                                                                             *\n * WARNINGS:                                                                                   *\n *                                                                                             *\n * HISTORY:                                                                                    *\n *   3/4/2001   gth : Created.                                                                 *\n *=============================================================================================*/\nvoid ShdSubMeshClass::Get_Deformed_Vertices(Vector3 *dst_vert,const HTreeClass* htree)\n{\n\tWWASSERT(Get_Flag(MeshGeometryClass::SKIN));\n\tWWASSERT(htree);\n\n\tget_deformed_vertices(dst_vert,htree);\n}\n',
         '\treturn WW3D_ERROR_OK;\n}\n',
         "E&B's earlier ShdSubMeshClass (0xA0DCE0-0xA0EC30): no Define_FVF/Init_From_Legacy_Mesh_Model/Get_Deformed_Vertices, Load_W3D without the legacy test (0xA0E040), read_chunks without the vertex-influence chunk (jump table 0xB40-0xB4B, 0xA0E310), read_shader keeps the definition (0xA0EA70)"),
    ],
    ('wwshade', 'shdsubmesh.h'): [
        ('\tvoid\t\t\t\t\t\t\tInit_From_Legacy_Mesh_Model(MeshModelClass* model,int first_polygon);\n\n',
         '',
         "E&B's earlier ShdSubMeshClass: Shader at +0xA4 right after SxT (read_shader 0xA0EA97), no visible-polygon range or sorting flag, no legacy conversion or skin deformation, no vertex-influence chunk (shdsubmesh.obj 0xA0DCE0-0xA0EC30)"),
        ('\t// First and last visible polygon. Default is both zero, which means that all polygons\n\t// are visible.\n\tint\t\t\t\t\t\t\tGet_First_Visible_Polygon() const { return FirstVisiblePolygon; }\n\tint\t\t\t\t\t\t\tGet_Visible_Polygon_Count() const { return VisiblePolygonCount; }\n\tbool\t\t\t\t\t\t\tIs_Sorting() const { return Sorting; }\n\n\tvoid\t\t\t\t\t\t\tGet_Deformed_Vertices(Vector3 *dst_vert, Vector3 *dst_norm, const HTreeClass* htree);\n\tvoid\t\t\t\t\t\t\tGet_Deformed_Vertices(Vector3 *dst_vert, const HTreeClass* htree);\n\n',
         '',
         "E&B's earlier ShdSubMeshClass: Shader at +0xA4 right after SxT (read_shader 0xA0EA97), no visible-polygon range or sorting flag, no legacy conversion or skin deformation, no vertex-influence chunk (shdsubmesh.obj 0xA0DCE0-0xA0EC30)"),
        ('\n\tint\t\t\t\t\t\t\t\t\t\t\t\t\t\tFirstVisiblePolygon;\n\tint\t\t\t\t\t\t\t\t\t\t\t\t\t\tVisiblePolygonCount;\n\tbool\t\t\t\t\t\t\t\t\t\t\tSorting;\n',
         '',
         "E&B's earlier ShdSubMeshClass: Shader at +0xA4 right after SxT (read_shader 0xA0EA97), no visible-polygon range or sorting flag, no legacy conversion or skin deformation, no vertex-influence chunk (shdsubmesh.obj 0xA0DCE0-0xA0EC30)"),
        ('\tWW3DErrorType read_vertex_influences(ChunkLoadClass& cload);\n',
         '',
         "E&B's earlier ShdSubMeshClass: Shader at +0xA4 right after SxT (read_shader 0xA0EA97), no visible-polygon range or sorting flag, no legacy conversion or skin deformation, no vertex-influence chunk (shdsubmesh.obj 0xA0DCE0-0xA0EC30)"),
    ],
    ('wwshade', 'shdhwshader.h'): [
        ('\t\tVector4&\t\t\t\t\tdiffuse\n\t);\n\nprivate:\n\tstatic bool Using_Hardware;',
         '\t\tVector4&\t\t\t\t\tdiffuse\n\t);\n\n\tstatic void Unknown_Set_Lights();\t// E&B; name not known\n\nprivate:\n\tstatic bool Using_Hardware;',
         "E&B's light setup for the fixed function shaders (0xA0FC60, after Light in shdhwshader.obj; name not known), called at the end of Shd6CubeMapClass::Apply_Shared 0xA0AF40, Shd6SimpleClass and Shd6GlossMaskClass::Apply_Shared (0xA0CD90, 0xA0D910)"),
    ],
    ('wwshade', 'shdhwshader.cpp'): [
        ('void ShdHWVertexShader::Light\n(\n\tRenderInfoClass&\t\trinfo,\n\tVector4&\t\t\t\t\tambient,\n\tVector4&\t\t\t\t\tdiffuse\n)\n{\n\tVector4 specular(0,0,0,0);\n\tLight(rinfo,ambient,diffuse,specular);\n}\n',
         "void ShdHWVertexShader::Light\n(\n\tRenderInfoClass&\t\trinfo,\n\tVector4&\t\t\t\t\tambient,\n\tVector4&\t\t\t\t\tdiffuse\n)\n{\n\tVector4 specular(0,0,0,0);\n\tLight(rinfo,ambient,diffuse,specular);\n}\n\n//**********************************************************************************************\n//! E&B: sets the D3D lights from DX8Wrapper's render state for the fixed function shaders,\n//! light 0 with a white specular and DX8Wrapper's ambient colour. Name not known (0xA0FC60).\nvoid ShdHWVertexShader::Unknown_Set_Lights()\n{\n\tRenderStateStruct state;\n\tDX8Wrapper::Get_Render_State(state);\n\n\tfor (int i=0;i<4;i++)\n\t{\n\t\tif (state.LightEnable[i])\n\t\t{\n\t\t\tif (i==0)\n\t\t\t{\n\t\t\t\tconst Vector3& amb=DX8Wrapper::Get_Ambient();\n\t\t\t\tstate.Lights[0].Specular.r=1.0f;\n\t\t\t\tstate.Lights[0].Specular.g=1.0f;\n\t\t\t\tstate.Lights[0].Specular.b=1.0f;\n\t\t\t\tstate.Lights[0].Ambient.r=amb.X;\n\t\t\t\tstate.Lights[0].Ambient.g=amb.Y;\n\t\t\t\tstate.Lights[0].Ambient.b=amb.Z;\n\t\t\t}\n\t\t\tDX8Wrapper::Set_DX8_Light(i,&state.Lights[i]);\n\t\t}\n\t}\n}\n",
         "E&B's light setup for the fixed function shaders (0xA0FC60, after Light in shdhwshader.obj; name not known), called at the end of Shd6CubeMapClass::Apply_Shared 0xA0AF40, Shd6SimpleClass and Shd6GlossMaskClass::Apply_Shared (0xA0CD90, 0xA0D910)"),
    ],
    ('wwshade', 'shdbumpdiff.cpp'): [
        (':\tShdDefClass(SHDDEF_CLASSID_BUMPDIFF),\n\tAmbient(1,1,1),\n',
         ':\tShdDefClass(SHDDEF_CLASSID_BUMPDIFF),\n\tAmbient(0,0,0),\n',
         "E&B's definitions start Ambient at 0,0,0: ShdBumpDiffDefClass() 0xA0B370 (+0x20..0x28 zero)"),
        ('\tAmbient(that.Ambient),\n\tDiffuse(that.Diffuse),\n\tDiffuse_Bumpiness(that.Diffuse_Bumpiness)\n{\n\tTextureName=that.TextureName;\n\tBumpMapName=that.BumpMapName;\n}\n',
         '\tAmbient(0,0,0),\n\tDiffuse(1,1,1),\n\tDiffuse_Bumpiness(1,0)\n{\n\tTextureName=that.TextureName;\n\tBumpMapName=that.BumpMapName;\n\n\tAmbient=that.Ambient;\n\tDiffuse=that.Diffuse;\n\tDiffuse_Bumpiness=that.Diffuse_Bumpiness;\n}\n',
         "ShdBumpDiffDefClass(const&) 0xA0B450: copy constructor initialises the members to the defaults and assigns them from that in its body (as ShdSimpleDefClass's)"),
    ],
    ('wwshade', 'shdbumpspec.cpp'): [
        (':\tShdDefClass(SHDDEF_CLASSID_BUMPSPEC),\n\tAmbient(1,1,1),\n',
         ':\tShdDefClass(SHDDEF_CLASSID_BUMPSPEC),\n\tAmbient(0,0,0),\n',
         "E&B's definitions start Ambient at 0,0,0: ShdBumpSpecDefClass() 0xA0BCF0 (+0x20..0x28 zero)"),
        ('\tAmbient(that.Ambient),\n\tDiffuse(that.Diffuse),\n\tSpecular(that.Specular),\n\tDiffuse_Bumpiness(that.Diffuse_Bumpiness),\n\tSpecular_Bumpiness(that.Specular_Bumpiness)\n{\n\tTextureName=that.TextureName;\n\tBumpMapName=that.BumpMapName;\n}\n',
         '\tAmbient(0,0,0),\n\tDiffuse(1,1,1),\n\tSpecular(1,1,1),\n\tDiffuse_Bumpiness(1,0),\n\tSpecular_Bumpiness(1,0)\n{\n\tTextureName=that.TextureName;\n\tBumpMapName=that.BumpMapName;\n\n\tAmbient=that.Ambient;\n\tDiffuse=that.Diffuse;\n\tSpecular=that.Specular;\n\tDiffuse_Bumpiness=that.Diffuse_Bumpiness;\n\tSpecular_Bumpiness=that.Specular_Bumpiness;\n}\n',
         "ShdBumpSpecDefClass(const&) 0xA0BDE0: copy constructor initialises the members to the defaults and assigns them from that in its body (as ShdSimpleDefClass's)"),
        ('void ShdBumpSpecDefClass::Init()\n{\n\t// select shader version\n\tif ((DX8Wrapper::Get_Current_Caps()->Get_Pixel_Shader_Major_Version())==1 &&\n\t\t (DX8Wrapper::Get_Current_Caps()->Get_Pixel_Shader_Minor_Version())>=1)\n\t{\n\t\tVersion.Set(SHDVER_8);\n\t}\n\telse if (DX8Wrapper::Get_Current_Caps()->Support_Dot3())\n\t{\n\t\tVersion.Set(SHDVER_7);\n\t}\n\telse\n\t{\n\t\tVersion.Set(SHDVER_6);\n\t}\n',
         'void ShdBumpSpecDefClass::Init()\n{\n\t// select shader version\n\t// E&B: always the DX6 version (0xA0C3B0 sets SHDVER_6 and calls Shd6BumpSpecClass::Init)\n#if 0\n\tif ((DX8Wrapper::Get_Current_Caps()->Get_Pixel_Shader_Major_Version())==1 &&\n\t\t (DX8Wrapper::Get_Current_Caps()->Get_Pixel_Shader_Minor_Version())>=1)\n\t{\n\t\tVersion.Set(SHDVER_8);\n\t}\n\telse if (DX8Wrapper::Get_Current_Caps()->Support_Dot3())\n\t{\n\t\tVersion.Set(SHDVER_7);\n\t}\n\telse\n#endif\n\t{\n\t\tVersion.Set(SHDVER_6);\n\t}\n',
         "E&B's bump specular shader always uses its DX6 version: ShdBumpSpecDefClass::Init 0xA0C3B0 sets SHDVER_6 (0xC40730 = 1) and jumps to Shd6BumpSpecClass::Init"),
    ],
}
for _key, _edits in WWSHADE_EARLY_PATCHES.items():
    PATCHES.setdefault(_key, []).extend(_edits)
# end of WWSHADE_EARLY_PATCHES
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
    return drop_not_linked(json.load(open(STATE))) if os.path.exists(STATE) else {}


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
            if not copies[best].get(k) or not_linked(lib, k + '.cpp'):
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
            if not_linked(lib, name):
                continue
            shutil.copy2(path, os.path.join(d, name))
            origin[name] = base
        # Headers only another copy has (a file chosen from that copy may need them).
        for c in LIB_DIR[lib]:
            if c == base:
                continue
            for low, (name, path) in headers(lib, c).items():
                if low not in hs and not not_linked(lib, name):
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


# ------------------------------------------------------------------------------------------ replay

def read(path):
    return open(path, encoding='latin1', newline='').read()


def replay(state, out):
    """Rebuild the libraries under `out` from the EA copies and the decisions recorded in the state
    (each header's origin, each source file's copy, spliced functions), then the local changes
    (PATCHES, annotations, APPENDS, NEW_FILES, RECONSTRUCTED from the repository). The `headers`
    map is the authority for headers; `headers_from` on a file only records why one was swapped.
    Returns the steps that failed."""
    problems = []
    kept = {(rlib, rname): open(os.path.join(dest(rlib), rname), 'rb').read() for rlib, rname in RECONSTRUCTED}
    for lib, s in state.items():
        d = os.path.join(out, lib)
        if os.path.isdir(d):
            shutil.rmtree(d)
        os.makedirs(d)

        def place(name, src):
            for old in [n for n in os.listdir(d) if n.lower() == name.lower()]:
                os.remove(os.path.join(d, old))
            shutil.copy2(src, os.path.join(d, name))

        for name, c in sorted(s.get('headers', {}).items()):
            if c not in COPY_ROOT:
                continue
            hs = headers(lib, c)
            if name.lower() not in hs:
                problems.append(f'{lib}/{name}: no such header in {c}')
                continue
            place(name, hs[name.lower()][1])
        for k, f in sorted(s['files'].items()):
            if f['copy'] not in COPY_ROOT:
                continue
            text = read(source_files(lib, f['copy'])[k][1])
            for m in f['merged']:
                text, done = splice(text, read(source_files(lib, m['copy'])[k][1]), [m['function']])
                if not done:
                    problems.append(f'{lib}/{f["name"]}: splice of {m["function"]} from {m["copy"]} failed')
            open(os.path.join(d, f['name']), 'w', encoding='latin1', newline='').write(text)
        for name in os.listdir(d):
            p = os.path.join(d, name)
            try:
                apply_patches(lib, p, name)
                annotations.apply(lib, p, name)
            except SystemExit as e:
                problems.append(str(e))
            for (alib, aname), (text, _) in APPENDS.items():
                if alib == lib and aname == name.lower():
                    with open(p, 'a', newline='\r\n') as fp:
                        fp.write(text)
        for (nlib, nname), text in NEW_FILES.items():
            if nlib == lib:
                open(os.path.join(d, nname), 'w', newline='\r\n').write(text)
        for (rlib, rname), data in kept.items():
            if rlib == lib:
                for old in [n for n in os.listdir(d) if n.lower() == rname.lower()]:
                    os.remove(os.path.join(d, old))
                open(os.path.join(d, rname), 'wb').write(data)
    return problems


def compare_tree(out, libs):
    """Files that differ between `out` and the repository: (lib, name, what)."""
    diffs = []
    for lib in libs:
        mine = {n.lower(): n for n in os.listdir(os.path.join(out, lib))}
        repo = {n.lower(): n for n in os.listdir(dest(lib))}
        for low in sorted(set(mine) | set(repo)):
            if low not in repo:
                diffs.append((lib, mine[low], 'only in replay'))
            elif low not in mine:
                diffs.append((lib, repo[low], 'only in repository'))
            elif mine[low] != repo[low]:
                diffs.append((lib, repo[low], f'name case differs ({mine[low]})'))
            elif open(os.path.join(out, lib, mine[low]), 'rb').read() != open(os.path.join(dest(lib), repo[low]), 'rb').read():
                diffs.append((lib, repo[low], 'content differs'))
    return diffs


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
    rc, text = vc6.compile_sources(paths, tree_includes(), out=out, flags=flags_for(paths))
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


def tree_sets(state):
    """Per library, every (file, client address) the tree reproduces."""
    return {l: {(k, a) for k, f in s['files'].items() for a in f.get('in_tree', [])}
            for l, s in state.items()}


def try_with_headers(state, lib, f, c, k, donor):
    """Swap a file and its same-named header(s) to copy c; keep it only if no function anywhere
    is lost. Returns the file's matched set, or None (and restores everything) if rejected."""
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
    before = tree_sets(state)
    trial = json.loads(json.dumps(state))
    verify(trial)
    after = tree_sets(trial)
    got = set(trial[lib]['files'][k].get('in_tree', []))
    if all(before[l] <= after[l] for l in before) and sum(map(len, after.values())) > sum(map(len, before.values())):
        for l in state:
            state[l]['in_tree_total'] = trial[l]['in_tree_total']
            for kk, ff in state[l]['files'].items():
                ff['in_tree'] = trial[l]['files'][kk].get('in_tree', [])
        hs = state[lib].setdefault('headers', {})
        for n, _ in pairs:
            for old in [x for x in hs if x.lower() == n.lower()]:
                del hs[old]
            hs[n] = c
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
                        if cur <= got and len(got) > len(cur):
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
    ap.add_argument('command', choices=['plan', 'assemble', 'replay', 'verify', 'refine', 'report', 'all'])
    ap.add_argument('--out', default=os.path.join(REPO, '.work', 'replay'),
                    help='replay: where to rebuild the libraries (default .work/replay)')
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
    if a.command == 'replay':
        problems = replay(state, a.out)
        for p in problems:
            print('  problem:', p)
        diffs = compare_tree(a.out, list(state))
        for lib, name, what in diffs:
            print(f'  {lib}/{name}: {what}')
        print(f'replay into {a.out}: {len(problems)} problems, {len(diffs)} files differ from the repository')
        if problems or diffs:
            raise SystemExit(1)
    if a.command in ('verify', 'all'):
        save_state(verify(state, a.lib))
    if a.command in ('refine', 'all'):
        save_state(refine(state, ev, a.lib, [key_of(f) for f in a.file] if a.file else None))
    if a.command in ('report', 'all'):
        report(state)


if __name__ == '__main__':
    main()
