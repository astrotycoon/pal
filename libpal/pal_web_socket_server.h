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
#include "libpal/pal_mem_blob.h"
#include "libpal/pal_tcp_listener.h"
#include "libpal/pal_event.h"
#include "libpal/pal_delegate.h"

class palWebSocketServer {
public:
  palWebSocketServer();

  void SetIncomingBuffer(unsigned char* incoming_buffer, uint64_t buffer_capacity);
  void SetOutgoingBuffer(unsigned char* outgoing_buffer, uint64_t buffer_capacity);
  void SetMessageBuffer(unsigned char* msg_buffer, uint64_t buffer_capacity);

  // total capacity of buffers
  uint64_t GetIncomingBufferCapacity();
  uint64_t GetOutgoingBufferCapacity();

  // amount currently used
  uint64_t GetIncomingBufferSize();
  uint64_t GetOutgoingBufferSize();

  // startup, waiting on given port
  // returns 0 if the server is active and waiting for a connection, -1 otherwise
  int Startup(palIPPort port);
  void Shutdown();

  // The state these functions reference are updated by calling Update
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

  int SendPing();

  int SendMessage(const char* msg);
  int SendMessage(const char* msg, int msg_length);

  typedef palDelegate<void (palMemBlob msg, bool binary_message)> OnMessageDelegate;
  uint64_t PendingMessageCount() const;
  void ProcessMessages(OnMessageDelegate del);
  void ClearMessages();
protected:
  void ClientDisconnect();
  void ResetBuffers();
  void SendPong(uint64_t payload_size, uint32_t mask);
  void ParseMessage(uint64_t header_size);
  int ParseMessages();
  bool HasCompleteHandshake(const unsigned char* s, int s_len);
  int ProcessHandshake(const unsigned char* s, int s_len);

  bool handshake_okay_;
  bool has_open_;
  bool has_close_;
  uint16_t _closed_code;
  bool has_error_;

  palTcpListener listener_;
  palTcpClient client_;

  palRingBlob _incoming_buffer;
  palRingBlob _outgoing_buffer;
  palAppendChopBlob _message_buffer;
  uint64_t _message_count;
  uint64_t _message_cursor;
  
};
