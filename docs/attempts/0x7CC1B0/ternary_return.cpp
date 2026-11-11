// 0x7CC1B0, form 2 (similarity 0.36, worse than the tree's 0.77): one return through ?:
Unknown_7C9F00Struct Unknown_7CA2D0Class::Unknown_7CC1B0(int id)
{
	Unknown_7C9F00Struct none(8, "", "", 1, 0);
	return (id < 8) ? Unknown04[id] : none;
}

// Forms 3-5 (drafts, each 0.77 with the tree's bytes): a reference local for the map entry before the return;
// the id test through an inline helper; return Unknown_7C9F00Struct(none). None moves the string copy
// constructor of the last return out of line.

// Forms 6-8 (drafts): the `none` return first in the source (if (id >= BROWSER_COUNT) return none; ..., with
// or without else): the client's 154 instructions and inlining, but the arms laid out the other way round and
// the EH states renumbered (0.46): the client's states (3-6 in the map arm, 7 in the none arm) put the map arm
// first in the source. The same through ?: (0.35).
// Forms 9-10: an unreachable second return after the two: `return none;` (0.41, too much out of line) or
// `return Unknown_7C9F00Struct();` (0.57).
// Exact (form 11, the tree's): one dead read after the last return, `none.Name.length();` (or c_str()): D130.
