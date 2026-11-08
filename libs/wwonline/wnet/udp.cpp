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

// E&B: the object ends with <string>'s guarded std::ctype<unsigned short>::id (0x8C0660, guard 0xC416FC)
//   and PermStringInit's constructor / destructor (0x8C0690 / 0x8C06C0, the object 0xBF52C8 after
//   the accounting callback 0xBF52C4): E&B's udp.cpp included both; through which header is not known
#include <string>
#include "..\..\..\app\shared\auxdata\permstr.h"
#include "udp.h"
#include "wlib/wdebug.h"

// E&B: the accounting callback (0xBF52C4)
void (*UDP::accountingCallback)(bool out, int len, unsigned int ip, unsigned short port)=NULL;

UDP::UDP()
{
  fd=0;
  unknownFD=0;
  myIP=0xDEADBEEF;
  myPort=0xDEAD;

  WSADATA wsadata;
  WSAStartup(0x0101,&wsadata);
}

UDP::~UDP()
{
  Close();
  WSACleanup();
  if (unknownFD)
    closesocket(unknownFD);
}

sint32 UDP::Bind(char *Host,uint16 port)
{
  char hostName[100];
  struct hostent *hostStruct;
  struct in_addr *hostNode;

  if (isdigit(Host[0]))
    return ( Bind( ntohl(inet_addr(Host)), port) );

  strcpy(hostName, Host);

  hostStruct = gethostbyname(Host);
  if (hostStruct == NULL)
    return (0);
  hostNode = (struct in_addr *) hostStruct->h_addr;
  return ( Bind(ntohl(hostNode->s_addr),port) );
}

// You must call bind, implicit binding is for sissies
//   Well... you can get implicit binding if you pass 0 for either arg
sint32 UDP::Bind(uint32 IP,uint16 Port)
{
  int retval;
  int status;

  IP=htonl(IP);
  Port=htons(Port);

  addr.sin_family=AF_INET;
  addr.sin_port=Port;
  addr.sin_addr.s_addr=IP;
  Close();
  fd=socket(AF_INET,SOCK_DGRAM,DEFAULT_PROTOCOL);
  #ifdef _WINDOWS
  if (fd==SOCKET_ERROR)
    fd=-1;
  #endif
  if (fd==-1)
    return(UNKNOWN);

  retval=bind(fd,(struct sockaddr *)&addr,sizeof(addr));

  #ifdef _WINDOWS
    if (retval==SOCKET_ERROR)
      retval=-1;
  #endif
  if (retval==-1)
  {
    status=GetStatus();
    //CERR("Bind failure (" << status << ") IP " << IP << " PORT " << Port )
    return(status);
  }

  int namelen=sizeof(addr);
  getsockname(fd, (struct sockaddr *)&addr, &namelen); 

  myIP=ntohl(addr.sin_addr.s_addr);
  myPort=ntohs(addr.sin_port);

  retval=SetBlocking(FALSE);
  if (retval==-1)
    fprintf(stderr,"Couldn't set nonblocking mode!\n");

  return(OK);
}

// E&B: close the socket
void UDP::Close(void)
{
  if (fd)
    closesocket(fd);
  fd=0;
}

bit8 UDP::getLocalAddr(uint32 &ip, uint16 &port)
{
  ip=myIP;
  port=myPort;
  return(TRUE);
}


// E&B: public, for any socket (this one's by default)
sint32 UDP::SetBlocking(bit8 block,sint32 sock)
{
  #ifdef _WINDOWS
   if (sock==-1)
     sock=fd;
   unsigned long flag=1;
   if (block)
     flag=0;
   int retval;
   retval=ioctlsocket(sock,FIONBIO,&flag);
   if (retval==SOCKET_ERROR)
     return(UNKNOWN);
   else
     return(OK);
  #else  // UNIX
   int flags = fcntl(fd, F_GETFL, 0);
   if (block==FALSE)          // set nonblocking
     flags |= O_NONBLOCK;
   else                       // set blocking
     flags &= ~(O_NONBLOCK);

   if (fcntl(fd, F_SETFL, flags) < 0)
   {
     return(UNKNOWN);
   }
   return(OK);
  #endif
}


