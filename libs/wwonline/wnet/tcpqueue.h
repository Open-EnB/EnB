// Earth & Beyond client (reconstruction): libs\wwonline\wnet\tcpqueue.h
/*
**	E&B's byte queue under TCPCon's read and write queues (TCPCon +0x14 and +0x38): the bytes in
**	1024-byte buffers, a list of the buffers in use and one of spare buffers kept for reuse.
**	Written from the client (tcpqueue.cpp, 0x8BE860-0x8BF5F0; its __FILE__ is
**	e:\build\beta\G\libs\wwonline\wnet\tcpqueue.cpp). The class and buffer names are not in the
**	client; the member Queue is (the assert "Queue.length()==0", 0x8BEAC0).
*/


#ifndef TCPQUEUE_HEADER
#define TCPQUEUE_HEADER

#include "wlib/wstypes.h"
#include "wlib/arraylist.h"

#define TCPQUEUE_BUFFER_SIZE  1024  // the bytes of one buffer (0x8BF1C0: new 0x400)  // NAME 2: what it is

// One buffer of the queue: 8 bytes (0x8BF1C0: new 8)
struct TCPQueueBuffer  // NAME 1: what it is
{
  TCPQueueBuffer() { buffer=new uint8[TCPQUEUE_BUFFER_SIZE]; len=0; }
  ~TCPQueueBuffer() { delete[](buffer); }

  uint8   *buffer;  // +0  // NAME 2: TCPCon's pumpWrites gets it as getPointer's buffer
  sint32   len;     // +4 the bytes in use, from buffer[0]  // NAME 2: getPointer's len
};

class TCPQueue  // NAME 1 [tcpqueue.cpp]: the file's name
{
 public:
           TCPQueue();
          ~TCPQueue();

   void    clear(void);
   sint32  length(void);  // the bytes queued
   bit8    getPointer(OUT uint8 **buffer, OUT sint32 *len);  // the first buffer's bytes
   sint32  remove(OUT uint8 *buffer, sint32 maxlen);  // from the head; buffer may be NULL  // NAME 2: what it does
   sint32  addHead(IN uint8 *buffer_in, sint32 length_in);  // TCPCon::unread  // NAME 2: what it does
   sint32  add(IN uint8 *buffer, sint32 length);  // to the tail  // NAME 2: what it does

 private:
   ArrayList<TCPQueueBuffer *>  Queue;   // +0x00 the buffers in use, head first
   sint32                       Length_; // +0x0C the bytes in them  // NAME 2: length() returns it
   ArrayList<TCPQueueBuffer *>  Spare_;  // +0x10 emptied buffers, at most 5 more than Queue holds  // NAME 2: what it holds
};

#endif
