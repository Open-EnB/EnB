// Earth & Beyond client (reconstruction)
// M1.3's harness (milestones.md 2.1 gates (b) and (c)): every AUX_DATA of a trace goes through the
// client's decode path into the root its object's CREATE made (the generated roots,
// app/client/aux_roots.h), is re-encoded from what the decode touched, and compared byte for byte;
// the touched fields are printed for tools/harness/auxreplay.py to compare with the reference decoder's aux-log.
//
//     auxreplay IN.txt OUT.txt        (IN: tools/harness/auxreplay.py's message file)
//
// IN lines:  C chan gameid type      a CREATE (a repeated id replaces the object, as the reference decoder's census)
//            A n chan hex            the n-th AUX_DATA (0x1B) the client received, its payload in hex
// OUT lines: M n chan gameid root form status   status: ok, noroot, decode, encode, differ
//            U idx path hex / D idx path / X idx path   the leaves the message updated (value as Write gives
//                                               it), defaulted, cleared; compounds expanded to leaves
//            W hex                              (differ) what we wrote
//            E status [hex]                     the client's own encoder (M1.3): equal, form (the other
//                                               form), differ (and what it wrote), none (nothing to send)
//            e U|D|X path                       (form, differ) the fields its collector listed ('*': a
//            m U|D|X path                       compound as a whole) and the ones the message has
// The end: a summary per root and form, as the reference decoder's aux-census, and one of the client-path encode.
//
// The client-path encode: after each decode the root writes an update through
// UnknownRootClass::Encode (0x4A3D50: the collector, the policy, the change table), as the server
// would, if its encoder is the same code. The server's fields count their changes up
// (DeltaClass::Unknown_34); the client's decode sets 2 instead. So the harness keeps a server-side
// version per field: an updated field and its parents get new ones, a defaulted field keeps its
// own; they go into m_Unknown70 before the encode. Player uses its own policy and change table
// (UnknownSendRootClass::Unknown_48, Encode_Changes 0x4A4660); the factory's roots get the same
// ones made here (the same policy: "*" INCLUDE, compiled per root by policy.cpp).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "aux_roots.h"
#include "linearbu.h"
#include "encode.h"
#include "policy.h"
#include "unknown_changes.h"
#include "..\duration.h"

// ------------------------------------------------------------------------- what a decode touched

// The decoders notify a field's parents (Unknown_3C updated, Unknown_40 defaulted, Unknown_44
// cleared) up to the root. A compound's whole state reaches the root before its fields' (updated)
// or after them (defaulted, cleared), so a field whose ancestor is listed is dropped, and a
// compound removes its listed fields: the lists end up as the message gave them.
struct Collector
{
	std::vector<DeltaClass *> List[3];
	void Clear()
	{
		for (int k = 0; k < 3; k++) {
			List[k].clear();
		}
	}
	static bool Under(DeltaClass *d, DeltaClass *ancestor)
	{
		for (DeltaClass *p = d->Parent; p != 0; p = p->Parent) {
			if (p == ancestor) {
				return true;
			}
		}
		return false;
	}
	void Note(int k, DeltaClass *d)
	{
		std::vector<DeltaClass *> &l = List[k];
		for (size_t i = 0; i < l.size(); i++) {
			if (l[i] == d || Under(d, l[i])) {
				return;
			}
		}
		std::vector<DeltaClass *> kept;
		for (size_t j = 0; j < l.size(); j++) {
			if (!Under(l[j], d)) {
				kept.push_back(l[j]);
			}
		}
		kept.push_back(d);
		l = kept;
	}
};

static Collector Touched;

// the server-side versions (see the top)
static std::map<DeltaClass *, unsigned int> Version;
static unsigned int Serial = 0;

static void Bump(DeltaClass *d)
{
	for (DeltaClass *p = d; p != 0; p = p->Parent) {
		Version[p] = ++Serial;
	}
}

static void Sync(DeltaClass *d)
{
	std::map<DeltaClass *, unsigned int>::iterator i = Version.find(d);
	d->m_Unknown70 = i == Version.end() ? 0 : i->second;
	DeltaCompoundClass *c = d->As_Compound();
	if (c != 0) {
		for (size_t j = 0; j < c->Children.size(); j++) {
			Sync(c->Children[j]);
		}
	}
}

template<class R> class Tracked : public R
{
public:
	Tracked(int unknown) : R(unknown) {}
	Tracked() {}		// Player (its constructor takes nothing)
	virtual void Unknown_3C(DeltaClass *delta) { R::Unknown_3C(delta); Touched.Note(0, delta); Bump(delta); }
	virtual void Unknown_40(DeltaClass *delta) { R::Unknown_40(delta); Touched.Note(1, delta); }
	virtual void Unknown_44(DeltaClass *delta) { R::Unknown_44(delta); Touched.Note(2, delta); }
};

