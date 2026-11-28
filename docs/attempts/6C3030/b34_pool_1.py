HYPOTHESIS = ('0x6C3030 one row (the inlined erase second _M_delete_node site inlines ~pair where the client calls ??_G(0)): '
              'B31 pool lever: dead sums grow the function pool, so a dead read after the erase can cut only the deepest '
              'nested expansion (~pair) instead of the whole erase; also sums alone')
FILE = 'app/client/fontdatabase.cpp'
SYMBOL = 'FontDatabaseClass::Remove_String'
ADDRESS = 0x6C3030

OLD = '''	if (name != NULL) {
		std::hash_map<std::string, FontStringStruct *>::iterator it = Strings.find(std::string(name));
		if (it != Strings.end()) {
			delete (*it).second;
			Strings.erase(it);
		}
	}
}
'''
IT = 'std::hash_map<std::string, FontStringStruct *>::iterator'


def sums(n, v='name'):
    return ''.join(' (void)(%s + %d);' % (v, k) for k in range(n))


def form(top='', post=''):
    return ('\tif (name != NULL) {' + top + '\n\t\t%s it = Strings.find(std::string(name));\n' % IT +
            '\t\tif (it != Strings.end()) {\n\t\t\tdelete (*it).second;\n\t\t\tStrings.erase(it);' + post +
            '\n\t\t}\n\t}\n}\n')


FORMS = {}
for n in (1, 2, 4, 8):
    FORMS['s%d' % n] = [(OLD, form(top=sums(n)))]
for n in (2, 4, 8):
    FORMS['s%d_sz1' % n] = [(OLD, form(top=sums(n), post=' Strings.size();'))]
    FORMS['s%d_end1' % n] = [(OLD, form(top=sums(n), post=' Strings.end();'))]
FORMS['s4_post_sz1'] = [(OLD, form(post=sums(4) + ' Strings.size();'))]
