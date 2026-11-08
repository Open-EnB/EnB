/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TCPMGR_HEADER
#define TCPMGR_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "wlib/wstypes.h"
#include "wlib/wdebug.h"
#include "wlib/arraylist.h"

#ifdef _WINDOWS
#include <windows.h>
#include <winsock.h>
#include <io.h>

#else  //UNIX
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <limits.h>

typedef sint32         SOCKET;
#define closesocket    close
#define SOCKET_ERROR   -1
#define INVALID_SOCKET -1
 
#endif

#ifdef AIX
#include <sys/select.h>
#endif

class TCPCon;

#define DEFAULT_PROTOCOL 0
#define INVALID_HANDLE   0


class TCPMgr {
 public:

  enum CONN_STATE
  {
    CLOSED,
    CONNECTING,
    CONNECTED,
    FAILED=-1  // E&B: a failed connect (0x8BE270 stores -1, 0x8BDF80 returns it with *conn NULL)  // NAME 2: what it means (winerror.h's FAILED(x) is function-like: no clash)
  };

  // These defines specify a system independent way to
  //	 get error codes for socket services.
  enum STATUS
  {
    OK				=	0,		 // Everything's cool
    UNKNOWN			= -1,		 // There was an error of unknown type
    ISCONN			= -2,		 // The socket is already connected
    INPROGRESS		= -3,		 // The socket is non-blocking and the operation
													 //	 isn't done yet
    ALREADY			= -4,		 // The socket is already attempting a connection
													 //	 but isn't done yet
    AGAIN			= -5,		 // Try again.
    ADDRINUSE		= -6,		 // Address already in use
    ADDRNOTAVAIL	= -7,		 // That address is not available on the remote host
    BADF				= -8,		 // Not a valid FD
    CONNREFUSED	= -9,		 // Connection was refused
    INTR				=-10,		 // Operation was interrupted
    NOTSOCK			=-11,		 // FD wasn't a socket
    PIPE				=-12,		 // That operation just made a SIGPIPE
    WOULDBLOCK		=-13,		 // That operation would block
    INVAL			=-14,	    // Invalid
    TIMEDOUT		=-15,		 // Timeout
    CONNRESET		=-16		 // E&B: WSAECONNRESET or WSAENOTCONN (getStatus 0x8BDD40)  // NAME 2: what it means
  };

  enum DIRECTION
  {
    INCOMING = 1,
    OUTGOING = 2,
    EITHER   = 3
  };

            TCPMgr();
            ~TCPMgr() throw();  // E&B: no unwind states (0x8BCA90)

  bit8      addListener(uint32 ip, uint16 port, bit8 reuseAddr);
  bit8      removeListener(uint32 ip, uint16 port);
  bit8      getListener(uint32 ip, uint16 port, OUT SOCKET &outsock);

  bit8      connect(char *address, uint16 port, uint32 *handle);
  bit8      connect(uint32 ip, uint16 port,OUT uint32 *handle);

  bit8      getOutgoingConnection(TCPCon **conn, uint32 handle, sint32 wait_secs);
  bit8      getIncomingConnection(TCPCon **conn, uint16 port, sint32 wait_secs);
  bit8      cancelConnect(uint32 handle);  // E&B 0x8BD940: drops a pending connect  // NAME 2: what it does

  bit8      setBufferedWrites(TCPCon *con, bit8 enabled);
  void      pumpWriters(void);  // pump the buffered writer connections

  // Static methods
  static int       wait(uint32 sec, uint32 usec, SOCKET *sockets, int count, bit8 readMode=TRUE);
  static STATUS    getStatus(void);

 private:

  SOCKET    createSocket(uint32 ip, uint16 port, bit8 reuseAddr=TRUE);
  bit8      setBlocking(SOCKET fd, bit8 block);
  bit8      getConnection(TCPCon **conn, uint32 handle, uint16 port, sint32 wait_secs, DIRECTION dir);
  void      pumpConnections(void);

  struct ListenSocket
  {
    SOCKET   fd;
    uint32   ip;
    uint32   port;
  };

  ArrayList<ListenSocket>       ListenArray_;

  ArrayList<TCPCon * >          BufferedWriters_;  // need to be pumped

  struct PendingConn
  {
    SOCKET      fd;
    uint32      ip;
    uint16      port;
    time_t      startTime;
    uint32      handle;
    CONN_STATE  state;
    bit8        incoming;
    uint32      remoteIp;
    uint16      remotePort;
  };

  ArrayList<PendingConn>  ConnectArray_;
  uint32                  HandleSequence_;
};

#endif

// Earth & Beyond client (reconstruction): changes to Renegade's tcpmgr.h (frozen 2026-11-07 from importer.py)
// EnB: class TCPMgr (line 73 at the freeze): E&B's pending connects have a fourth state, -1: pumpConnections 0x8BE270
//       stores it (`mov [esi+0x14], 0xFFFFFFFF`) after closesocket on an except set or CONNREFUSED / TIMEDOUT /
//       CONNRESET, getConnection 0x8BDF80 returns such an entry (`cmp eax, -1; jne`) with *conn NULL; named instead of
//       tcpmgr.cpp's FAILED casts (P-NW2-1)
// EnB: enum STATUS (line 98 at the freeze): E&B's STATUS has a -16 (CONNRESET): getStatus 0x8BDD40 returns it for
//       WSAECONNRESET (0x2746) and WSAENOTCONN (0x2749); TCPCon's read 0x8BC280, normalWrite 0x8BC170 and pumpReads
//       0x8BC4F0 end quietly on it, isConnected 0x8BC8C0 treats it as closed
// EnB: class TCPMgr (line 110 at the freeze): wnet is built with /GX (TCPMgr::wait 0x8BDA80 has an EH frame for its
//       Wtime locals), yet E&B's ~TCPMgr 0x8BCA90 has none: the three ArrayList members' destructors inlined without
//       unwind states. VC6 /GX gives exactly that for a destructor declared throw() (without it: an EH frame with
//       states 1, 0, -1). The form is the compiler's evidence, not a source line
// EnB: class TCPMgr (line 121 at the freeze): E&B's addition 0x8BD940 (after getIncomingConnection 0x8BD910): finds
//       the pending connection whose handle (+0x10 of its 0x24-byte record) is the argument, closes its socket and
//       removes it (ArrayList remove inlined); tcpconn.cpp's destructor 0x828EC0 and Disconnect 0x829960 call it with
//       their handle (+0x28)
