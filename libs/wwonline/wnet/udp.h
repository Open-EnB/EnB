/*
**	Command & Conquer Generals Zero Hour(tm)
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

#ifndef UDP_HEADER
#define UDP_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#ifdef _WINDOWS
#include <winsock.h>
#include <io.h>
#define close _close
#define read  _read
#define write _write

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
#endif

#ifdef AIX
#include <sys/select.h>
#endif

#define DEFAULT_PROTOCOL 0

#include <wlib/wstypes.h>
#include <wlib/wtime.h>
#include <list>

class UDP
{
 // DATA
 private:
  sint32       fd;
  sint32       unknownFD;  // E&B: a second socket the destructor closes (0x8BFA10)  // NAME 3: neutral, what the destructor does with it
  uint32       myIP;
  uint16       myPort;
  struct       sockaddr_in  addr;
  std::list<sockaddr_in> resetList;  // E&B: the peers whose datagrams came back as WSAECONNRESET, handed out by Read (0x8BFE30)  // NAME 2: what it holds

  // E&B: the bandwidth accounting's callback (0xBF52C4), called by Write and Read with every datagram
  static void (*accountingCallback)(bool out, int len, unsigned int ip, unsigned short port);  // NAME 2: what it is

  // These defines specify a system independent way to
  //   get error codes for socket services.
  enum sockStat
  {
    OK           =  0,     // Everything's cool
    UNKNOWN      = -1,     // There was an error of unknown type
    ISCONN       = -2,     // The socket is already connected
    INPROGRESS   = -3,     // The socket is non-blocking and the operation
                           //   isn't done yet
    ALREADY      = -4,     // The socket is already attempting a connection
                           //   but isn't done yet
    AGAIN        = -5,     // Try again.
    ADDRINUSE    = -6,     // Address already in use
    ADDRNOTAVAIL = -7,     // That address is not available on the remote host
    BADF         = -8,     // Not a valid FD
    CONNREFUSED  = -9,     // Connection was refused
    INTR         =-10,     // Operation was interrupted
    NOTSOCK      =-11,     // FD wasn't a socket
    PIPE         =-12,     // That operation just made a SIGPIPE
    WOULDBLOCK   =-13,     // That operation would block
    INVAL        =-14,     // Invalid
    TIMEDOUT     =-15,     // Timeout
    CONNRESET    =-16      // E&B: the peer's port is closed (WSAECONNRESET, 0x8C0100)
  };

// CODE
 public:
                   UDP();
                  ~UDP();
  sint32           Bind(uint32 IP,uint16 port);
  sint32           Bind(char *Host,uint16 port);
  void             Close(void);  // E&B: 0x8BFCA0  // NAME 2: closes the socket
  sint32           SetBlocking(bit8 block,sint32 sock=-1);  // E&B: public, any socket (0x8BFD00)
  sint32           Write(uint8 *msg,uint32 len,uint32 IP,uint16 port);
  sint32           Read(uint8 *msg,uint32 len,sockaddr_in *from);
  sockStat         GetStatus(void);
  void             ClearStatus(void);
  void             AddReset(sockaddr_in &from);  // E&B: 0x8C0060  // NAME 2: what it does
  void             Unknown8C00E0(void);  // E&B: empty, no caller (0x8C00E0)  // NAME 3: neutral

  // E&B: the bandwidth accounting's hook (bandwidthaccounting.cpp)
  static void      Set_Accounting_Callback(void (*callback)(bool out, int len, unsigned int ip, unsigned short port));  // NAME 1: what it does
  static void      Clear_Accounting_Callback(void);  // NAME 1: what it does
  int              Wait(sint32 sec,sint32 usec,fd_set &returnSet);
  int              Wait(sint32 sec,sint32 usec,fd_set &givenSet,fd_set &returnSet);

  bit8             getLocalAddr(uint32 &ip, uint16 &port);
  sint32           getFD(void) { return(fd); }
 
  bit8             SetInputBuffer(uint32 bytes);
  bit8             SetOutputBuffer(uint32 bytes);
  int              GetInputBuffer(void);
  int              GetOutputBuffer(void);
};

#endif

// Earth & Beyond client (reconstruction): changes to Zero Hour's udp.h (frozen 2026-11-07 from importer.py)
// EnB: Line 55 at the freeze: E&B's UDP is 0x2C bytes (CordMgr news 0x2C before the constructor, 0x856EF4): fd +0, a
//       second socket +4 that only the destructor 0x8BFA10 touches (closesocket if set), myIP +8 / myPort +0xC
//       (getLocalAddr 0x8BFCD0, Bind 0x8BFB70), addr +0x10, a std::list<sockaddr_in> +0x20 (allocator byte +0x20, head
//       +0x24, size +0x28: constructor 0x8BF990's 0x18-byte head node, Read 0x8BFE30's push_back / pop_front); the
//       accounting callback is one static (0xBF52C4, Write 0x8BFD50 / Read 0x8BFE30)
// EnB: enum sockStat (line 92 at the freeze): GetStatus 0x8C0100 maps WSAECONNRESET (10054) to -16 after WSAEBADF
//       (`sub eax, 0x2746; neg; sbb; and 0xF; add -0x10`); SetBlocking 0x8BFD00 takes a socket (-1: fd) and has no
//       caller in udp.cpp but Bind's inlined copy: public; Close 0x8BFCA0 (between Bind and getLocalAddr), AddReset
//       0x8C0060 and an empty member 0x8C00E0 (after ClearStatus 0x8C0040, before GetStatus) have no caller in the
//       client; Set_Accounting_Callback 0x8C05E0 / Clear_Accounting_Callback 0x8C0600 (bandwidthaccounting.cpp's
//       constructor 0x84C795 / destructor 0x84C868): app/wwudp/udp.h's declarations, same mangled names
