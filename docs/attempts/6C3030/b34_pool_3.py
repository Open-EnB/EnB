HYPOTHESIS = ('0x6C3030: batch 1 pointer sums (name + k) changed no inlining (they may fold); integer sums over a '
              'non-constant value (*name + k, (int)Strings.size() is a spender) do grow pools in 0x7AD090 / 0x7B9DC0: '
              'grow with them so a dead read after the erase (which otherwise sends the whole erase out) cuts only the '
              'deepest ~pair expansion')
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


def sums(n, v='*name'):
    return ''.join(' (void)(%s + %d);' % (v, k) for k in range(n))


def form(top='', post='', mid=''):
    return ('\tif (name != NULL) {' + top + '\n\t\t%s it = Strings.find(std::string(name));\n' % IT +
            '\t\tif (it != Strings.end()) {' + mid + '\n\t\t\tdelete (*it).second;\n\t\t\tStrings.erase(it);' + post +
            '\n\t\t}\n\t}\n}\n')


FORMS = {}
for n in (2, 4, 8):
    FORMS['c%d' % n] = [(OLD, form(top=sums(n)))]
    FORMS['c%d_sz1' % n] = [(OLD, form(top=sums(n), post=' Strings.size();'))]
FORMS['c16_sz1'] = [(OLD, form(top=sums(16), post=' Strings.size();'))]
FORMS['c4_cp'] = [(OLD, form(top=sums(4), post=' std::hash_map<std::string, FontStringStruct *>::iterator j = it;'))]
FORMS['c4_mid'] = [(OLD, form(mid=sums(4)))]
FORMS['c4_post'] = [(OLD, form(post=sums(4)))]
FORMS['i4'] = [(OLD, form(mid=sums(4, 'it._M_cur->_M_val.first.size()')))]
FORMS['nm4_sz1'] = [(OLD, form(top=sums(4, '(int)name'), post=' Strings.size();'))]