sint32 UDP::Write(uint8 *msg,uint32 len,uint32 IP,uint16 port)
{
  sint32 retval;
  struct sockaddr_in to;

  // This happens frequently
  if ((IP==0)||(port==0)) return(ADDRNOTAVAIL);

  errno=0;
  to.sin_port=htons(port);
  to.sin_addr.s_addr=htonl(IP);
  to.sin_family=AF_INET;

  ClearStatus();
  while (1)
  {
    retval=sendto(fd,(char *)msg,len,0,(struct sockaddr *)&to,sizeof(to));
    if (retval!=SOCKET_ERROR)
    {
      if (accountingCallback)  // E&B: the bandwidth accounting
        accountingCallback(true,len,IP,port);
      break;
    }
    retval=-1;
    if (GetStatus()!=INTR)  // E&B: send again when interrupted
      break;
  }

  return(retval);
}

sint32 UDP::Read(uint8 *msg,uint32 len,sockaddr_in *from)
{
  sint32 retval;
  int    alen=sizeof(sockaddr_in);

  if (from!=NULL)
  {
    from->sin_addr.s_addr=0;
    retval=recvfrom(fd,(char *)msg,len,0,(struct sockaddr *)from,&alen);
    #ifdef _WINDOWS
    if (retval==SOCKET_ERROR)
      retval=-1;
    else
    #endif
    if ((retval>0)&&(accountingCallback))  // E&B
      accountingCallback(false,retval,ntohl(from->sin_addr.s_addr),ntohs(from->sin_port));
  }
  else
  {
    sockaddr_in sender;  // E&B: the sender, for the accounting
    retval=recvfrom(fd,(char *)msg,len,0,(struct sockaddr *)&sender,&alen);
    #ifdef _WINDOWS
    if (retval==SOCKET_ERROR)
      retval=-1;
    else
    #endif
    if ((retval>0)&&(accountingCallback))  // E&B
      accountingCallback(false,retval,ntohl(sender.sin_addr.s_addr),ntohs(sender.sin_port));
  }

  // E&B: a datagram to a closed port comes back as WSAECONNRESET on the next read; keep its
  //   address and hand it out as an empty read, so that the caller can drop that peer
  sint32 status=GetStatus();
  if ((retval==-1)&&(from!=NULL)&&(status==CONNRESET))
    AddReset(*from);
  if ((retval==-1)&&(from!=NULL)&&
      ((status==AGAIN)||(status==WOULDBLOCK)||(status==CONNRESET)||(status==CONNREFUSED))&&
      (resetList.size()!=0))
  {
    *from=resetList.front();
    resetList.pop_front();
    retval=0;
  }
  return(retval);
}


void UDP::ClearStatus(void)
{
  #ifndef _WINDOWS
  errno=0;
  #endif
}

// E&B: keep a peer whose datagram came back as WSAECONNRESET
void UDP::AddReset(sockaddr_in &from)
{
  resetList.push_back(from);
}

// E&B: an empty member (0x8C00E0, no caller)
void UDP::Unknown8C00E0(void)
{
}

UDP::sockStat UDP::GetStatus(void)
{
 #ifdef _WINDOWS
  int status=WSAGetLastError();
  if (status==0) return(OK);
  else if (status==WSAEINTR) return(INTR);
  else if (status==WSAEINPROGRESS) return(INPROGRESS);
  else if (status==WSAECONNREFUSED) return(CONNREFUSED);
  else if (status==WSAEINVAL) return(INVAL);
  else if (status==WSAEISCONN) return(ISCONN);
  else if (status==WSAENOTSOCK) return(NOTSOCK);
  else if (status==WSAETIMEDOUT) return(TIMEDOUT);
  else if (status==WSAEALREADY) return(ALREADY);
  else if (status==WSAEWOULDBLOCK) return(WOULDBLOCK);
  else if (status==WSAEBADF) return(BADF);
  else if (status==WSAECONNRESET) return(CONNRESET);  // E&B
  else     return(UNKNOWN);
 #else
  int status=errno;
  if (status==0) return(OK);
  else if (status==EINTR) return(INTR);
  else if (status==EINPROGRESS) return(INPROGRESS);
  else if (status==ECONNREFUSED) return(CONNREFUSED);
  else if (status==EINVAL) return(INVAL);
  else if (status==EISCONN) return(ISCONN);
  else if (status==ENOTSOCK) return(NOTSOCK);
  else if (status==ETIMEDOUT) return(TIMEDOUT);
  else if (status==EALREADY) return(ALREADY);
  else if (status==EAGAIN) return(AGAIN);
  else if (status==EWOULDBLOCK) return(WOULDBLOCK);
  else if (status==EBADF) return(BADF);
  else     return(UNKNOWN);
 #endif
}



