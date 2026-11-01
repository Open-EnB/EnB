import sys
form = sys.argv[1]
src = '.work/tk9/m/landdmap.cpp'
s = open('.work/tk9/m/landdmap.base.cpp').read()
i = s.index('int LandUnknown466BE0Class::Unknown_468760(const Vector3 &center, const Vector3 &extent)\n')
j = s.index('\n}\n', i) + 3
forms = {}
forms['A'] = '''int LandUnknown466BE0Class::Unknown_468760(const Vector3 &center, const Vector3 &extent)
{
	Vector3 lo = center - extent;
	Vector3 hi = center + extent;
	for (int i = 0; i < 4; i++) {
		float nx, ny, nz;
		Vector3 f;
		if (Planes[i].a >= 0.0f) { nx = lo.X; f.X = hi.X; } else { nx = hi.X; f.X = lo.X; }
		if (Planes[i].b >= 0.0f) { ny = lo.Y; f.Y = hi.Y; } else { ny = hi.Y; f.Y = lo.Y; }
		if (Planes[i].c >= 0.0f) { nz = lo.Z; f.Z = hi.Z; } else { nz = hi.Z; f.Z = lo.Z; }
		float d = Planes[i].d;
		if (nz * Planes[i].c + nx * Planes[i].a + ny * Planes[i].b + d > 0.0f) {
			return 0;
		}
		if (f.Z * Planes[i].c + f.X * Planes[i].a + f.Y * Planes[i].b + d >= 0.0f) {
			return 1;
		}
	}
	return 2;
}
'''
forms['B'] = forms['A'].replace('''		float nx, ny, nz;
		Vector3 f;
''', '''		Vector3 n, f;
''').replace('nx', 'n.X').replace('ny', 'n.Y').replace('nz', 'n.Z')
forms['C'] = forms['A'].replace('''		float d = Planes[i].d;
''', '').replace(' + d ', ' + Planes[i].d ')
forms['D'] = forms['A'].replace('''		float nx, ny, nz;
		Vector3 f;
''', '''		float nx, ny, nz;
		float fx, fy, fz;
''').replace('f.X', 'fx').replace('f.Y', 'fy').replace('f.Z', 'fz')
forms['E'] = forms['C'].replace('Vector3 hi = center + extent;', 'Vector3 hi = extent + center;')
forms['F'] = forms['C'].replace('''	Vector3 lo = center - extent;
	Vector3 hi = center + extent;
''', '''	Vector3 lo, hi;
	Vector3::Subtract(center, extent, &lo);
	Vector3::Add(center, extent, &hi);
''')
s = s[:i] + forms[form] + s[j:]
open(src, 'w', newline='\n').write(s)
