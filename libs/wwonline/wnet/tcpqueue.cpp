/*
**	Earth & Beyond client (reconstruction): libs\wwonline\wnet\tcpqueue.cpp
**	Written from the client, 0x8BE860-0x8BF5F0 (tcpqueue.h); the FAIL_IFs on the client's lines.
*/

#include "tcpqueue.h"

// E&B's FAIL_IF (app\shared\unknown_debug.h's: 0x4E9450 reports, then the statement runs)
void Unknown_Fail_If_Report(const char *expr, const char *file, int line);
#define FAIL_IF(expr)	if ((expr) ? (Unknown_Fail_If_Report(#expr, __FILE__, __LINE__), 1) : 0)

TCPQueue::TCPQueue() :
  Length_(0)
{
}

TCPQueue::~TCPQueue()
{
  clear();
}

void TCPQueue::clear(void)
{
  while (remove(NULL, 0xFFFFFF))
    ;
  int count=Spare_.length();
  for (int i=0; i<count; i++)
  {
    TCPQueueBuffer *vecptr=NULL;
    Spare_.get(vecptr, i);
    delete(vecptr);
  }
  Spare_.clear();
}

sint32 TCPQueue::length(void)
{
  return(Length_);
}



// A pointer to the first buffer's bytes and their count
bit8 TCPQueue::getPointer(OUT uint8 **buffer, OUT sint32 *len)
{
  FAIL_IF(!buffer) return(FALSE);
  FAIL_IF(!len) return(FALSE);
  if (Length_==0)
    return(FALSE);
  TCPQueueBuffer *vecptr=NULL;
  Queue.getHead(vecptr);
  *buffer=vecptr->buffer;
  *len=vecptr->len;
  return(TRUE);
}

//
//
//
// Take up to maxlen bytes from the head (into buffer unless it is NULL)
//
// Returns the bytes taken.
//
sint32 TCPQueue::remove(OUT uint8 *buffer, sint32 maxlen)
{
  FAIL_IF(maxlen < 0) return(0);
  if ((maxlen==0) || (Length_==0))
    return(0);
  sint32 retval=0;
  TCPQueueBuffer *vecptr=NULL;
  do
  {

    // the first buffer
    //
    FAIL_IF(Queue.length()==0) return(0);

    Queue.getHead(vecptr);
    FAIL_IF(vecptr == 0) return(0);

    sint32 len=MIN(vecptr->len, maxlen);
    if (buffer)
    {
      memcpy(buffer, vecptr->buffer, len);
      buffer+=len;
    }
    retval+=len;
    maxlen-=len;
    vecptr->len-=len;
    Length_-=len;

    if (vecptr->len==0)
    {
      // emptied: keep it for reuse, but no more than 5 beyond the buffers in use
      Queue.removeHead(vecptr);
      Spare_.addTail(vecptr);
      while (Spare_.length() > Queue.length()+5)
      {
        TCPQueueBuffer *spare=NULL;
        Spare_.removeTail(spare);
        delete(spare);
      }
    }
    else
      memmove(vecptr->buffer, vecptr->buffer+len, vecptr->len);
  } while ((maxlen) && (Length_));
  return(retval);
}


//



// Put bytes back at the head (TCPCon::unread), in the order given
//
// The last buffer-sized piece goes first, each to the front of a buffer of its own.
//
// Returns the bytes added.
//
sint32 TCPQueue::addHead(IN uint8 *buffer_in, sint32 length_in)
{
  FAIL_IF(!buffer_in) return(0);
  FAIL_IF(length_in < 0) return(0);
  if (length_in<=0)
    return(0);

  TCPQueueBuffer *vecptr=NULL;
  if (Queue.length())
    Queue.getHead(vecptr);

  IN uint8 *ptr=buffer_in+((length_in/TCPQUEUE_BUFFER_SIZE)*TCPQUEUE_BUFFER_SIZE);
  sint32 len=length_in%TCPQUEUE_BUFFER_SIZE;
  if (len==0)
  {
    ptr-=TCPQUEUE_BUFFER_SIZE;
    len=TCPQUEUE_BUFFER_SIZE;
  }
  do
  {
    sint32 copylen=MIN(len, TCPQUEUE_BUFFER_SIZE);
    if ((vecptr==NULL) || ((vecptr->len+copylen) > TCPQUEUE_BUFFER_SIZE))
    {
      if (Spare_.length())
        Spare_.removeHead(vecptr);
      else
        vecptr=new TCPQueueBuffer;
      Queue.addHead(vecptr);
    }
    if (vecptr->len)
      memmove(vecptr->buffer+copylen, vecptr->buffer, vecptr->len);
    memcpy(vecptr->buffer, ptr, copylen);
    ptr-=TCPQUEUE_BUFFER_SIZE;
    len=TCPQUEUE_BUFFER_SIZE;
    Length_+=copylen;
    vecptr->len+=copylen;
    vecptr=NULL;
  } while (ptr >= buffer_in);
  return(length_in);
}











//











// Add bytes at the tail: the last buffer is filled first
//
// Returns the bytes added.
//
sint32 TCPQueue::add(IN uint8 *buffer, sint32 length)
{
  FAIL_IF(!buffer) return(0);
  FAIL_IF(length < 0) return(0);
  if (length==0)
    return(0);

  sint32 retval=length;
  TCPQueueBuffer *vecptr=NULL;
  if (Queue.length())
    Queue.getTail(vecptr);
  do
  {
    sint32 len=MIN(length, TCPQUEUE_BUFFER_SIZE);
    if (vecptr)
    {
      if ((vecptr->len < TCPQUEUE_BUFFER_SIZE) && ((TCPQUEUE_BUFFER_SIZE-vecptr->len) < len))
        len=TCPQUEUE_BUFFER_SIZE-vecptr->len;
    }
    if ((vecptr==NULL) || ((vecptr->len+len) > TCPQUEUE_BUFFER_SIZE))
    {
      if (Spare_.length())
        Spare_.removeHead(vecptr);
      else
        vecptr=new TCPQueueBuffer;
      Queue.addTail(vecptr);
    }
    memcpy(vecptr->buffer+vecptr->len, buffer, len);
    buffer+=len;
    Length_+=len;
    length-=len;
    vecptr->len+=len;
    vecptr=NULL;
  } while (length);
  return(retval);
}
