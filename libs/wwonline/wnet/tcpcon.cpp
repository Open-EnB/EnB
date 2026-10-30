// Earth & Beyond client (reconstruction): libs\wwonline\wnet\tcpcon.cpp
/*
**	E&B's TCPCon (tcpcon.h), written from the client (0x8BBA70-0x8BCA50) on the plan of Renegade's
**	Code\Tools\RenegadeGR tcpcon.cpp: the queues are TCPQueues, each call takes the lock when
**	Locking_ is set, a reset connection (TCPMgr::CONNRESET) ends a read or write quietly.
*/

#include "tcpcon.h"
#include "wlib/wtime.h"
#include <time.h>

#define DEFAULT_TCP_DELAY 15

TCPCon::TCPCon(SOCKET sock) :
  MaxReadQueue_(0x10000),
  BufferedWrites_(false),
  TCPMgrPtr_(NULL),
  Locking_(false),
  RemoteIp_(0),
  RemotePort_(0)
{
  Socket_=sock;
  isConnected();  // This will set the internal connect status
  setInputDelay(DEFAULT_TCP_DELAY);
  setOutputDelay(DEFAULT_TCP_DELAY);
  getRemoteAddr(&RemoteIp_, &RemotePort_);  // E&B: remember the peer
}

TCPCon::~TCPCon()
{
  if (Locking_)
    Lock_.lock();
  close(FALSE);  // will try and flush the write queue
  closesocket(Socket_);
  Socket_=INVALID_SOCKET;

  if ((BufferedWrites_) && (TCPMgrPtr_))
    TCPMgrPtr_->setBufferedWrites(this, FALSE);
  if (Locking_)
    Lock_.unlock();
}

void TCPCon::setLocking(bit8 enabled)
{
  Locking_=enabled;
}

SOCKET TCPCon::getFD(void)
{  return(Socket_); }


void TCPCon::close(bit8 now)
{
  if (Locking_)
    Lock_.lock();
  if ((BufferedWrites_) && (!now))
  {
    while(WriteQueue_.length())
    {
      msleep(1);
      pumpWrites();
    }
  }
  if (State_!=TCPMgr::CLOSED)
    shutdown(Socket_, 2);
  State_=TCPMgr::CLOSED;
  WriteQueue_.clear();
  if (Locking_)
    Lock_.unlock();
}


//
// Write data
//
// Returns 'n' bytes written, 0 if closed, or -1 for error.
//
sint32 TCPCon::write(IN uint8 *msg,uint32 len, sint32 wait_secs)
{
  if (State_==TCPMgr::CLOSED)
    return(0);

  if (BufferedWrites_)
  {
    if (Locking_)
      Lock_.lock();
    WriteQueue_.add(msg, len);  // add to tail
    if (Locking_)
      Lock_.unlock();
    return(pumpWrites() ? len : 0);  // send what I can
  }
  else
  {
    return (normalWrite(msg, len, wait_secs));
  }
}


//
// set buffered status
//
void TCPCon::setBufferedWrites(TCPMgr *mgrptr, bit8 enabled)
{
  if (enabled)
    BufferedWrites_=TRUE;
  else
  {
    if (Locking_)
      Lock_.lock();
    while(WriteQueue_.length())
    {
      msleep(1);
      pumpWrites();
    }
    if (Locking_)
      Lock_.unlock();
    BufferedWrites_=FALSE;
  }
  TCPMgrPtr_=mgrptr;
}


//
// Try and send queued data (PRIVATE)
//
// Returns the bytes sent, -1 if none went (0 once the queue is empty or the connection closed).
//
sint32 TCPCon::pumpWrites(void)
{
  if (Locking_)
    Lock_.lock();
  sint32 total=0;
  if (State_==TCPMgr::CLOSED)
    WriteQueue_.clear();

  if (WriteQueue_.length() <= 0)
  {
    if (Locking_)
      Lock_.unlock();
    return((State_!=TCPMgr::CLOSED) ? -1 : 0);
  }

  sint32 sendlen;
  int   retval;
  uint8 *bufptr=NULL;
  bit8  closed=FALSE;

  while(1)
  {
    if ((!WriteQueue_.getPointer(&bufptr, &sendlen)) || (sendlen <= 0))  // the first buffer
      break;
    retval=normalWrite((uint8 *)bufptr, sendlen, 0);
    if (retval > 0)
      total+=retval;
    if (retval == 0)
    {
      WriteQueue_.clear();
      closed=TRUE;
      break;
    }
    if (retval <= 0)
      break;

    WriteQueue_.remove(NULL, retval);  // successful send
    if (retval < sendlen)
      break;
  }
  if (Locking_)
    Lock_.unlock();
  if (closed)
    return(total);
  if (total)
    return(total);
  return(-1);
}


