/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**
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

/*
**	Earth & Beyond client (reconstruction): E&B's TCPCon, on Renegade's Code\Tools\RenegadeGR
**	tcpcon.h. E&B's class (0x7C bytes, the constructor 0x8BBA70's stores) is an OutputDevice, keeps
**	its read and write queues in tcpqueue.cpp's TCPQueue, can lock itself (a CritSec, when Locking_ is
**	set) and remembers the peer's address; tcpcon.cpp is written from the client (0x8BBA70-0x8BCA50).
*/

#ifndef TCP_CON_HEADER
#define TCP_CON_HEADER

#include "tcpmgr.h"
#include "tcpqueue.h"
#include "wlib/odevice.h"
#include "wlib/critsec.h"

class TCPMgr;

class TCPCon : public OutputDevice  // E&B: the base RenegadeGR commented out (0x8BBA70 stores OutputDevice's vtable 0xB0C47C, then its own 0xB0C4C8)
{
 public:
           TCPCon(SOCKET sock);
           ~TCPCon();

   void    setLocking(bit8 enabled);  // E&B 0x8BBE00: lock around each call  // NAME 2: what it sets
   SOCKET  getFD(void);
   void    close(bit8 now);  // E&B 0x8BBE40: unless now, the buffered writes go first  // NAME 2: what it does
   sint32  write(IN uint8 *msg, uint32 len, sint32 wait_secs=-1);
   sint32  read(OUT uint8 *msg, uint32 maxlen, sint32 wait_secs=-1);
   sint32  pumpReads(void);  // E&B 0x8BC4F0: what the socket has, into the read queue  // NAME 2: what it does
   sint32  peekRead(OUT uint8 **buffer);  // E&B 0x8BC600: the first read buffer; leaves the lock taken  // NAME 2: what it does
   void    endPeekRead(void);  // E&B 0x8BC680: gives the lock back  // NAME 2: what it does
   bit8    unread(uint8 *data, int length);
   bit8    getRemoteAddr(uint32 *ip, uint16 *port);
   sint32  printf(const char *format, ...);
   bit8    isConnected(void);

   bit8    setInputDelay(sint32 delay) { InputDelay_=delay; return(TRUE); };
   bit8    setOutputDelay(sint32 delay) { OutputDelay_=delay; return(TRUE); };

   // For OutputDevice
   virtual int print(IN char *str, int len);  // E&B: RenegadeGR's commented-out print (0x8BC980)

 private:
   friend  class TCPMgr;
   friend  class TCPConnection;  // E&B: tcpconn.cpp pumps the writes (0x829960, 0x829BC0)
   sint32  pumpWrites(void);  // for buffered mode  // E&B 0x8BC040: returns a value (0x8BBF00 tests it)
   void    setBufferedWrites(TCPMgr *mgrptr, bit8 enabled);

   sint32  normalWrite(IN uint8 *msg, uint32 len, sint32 wait_secs=-1);

   sint32              InputDelay_;  // default max time for input        +0x04
   sint32              OutputDelay_; // default max time for output       +0x08

   TCPMgr::CONN_STATE  State_;       // +0x0C
   SOCKET              Socket_;      // +0x10
   TCPQueue            ReadQueue_;       // reads are buffered            +0x14
   sint32              MaxReadQueue_;    // E&B: pumpReads stops at this many bytes (0x10000)  +0x30  // NAME 2: what it holds

   bit8                BufferedWrites_;  // T/F buffer writes?            +0x34
   TCPQueue            WriteQueue_;      // writes _can_ be buffered      +0x38
   TCPMgr              *TCPMgrPtr_;      // pointer to my manager object  +0x54
   CritSec             Lock_;            // E&B  +0x58  // NAME 2: what it is
   bit8                Locking_;         // E&B: take Lock_?  +0x70  // NAME 2: what it holds
   uint32              RemoteIp_;        // E&B: the peer, host order (getRemoteAddr's cache)  +0x74  // NAME 2: what it holds
   uint16              RemotePort_;      // E&B  +0x78  // NAME 2: what it holds
};

#endif
