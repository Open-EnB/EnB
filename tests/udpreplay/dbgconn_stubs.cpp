// Earth & Beyond client (reconstruction)
// udpreplay: stand-ins for app\common\dbgconn.cpp's wrappers (not compiled into the replay), which
// connect.cpp's factory and ConnectionManagerClass::Add_Connection name. The replay never adds a
// connection to a manager. The classes are declared here with only the constructor defined, so no
// vtable is emitted (the names mangle as the real ones); each stops the program if it is reached.
#include <stdio.h>
#include <stdlib.h>

class ConnectionClass;

static void Not_In_Replay(const char *what)
{
	printf("udpreplay: %s is a stand-in (dbgconn_stubs.cpp)\n", what);
	exit(3);
}

class TimedLogConnectionClass { public: TimedLogConnectionClass(ConnectionClass *connection, const char *name); };
TimedLogConnectionClass::TimedLogConnectionClass(ConnectionClass *, const char *) { Not_In_Replay("TimedLogConnectionClass"); }

class StatsConnectionClass { public: StatsConnectionClass(ConnectionClass *connection); };
StatsConnectionClass::StatsConnectionClass(ConnectionClass *) { Not_In_Replay("StatsConnectionClass"); }
