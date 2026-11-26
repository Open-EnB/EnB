"""The one place that finds and renames a class inside an MSVC 6 decorated name (two bugs of 2026-11-26 came from
copies of a boundary pattern; every tool here uses these functions).

A class name is spelled out as an identifier followed by '@'. It starts
  - after '??0' / '??1' / '??4' ... or '??_G' / '??_E' ... (a special member: the class follows the code letter),
  - after its type code 'V' / 'U' (class / struct) when that follows a type letter, a digit, '@', '?' or '$'
    (PAVFoo@@, ABV?$RefCountPtr@VFoo@@@, ?AVFoo@@ ...),
  - or after any character that cannot be part of an identifier ('?', '@', '$').
Back-references (single digits) are not spellings and are left alone, and neither is the method name (the
identifier right after a single leading '?'), even when it equals a class name (found by the self-test; the
pre-2026-11-26 copies renamed it).
"""
import re

NAME_START = r'(?<!^\?)(?:(?<=^\?\?[0-9A-Z])|(?<=^\?\?_[A-Z])|(?<=[A-Z0-9@?$][VU])|(?<![A-Za-z0-9_]))'
_QUAL = (re.compile(r'\?\?(?:0|1|_G|_E|4)([A-Za-z_][A-Za-z0-9_]*)@@'),
         re.compile(r'\?[A-Za-z_][A-Za-z0-9_]*@([A-Za-z_][A-Za-z0-9_]*)@@'))


def class_of(name):
    """The simple class qualifier of a member name (constructor, destructor, deleting destructor, operator=, or
    ?method@Class@@), or None."""
    for p in _QUAL:
        m = p.match(name)
        if m:
            return m.group(1)
    return None


def spelling(cls):
    """A compiled pattern for every spelled-out occurrence of class `cls`."""
    return re.compile(NAME_START + re.escape(cls) + '@')


def rename(name, old, new):
    """`name` with every spelled-out occurrence of class `old` replaced by `new`."""
    return spelling(old).sub(new + '@', name)


def rename_all(name, mapping, _cache={}):
    """`name` with every class in `mapping` (old -> new) renamed, in one pass (longest names first)."""
    key = id(mapping), len(mapping)
    if key not in _cache:
        _cache.clear()
        alt = '|'.join(sorted(map(re.escape, mapping), key=len, reverse=True))
        _cache[key] = re.compile(NAME_START + '(' + alt + ')@')
    return _cache[key].sub(lambda m: mapping[m.group(1)] + '@', name)


def contains(name, cls):
    return spelling(cls).search(name) is not None


def _selftest():
    cases = [
        ('??_GPacket_7530Class@@UAEPAXI@Z', 'Packet_7530Class', 'X', '??_GX@@UAEPAXI@Z'),
        ('??1Foo@@UAE@XZ', 'Foo', 'Bar', '??1Bar@@UAE@XZ'),
        ('??0Foo@@QAE@PAVFoo@@@Z', 'Foo', 'Bar', '??0Bar@@QAE@PAVBar@@@Z'),
        ('?Build@P@@UAEPAVRawPacketClass@@XZ', 'RawPacketClass', 'PacketClass', '?Build@P@@UAEPAVPacketClass@@XZ'),
        ('?I@C@@QAEXABV?$RefCountPtr@VElem@@@@H@Z', 'Elem', 'E2', '?I@C@@QAEXABV?$RefCountPtr@VE2@@@@H@Z'),
        ('?F@GuildPacket@@QAEXXZ', 'Packet', 'Y', '?F@GuildPacket@@QAEXXZ'),          # no substring rename
        ('?F@Bar@@QAEXPAVXFoo@@@Z', 'Foo', 'Y', '?F@Bar@@QAEXPAVXFoo@@@Z'),
        ('?Foo@Foo@@QAEXXZ', 'Foo', 'Y', '?Foo@Y@@QAEXXZ'),                          # the method keeps its name
    ]
    for name, old, new, want in cases:
        got = rename(name, old, new)
        assert got == want, (name, old, new, got, want)
    assert class_of('??_EFoo@@UAEPAXI@Z') == 'Foo' and class_of('?m@Foo@@QAEXXZ') == 'Foo'
    return len(cases)


if __name__ == '__main__':
    print(f'mangle self-test: {_selftest()} cases passed')
