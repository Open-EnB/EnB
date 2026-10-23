"""Prepares the third-party SDK headers the way E&B's build used them, outside the repository.

The RAD (Miles) headers are not ours to publish, so instead of a patched copy in the repository
this script derives one beside the SDK: C:\\EnBDevelopment\\SDK\\radtools\\include_enb, a copy of
include\\ with the changes below. The build uses include_enb (importer.SDK_INCLUDES).

    python sdk_patch.py

Changes:
  mss.h  The Miles API is declared __declspec(dllimport), as in the client: its Miles calls go
         through the import table (call dword ptr [__imp__AIL_...]). This 6.1a header hard-codes
         plain `extern` (`#if 1 /*def __BORLANDC__*/`); docs/toolchain.md §2.
  mss.h  AIL_startup() calls MSS_auto_cleanup() first, as the client's WWAudioClass constructor
         does (0x9EEC54 calls MSS_auto_cleanup 0xA03A30, then AIL_startup); this header has the
         macro commented out ("ODCODENOTE Remove").
"""
import os
import shutil

SDK = r'C:\EnBDevelopment\SDK\radtools'
SRC = os.path.join(SDK, 'include')
DST = os.path.join(SDK, 'include_enb')

PATCHES = {
    'mss.h': [('#if 1 /*def __BORLANDC__*/', '#if 0 /*def __BORLANDC__*/ /* EnB: dllimport, as the client */'),
              ('//#define AIL_startup() (MSS_auto_cleanup(),AIL_startup())',
               '#define AIL_startup() (MSS_auto_cleanup(),AIL_startup()) /* EnB: as the client */')],
}


def main():
    if os.path.isdir(DST):
        shutil.rmtree(DST)
    shutil.copytree(SRC, DST)
    for name, edits in PATCHES.items():
        path = os.path.join(DST, name)
        text = open(path, encoding='latin1', newline='').read()
        for old, new in edits:
            if text.count(old) != 1:
                raise SystemExit(f'{name}: expected one "{old}"')
            text = text.replace(old, new)
        open(path, 'w', encoding='latin1', newline='').write(text)
    print(f'{DST}: {len(os.listdir(DST))} headers, {sum(len(e) for e in PATCHES.values())} change(s)')


if __name__ == '__main__':
    main()