// The factory 0x6F6000's cases (aux_roots.cpp's Unknown_Create_Aux_Root) with tracked roots
static UnknownRootClass *Create(int type, const char **name)
{
	switch (type) {
		case 0:
		case 1:
			*name = "Ship";
			return new Tracked<DeltaShipClass>(0);
		case 3:
			*name = "Planet";
			return new Tracked<DeltaPlanetClass>(0);
		case 4:
			*name = "Component";
			return new Tracked<DeltaComponentClass>(0);
		case 8:
			*name = "Light";
			return new Tracked<DeltaLightClass>(0);
		case 11:
			*name = "Stargate";
			return new Tracked<DeltaStargateClass>(0);
		case 12:
			*name = "Starbase";
			return new Tracked<DeltaStarbaseClass>(0);
		case 25:
			*name = "Hulk";
			return new Tracked<DeltaHulkClass>(0);
		case 34:
		case 37:
		case 41:
			*name = "Decoration";
			return new Tracked<DeltaDecorationClass>(0);
		case 38:
			*name = "Harvestable";
			return new Tracked<DeltaHarvestableClass>(0);
		case 39:
			*name = "ManufacturingLab";
			return new Tracked<DeltaManufacturingLabClass>(0);
	}
	*name = 0;
	return 0;
}

// ------------------------------------------------------------------------- helpers

static int Unhex(const char *s, std::vector<unsigned char> &out)
{
	out.clear();
	for (; s[0] && s[1] && s[0] != '\n' && s[0] != '\r'; s += 2) {
		unsigned int v;
		char two[3] = {s[0], s[1], 0};
		sscanf(two, "%x", &v);
		out.push_back((unsigned char)v);
	}
	return out.size();
}

static void Hex(FILE *f, const void *data, int n)
{
	const unsigned char *p = (const unsigned char *)data;
	for (int i = 0; i < n; i++) {
		fprintf(f, "%02x", p[i]);
	}
}

// the root by the factory's name (Player's own is "Hull": 0x4A3FF0 names every send root so)
static const char *RootName = "";

static void Path(DeltaClass *d, std::string &out)
{
	if (d->Parent) {
		Path(d->Parent, out);
		out += '.';
		out += d->Name.Get_String();
	} else {
		out += RootName;
	}
}

static void Lists(FILE *f)
{
	for (int k = 0; k < 3; k++) {
		for (size_t j = 0; j < Touched.List[k].size(); j++) {
			std::string path;
			Path(Touched.List[k][j], path);
			fprintf(f, "m %c %s%s\n", "UDX"[k], path.c_str(), Touched.List[k][j]->As_Compound() ? " *" : "");
		}
	}
}

static void Leaves(FILE *f, int k, DeltaClass *d)
{
	DeltaCompoundClass *c = d->As_Compound();
	if (c != 0) {
		for (size_t i = 0; i < c->Children.size(); i++) {
			Leaves(f, k, c->Children[i]);
		}
		return;
	}
	std::string path;
	Path(d, path);
	if (k == 0) {
		LinearBufferClass buf(0x100, 0x100);
		d->Write(buf);
		fprintf(f, "U %d %s ", d->m_Unknown7C, path.c_str());
		Hex(f, buf.Buffer, buf.Size);
		fprintf(f, "\n");
	} else {
		fprintf(f, "%c %d %s\n", k == 1 ? 'D' : 'X', d->m_Unknown7C, path.c_str());
	}
}

struct Tally
{
	int Ok, Failed;
	Tally() : Ok(0), Failed(0) {}
};

struct Object
{
	UnknownRootClass *Root;
	const char *Name;
	PolicyClass *Policy;			// the factory's roots: made here (Player has its own)
	ChangeTableClass *Changes;
};

// the policy UnknownSendRootClass::Unknown_48 (0x4A40B0) gives Player: one rule set, always
// enabled, with "*" INCLUDE (the client's classes are local to unknown_root.cpp)
class HarnessAlwaysClass : public PolicyConditionClass
{
public:
	virtual bool Evaluate(int, int) { return true; }
	virtual void Attach(PolicyStateClass *, int) {}
};

class HarnessIncludeAllClass : public PolicyRuleSetClass
{
public:
	HarnessIncludeAllClass() : Condition(Create_NEW((PolicyConditionClass *)new HarnessAlwaysClass))
	{
		Add_Rule(NameClass("*"), INCLUDE, -1);
	}
	HarnessIncludeAllClass(int) : Condition(Create_NEW((PolicyConditionClass *)new HarnessAlwaysClass))
	{
		// the factory's roots: the captures never update a whole root, so the fields below it get a
		// rule of their own (value 1, which the harness clock always passes); the compile then gives
		// the root DESCEND
		Add_Rule(NameClass("*.*"), INCLUDE, 1);
		Add_Rule(NameClass("*"), INCLUDE, -1);
	}
	virtual RefCountPtr<PolicyConditionClass> Get_Condition(int, int) { return Condition; }

