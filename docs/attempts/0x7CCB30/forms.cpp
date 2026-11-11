// 0x7CCB30 Set_String: exact since run 2026-11-10b (L1). The forms that were not:
// 1 (0.28): the value's buffer and size declared inside `if (value != NULL)`;
//    Unknown60[std::string(canon_key)] = canon_value; return; ... then find(std::string(canon_key)) / erase
// 2 (0.29): the client's head (both buffers and sizes first), the same arms (on the char buffer or on a
//    std::string temporary: the same code)
// 3 / 4 (0.35): the key as a named local and insert(value_type(name, std::string())) written out: pair's
//    constructor expands as in the client, but so does map::insert, and Get_String stops matching
// Exact (form 5): the erase arm first in the source,
//	if (value != NULL) { if (!InternetCanonicalizeUrlA(value, ...)) return; }
//	if (value == NULL || strlen(value) == 0) { find / erase } else { Unknown60[std::string(canon_key)] = canon_value; }
// VC6 lays the else arm out first; the inline budget goes in source order.
