// Earth & Beyond client (reconstruction)
// M1.2's check (milestones.md 2.2: harness (b) on a hand-built root): a hand-built root goes through the rebuilt
// encoder side (DeltaListClass, RecordTableClass, the records, DeltaListSetClass::Write) and back
// through DeltaCompoundClass::Decode (both codecs, the lookup build), and the decoded tree
// re-encodes to the same bytes. Built and run by tools/harness/auxdata_smoke.py.
#include <stdio.h>
#include <string.h>
#include "delta.h"
#include "delta_template.h"
#include "linearbu.h"
#include "encode.h"
#include "lookup.h"
#include "..\duration.h"

struct Tree
{
	DeltaCompoundClass *Root;
	DeltaCompoundClass *Sub;
	DeltaTemplateClass<float> *Leaf[5];
};

static void Make(Tree &t, float base)
{
	t.Root = new DeltaCompoundClass("root");
	t.Sub = new DeltaCompoundClass("sub");
	char name[8];
	for (int i = 0; i < 5; i++) {
		sprintf(name, "f%d", i);
		t.Leaf[i] = new DeltaTemplateClass<float>(strdup(name), base + i);
	}
	t.Root->Add(t.Leaf[0]);
	t.Root->Add(t.Sub);
	t.Sub->Add(t.Leaf[1]);
	t.Sub->Add(t.Leaf[2]);
	t.Sub->Add(t.Leaf[3]);
	t.Root->Add(t.Leaf[4]);
	int index = 0;
	t.Root->Set_Parent(0, 0, index);
}

// updated: f0, f2; defaulted: f1; cleared: f4 (f3 untouched)
static int Encode(Tree &t, int form, unsigned char *out)
{
	DeltaListClass updated, defaulted, cleared;
	updated.Add(t.Leaf[0]);
	updated.Add(t.Leaf[2]);
	defaulted.Add(t.Leaf[1]);
	cleared.Add(t.Leaf[4]);
	LinearBufferClass buf(0x400, 0x400);
	unsigned char f = (unsigned char)form;
	buf.Write(&f, 1);
	if (form) {
		RecordTableClass table(t.Root->m_Unknown98, &updated, &defaulted, &cleared);
		if (table.Unknown_All(&buf) == 0) {
			return -1;
		}
	} else {
		DeltaListSetClass lists(&updated, &defaulted, &cleared);
		if (lists.Write(&buf) == 0) {
			return -1;
		}
	}
	memcpy(out, buf.Buffer, buf.Size);
	return buf.Size;
}

static int Check(int form)
{
	Tree a, b;
	Make(a, 10.0f);
	Make(b, 100.0f);
	unsigned char one[0x400], two[0x400];
	int n1 = Encode(a, form, one);
	printf("form %d: %d bytes:", form, n1);
	for (int i = 0; i < n1; i++) {
		printf(" %02x", one[i]);
	}
	printf("\n");
	if (n1 <= 0) {
		return 1;
	}
	LinearBufferClass in(one, n1, 0x400, false);
	DurationClass base(0);
	bool result = b.Root->Decode(in, base);
	int fails = 0;
	// the tree form's Decode returns Error, the record form's success
	bool ok = form ? !result : result;
	if (!ok) {
		printf("  Decode returned %d\n", result);
		fails++;
	}
	float want[5] = {10.0f, 101.0f, 12.0f, 103.0f, 104.0f};	// f0, f2 updated from a; others b's
	for (int k = 0; k < 5; k++) {
		printf("  f%d = %g (m_Unknown70 %u)\n", k, b.Leaf[k]->Value, b.Leaf[k]->m_Unknown70);
		if (b.Leaf[k]->Value != want[k]) {
			fails++;
		}
	}
	// the decoded values re-encode to the same bytes
	int n2 = Encode(b, form, two);
	if (n2 != n1 || memcmp(one, two, n1) != 0) {
		printf("  re-encode differs (%d bytes)\n", n2);
		fails++;
	}
	printf("  %s\n", fails ? "FAIL" : "ok");
	return fails;
}

int main()
{
	int fails = Check(1) + Check(0);
	printf("%s\n", fails ? "SMOKE FAIL" : "SMOKE OK");
	return fails ? 1 : 0;
}