//
// Non-buffered write (PRIVATE METHOD)
//
// Returns 'n' bytes written, 0 if closed, or -1 for error.
//
sint32 TCPCon::normalWrite(IN uint8 *msg,uint32 len, sint32 wait_secs)
{
  if (State_==TCPMgr::CLOSED)
    return(0);

  if (wait_secs < 0)
    wait_secs=OutputDelay_;

  sint32 retval;
  sint32 sendCount=0;
  time_t start=0;

  TCPMgr::STATUS status;

  while(1)
  {
    retval=send(Socket_,(const char *)(msg+sendCount),(len-sendCount),0);
    if (retval==SOCKET_ERROR)
    {
      status=TCPMgr::getStatus();
      if (status == TCPMgr::CONNRESET)
        return(sendCount);
      if ((status != TCPMgr::INTR) && (status != TCPMgr::WOULDBLOCK) && (status != TCPMgr::INPROGRESS) &&
          (status != TCPMgr::AGAIN))
        break;
    }
    else if (retval > 0)
    {
      sendCount+=retval;
      if (retval==int(len))
        break;
    }

    if (wait_secs==0)
      break;
    time_t now=time(NULL);
    if (start==0)
      start=now;
    sint32 remaining_wait=wait_secs - (now-start);
    if (remaining_wait <= 0)
      break;
    TCPMgr::wait(remaining_wait,0,&Socket_,1,FALSE);
  }
  if (sendCount==0)
    return(-1);
  return(sendCount);
}

//
// Read data
//
// Returns 'n' bytes read, 0 for close, or -1 for error.
// This may return less than we asked for
//
sint32 TCPCon::read(OUT uint8 *msg,uint32 maxlen, sint32 wait_secs)
{
  sint32 retval;
  time_t start=0;
  char   readBuffer[1025];

  if (wait_secs < 0)
    wait_secs=InputDelay_;

  if (Locking_)
    Lock_.lock();
  if ((State_==TCPMgr::CLOSED) && (ReadQueue_.length()==0))
  {
    if (Locking_)
      Lock_.unlock();
    return(0);
  }

  TCPMgr::STATUS status;

  while(1)
  {
    // Do we even nead to read from the net?
    if ((ReadQueue_.length() >= int(maxlen)) || (State_==TCPMgr::CLOSED))
    {
      maxlen=MIN(maxlen, (uint32)ReadQueue_.length());
      ReadQueue_.remove(msg, maxlen);
      if (Locking_)
        Lock_.unlock();
      return(maxlen);
    }

    do
    {
      retval=recv(Socket_,readBuffer, 1024,0);
      if (retval > 0)   // add to the tail of the list
      {
        readBuffer[retval]=0;
        ReadQueue_.add((uint8 *)readBuffer, retval);  // Add to tail
      }
    } while ((retval > 0)&&(ReadQueue_.length() < int(maxlen)));

    if (ReadQueue_.length() > 0)  // OK, we'll take what we've got
    {
      retval=ReadQueue_.remove(msg, maxlen);
      if (Locking_)
        Lock_.unlock();
      return(retval);
    }
    else if (retval==0)
    {
      if (Locking_)
        Lock_.unlock();
      return(0);
    }
    else if (retval==SOCKET_ERROR)
    {
      status=TCPMgr::getStatus();
      if (status == TCPMgr::CONNRESET)
      {
        if (Locking_)
          Lock_.unlock();
        return(0);
      }
      if ((status != TCPMgr::INTR) && (status != TCPMgr::WOULDBLOCK) &&
          (status != TCPMgr::INPROGRESS) && (status != TCPMgr::AGAIN))
      {
        if (Locking_)
          Lock_.unlock();
        return(-1);
      }
    }

    if (wait_secs==0)
      break;
    time_t now=time(NULL);
    if (start==0)
      start=now;
    sint32 remaining_wait=wait_secs - (now-start);
    if (remaining_wait <= 0)
      break;
    TCPMgr::wait(remaining_wait,0,&Socket_,1,TRUE);
  }
  if (Locking_)
    Lock_.unlock();
  return(retval);
}

