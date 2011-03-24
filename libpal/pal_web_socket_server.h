#pragma once

/*
	Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#include "libpal/pal_array.h"
#include "libpal/pal_tcp_listener.h"
#include "libpal/pal_event.h"
#include "libpal/pal_delegate.h"

class palWebSocketServer {
public:
  palWebSocketServer(unsigned char* incoming_buffer,
                     int incoming_buffer_capacity,
                     unsigned char* outgoing_buffer,
                     int outgoing_buffer_capacity);
  
  void SetIncomingBuffer(unsigned char* incoming_buffer, int buffer_capacity);
  void SetOutgoingBuffer(unsigned char* outgoing_buffer, int buffer_capacity);

  // total capacity of buffers
  int GetIncomingBufferCapacity();
  int GetOutgoingBufferCapacity();

  // amount currently used
  int GetIncomingBufferSize();
  int GetOutgoingBufferSize();

  // startup, waiting on given port
  // returns 0 if the server is active and waiting for a connection, -1 otherwise
  int Startup(palIPPort port);
  void Shutdown();

  // these functions are cleared by calling Update
  bool HasOpen() const;
  bool HasClose() const;
  bool HasError() const;

  bool Connected() const;

  int ConnectionError();

  // 1) accept/disconnect a connection
  // 2) send any pending messages
  // 3) receive any pending messages
  // 4) sets HasOpen, Close, Error flags
  void Update();

  int SendMessage(const char* msg);
  int SendMessage(const char* msg, int msg_length);

  typedef palDelegate<void (const char* msg, int msg_length)> OnMessageDelegate;

  
  int PendingMessageCount() const;

  void ProcessMessages(OnMessageDelegate del);
  
  void ClearMessages();
protected:
  void ClientDisconnect();
  void ResetBuffers();
  int ParseMessages();
  struct message_header {
    const unsigned char* msg;
    int msg_length;
  };
  bool handshake_okay_;
  bool has_open_;
  bool has_close_;
  bool has_error_;
  palTcpListener listener_;
  palTcpClient client_;
  palArray<message_header> messages_;
  int last_message_cursor_;
  unsigned char* incoming_buffer_;
  int incoming_buffer_length_;
  int incoming_buffer_cursor_;
  unsigned char* outgoing_buffer_;
  int outgoing_buffer_length_;
  int outgoing_buffer_cursor_;

  bool HasCompleteHandshake(const char* s, int s_len);
  int ProcessHandshake(const char* s, int s_len);
};