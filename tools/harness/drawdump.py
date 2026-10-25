"""Reads the draw dumps of d3d8shot.c (draws_NNNNN.bin, run_reference.py --dump) (M2.4).

    python tools/harness/drawdump.py list FILE [--fvf HEX]            # one line per indexed draw
    python tools/harness/drawdump.py groups FILE                      # draws grouped by FVF and textures
    python tools/harness/drawdump.py show FILE N                      # draw N: states, first vertices

Each record is one DrawIndexedPrimitive: its counts, stream 0 stride, the vertex shader (an FVF),
textures 0 and 1 (pointer, size, format), the world, view and projection matrices, the indices
and the vertices it uses. read() gives them to other scripts as dicts; positions() decodes an
XYZ-first vertex's position.
"""
import collections
import struct
import sys


# d3d8shot.c's DumpStates and DumpStageStates (D3DRENDERSTATETYPE, D3DTEXTURESTAGESTATETYPE)
STATES = ['ZENABLE', 'ZWRITEENABLE', 'ALPHATESTENABLE', 'SRCBLEND', 'DESTBLEND', 'CULLMODE',
          'ALPHABLENDENABLE', 'FOGENABLE', 'FOGCOLOR', 'FOGTABLEMODE', 'FOGSTART', 'FOGEND', 'ZBIAS',
          'TEXTUREFACTOR', 'LIGHTING', 'AMBIENT', 'FOGVERTEXMODE', 'COLORVERTEX', 'DIFFUSEMATERIALSOURCE',
          'AMBIENTMATERIALSOURCE', 'EMISSIVEMATERIALSOURCE', 'RANGEFOGENABLE', 'SHADEMODE', 'ALPHAREF']
STAGE_STATES = ['COLOROP', 'COLORARG1', 'COLORARG2', 'ALPHAOP', 'ALPHAARG1', 'ALPHAARG2', 'TEXCOORDINDEX',
                'TEXTURETRANSFORMFLAGS']


def read(path):
    data = open(path, 'rb').read()
    at, out = 0, []
    while at + 40 <= len(data):
        head = struct.unpack_from('<4s9I', data, at)
        if head[0] not in (b'DIP1', b'DIP2'):
            raise SystemExit(f'{path}: bad record at {at}')
        at += 40
        tex = [struct.unpack_from('<4I', data, at), struct.unpack_from('<4I', data, at + 16)]
        at += 32
        mats = [struct.unpack_from('<16f', data, at + 64 * k) for k in range(3)]
        at += 192
        states, stages = {}, [{}, {}]
        if head[0] == b'DIP2':
            v = struct.unpack_from(f'<{len(STATES)}I', data, at)
            states = dict(zip(STATES, v))
            at += 4 * len(STATES)
            for s in range(2):
                v = struct.unpack_from(f'<{len(STAGE_STATES)}I', data, at)
                stages[s] = dict(zip(STAGE_STATES, v))
                at += 4 * len(STAGE_STATES)
        n = struct.unpack_from('<I', data, at)[0]
        at += 4
        idx = struct.unpack_from(f'<{n}I', data, at)
        at += 4 * n
        _, ptype, minidx, count, start, prims, base, stride, fvf, isz = head
        verts = data[at:at + count * stride]
        at += count * stride
        out.append(dict(type=ptype, min=minidx, count=count, start=start, prims=prims, base=base,
                        stride=stride, fvf=fvf, isize=isz, tex=tex, world=mats[0], view=mats[1],
                        proj=mats[2], states=states, stages=stages, indices=idx, verts=verts, n=len(out)))
    return out


def positions(d):
    """The vertices' x, y, z (an FVF with D3DFVF_XYZ first: every stride starts with 3 floats)."""
    s = d['stride']
    return [struct.unpack_from('<3f', d['verts'], i * s) for i in range(d['count'])]


def floats(d, i):
    s = d['stride']
    return struct.unpack_from(f'<{s // 4}f', d['verts'], i * s)


def describe(d):
    t0, t1 = d['tex']
    w = d['world']
    return (f"#{d['n']:4d} type {d['type']} verts {d['count']:5d} prims {d['prims']:5d} stride {d['stride']:2d} "
            f"fvf {d['fvf']:04X} tex0 {t0[1]}x{t0[2]} f{t0[3]} tex1 {t1[1]}x{t1[2]} f{t1[3]} "
            f"world T ({w[12]:.0f}, {w[13]:.0f}, {w[14]:.0f})")


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ('list', 'groups', 'show'):
        raise SystemExit(__doc__)
    draws = read(sys.argv[2])
    if sys.argv[1] == 'show':
        d = draws[int(sys.argv[3])]
        print(describe(d))
        print('states', ' '.join(f'{k}={v:X}' for k, v in d['states'].items()))
        for s in range(2):
            print(f'stage {s}', ' '.join(f'{k}={v:X}' for k, v in d['stages'][s].items()))
        for i in range(min(4, d['count'])):
            print('  vertex', i, [round(x, 4) for x in floats(d, i)])
        return
    if sys.argv[1] == 'list':
        want = int(sys.argv[sys.argv.index('--fvf') + 1], 16) if '--fvf' in sys.argv else None
        for d in draws:
            if want is None or d['fvf'] == want:
                print(describe(d))
    else:
        g = collections.OrderedDict()
        for d in draws:
            k = (d['fvf'], d['stride'], d['tex'][0][1:], d['tex'][1][1:])
            e = g.setdefault(k, [0, 0, 0])
            e[0] += 1
            e[1] += d['prims']
            e[2] += d['count']
        for (fvf, stride, t0, t1), (n, p, v) in g.items():
            print(f'fvf {fvf:04X} stride {stride:2d} tex0 {t0} tex1 {t1}: {n} draws, {p} prims, {v} verts')


if __name__ == '__main__':
    main()
