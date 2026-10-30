"""The art folder our vidtest reads (D120, a D69 safety default).

LandClass::Load_Assets rewrites the .pld beside the art whenever loading it fails (Save_PLD), so our
vidtest must never read the game's install folder. drawcheck.py, vidtest_sbs.py and run_vidtest.py
pass `-art build/art` whenever that folder exists and no art folder was given; without it they keep
vidtest's own default (the working directory when it holds mu01a.w3d, else the install's EBCONFIG)
and print a warning.

build/art is a copy of EBCONFIG made with its dates (without -p the .pld's date changes and
Save_PLD rewrites it):

    cp -rp "/c/Program Files (x86)/EA GAMES/Earth & Beyond/EBCONFIG" build/art
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
BUILD_ART = os.path.join(REPO, 'build', 'art')
MAKE = 'cp -rp "/c/Program Files (x86)/EA GAMES/Earth & Beyond/EBCONFIG" build/art'


def default_art(art=None):
    """The folder to pass as vidtest's -art: `art` when given, else build/art when it exists, else
    None (vidtest's own default) after a warning."""
    if art:
        return os.path.abspath(art)
    if os.path.isdir(BUILD_ART):
        return BUILD_ART
    print(f'WARNING: no {BUILD_ART} and no art folder given: vidtest falls back to the install folder '
          f'unless its working directory holds the art, and LandClass may rewrite the .pld there (D69, D120). '
          f'Make the copy: {MAKE}', file=sys.stderr)
    return None


def art_args(extra):
    """vidtest arguments `-art DIR` to put before `extra`, unless `extra` already gives -art."""
    if '-art' in extra:
        return []
    art = default_art()
    return ['-art', art] if art else []
