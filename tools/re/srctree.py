"""Recovers what client.exe says about its own source: file paths, class names, RTTI.

Writes to docs/data/:
  source-paths.txt   every source path compiled into an assert or __FILE__ string
  class-methods.txt  every `Class::Method` named in a string (profile labels, asserts, logs)
  rtti.txt           MSVC RTTI type descriptors (.?AV...), i.e. polymorphic class names
  asserts.txt        strings that look like assert expressions from the client's own code

    python srctree.py [--client PATH]
"""
import argparse
import os
import re

from clientimage import ClientImage, DEFAULT_CLIENT

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.normpath(os.path.join(HERE, '..', '..', 'docs', 'data'))
SOURCE = re.compile(r'\.(cpp|c|h|hpp|inl)$', re.I)
CLASS_METHOD = re.compile(r'\b([A-Z][A-Za-z0-9_]+)::(~?[A-Za-z_][A-Za-z0-9_]*)')
ASSERT_LIKE = re.compile(r'(->|\.|==|!=|<=|>=|&&|\|\|)')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--client', default=DEFAULT_CLIENT)
    args = ap.parse_args()
    img = ClientImage(args.client)
    strings = img.strings(4)
    os.makedirs(DATA, exist_ok=True)

    paths = sorted({s for s in strings.values() if SOURCE.search(s) and ('\\' in s or '/' in s)},
                   key=str.lower)
    with open(os.path.join(DATA, 'source-paths.txt'), 'w') as f:
        f.write('\n'.join(paths) + '\n')

    methods = sorted({m.group(0) for s in strings.values() for m in CLASS_METHOD.finditer(s)})
    with open(os.path.join(DATA, 'class-methods.txt'), 'w') as f:
        f.write('\n'.join(methods) + '\n')

    rtti = sorted({s for s in strings.values() if s.startswith(('.?AV', '.?AU'))})
    with open(os.path.join(DATA, 'rtti.txt'), 'w') as f:
        f.write('\n'.join(rtti) + '\n')

    # Assert expressions sit next to the __FILE__ string of their source file. Record the most
    # recent path seen (string order follows object order) so each expression has a home.
    current = None
    rows = []
    for va in sorted(strings):
        s = strings[va]
        if SOURCE.search(s) and '\\' in s:
            current = s
            continue
        if current and ASSERT_LIKE.search(s) and ' ' not in s.strip()[:1] and len(s) < 200 \
                and not s.startswith(('%', 'http')) and re.search(r'[A-Za-z_]\w*', s):
            if re.fullmatch(r'[\w\s()!<>=&|.\->\[\]*+,:/"\'?~^%]+', s):
                rows.append(f'0x{va:06X}\t{current}\t{s}')
    with open(os.path.join(DATA, 'asserts.txt'), 'w') as f:
        f.write('\n'.join(rows) + '\n')

    print(f'{len(paths)} source paths, {len(methods)} Class::Method names, {len(rtti)} RTTI names, '
          f'{len(rows)} assert-like strings -> {DATA}')


if __name__ == '__main__':
    main()
