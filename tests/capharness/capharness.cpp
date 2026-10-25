/*
** capharness: the M1 capture harness (docs/milestones.md section 2).
**
** Skeleton (M1.0): no client classes yet. It reads
**   - a message-level trace (the reference decoder's trace.jsonl: one game message per line), and
**   - a datagram file written by tools/harness/convert.py (format: tools/harness/README.md),
** and prints counts: messages per direction and opcode, datagrams per type, ack entries,
** blocks per type, and the game messages rebuilt from the blocks (resends dropped, fragments
** joined). (The M1.0 count check against the reference decoder, compare.py, is retired; git history has it.)
**
**   capharness [--trace FILE.jsonl] [--dgram FILE.dgram]
**
** Plain C-style C++ so that VC6 (no STL, old for-scope rules) builds it as is.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------- line reader */

static char *Line = NULL;
static size_t LineCap = 0;

/* Reads one line (any length) into Line, without the newline. Returns 0 at end of file. */
static int Read_Line(FILE *f)
{
	size_t len = 0;
	if (Line == NULL) {
		LineCap = 65536;
		Line = (char *)malloc(LineCap);
	}
	for (;;) {
		if (fgets(Line + len, (int)(LineCap - len), f) == NULL) {
			Line[len] = 0;
			return len != 0;
		}
		len += strlen(Line + len);
		if (len > 0 && Line[len - 1] == '\n') {
			Line[--len] = 0;
			if (len > 0 && Line[len - 1] == '\r') Line[--len] = 0;
			return 1;
		}
		if (len + 1 >= LineCap) {
			LineCap *= 2;
			Line = (char *)realloc(Line, LineCap);
		}
	}
}

