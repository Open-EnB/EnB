// Earth & Beyond client (reconstruction)
// clienttest: what the harness's files share (clienttest.cpp, client_stubs.cpp, the generated tables).
#ifndef CLIENTTEST_H
#define CLIENTTEST_H

// One line in the trace (OUT.txt): every stub and fake appends one
void Trace(const char *format, ...);
const char *Aux_Root_Name(const void *root);	// clienttest.cpp: which fake aux root (AUX_DATA 27 / 28)
void Install_Fake_D3D_Device();	// d3d_stubs.cpp: DX8Wrapper's device for sclient.cpp's 0x7511A0

// The factory's opcodes (clienttest_tables.cpp, gen_stubs.py)
struct OpcodeInfo
{
	int Opcode;
	const char *Name;		// docs/data/packet-factory.txt (the Net-7 opcode table)
	const char *Class;		// pk.cpp's switch
	int Handler;			// 1: cpacket.cpp's handler, rebuilt constructor (driven); 2: handler, stub constructor; 0: none
};
extern const OpcodeInfo Opcode_Table[];
extern const int Opcode_Count;

// What the stubs return where a handler branches on it (clienttest.cpp sets them)
extern bool Stub_744B60;	// SClient::Unknown_744B60 (REMOVE 7)
extern bool Stub_Focused;	// TaskClass::Is_Focused (Process_Packets' loading view)

#endif
