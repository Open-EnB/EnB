#!/usr/bin/env python3
"""Prepares the VC6 install media for the build VM (docs/setup-vc6.md).

The project never redistributes Microsoft's media: each contributor downloads the two Internet
Archive items below. This script checks the downloads against the archive's SHA-1 hashes, unpacks
them with 7-Zip, and turns each disc image (an Alcohol 120% MDF of raw 2,448-byte sectors: 2,352
bytes of sector plus 96 of subchannel) into a plain ISO, which Hyper-V attaches as a DVD with no
mounting tool (WinCDEmu not needed).

    python tools/vc6/setup/media.py --media DIR            # DIR holds the two .7z downloads
    python tools/vc6/setup/media.py --media DIR --download # fetch them first (about 700 MB)

Writes DIR/vc6pro-disc1.iso and DIR/vs6sp4.iso and checks each ISO's volume label. Standard
library plus 7-Zip (`7z` on PATH, or C:\\Program Files\\7-Zip\\7z.exe).
"""
import argparse
import hashlib
import os
import shutil
import subprocess
import sys
import urllib.request

# name -> (Internet Archive item, file in the item, SHA-1 from the item's metadata, volume label)
MEDIA = {
    'vc6pro-disc1': ('X04-14444', '1999-02_X04-14444_X04-14444_VSP600ENU1.7z',
                     '50a0674f8e881410c3f35fc7e09bb39213167797', 'VSP600ENU1'),
    'vs6sp4': ('X05-86594', '2000-08_X05-86594_X05-86829_vs6sp4.7z',
               '7ba4001a8e6a3af8f9807d85471b5ea83000eae8', 'VS6SP4'),
}
RAW = 2448          # bytes per sector in the MDF: 2,352 raw + 96 subchannel
DATA_OFF = 16       # Mode 1: 12 sync + 4 header bytes, then 2,048 bytes of user data
DATA = 2048


def sha1(path):
    h = hashlib.sha1()
    with open(path, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return h.hexdigest()


def seven_zip():
    for c in (shutil.which('7z'), r'C:\Program Files\7-Zip\7z.exe', r'C:\Program Files (x86)\7-Zip\7z.exe'):
        if c and os.path.exists(c):
            return c
    raise SystemExit('7-Zip not found: install it (https://www.7-zip.org/) or put 7z on PATH')


def find_archive(media, name):
    item, fname, _, _ = MEDIA[name]
    for cand in (fname, name + '.7z'):
        p = os.path.join(media, cand)
        if os.path.exists(p):
            return p
    return None


def download(media, name):
    item, fname, _, _ = MEDIA[name]
    url = f'https://archive.org/download/{item}/{fname}'
    out = os.path.join(media, fname)
    print(f'downloading {url}')
    tmp = out + '.part'
    if shutil.which('curl'):   # Windows' curl uses the system's certificates; Python's may lack the issuer
        subprocess.run(['curl', '-fsSL', '-o', tmp, url], check=True)
    else:
        with urllib.request.urlopen(url) as r, open(tmp, 'wb') as f:
            shutil.copyfileobj(r, f, 1 << 20)
    os.replace(tmp, out)
    return out


def mdf_to_iso(mdf, iso):
    size = os.path.getsize(mdf)
    if size % RAW:
        raise SystemExit(f'{mdf}: {size} bytes is not a whole number of {RAW}-byte sectors')
    with open(mdf, 'rb') as src, open(iso + '.part', 'wb') as dst:
        for _ in range(size // RAW):
            s = src.read(RAW)
            if s[:12] != b'\x00' + b'\xff' * 10 + b'\x00' or s[15] != 1:
                raise SystemExit(f'{mdf}: not a Mode 1 data sector')
            dst.write(s[DATA_OFF:DATA_OFF + DATA])
    os.replace(iso + '.part', iso)


def iso_label(iso):
    with open(iso, 'rb') as f:
        f.seek(16 * DATA)
        pvd = f.read(DATA)
    if pvd[1:6] != b'CD001':
        return None
    return pvd[40:72].decode('ascii', 'replace').strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--media', required=True, help='folder with (or for) the two .7z downloads')
    ap.add_argument('--download', action='store_true', help='download missing archives from archive.org')
    a = ap.parse_args()
    os.makedirs(a.media, exist_ok=True)
    sz = seven_zip()
    ok = True
    for name, (item, fname, want, label) in MEDIA.items():
        iso = os.path.join(a.media, name + '.iso')
        if os.path.exists(iso) and iso_label(iso) == label:
            print(f'{name}: {iso} present (label {label})')
            continue
        arc = find_archive(a.media, name)
        if not arc:
            if not a.download:
                print(f'{name}: missing; download https://archive.org/details/{item} ({fname}) into {a.media}, '
                      f'or rerun with --download')
                ok = False
                continue
            arc = download(a.media, name)
        got = sha1(arc)
        if got != want:
            raise SystemExit(f'{arc}: SHA-1 {got}, expected {want} (archive.org metadata); delete it and download again')
        print(f'{name}: {os.path.basename(arc)} SHA-1 OK')
        work = os.path.join(a.media, name + '.x')
        subprocess.run([sz, 'x', '-y', '-o' + work, arc], check=True, stdout=subprocess.DEVNULL)
        mdfs = [os.path.join(d, f) for d, _, fs in os.walk(work) for f in fs if f.lower().endswith('.mdf')]
        if len(mdfs) != 1:
            raise SystemExit(f'{arc}: expected one .mdf inside, found {len(mdfs)}')
        mdf_to_iso(mdfs[0], iso)
        shutil.rmtree(work)
        got_label = iso_label(iso)
        if got_label != label:
            raise SystemExit(f'{iso}: volume label {got_label!r}, expected {label!r}')
        print(f'{name}: wrote {iso} (label {label})')
    if not ok:
        sys.exit(1)


if __name__ == '__main__':
    main()
