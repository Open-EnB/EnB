// 0x79E300 ClientObjectDataAccessClass<RadarDataClass>::Get (twins 0x79E8B0, 0x79EC90, 0x79F070, 0x79F450)
// Run 2026-11-11, builder L7 (Fable). The tree's form (co_data.h's Get, data_binder.h's Bind and
// Make_Value_Map) is the best one: the client's calls, frame (0x50) and length (0x304); register-only,
// 10 rows: the type name in edi and the type's map in esi where the client has esi and edi.
//
// The client's call pattern, the target of every form (HT: the type map's (n, hash, equal) constructor
// 0x79E6E0; FIND*: the type map's const find 0x7A1E60; cit: const_iterator(const iterator &) 0x7A0920;
// OP*: the type map's operator[] 0x7A1F30; hm(): the value map's constructor 0x7A2140; pair 0x7A0A80;
// foi: find_or_insert 0x7A0EC0):
//   HT atexit name STR FIND* _Tidy find cit cit  name STR FIND* _Tidy assert find cit cit assert
//   new T() name STR OP* _Tidy new hm() STR OP* _Tidy pair foi
//
// What each part needed (each checked by removing it):
//   * Bind = Make_Value_Map (separate member) + operator[]: two string temporaries in one inline
//     instance (the client's second has its own slot; Bind flat under a layer gives frame 0x48).
//   * the type name in a 'const char *' local, converted implicitly (Types[name], Types.find(name)):
//     the client calls name() before it pushes the allocator temporary and reloads the temporary's
//     address (an explicit std::string(name) pushes the constructor's result).
//   * a dead read before 'if (!create)' and one at Bind's end (D130): without the first the (n, hash,
//     equal) constructor is expanded and Make_Value_Map stays out of line; without the second the pair
//     constructor is expanded.
//   * plain 'new std::hash_map<KEY, T *>': a derived struct gives the call pattern without Bind's dead
//     read, but then the new's value is the pointer, not the constructor's result (the client's).
//
// A lead without dead reads (not exact either, same register swap): an empty user constructor on the tag,
//     template<class T> struct DataBinderType { DataBinderType() {} };
// makes every DataBinderType<T>() temporary an inline call. With no dead read at all, Get has the
// client's pattern but for one const_iterator conversion, and Remove_All keeps Has out of line at its
// first three removals (the last two expand it).
//
// P-L7-2 round (run 2026-11-12, builder C2, Opus): that constructor written on the tag's brace line
// (the asserts keep their lines) with the dead reads all off, Get's off, Bind's off, Remove_All's off
// too, and all on: none is better than the tree's form. Get's read off (or all on): shape, 30 calls vs
// the client's 31 (edi pushed before the jne, one 0x7A0920 conversion short); Get's kept and Bind's off:
// inline-budget (basic_string::assign called). The lead is spent.
//
// Forms that did not swap the two registers (all 41-43 objdiff rows, the same 10 real ones): the locals'
// declaration order; 'void *' and reference locals in Make_Value_Map and Bind; the name as Bind's local
// passed to Make_Value_Map (either parameter position); 'Types[name] = value_map = new ...'; an early
// return in Make_Value_Map (76 rows); Bind returning data, or the slot through a reference; dead reads of
// name; a tag local in Bind; the members' definition order; the dead reads moved into Make_Value_Map.