	RefCountPtr<PolicyConditionClass> Condition;
};

// a factory root's encoder state, as UnknownSendRootClass::Unknown_48 makes Player's
static void Encoder_State(Object &o)
{
	o.Policy = 0;
	o.Changes = 0;
	if (o.Root != 0) {
		o.Policy = new PolicyClass;
		o.Policy->Add(new HarnessIncludeAllClass(0));
		o.Policy->Bind(o.Policy->Make_State(0, 0), o.Root);
		o.Changes = new ChangeTableClass(o.Root);
	}
}

// UnknownSendRootClass::Encode_Changes (0x4A4660) for a root with its policy and change table
static LinearBufferClass *Encode_Changes(UnknownRootClass *root, PolicyClass *policy, ChangeTableClass *changes, unsigned int time)
{
	changes->Time = time;
	if (changes->Root->Dirty || changes->Delta->m_Unknown70 != changes->Root->Stamp) {
		changes->Scan();
	}
	if (changes->Entries[root->m_Unknown7C].Changed > 0) {
		return root->Encode(policy, changes);
	}
	return 0;
}

// what the collector lists (for a message the encoder doesn't give back): run again on a copy of
// the change table's entries as they were before the encode
static void Print_Lists(FILE *f, UnknownRootClass *root, PolicyClass *policy, ChangeTableClass *changes,
	unsigned int time, std::vector<ChangeEntryClass> &before)
{
	std::vector<ChangeEntryClass> after = changes->Entries;
	std::copy(before.begin(), before.end(), changes->Entries.begin());
	changes->Time = time;
	if (changes->Root->Dirty || changes->Delta->m_Unknown70 != changes->Root->Stamp) {
		changes->Scan();
	}
	DeltaCollectorClass collector(policy, changes);
	root->Accept(collector);
	DeltaListClass *lists[3] = {&collector.Updated, &collector.Defaulted, &collector.Cleared};
	for (int k = 0; k < 3; k++) {
		for (DeltaListNodeClass *node = lists[k]->Head; node != 0; node = node->Next) {
			std::string path;
			Path(node->Delta, path);
			fprintf(f, "e %c %s%s\n", "UDX"[k], path.c_str(), node->Delta->As_Compound() ? " *" : "");
		}
	}
	std::copy(after.begin(), after.end(), changes->Entries.begin());
}

struct EncodeTally
{
	int Equal, Form, Differ, None;
	EncodeTally() : Equal(0), Form(0), Differ(0), None(0) {}
};

// ------------------------------------------------------------------------- main