//
// Wait for net activity on this socket
//
int UDP::Wait(sint32 sec,sint32 usec,fd_set &returnSet)
{
  fd_set inputSet;
 
  FD_ZERO(&inputSet);
  FD_SET(fd,&inputSet);
 
  return(Wait(sec,usec,inputSet,returnSet));
}
 
//
// Wait for net activity on a list of sockets
//
int UDP::Wait(sint32 sec,sint32 usec,fd_set &givenSet,fd_set &returnSet)
{
  Wtime        timeout,timenow,timethen;
  fd_set       backupSet;
  int          retval=0,done,givenMax;
  bit8         noTimeout=FALSE;
  timeval      tv;
 
  returnSet=givenSet;
  backupSet=returnSet;
 
  if ((sec==-1)&&(usec==-1))
    noTimeout=TRUE;
 
  timeout.SetSec(sec);
  timeout.SetUsec(usec);
  timethen+=timeout;
 
  givenMax=fd;
  for (uint32 i=0; i<(sizeof(fd_set)*8); i++)   // i=maxFD+1
  {
    if (FD_ISSET(i,&givenSet))
      givenMax=i;
  }
  ///DBGMSG("WAIT  fd="<<fd<<"  givenMax="<<givenMax);
 
  done=0;
  while( ! done)
  {
    if (noTimeout)
      retval=select(givenMax+1,&returnSet,0,0,NULL);
    else
    {
      timeout.GetTimevalMT(tv);
      retval=select(givenMax+1,&returnSet,0,0,&tv);
    }
 
    if (retval>=0)
      done=1;

    else if ((retval==-1)&&(errno==EINTR))  // in case of signal
    {
      if (noTimeout==FALSE)
      {
        timenow.Update();
        timeout=timethen-timenow;
      }
      if ((noTimeout==FALSE)&&(timenow.GetSec()==0)&&(timenow.GetUsec()==0))
        done=1;
      else
        returnSet=backupSet;
    }
    else  // maybe out of memory?
    {
      done=1;
    }
  }
  ///DBGMSG("Wait retval: "<<retval);
  return(retval);
}





// Set the kernel buffer sizes for incoming, and outgoing packets
//
// Linux seems to have a buffer max of 32767 bytes for this,
//  (which is the default). If you try and set the size to
//  greater than the default it just sets it to 32767.

bit8 UDP::SetInputBuffer(uint32 bytes)
{
  // E&B: on Windows too
   int retval,arg=bytes;

   retval=setsockopt(fd,SOL_SOCKET,SO_RCVBUF,
     (char *)&arg,sizeof(int));
   if (retval==0)
     return(TRUE);
   else
     return(FALSE);
}

// Same note goes for the output buffer

bit8 UDP::SetOutputBuffer(uint32 bytes)
{
  // E&B: on Windows too
   int retval,arg=bytes;

   retval=setsockopt(fd,SOL_SOCKET,SO_SNDBUF,
     (char *)&arg,sizeof(int));
   if (retval==0)
     return(TRUE);
   else
     return(FALSE);
}

// Get the system buffer sizes 

int UDP::GetInputBuffer(void)
{
  // E&B: on Windows too
   int retval,arg=0,len=sizeof(int);

   retval=getsockopt(fd,SOL_SOCKET,SO_RCVBUF,
     (char *)&arg,&len);
   return(arg);
}


int UDP::GetOutputBuffer(void)
{
  // E&B: on Windows too
   int retval,arg=0,len=sizeof(int);

   retval=getsockopt(fd,SOL_SOCKET,SO_SNDBUF,
     (char *)&arg,&len);
   return(arg);
}


// E&B: the bandwidth accounting's hook (bandwidthaccounting.cpp)
void UDP::Set_Accounting_Callback(void (*callback)(bool out, int len, unsigned int ip, unsigned short port))
{
  accountingCallback=callback;
}

void UDP::Clear_Accounting_Callback(void)
{
  accountingCallback=NULL;
}

