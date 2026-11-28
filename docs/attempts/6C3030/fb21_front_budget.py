HYPOTHESIS = '0x6C3030 one row (the erase second _M_delete_node site inlines ~pair where the client calls ??_G(0)): budget spent before the find instead of after it (dead size() / end() / bucket_count() in front of the string temporary), a named key string, the test as a named end iterator'
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
def form(pre='', find='%s it = Strings.find(std::string(name));' % IT, test='it != Strings.end()', post=''):
    return ('\tif (name != NULL) {\n\t\t' + pre + find + '\n\t\tif (' + test + ') {\n\t\t\tdelete (*it).second;\n\t\t\tStrings.erase(it);' + post + '\n\t\t}\n\t}\n}\n')

src = open(FILE, encoding='utf-8').read()
assert src.count(OLD) == 1
FORMS = {
    'sz1': [(OLD, form(pre='Strings.size(); '))],
    'sz2': [(OLD, form(pre='Strings.size(); Strings.size(); '))],
    'sz4': [(OLD, form(pre='Strings.size(); Strings.size(); Strings.size(); Strings.size(); '))],
    'end1': [(OLD, form(pre='Strings.end(); '))],
    'end2': [(OLD, form(pre='Strings.end(); Strings.end(); '))],
    'bc1': [(OLD, form(pre='Strings.bucket_count(); '))],
    'key': [(OLD, form(find='std::string key(name); %s it = Strings.find(key);' % IT))],
    'endit': [(OLD, form(find='%s it = Strings.find(std::string(name)); %s e = Strings.end();' % (IT, IT), test='it != e'))],
    'neq': [(OLD, form(test='!(it == Strings.end())', pre='Strings.empty(); '))],
    'cur': [(OLD, form(test='it._M_cur != NULL'))],
}
