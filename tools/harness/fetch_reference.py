#!/usr/bin/env python3
"""Fetch and unpack the EA captures' frozen reference data (D154) for the capture harness.

    python tools/harness/fetch_reference.py                 # download, check, unpack
    python tools/harness/fetch_reference.py --archive F     # unpack a copy you already have

The archive is a release asset of this repository (6.6 MB; about 260 MB unpacked): for each EA
capture (capture_1..3, alex3, alex7) the decoded message trace (ea/NAME.trace.jsonl) and the
reference readings the harness compares with (ref/NAME.*.txt). It is checked against its SHA-256
and unpacked into tools/re/roots.py's EA_REFERENCE (ENB_EA_REFERENCE overrides it). The data is
fixed: it will not be regenerated. Standard library only.
"""
import argparse
import hashlib
import os
import shutil
import subprocess
import sys
import tarfile
import urllib.request

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HERE), 're'))
import roots  # noqa: E402

TAG = 'ea-reference-2026-10-30'
NAME = TAG + '.tar.xz'
URL = f'https://github.com/Open-EnB/EnB/releases/download/{TAG}/{NAME}'
SHA256 = '5b1ceebd46d4ae9e88d5e7d14080fb15b2860d013e4b4482d1df3b9a34f0c35e'
MEDIA = os.path.dirname(roots.EA_REFERENCE)


def sha256(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return h.hexdigest()


def download(dest):
    print(f'downloading {URL}')
    try:
        # curl (in Windows 10+) uses the system's certificates; Python's own may lack the issuer
        if shutil.which('curl'):
            subprocess.run(['curl', '-fsSL', '-o', dest + '.part', URL], check=True)
        else:
            with urllib.request.urlopen(URL) as r, open(dest + '.part', 'wb') as f:
                shutil.copyfileobj(r, f, 1 << 20)
        os.replace(dest + '.part', dest)
    except (OSError, subprocess.CalledProcessError) as e:
        # a private repository's assets need the GitHub CLI's login
        print(f'  {e}; trying gh release download')
        subprocess.run(['gh', 'release', 'download', TAG, '-R', 'Open-EnB/EnB', '-p', NAME, '-D',
                        os.path.dirname(dest), '--clobber'], check=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--archive', help='an already downloaded copy of the archive')
    a = ap.parse_args()
    arc = a.archive or os.path.join(MEDIA, NAME)
    os.makedirs(MEDIA, exist_ok=True)
    if not os.path.exists(arc) or sha256(arc) != SHA256:
        if a.archive:
            raise SystemExit(f'{arc}: SHA-256 {sha256(arc)}, expected {SHA256}')
        download(arc)
    got = sha256(arc)
    if got != SHA256:
        raise SystemExit(f'{arc}: SHA-256 {got}, expected {SHA256}; delete it and rerun')
    out = roots.EA_REFERENCE
    with tarfile.open(arc, 'r:xz') as tar:
        for m in tar.getmembers():
            if m.name.startswith(('/', '..')) or '..' in m.name.split('/') or not (m.isfile() or m.isdir()):
                raise SystemExit(f'{arc}: unexpected member {m.name}')
        tar.extractall(out)
    print(f'unpacked {NAME} (SHA-256 OK) into {out}')


if __name__ == '__main__':
    main()