//
// E&B: read what the socket has into the read queue, up to MaxReadQueue_ bytes queued
//
// Returns the last recv's result: 0 once the connection is closed or reset.
//
sint32 TCPCon::pumpReads(void)
{
  if (State_==TCPMgr::CLOSED)
    return(0);
  if (Locking_)
    Lock_.lock();
  if (ReadQueue_.length() >= MaxReadQueue_)
  {
    if (Locking_)
      Lock_.unlock();
    return(0);
  }

  char   readBuffer[1025];
  sint32 retval;
  if (Locking_)
    Lock_.unlock();
  while(1)
  {
    retval=recv(Socket_, readBuffer, 1024, 0);
    if (retval <= 0)
      break;
    if (Locking_)
      Lock_.lock();
    ReadQueue_.add((uint8 *)readBuffer, retval);
    if (Locking_)
      Lock_.unlock();
  }
  if (retval==SOCKET_ERROR)
  {
    retval=-1;
    if (TCPMgr::getStatus()==TCPMgr::CONNRESET)
      return(0);
  }
  return(retval);
}

//
// E&B: the first buffer of the read queue, without taking it; the lock stays taken (endPeekRead)
//
// Returns its bytes.
//
sint32 TCPCon::peekRead(OUT uint8 **buffer)
{
  if (Locking_)
    Lock_.lock();
  if (ReadQueue_.length()==0)
  {
    *buffer=NULL;
    return(0);
  }
  sint32 len=0;
  ReadQueue_.getPointer(buffer, &len);
  return(len);
}

void TCPCon::endPeekRead(void)
{
  if (Locking_)
    Lock_.unlock();
}

// Push data back onto the read queue
bit8 TCPCon::unread(uint8 *data, int length)
{
  if (length <= 0)
    return(FALSE);
  if (Locking_)
    Lock_.lock();
  ReadQueue_.addHead(data, length);
  if (Locking_)
    Lock_.unlock();
  return(TRUE);
}


// Returns 0 on failure
// Returns IP in host byte order!
bit8 TCPCon::getRemoteAddr(uint32 *ip, uint16 *port)
{
  if ((RemoteIp_) && (RemotePort_))  // E&B: known already
  {
    if (ip)
      *ip=RemoteIp_;
    if (port)
      *port=RemotePort_;
    return(TRUE);
  }

  struct sockaddr_in sin;
  int    sinSize=sizeof(sin);

  if(getpeername(Socket_,(sockaddr *)&sin,&sinSize)==0)
  {
    if (ip)
      *ip=ntohl(sin.sin_addr.s_addr);
    if (port)
      *port=ntohs(sin.sin_port);
    return(TRUE);
  }
  return(FALSE);
}

//
// only use for strings up to 4096 chars!
//
sint32 TCPCon::printf(const char *format, ...)
{
  va_list arg;
  char string[4097];
  sint32 retval;
  va_start(arg,format);
  vsprintf(string,format,arg);
  va_end(arg);
  string[4096]=0;

  retval=write((IN uint8 *)string,strlen(string), OutputDelay_);
  return(retval);
}


//
// E&B: a peek at the socket tells whether it is still connected
//
bit8 TCPCon::isConnected(void)
{
  if (Socket_==INVALID_SOCKET)
  {
    State_=TCPMgr::CLOSED;
    return(FALSE);
  }

  char c;
  int retval=recv(Socket_, &c, 1, MSG_PEEK);
  if (retval==SOCKET_ERROR)
  {
    switch (TCPMgr::getStatus())
    {
      case TCPMgr::CONNRESET:
      case TCPMgr::INVAL:
      case TCPMgr::NOTSOCK:
      case TCPMgr::BADF:
        retval=0;
        break;
    }
  }
  if (retval==0)
  {
    State_=TCPMgr::CLOSED;
    shutdown(Socket_, 2);
    return(FALSE);
  }
  State_=TCPMgr::CONNECTED;
  return(TRUE);
}

// For the OutputDevice interface
int TCPCon::print(IN char *str, int len)
{
  return(write((IN uint8 *)str,len,0));
}