// Earth & Beyond client (reconstruction): changes to Zero Hour's udp.cpp (frozen 2026-11-07 from importer.py)
// EnB: Line 19 at the freeze: udp.cpp's object ends with <string>'s guarded static (0x8C0660: `mov cl, [0xC416FC]`,
//       atexit 0x42B320, the end of every <string> includer) and a PermStringInitClass static (0x8C0690: `mov ecx,
//       0xBF52C8`, PermStringInitClass 0x4AE1E0, atexit 0x8C06C0 -> ~PermStringInitClass), its data right after the
//       accounting callback 0xBF52C4, before unknown_login_data.cpp 0x8C06E0; permstr.h's `static PermStringInitClass
//       PermStringInit` is the one such static
// EnB: Line 27 at the freeze: constructor 0x8BF990: the list, fd = 0, the second socket 0, myIP 0xDEADBEEF, myPort
//       0xDEAD, WSAStartup(0x101, &wsadata) (0x190-byte local); destructor 0x8BFA10: Close inlined (closesocket if fd,
//       fd = 0), WSACleanup, closesocket(+4) if set, then the list's destructor
// EnB: UDP::Bind (line 80 at the freeze): Bind 0x8BFB70 closes a previous socket (`if (fd) closesocket(fd); fd = 0`,
//       Close inlined) between filling addr and socket()
// EnB: Line 115 at the freeze: Close 0x8BFCA0 (no caller; inlined into the destructor and Bind); getLocalAddr 0x8BFCD0
//       returns `mov al, 1` (TRUE: matchbot's OK is 0); SetBlocking 0x8BFD00 takes the socket as a second argument
//       (`cmp eax, -1; jne; mov eax, [ecx]`)
// EnB: UDP::Write (line 162 at the freeze): D64: Write 0x8BFD50 is functional, not byte-exact yet (NOT EXACT note)
// EnB: UDP::Write (line 162 at the freeze): Write 0x8BFD50 is byte-exact now (its NOT EXACT note goes)
// EnB: UDP::Write (line 176 at the freeze): Write 0x8BFD50: sendto again while GetStatus is INTR (`cmp eax, -0xA; jne;
//       jmp` back to the sendto), then the accounting callback (out = 1, len, IP, port in host order) when sent
// EnB: UDP::Write (line 179 at the freeze): Write 0x8BFD50: the sent case first (`cmp esi, -1; jne` to the accounting
//       call after the loop's `jmp`), then retval = -1 and GetStatus (`cmp eax, -0xA; jne` out, `jmp` back to the
//       sendto)
// EnB: UDP::Read (line 193 at the freeze): Read 0x8BFE30 is byte-exact now (its NOT EXACT note goes)
// EnB: UDP::Read (line 200 at the freeze): Read 0x8BFE30: clears from->sin_addr first; reads into a local sockaddr_in
//       when from is NULL; the accounting callback (out = 0, the length, ntohl(addr), ntohs(port)) for a positive
//       length; then GetStatus, and on an error with from: a CONNRESET (-16) keeps *from in the list (0x8C0060's
//       push_back inlined, std::_Construct 0x8C0620 out of line), and an AGAIN / WOULDBLOCK / CONNRESET / CONNREFUSED
//       with a non-empty list pops its front into *from and returns 0
// EnB: UDP::Read (line 226 at the freeze): Read 0x8BFE30: two separate tests, each with retval == -1 and from (the
//       client re-tests all four codes after the push_back: `mov eax, [esp+0x28]; cmp eax, -5 ... cmp eax, -0x10`), and
//       retval = 0 for a handed-out peer (one epilogue)
// EnB: Line 247 at the freeze: AddReset 0x8C0060 (list push_back; no caller but Read's inlined copy) and the empty
//       0x8C00E0 after ClearStatus 0x8C0040 (both `ret`, neither called: which is ClearStatus is taken from matchbot's
//       order)
// EnB: UDP::GetStatus (line 273 at the freeze): GetStatus 0x8C0100: WSAECONNRESET -> CONNRESET (-16) after WSAEBADF,
//       else UNKNOWN
// EnB: UDP::SetInputBuffer (line 384 at the freeze): SetInputBuffer 0x8C04C0 / SetOutputBuffer 0x8C0500:
//       setsockopt(fd, SOL_SOCKET, SO_RCVBUF 0x1002 / SO_SNDBUF 0x1001, &bytes, 4) == 0 on Windows too (matchbot: FALSE
//       on Windows); CordMgr calls both with 0x8000 (0x856F20)
// EnB: UDP::SetInputBuffer (line 393 at the freeze): SetInputBuffer 0x8C04C0 / SetOutputBuffer 0x8C0500 on Windows too
//       (see above)
// EnB: UDP::SetOutputBuffer (line 408 at the freeze): GetInputBuffer 0x8C0540 / GetOutputBuffer 0x8C0590:
//       getsockopt(fd, SOL_SOCKET, SO_RCVBUF / SO_SNDBUF, &arg, &len) on Windows too (no caller in the client);
//       Set_Accounting_Callback 0x8C05E0 / Clear_Accounting_Callback 0x8C0600 (store / clear 0xBF52C4) at the end of
//       the object
