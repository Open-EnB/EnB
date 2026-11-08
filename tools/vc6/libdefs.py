"""The libraries' definitions: EA's copies, the tree's library directories (dest), the compile flags,
the include paths, the non-EA app libraries (APP_LIBS) and the files the client does not link
(split from importer.py, docs/pipeline-plan.md 3.3).
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
if HERE not in sys.path:
    sys.path.insert(0, HERE)
sys.path.insert(1, os.path.join(REPO, 'tools', 're'))
import vc6  # noqa: E402

EA = r'C:\EnBDevelopment\EA'
SDK_INCLUDES = [r'C:\EnBDevelopment\SDK\directx8\include', r'C:\EnBDevelopment\SDK\radtools\include_enb']

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
    # D73: WWOnline's wlib and wnet (no WWVegas copy: the paths are relative to the copy roots above).
    # wlib: Zero Hour's matchbot copy (Generals' and the mangler's are the same but for the banner;
    # Renegade's Code\Tools\wlib too, but for its arraylist.h's addMany/removeMany, written in 2025 from a
    # DLL: "Implementation was missing from source code", and not compiling with VC6). wnet: matchbot's
    # udp.cpp (Z) and Renegade's Code\Tools\RenegadeGR tcpmgr.cpp / tcpcon.cpp (R: no other copy has them).
    # wlib R: RenegadeGR's older wtime.cpp (no localtime_r, no "aa" AM/PM format), the client's: its Get*
    # call localtime directly (0x8B95A0-0x8B9700), FormatTime 0x8B8780 has no AM/PM suffix (TK4 2026-10-31).
    'wlib': {'Z': r'..\..\..\Tools\matchbot\wlib', 'R': r'Tools\RenegadeGR'},
    'wnet': {'Z': r'..\..\..\Tools\matchbot\wnet', 'R': r'Tools\RenegadeGR'},
    # D73: Renegade's Code\BinkMovie (BINKMovie, the subtitles), 0xA16190-0xA17Bxx
    'binkmovie': {'R': 'BinkMovie'},
}
LIBS = list(LIB_DIR)
# Where a library lives in the tree when not at the repository root: the client's own paths
# (e:\build\beta\G\libs\wwonline\wnet\tcpqueue.cpp, D67).
LIB_TREE = {'wlib': r'libs\wwonline\wlib', 'wnet': r'libs\wwonline\wnet'}
# Libraries whose directories are NOT include directories (their headers would shadow the app's and
# the programs': wlib's wtime.h, wnet's udp.h): their own files include their headers from their
# directory, wnet reaches wlib as "wlib/wdebug.h" (libs\wwonline is searched after the SDKs), the app
# by relative path (..\..\libs\wwonline\wlib\critsec.h). Their directories are synced to the VM
# with every compile (late_sync).
LATE_INCLUDE_LIBS = ('wlib', 'wnet', 'binkmovie')
HEADER_EXT = ('.h', '.hpp', '.inl')

# Non-EA libraries (D23): E&B's own code, which no EA copy has. Every file is in RECONSTRUCTED
# (written from the client, docs/workflow.md "Adding an app file"). The directory mirrors E&B's
# e:\build\beta\G\<lib>\... (A1: the repository root stands for G\, directories in lower case), so
# the file names in RECONSTRUCTED and imports.json are paths relative to <lib> with '/'
# (`shared/auxdata/bitset.cpp`); file keys stay the base name (key_of), so two .cpp files of one
# such library may not share a base name (load_state refuses it). Per library: the defines
# (vc6's, with NDEBUG, D61: E&B's G_ASSERT/FAIL_IF don't test it), the client range verify searches (the
# app region, docs/binary.md; objmatch region) and the __FILE__ root: the sources are named to cl
# under e:\build\beta\G (vc6.compile_sources source_alias), so __FILE__ is the client's string.
# Their state entry (base 'reconstructed', no evidence totals) is derived from RECONSTRUCTED by
# load_state; plan, assemble and refine leave them alone. Not in LIBS (the EA libraries), so the
# libraries' include paths don't see them.
APP_LIBS = {
    'app': {'defines': list(vc6.DEFAULT_DEFINES) + ['ENABLE_WWPROFILE'],   # D74
            'region': (0x401000, 0x8C1B50),
            'file_root': r'e:\build\beta\G'},
}

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
# USE_WWSHADE in wwshade: neutral in the bytes (verify 138 before and after, none lost); set for
# consistency with the project-wide define WW3D2 needs (e278d81).
LIB_FLAGS = {'wwshade': '/nologo /c /MT /W3 /O2 /Ob1 /GX /D _MBCS /D ENABLE_WWPROFILE /D USE_WWSHADE',
             # ENABLE_WWPROFILE also in WWAudio (+32 pairs: WWAudioClass, SoundSceneClass, sound buffers) and WWMath (+1)
             'wwaudio': '/nologo /c /MT /W3 /O2 /Ob2 /D _MBCS /D ENABLE_WWPROFILE',
             'wwmath': '/nologo /c /MT /W3 /O2 /Ob2 /D ENABLE_WWPROFILE',
             # WWDebug: neutral in the bytes; set for consistency with a project-wide define (D14, soft)
             'wwdebug': '/nologo /c /MT /W3 /O2 /Ob2 /D ENABLE_WWPROFILE',
             # WW3D2 too was built with ENABLE_WWPROFILE: +20 pairs (assetmgr, dazzle, sortingrenderer, ...)
             # and with USE_WWSHADE: dx8wrapper calls SHD_Init 0xA07CE0 / SHD_Shutdown 0xA07CF0 (0x921551, 0x921E90),
             # WW3D::Render SHD_Flush 0xA07D20 (0x917F15), the asset manager SHD_Register_Loader 0xA07D30 (0x91C4E3)
             'ww3d2': '/nologo /c /MT /W3 /O2 /Ob2 /D ENABLE_WWPROFILE /D USE_WWSHADE',
             # E&B's own code (D23): /GX (the STL corpus matches 533 functions with it, 501 without,
             # docs/app-plan.md 1; bitset.cpp 12/12 either way), otherwise the libraries' flags;
             # NDEBUG through APP_LIBS' defines (D61). tools/re/appfile.py's APP_FLAGS is the same.
             'app': '/nologo /c /MT /W3 /O2 /Ob2 /GX',
             # WWOnline's wnet: /GX. E&B's TCPCon constructor 0x8BBA70 and destructor 0x8BBC90 and tcpqueue.cpp's
             # TCPQueue::~TCPQueue 0x8BE890, add 0x8BF1C0, addHead 0x8BED40 have EH frames (handlers 0xAE03E9..0xAE050B)
             'wnet': '/nologo /c /MT /W3 /O2 /Ob2 /GX',
             # WWOnline's wlib: /GX too. Wtime's SignedAdd 0x8B9970 / SignedSubtract 0x8B9C80 have EH frames
             # (handlers 0xAE0388...) around their Wtime temporaries, and the by-value operators - / + 0x8BA060,
             # 0x8BA0F0, 0x8BA1D0, 0x8BA210 keep /GX's return-value flag (`push ecx; mov [esp+8], 0`)
             'wlib': '/nologo /c /MT /W3 /O2 /Ob2 /GX'}


# EA files the client does not link: not imported (plan, assemble) and dropped from the state.
# (library, file name in lower case) -> the evidence.
NOT_LINKED = {
    ('wwshade', 'shdlegacyw3d.cpp'):
        'no LegacyW3D shader in the client: no "LegacyW3D" string, SHD_Force_Links (0xA08C70) links 5 '
        'shaders, ShdRendererClass::Init_Shaders (0xA07E00) loops class ids 1..5, one prototype loader',
    ('wwshade', 'shdlegacyw3d.h'): 'its header (included by nothing else in E&B\'s wwshade)',
    ('wwaudio', 'audiosaveload.cpp'):
        'no audio save/load subsystem in the client: no "StaticAudioSaveLoadClass" or '
        '"DynamicAudioSaveLoadClass" string (their Name()), no "SaveLoadSubSystem"; its only pairs were shared '
        'helpers matched by other files (0x42F250, 0x909500, 0x9DA8C0)',
    ('wwaudio', 'audiosaveload.h'): 'its header (included by nothing else in the tree)',
    ('ww3d2', 'texturefilter.cpp'):
        'D9: Zero Hour\'s 08/05/02 KM "Texture filter class abstraction"; E&B\'s TextureClass keeps Renegade\'s '
        'filter members and accessors (TEXTURE_FILTER_PATCHES): the client has no out-of-line TextureFilterClass '
        'constructor or Apply (the four TextureClass constructors 0x94B240/0x94B3D0/0x94B5B0/0x94B660 store the five '
        'members themselves, Apply 0x94BC80 sets the filter states inline), and Set_Mip_Mapping 0x94CA20, '
        '_Init_Filters 0x94CA30 and _Set_Default_*_Filter 0x94CC30/0x94CC60/0x94CC90 lie inside texture.obj',
    ('ww3d2', 'texturefilter.h'): 'its header (MipCountType moved to texture.h, the only file that included it)',
    ('ww3d2', 'pot.cpp'):
        'D55: identical to wwmath\'s pot.cpp; both matched the same two client functions (Find_POT, Find_POT_Log2), '
        'and a program linking both libraries fails on the duplicate. wwmath\'s copy supplies them; ww3d2/pot.h stays',
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


def lib_of(path):
    """The library a file belongs to: an APP_LIBS library for files anywhere under its directory,
    else the file's directory name."""
    full = os.path.abspath(path)
    try:
        top = os.path.relpath(full, REPO).split(os.sep)[0].lower()
    except ValueError:  # another drive
        top = ''
    return top if top in APP_LIBS else os.path.basename(os.path.dirname(full)).lower()