int main(int argc, char **argv)
{
	if (argc < 3) {
		fprintf(stderr, "usage: auxreplay IN.txt OUT.txt\n");
		return 2;
	}
	FILE *in = fopen(argv[1], "rb");
	FILE *out = fopen(argv[2], "wb");
	if (!in || !out) {
		fprintf(stderr, "can't open the files\n");
		return 2;
	}
	std::map<std::string, std::map<int, Object> > objects;		// per channel
	std::map<std::string, UnknownRootClass *> players;
	std::map<std::string, Tally> tallies;
	std::map<std::string, EncodeTally> encodes;
	int noroot = 0;
	std::vector<char> line(1 << 20);
	std::vector<unsigned char> payload;
	while (fgets(&line[0], line.size(), in)) {
		char chan[128];
		if (line[0] == 'C') {
			int id, type;
			if (sscanf(&line[0], "C %127s %d %d", chan, &id, &type) == 3) {
				Object o;
				o.Root = Create(type, &o.Name);
				Encoder_State(o);
				objects[chan][id] = o;		// the old object is kept alive (the census forgets it)
				fprintf(out, "C %s %d %d\n", chan, id, type);
			}
			continue;
		}
		if (line[0] != 'A') {
			continue;
		}
		int n, skip = 0;
		if (sscanf(&line[0], "A %d %127s %n", &n, chan, &skip) < 2 || skip == 0) {
			continue;
		}
		Unhex(&line[skip], payload);
		if (payload.size() < 7) {
			continue;
		}
		int id;
		memcpy(&id, &payload[0], 4);
		UnknownRootClass *root = 0;
		const char *name = 0;
		PolicyClass *policy = 0;
		ChangeTableClass *changes = 0;
		if (id == 0) {
			UnknownRootClass *&p = players[chan];
			if (p == 0) {
				p = new Tracked<DeltaPlayerClass>();
			}
			root = p;
			name = "Player";
		} else {
			std::map<int, Object>::iterator i = objects[chan].find(id);
			if (i != objects[chan].end()) {
				root = i->second.Root;
				name = i->second.Name;
				policy = i->second.Policy;
				changes = i->second.Changes;
			}
		}
		int form = payload[6];
		fprintf(out, "M %d %s %d %s %s ", n, chan, id, name ? name : "-", form ? "tree" : "records");
		if (root == 0) {
			fprintf(out, "noroot\n");
			noroot++;
			continue;
		}
		std::string key = std::string(name) + (form ? " tree" : " records");
		Tally &tally = tallies[key];

		// the packet entry 0x7364C0's body: the data after the game id and the length
		Touched.Clear();
		LinearBufferClass buf(&payload[6], payload.size() - 6, 0x400, false);
		DurationClass base(0);
		bool result = root->Decode(buf, base);
		bool decoded = form ? !result : result;
		if (!decoded || buf.Get_Pos() != buf.Size) {
			fprintf(out, "decode %d/%d\n", buf.Get_Pos(), buf.Size);
			tally.Failed++;
			continue;
		}

		// re-encode the lists in the message's form
		DeltaListClass lists[3];
		for (int k = 0; k < 3; k++) {
			for (size_t j = 0; j < Touched.List[k].size(); j++) {
				lists[k].Add(Touched.List[k][j]);
			}
		}
		LinearBufferClass w(0x400, 0x400);
		w.Write(&payload[0], 4);
		unsigned short len = 0;
		w.Write(&len, 2);
		unsigned char f = payload[6];
		w.Write(&f, 1);
		LinearBufferClass *ok;
		if (form) {
			RecordTableClass table(root->m_Unknown98, &lists[0], &lists[1], &lists[2]);
			ok = table.Unknown_All(&w);
		} else {
			DeltaListSetClass set(&lists[0], &lists[1], &lists[2]);
			ok = set.Write(&w);
		}
		len = (unsigned short)(w.Size - 6);
		memcpy(w.Buffer + 4, &len, 2);
		if (ok == 0) {
			fprintf(out, "encode\n");
			tally.Failed++;
		} else if (w.Size != (int)payload.size() || memcmp(w.Buffer, &payload[0], w.Size) != 0) {
			fprintf(out, "differ\nW ");
			Hex(out, w.Buffer, w.Size);
			fprintf(out, "\n");
			tally.Failed++;
		} else {
			fprintf(out, "ok\n");
			tally.Ok++;
		}
		RootName = name;
		for (int list = 0; list < 3; list++) {
			for (size_t j = 0; j < Touched.List[list].size(); j++) {
				Leaves(out, list, Touched.List[list][j]);
			}
		}

		// the client's own encoder on the server-side versions
		Sync(root);
		if (id == 0) {
			policy = ((UnknownSendRootClass *)root)->Policy;
			changes = ((UnknownSendRootClass *)root)->Changes;
		}
		std::vector<ChangeEntryClass> before = changes->Entries;
		LinearBufferClass *e;
		if (id == 0) {
			e = ((UnknownSendRootClass *)root)->Encode_Changes(n);
		} else {
			e = Encode_Changes(root, policy, changes, n);
		}
		EncodeTally &et = encodes[key];
		if (e == 0) {
			fprintf(out, "E none\n");
			et.None++;
		} else {
			int m = payload.size() - 6;
			if (e->Size == m && memcmp(e->Buffer, &payload[6], m) == 0) {
				fprintf(out, "E equal\n");
				et.Equal++;
			} else {
				fprintf(out, "E %s ", e->Buffer[0] != payload[6] ? "form" : "differ");
				Hex(out, e->Buffer, e->Size);
				fprintf(out, "\n");
				if (e->Buffer[0] != payload[6]) {
					et.Form++;
				} else {
					et.Differ++;
				}
				Print_Lists(out, root, policy, changes, n, before);
				Lists(out);
			}
			delete e;
		}
	}
	int eq = 0, all = 0;
	for (std::map<std::string, EncodeTally>::iterator k = encodes.begin(); k != encodes.end(); ++k) {
		printf("encode %-17s %7d equal %5d form %5d differ %5d none\n", k->first.c_str(), k->second.Equal,
			k->second.Form, k->second.Differ, k->second.None);
		eq += k->second.Equal;
		all += k->second.Equal + k->second.Form + k->second.Differ + k->second.None;
	}
	printf("client-path encode: %d of %d equal the message\n", eq, all);
	int checked = 0, failed = 0;
	for (std::map<std::string, Tally>::iterator t = tallies.begin(); t != tallies.end(); ++t) {
		printf("%-24s %7d ok %5d failed\n", t->first.c_str(), t->second.Ok, t->second.Failed);
		checked += t->second.Ok + t->second.Failed;
		failed += t->second.Failed;
	}
	printf("%d AUX_DATA checked, %d failed; %d for objects the client had not been told about\n", checked, failed, noroot);
	fclose(out);
	return failed ? 1 : 0;
}
