HYPOTHESIS = ('0x6C3030 one row (push 1 vs push 0 at 0x6C3295: ours inlines ~pair at the inlined erase second '
              '_M_delete_node site, the client calls ??_G(0)): batch 1 showed sums do not grow this pool and a dead '
              'size()/end() after the erase sends the whole erase out; cheaper dead spenders after the erase (iterator '
              'deref / arrow / compare / field reads) may cut only the deepest nested ~pair; also between delete and erase')
FILE = 'app/client/fontdatabase.cpp'
SYMBOL = 'FontDatabaseClass::Remove_String'
ADDRESS = 0x6C3030

OLD = '''			delete (*it).second;
			Strings.erase(it);
		}
'''


def post(s):
    return [(OLD, '\t\t\tdelete (*it).second;\n\t\t\tStrings.erase(it); ' + s + '\n\t\t}\n')]


def mid(s):
    return [(OLD, '\t\t\tdelete (*it).second; ' + s + '\n\t\t\tStrings.erase(it);\n\t\t}\n')]


FORMS = {
    'p_star': post('(*it);'),
    'p_second': post('(*it).second;'),
    'p_arrow': post('it->second;'),
    'p_eq': post('(void)(it == it);'),
    'p_ht': post('it._M_ht;'),
    'p_bc': post('Strings.bucket_count();'),
    'p_max': post('Strings.max_size();'),
    'p_copy': post('std::hash_map<std::string, FontStringStruct *>::iterator j = it;'),
    'p_inc': post('++it;'),
    'm_second': mid('(*it).second;'),
    'm_sz': mid('Strings.size();'),
    'm_end': mid('Strings.end();'),
}