# Files compiled with other flags than their library's (P-B2, D84: "not added until a library really
# mixes /GX and none"). wwlib's rawfile.cpp: every RawFileClass function is in the app range
# (0x683BE0-0x684DD0, none in wwlib's 0x9C7000-0x9E9300) and its constructors / destructor have EH
# frames (handlers 0xABF368, 0xABF393): E&B compiled it with the app's /GX.
FILE_FLAGS = {('wwlib', 'rawfile.cpp'): vc6.DEFAULT_FLAGS + ' /GX'}


def flags_of(path):
    """One file's compile flags: FILE_FLAGS, else its library's."""
    lib = lib_of(path)
    return FILE_FLAGS.get((lib, os.path.basename(path).lower()), LIB_FLAGS.get(lib, vc6.DEFAULT_FLAGS))


def flags_for(paths):
    """The compile flags of the files (all with the same flags)."""
    libs = {lib_of(p) for p in paths}
    flags = {flags_of(p) for p in paths}
    assert len(flags) == 1, f'files with different flags in one compile: {sorted(libs)}'
    return flags.pop()


def key_of(filename):
    return os.path.splitext(os.path.basename(filename))[0].lower()


def source_files(lib, copy):
    """{file key: (filename, full path)} for a copy's .cpp files."""
    d = os.path.join(COPY_ROOT[copy], LIB_DIR[lib][copy])
    return {key_of(f): (f, os.path.join(d, f)) for f in os.listdir(d) if f.lower().endswith('.cpp')}


