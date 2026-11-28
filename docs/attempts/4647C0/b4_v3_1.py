HYPOTHESIS = ('B4 fn3 batch 1 (lever: workflow.md 3.1.2 frame row "temporaries share slots with named locals, named locals '
              'don\'t share with each other"): the client stores (float)HalfSize and (float)(Size - 1) in the same slot '
              '(-0x18), so at least one is a compiler temporary (a cast written at each use, CSE\'d), not a named local')
FILE = 'app/shared/heightfield.cpp'
SYMBOL = ''
ADDRESS = 0x4647C0

HALF_OLD = ('\tfloat half = (float)HalfSize;\n'
            '\tfloat x0 = start.X / XSpacing + half;\n'
            '\tfloat y0 = start.Y / YSpacing + half;\n'
            '\tfloat x1 = end.X / XSpacing + half;\n'
            '\tfloat y1 = end.Y / YSpacing + half;\n')
HALF_NEW = HALF_OLD.replace('\tfloat half = (float)HalfSize;\n', '\n').replace('+ half;', '+ (float)HalfSize;')
LIM_OLD = '\t\tfloat limit = (float)(Size - 1);\n'
LIM_USES = [('\t\t\tif (x0 < 0.0f || x0 > limit) {\n', '\t\t\tif (x0 < 0.0f || x0 > (float)(Size - 1)) {\n'),
            ('\t\t\ttmax_x = (limit - x0) / dx;\n', '\t\t\ttmax_x = ((float)(Size - 1) - x0) / dx;\n'),
            ('\t\t\tif (y0 < 0.0f || y0 > limit) {\n', '\t\t\tif (y0 < 0.0f || y0 > (float)(Size - 1)) {\n'),
            ('\t\t\ttmax_y = (limit - y0) / dy;\n', '\t\t\ttmax_y = ((float)(Size - 1) - y0) / dy;\n')]
LIM = [(LIM_OLD, '\n')] + LIM_USES
LIMU = [(LIM_OLD, '\n')] + [(a, b.replace('(float)(Size - 1)', '(float)(unsigned int)(Size - 1)')) for a, b in LIM_USES]
HALFU = [(HALF_OLD, HALF_OLD.replace('\tfloat half = (float)HalfSize;\n', '\n').replace('+ half;', '+ (float)(unsigned int)HalfSize;'))]
H = [(HALF_OLD, HALF_NEW)]
FORMS = {
    'half': H,
    'limit': LIM,
    'both': H + LIM,
    'halfu': HALFU,
    'both_u': HALFU + LIMU,
    'half_limu': H + LIMU,
}
