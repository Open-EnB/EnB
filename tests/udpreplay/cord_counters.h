// udpreplay's counters per CordCon (D176): the client's CordCon (0xC0 bytes) has no room for them, so
// they live in a side table here. udpreplay.py build includes this header before app\wwudp\cordcon.cpp,
// whose CORD_HARNESS_COUNT(this, X) then counts into the table (in the client it is nothing).
#ifndef CORD_COUNTERS_H
#define CORD_COUNTERS_H

class CordCon;

struct CordHarnessCounters
{
	CordHarnessCounters() : AcksMatched(0), ResendRequests(0), Rejected(0) {}
	int AcksMatched;		// handlePacket: sent records an ack (kinds 0 and 2) removed
	int ResendRequests;		// handlePacket: resend requests (kind 1) that found their record
	int Rejected;			// handlePacket: datagrams examinePacket refused
};

// The connection's counters (udpreplay.cpp: a map from the CordCon).
CordHarnessCounters *Cord_Harness_Counters(const CordCon *con);

#define CORD_HARNESS_COUNT(con, counter) (Cord_Harness_Counters(con)->counter++)

#endif