static int Hex_Digit(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

/* ---------------------------------------------------------------- counters */

/* Directions as the reference decoder's summary writes them. */
static const char *DirNames[] = { "c>s", "s>c", "c>p", "p>c", "p>s", "s>p" };
enum { DIR_COUNT = 6 };

struct OpTable {
	unsigned long Count[DIR_COUNT][65536];
	unsigned long Bytes[DIR_COUNT][65536];
	unsigned long Total;
};

static OpTable *New_Table(void)
{
	OpTable *t = (OpTable *)calloc(1, sizeof(OpTable));
	return t;
}

static void Print_Table(const char *tag, OpTable *t)
{
	int d, op;
	printf("%s messages %lu\n", tag, t->Total);
	for (d = 0; d < DIR_COUNT; d++) {
		for (op = 0; op < 65536; op++) {
			if (t->Count[d][op]) {
				printf("%s op %s 0x%04x %lu %lu\n", tag, DirNames[d], op, t->Count[d][op], t->Bytes[d][op]);
			}
		}
	}
}

/* ---------------------------------------------------------------- trace.jsonl */

/* The string value of "key":"..." in the line, copied into out (at most n-1 chars). */
static int Json_String(const char *line, const char *key, char *out, int n)
{
	char pat[64];
	const char *p;
	int i = 0;
	sprintf(pat, "\"%s\":\"", key);
	p = strstr(line, pat);
	if (p == NULL) return 0;
	p += strlen(pat);
	while (*p && *p != '"' && i < n - 1) out[i++] = *p++;
	out[i] = 0;
	return 1;
}

static int Dir_Index(const char *name)
{
	if (strcmp(name, "ClientToServer") == 0) return 0;
	if (strcmp(name, "ServerToClient") == 0) return 1;
	if (strcmp(name, "ClientToProxy") == 0) return 2;
	if (strcmp(name, "ProxyToClient") == 0) return 3;
	if (strcmp(name, "ProxyToServer") == 0) return 4;
	if (strcmp(name, "ServerToProxy") == 0) return 5;
	return -1;
}

static int Read_Trace(const char *path)
{
	FILE *f = fopen(path, "rb");
	OpTable *t;
	unsigned long lines = 0, bad = 0, lenmismatch = 0;
	char dir[32], op[16];
	if (f == NULL) {
		printf("error cannot open %s\n", path);
		return 1;
	}
	t = New_Table();
	while (Read_Line(f)) {
		const char *data, *lenp;
		int d;
		unsigned long opcode, len, hexlen = 0;
		if (Line[0] == 0) continue;
		lines++;
		if (!Json_String(Line, "dir", dir, sizeof(dir)) || !Json_String(Line, "op", op, sizeof(op))
			|| (d = Dir_Index(dir)) < 0) {
			bad++;
			continue;
		}
		opcode = strtoul(op, NULL, 16) & 0xFFFF;
		lenp = strstr(Line, "\"len\":");
		len = lenp ? strtoul(lenp + 6, NULL, 10) : 0;
		data = strstr(Line, "\"data\":\"");
		if (data != NULL) {
			data += 8;
			while (data[hexlen] && data[hexlen] != '"') hexlen++;
		}
		if (hexlen != len * 2) lenmismatch++;
		t->Count[d][opcode]++;
		t->Bytes[d][opcode] += len;
		t->Total++;
	}
	fclose(f);
	printf("trace %s\n", path);
	printf("trace lines %lu unreadable %lu len-mismatch %lu\n", lines, bad, lenmismatch);
	Print_Table("trace", t);
	free(t);
	return 0;
}

/* ---------------------------------------------------------------- datagram file */

enum { MAX_SESSIONS = 64, MAX_PENDING = 256 };

static char Sessions[MAX_SESSIONS][16];
static int SessionCount = 0;

static int Session_Index(const char *name)
{
	int i;
	for (i = 0; i < SessionCount; i++) {
		if (strcmp(Sessions[i], name) == 0) return i;
	}
	if (SessionCount == MAX_SESSIONS) return -1;
	strncpy(Sessions[SessionCount], name, 15);
	Sessions[SessionCount][15] = 0;
	return SessionCount++;
}

/* Blocks already seen, per session, direction and channel: one bit per sequence number. */
static unsigned char *Seen[MAX_SESSIONS][2][256];

static int Test_And_Set(int s, int d, int ch, int seq)
{
	unsigned char *bits = Seen[s][d][ch];
	int was;
	if (bits == NULL) {
		bits = Seen[s][d][ch] = (unsigned char *)calloc(65536 / 8, 1);
	}
	was = (bits[seq >> 3] >> (seq & 7)) & 1;
	bits[seq >> 3] |= (unsigned char)(1 << (seq & 7));
	return was;
}

/* A message arriving in fragments. */
struct Pending {
	int Used;
	int Session, Dir, Channel, First;
	int Size, Have;
	unsigned char *Data;
};
static Pending PendingList[MAX_PENDING];

static unsigned char *Datagram = NULL;
static size_t DatagramCap = 0;
static int DatagramLen = 0;

static void Message(OpTable *t, int d, const unsigned char *m, int len, unsigned long *notmsg)
{
	int op;
	if (len < 4 || (m[0] | (m[1] << 8)) != len) {
		(*notmsg)++;
		return;
	}
	op = m[2] | (m[3] << 8);
	t->Count[d][op]++;
	t->Bytes[d][op] += len - 4;
	t->Total++;
}

static int Read_Dgram(const char *path)
{
	FILE *f = fopen(path, "rb");
	OpTable *t;
	unsigned long keys = 0, datagrams = 0, shortdg = 0, acks = 0, blocks = 0, problems = 0;
	unsigned long dgtype[256], dgdir[2], btype[256], resent = 0, notmsg = 0, frags = 0, control = 0;
	int cur_session = -1, cur_dir = 0;
	int i;
	if (f == NULL) {
		printf("error cannot open %s\n", path);
		return 1;
	}
	memset(dgtype, 0, sizeof(dgtype));
	memset(btype, 0, sizeof(btype));
	memset(dgdir, 0, sizeof(dgdir));
	t = New_Table();
	while (Read_Line(f)) {
		char kind = Line[0];
		if (kind == 'K') {
			char name[64], key[64];
			if (sscanf(Line + 2, "%63s %63s", name, key) == 2) {
				Session_Index(name);
				keys++;
			}
		} else if (kind == 'D') {
			char name[64], dirc[8];
			int n, seq, declared, len, type, flags, consumed = 0;
			const char *hex;
			if (sscanf(Line + 2, "%d %63s %7s %d %d %d %d %d %n", &n, name, dirc, &seq, &declared, &len,
				&type, &flags, &consumed) < 8) {
				printf("error bad D line\n");
				return 1;
			}
			cur_session = Session_Index(name);
			cur_dir = dirc[0] == 'C' ? 0 : 1;
			hex = Line + 2 + consumed;
			if ((size_t)len > DatagramCap) {
				DatagramCap = len + 1024;
				Datagram = (unsigned char *)realloc(Datagram, DatagramCap);
			}
			for (i = 0; i < len; i++) {
				Datagram[i] = (unsigned char)((Hex_Digit(hex[2 * i]) << 4) | Hex_Digit(hex[2 * i + 1]));
			}
			DatagramLen = len;
			datagrams++;
			dgdir[cur_dir]++;
			if (type >= 0 && type < 256) dgtype[type]++;
			if (declared != len) shortdg++;
		} else if (kind == 'A') {
			int hdr, count;
			if (sscanf(Line + 2, "%d %d", &hdr, &count) == 2) acks += count;
		} else if (kind == 'B') {
			int size, ch, seq, type, frag, off, len;
			if (sscanf(Line + 2, "%d %d %d %d %d %d %d", &size, &ch, &seq, &type, &frag, &off, &len) != 7
				|| off + len > DatagramLen || cur_session < 0) {
				printf("error bad B line\n");
				return 1;
			}
			blocks++;
			btype[type & 0xFF]++;
			if (size == 0 || ((type & 0x0F) != 3 && (type & 0x0F) != 4)) {
				control++;
				continue;
			}
			if (Test_And_Set(cur_session, cur_dir, ch, seq)) {
				resent++;
				continue;
			}
			if (frag < 0) {
				Message(t, cur_dir, Datagram + off, len, &notmsg);
			} else {
				int first = (seq - frag) & 0xFFFF, slot = -1, freeslot = -1;
				frags++;
				for (i = 0; i < MAX_PENDING; i++) {
					Pending *p = &PendingList[i];
					if (p->Used && p->Session == cur_session && p->Dir == cur_dir && p->Channel == ch
						&& p->First == first) {
						slot = i;
						break;
					}
					if (!p->Used && freeslot < 0) freeslot = i;
				}
				if (slot < 0) {
					if (freeslot < 0) {
						printf("error too many messages in fragments\n");
						return 1;
					}
					slot = freeslot;
					PendingList[slot].Used = 1;
					PendingList[slot].Session = cur_session;
					PendingList[slot].Dir = cur_dir;
					PendingList[slot].Channel = ch;
					PendingList[slot].First = first;
					PendingList[slot].Size = size;
					PendingList[slot].Have = 0;
					PendingList[slot].Data = (unsigned char *)calloc(size, 1);
				}
				{
					Pending *p = &PendingList[slot];
					int at = frag * 200;
					if (at + len <= p->Size) {
						memcpy(p->Data + at, Datagram + off, len);
						p->Have += len;
					}
					if (p->Have >= p->Size) {
						Message(t, cur_dir, p->Data, p->Size, &notmsg);
						free(p->Data);
						p->Used = 0;
					}
				}
			}
		} else if (kind == 'X') {
			problems++;
		}
	}
	fclose(f);
	printf("dgram %s\n", path);
	printf("dgram keys %lu\n", keys);
	printf("dgram datagrams %lu c>s %lu s>c %lu short %lu problems %lu\n", datagrams, dgdir[0], dgdir[1],
		shortdg, problems);
	for (i = 0; i < 256; i++) {
		if (dgtype[i]) printf("dgram datagram-type %d %lu\n", i, dgtype[i]);
	}
	printf("dgram acks %lu\n", acks);
	printf("dgram blocks %lu control %lu resent %lu fragments %lu not-a-message %lu\n", blocks, control,
		resent, frags, notmsg);
	for (i = 0; i < 256; i++) {
		if (btype[i]) printf("dgram block-type 0x%02x %lu\n", i, btype[i]);
	}
	{
		unsigned long open = 0;
		for (i = 0; i < MAX_PENDING; i++) open += PendingList[i].Used;
		printf("dgram incomplete %lu\n", open);
	}
	Print_Table("dgram", t);
	free(t);
	return 0;
}

int main(int argc, char **argv)
{
	int i, rc = 0;
	if (argc < 3) {
		printf("usage: capharness [--trace FILE.jsonl] [--dgram FILE.dgram]\n");
		return 2;
	}
	for (i = 1; i + 1 < argc; i += 2) {
		if (strcmp(argv[i], "--trace") == 0) {
			rc |= Read_Trace(argv[i + 1]);
		} else if (strcmp(argv[i], "--dgram") == 0) {
			rc |= Read_Dgram(argv[i + 1]);
		} else {
			printf("error unknown option %s\n", argv[i]);
			return 2;
		}
	}
	return rc;
}
