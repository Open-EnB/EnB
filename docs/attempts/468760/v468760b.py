import sys, itertools
order, side, dloc = sys.argv[1], sys.argv[2], sys.argv[3]
src = '.work/tk9/m/landdmap.cpp'
s = open('.work/tk9/m/landdmap.base.cpp').read()
i = s.index('int LandUnknown466BE0Class::Unknown_468760(const Vector3 &center, const Vector3 &extent)\n')
j = s.index('\n}\n', i) + 3
axis = {'X': ('a', 'X'), 'Y': ('b', 'Y'), 'Z': ('c', 'Z')}


def dot(v):
	terms = []
	for ax in order:
		p, c = axis[ax]
		if side == 'p':
			terms.append('Planes[i].%s * %s%s' % (p, v, c.lower()))
		else:
			terms.append('%s%s * Planes[i].%s' % (v, c.lower(), p))
	d = 'd' if dloc == 'd' else 'Planes[i].d'
	return ' + '.join(terms) + ' + ' + d


body = '''int LandUnknown466BE0Class::Unknown_468760(const Vector3 &center, const Vector3 &extent)
{
	Vector3 lo, hi;
	Vector3::Subtract(center, extent, &lo);
	Vector3::Add(center, extent, &hi);
	for (int i = 0; i < 4; i++) {
		float nx, ny, nz;
		float fx, fy, fz;
		if (Planes[i].a >= 0.0f) { nx = lo.X; fx = hi.X; } else { nx = hi.X; fx = lo.X; }
		if (Planes[i].b >= 0.0f) { ny = lo.Y; fy = hi.Y; } else { ny = hi.Y; fy = lo.Y; }
		if (Planes[i].c >= 0.0f) { nz = lo.Z; fz = hi.Z; } else { nz = hi.Z; fz = lo.Z; }
''' + ('		float d = Planes[i].d;\n' if dloc == 'd' else '') + '''		if (%s > 0.0f) {
			return 0;
		}
		if (%s >= 0.0f) {
			return 1;
		}
	}
	return 2;
}
''' % (dot('n'), dot('f'))
s = s[:i] + body + s[j:]
open(src, 'w', newline='\n').write(s)