def headers(lib, copy):
    d = os.path.join(COPY_ROOT[copy], LIB_DIR[lib][copy])
    return {f.lower(): (f, os.path.join(d, f)) for f in os.listdir(d) if f.lower().endswith(HEADER_EXT)}


def dest(lib):
    return os.path.join(REPO, LIB_TREE.get(lib, lib))


def lf(text):
    """The tree is LF (D147); EA's copies are CRLF."""
    return text.replace('\r\n', '\n')


def read(path):
    """An EA source or tree file as LF text."""
    return lf(open(path, encoding='latin1', newline='').read())


def tree_includes():
    wwonline = [os.path.join(REPO, 'libs', 'wwonline')] if os.path.isdir(os.path.join(REPO, 'libs', 'wwonline')) else []
    # D87: SGI STL 3.3's hash_map adapted for VC6 (libs\sgistl\hash_map.h), after the SDKs and VC6's own STL
    sgistl = [os.path.join(REPO, 'libs', 'sgistl')] if os.path.isdir(os.path.join(REPO, 'libs', 'sgistl')) else []
    return [dest(l) for l in LIBS if l not in LATE_INCLUDE_LIBS and os.path.isdir(dest(l))] + SDK_INCLUDES + \
        wwonline + sgistl + [REPO]  # the repository root last: BinkMovie's subtitle headers include <wwlib\vector.h>


def late_sync():
    """The LATE_INCLUDE_LIBS directories: not on the include list, but sent to the VM with every compile."""
    return [dest(l) for l in LATE_INCLUDE_LIBS if os.path.isdir(dest(l))]


def splice(text, donor_text, names):
    """Replace each named function definition in `text` with the donor's. Overloads are paired
    by order of definition. Returns (new text, names spliced)."""
    from easource import function_spans
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
