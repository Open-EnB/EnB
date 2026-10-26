// udpreplay: stand-ins for app\common\dbgconn.cpp's wrappers (not rebuilt), which
// ConnectionManagerClass::Add_Connection (connect.cpp) names. The replay never adds a connection to
// a manager; these only forward, as their ForwardingConnectionClass base does.
#include "..\..\..\app\common\dbgconn.h"

LogConnectionClass::LogConnectionClass(ConnectionClass *connection, const char *) :
	ForwardingConnectionClass(connection),
	Unknown0C(0),
	Unknown10(0)
{
}

TimedLogConnectionClass::TimedLogConnectionClass(ConnectionClass *connection, const char *name) :
	LogConnectionClass(connection, name),
	Unknown14(0)
{
}

StatsConnectionClass::StatsConnectionClass(ConnectionClass *connection) :
	ForwardingConnectionClass(connection)
{
}
