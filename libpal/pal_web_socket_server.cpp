
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

#include "libpal/pal_web_socket_server.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_string.h"
#include "libpal/pal_endian.h"
#include "libpal/pal_md5.h"
#include "libpal/pal_tokenizer.h"

palWebSocketServer::palWebSocketServer(unsigned char* incoming_buffer, int incoming_buffer_capacity, unsigned  char* outgoing_buffer, int outgoing_buffer_capacity) {
  SetIncomingBuffer(incoming_buffer, incoming_buffer_capacity);
  SetOutgoingBuffer(outgoing_buffer, outgoing_buffer_capacity);
  last_message_cursor_ = 0;
  handshake_okay_ = false;
}

void palWebSocketServer::SetIncomingBuffer(unsigned char* incoming_buffer, int buffer_capacity) {
  incoming_buffer_ = incoming_buffer;
  incoming_buffer_cursor_ = 0;
  incoming_buffer_length_ = buffer_capacity;
}

void palWebSocketServer::SetOutgoingBuffer(unsigned char* outgoing_buffer, int buffer_capacity) {
  outgoing_buffer_ = outgoing_buffer;
  outgoing_buffer_cursor_ = 0;
  outgoing_buffer_length_ = buffer_capacity;
}

// total capacity of buffers
int palWebSocketServer::GetIncomingBufferCapacity() {
  return incoming_buffer_length_;
}
int palWebSocketServer::GetOutgoingBufferCapacity() {
  return outgoing_buffer_length_;
}

// amount currently used
int palWebSocketServer::GetIncomingBufferSize() {
  return incoming_buffer_cursor_;
}

int palWebSocketServer::GetOutgoingBufferSize() {
  return outgoing_buffer_cursor_;
}

// startup, waiting on given port
int palWebSocketServer::Startup(palIPPort port) {
  Shutdown(); // shutdown first...
  listener_ = palTcpListener(port);
  int r = listener_.StartListening();
  if (listener_.Active()) {
    return 0;
  }
  return r;
}

void palWebSocketServer::Shutdown() {
  client_.Close();
  listener_.StopListening();
  incoming_buffer_cursor_ = 0;
  outgoing_buffer_cursor_ = 0;
  last_message_cursor_ = 0;
  handshake_okay_ = false;
}

bool palWebSocketServer::HasOpen() const {
  return has_open_;
}
bool palWebSocketServer::HasClose() const {
  return has_close_;
}
bool palWebSocketServer::HasError() const {
  return has_error_;
}


bool palWebSocketServer::Connected() const {
  return client_.Connected();
}

// 1) accept/disconnect a connection
// 2) send any pending messages
// 3) receive any pending messages
void palWebSocketServer::Update() {
  int r;
  // clear flags
  has_open_ = false;
  has_close_ = false;
  has_error_ = false;

  if (client_.Connected() == false) {
    // no connection
    if (handshake_okay_ == true) {
      // we had been connected.
      has_close_ = true;
      handshake_okay_ = false;
      ClientDisconnect();
    }
    if (listener_.PendingConnections()) {
      r = listener_.AcceptTcpClient(&client_);
    }
  } else if (handshake_okay_ == false) {
    // still need to handle handshake    
    if (client_.CanBeRead()) {
      int bytes_to_read = 512;
      if (incoming_buffer_cursor_ >= incoming_buffer_length_) {
        // we have filled our entire buffer without finding a valid handshake
        // kick this client off
        has_error_ = true;
        client_.Close();
        incoming_buffer_cursor_ = 0;
      } else {
        r = client_.Receive(&incoming_buffer_[incoming_buffer_cursor_], &bytes_to_read);
        if (bytes_to_read > 0) {
          incoming_buffer_cursor_ += bytes_to_read;
          palDynamicString s;
          s.Append((const char*)incoming_buffer_, incoming_buffer_cursor_);
          if (HasCompleteHandshake(s.C(), bytes_to_read)) {
            r = ProcessHandshake(s.C(), bytes_to_read);
            if (r == 0) {
              handshake_okay_ = true;
              has_open_ = true;
              incoming_buffer_cursor_ = 0;
            } else {
              has_error_ = true;
              client_.Close();
              incoming_buffer_cursor_ = 0;
            }
          }
        }
      } 
    } else if (incoming_buffer_cursor_ > 0) {
      // if we can't read from the socket and we've read part of a handshake, quit.
      has_error_ = true;
      client_.Close();
      incoming_buffer_cursor_ = 0;
    }
  } else {
    // have an active connection
    if (outgoing_buffer_cursor_ > 0) {
      // pump outgoing messages
      client_.Send(&outgoing_buffer_[0], outgoing_buffer_cursor_);
      outgoing_buffer_cursor_ = 0;
    }
    
    // read incoming messages

    const int buffer_available = incoming_buffer_length_ - incoming_buffer_cursor_;
    int num_bytes_to_read = buffer_available;

    if (client_.CanBeRead()) {
      r = client_.Receive(&incoming_buffer_[incoming_buffer_cursor_], &num_bytes_to_read);
      if (r < 0) {
        ClientDisconnect();
      } else {
        if (num_bytes_to_read > 0) {
          incoming_buffer_cursor_ += num_bytes_to_read;
          ParseMessages();
        }
      } 
    }
  }
}

int palWebSocketServer::SendMessage(const char* msg) {
  int len = palStringLength(msg);
  return SendMessage(msg, len);
}

int palWebSocketServer::SendMessage(const char* msg, int msg_length) {
  if (2 + msg_length + outgoing_buffer_cursor_ >= outgoing_buffer_length_) {
    return -1;
  }
  // copy message into outgoing buffer
  outgoing_buffer_[outgoing_buffer_cursor_] = 0x00;
  outgoing_buffer_cursor_++;
  palMemoryCopyBytes(&outgoing_buffer_[outgoing_buffer_cursor_], msg, msg_length);
  outgoing_buffer_cursor_ += msg_length;
  outgoing_buffer_[outgoing_buffer_cursor_] = 0xFF;
  outgoing_buffer_cursor_++;  
  return 0;
}

int palWebSocketServer::PendingMessageCount() const {
  return messages_.GetSize();
}

void palWebSocketServer::ProcessMessages(OnMessageDelegate del) {
  for (int i = 0; i < messages_.GetSize(); i++) {
    message_header* msg = &messages_[i];
    del((const char*)msg->msg, msg->msg_length);
  }
}

void palWebSocketServer::ClearMessages() {
  messages_.Clear();
  int bytes_to_parse = incoming_buffer_cursor_ - last_message_cursor_;
  if (bytes_to_parse > 0) {
    // move rest of messages down
    palMemoryCopyBytes(&incoming_buffer_[0], &incoming_buffer_[last_message_cursor_], bytes_to_parse);
    incoming_buffer_cursor_ -= last_message_cursor_;
    last_message_cursor_ = 0;
  }
}

void palWebSocketServer::ClientDisconnect() {
  ResetBuffers();
  messages_.Clear();
}

void palWebSocketServer::ResetBuffers() {
  incoming_buffer_cursor_ = 0;
  outgoing_buffer_cursor_ = 0;
  last_message_cursor_ = 0;  
}

int palWebSocketServer::ParseMessages() {
  int bytes_to_parse = incoming_buffer_cursor_ - last_message_cursor_;

  // state machine:
  // 0 - need to find 0 byte
  // 1 - reading message
  // 0 - back to 0 after reading in 0xff byte
  int state = 0;
  int cursor = last_message_cursor_;
  while (bytes_to_parse > 0) {
    unsigned char byte = incoming_buffer_[cursor];
    if (state == 0) {
      if (byte == 0x0) {
        state = 1;
        cursor++;
      } else {
        return -1;
      }
    } else {
      if (byte == 0xff) {
        // end of message
        cursor++;
        message_header msg;
        msg.msg = &incoming_buffer_[last_message_cursor_+1];
        msg.msg_length = cursor - last_message_cursor_;
        // subtract the 0x00 and 0xff bytes
        msg.msg_length = msg.msg_length - 2;
        messages_.push_back(msg);

        state = 0;
        last_message_cursor_ = cursor;
      } else {
        // inside message
        cursor++;
      }
    }

    bytes_to_parse--;
  }

  return 0;
}



static uint32_t ParseKey(palTokenizer* tokenizer) {  
  palDynamicString key_as_string;

  bool r = tokenizer->ReadRestOfLineAsString(&key_as_string);
  if (r == false) {
    return 0;
  }
  int spaces = 0;
  uint32_t key = 0;

  const char* ch = key_as_string.C();
  while (*ch != '\0') {
    if (palIsDigit(*ch)) {
      key *= 10;
      key += palDigitValue(*ch);
    } else if (palIsWhiteSpace(*ch)) {
      spaces++;
    }
    ch++;
  }
  // key_as_string starts off with a space
  spaces--;

  if (spaces == 0) {
    // bad client
    return 0;
  }

  if (key % spaces != 0) {
    // bad client
    return 0;
  }
#if 0
  palPrintf("Number = %d\n", key);
  palPrintf("Spaces = %d\n", spaces);
  palPrintf("Number mod Spaces = %d\n", key % spaces);
  palPrintf("number / spaces = %d\n", key / spaces);
#endif
  return key/spaces;
}

#define kKW_GET 1
#define kKW_HTTP_1_1 2
#define kKW_UpgradeColon 3
#define kKW_WebSocket 4
#define kKW_ConnectionColon 5
#define kKW_Upgrade 6
#define kKW_HostColon 7
#define kKW_OriginColon 8
#define kKW_key1 10
#define kKW_key2 11

static palTokenizerKeyword handshake_keywords[] = {
  {"GET", kKW_GET},
  {"HTTP/1.1", kKW_HTTP_1_1},
  {"Upgrade:", kKW_UpgradeColon},
  {"WebSocket", kKW_WebSocket},
  {"Connection:", kKW_ConnectionColon},
  {"Upgrade", kKW_Upgrade},
  {"Host:", kKW_HostColon},
  {"Origin:", kKW_OriginColon},
  {"Sec-WebSocket-Key1:", kKW_key1},
  {"Sec-WebSocket-Key2:", kKW_key2},
  { NULL, -1 }
};

bool palWebSocketServer::HasCompleteHandshake(const char* s, int s_len) {
  // super simple test for a complete handshake
  const char* first = s;
  if (s_len < 12) {
    return false;
  }

  if (*first != 'G') {
    return false;
  }
  first++;
  if (*first != 'E') {
    return false;
  }
  first++;
  if (*first != 'T') {
    return false;
  }
  const char* final_bytes = &s[s_len-12];
  // two cr+lf pairs
  if (*final_bytes != '\r') {
    return false;
  }
  final_bytes++;
  if (*final_bytes != '\n') {
    return false;
  }
  final_bytes++;
  if (*final_bytes != '\r') {
    return false;
  }
  final_bytes++;
  if (*final_bytes != '\n') {
    return false;
  }
  return true;
}

int palWebSocketServer::ProcessHandshake(const char* s, int s_len) {
  palTokenizer tokenizer;
  if (s_len < 0xb0) {
    // not likely
    return -1;
  }
  tokenizer.UseReadOnlyBuffer(s, s_len);
  tokenizer.SetKeywordArray(&handshake_keywords[0]);
  // don't detect punctuation or numbers
  tokenizer.SetNumberParsing(false);
  tokenizer.SetPunctuationParsing(false);
  tokenizer.SetNameSplitCharacters(" \n\r\t");
  palToken token;

  bool r;

  // parsing GET /resource HTTP/1.1
  r = tokenizer.FetchNextToken(&token);
  if (!r || token.type_flags != kKW_GET) {
    return -2;
  }
  r = tokenizer.FetchNextToken(&token);
  if (!r || token.type != kTokenName) {
    return -3;
  }
  palDynamicString resource_name(token.value_string);
  tokenizer.SkipRestOfLine();

  palDynamicString host_name;
  palDynamicString origin_name;
  uint32_t key1 = 0;
  uint32_t key2 = 0;
  do {
    r = tokenizer.FetchNextToken(&token);
    if (!r || token.type != kTokenKeyword) {
      return -4;
    }
    switch (token.type_flags) {
      case kKW_UpgradeColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_ConnectionColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_HostColon:
        tokenizer.FetchNextToken(&token);
        host_name = token.value_string;
        tokenizer.SkipRestOfLine();
      break;
      case kKW_OriginColon:
        tokenizer.FetchNextToken(&token);
        origin_name = token.value_string;
        tokenizer.SkipRestOfLine();
      break;
      case kKW_key1:
        key1 = ParseKey(&tokenizer);
        if (key1 == 0) {
          return -5;
        }
      break;
      case kKW_key2:
        key2 = ParseKey(&tokenizer);
        if (key2 == 0) {
          return -6;
        }
      break;
    }
  } while (key1 == 0 || key2 == 0);
  
  unsigned char key3[8];

  const char* final_bytes = &s[s_len-12];
  // two cr+lf pairs
  if (*final_bytes != '\r') {
    return -7;
  }
  final_bytes++;
  if (*final_bytes != '\n') {
    return -8;
  }
  final_bytes++;
  if (*final_bytes != '\r') {
    return -9;
  }
  final_bytes++;
  if (*final_bytes != '\n') {
    return -10;
  }
  final_bytes++;
  key3[0] = *final_bytes;
  final_bytes++;
  key3[1] = *final_bytes;
  final_bytes++;
  key3[2] = *final_bytes;
  final_bytes++;
  key3[3] = *final_bytes;
  final_bytes++;
  key3[4] = *final_bytes;
  final_bytes++;
  key3[5] = *final_bytes;
  final_bytes++;
  key3[6] = *final_bytes;
  final_bytes++;
  key3[7] = *final_bytes;
  final_bytes++;

  key1 = palEndianByteSwap(key1);
  key2 = palEndianByteSwap(key2);

  unsigned char challenge[16] = {0x36,0x09,0x65,0x65,0x0A,0xB9,0x67,0x33,0x57,0x6A,0x4E,0x7D,0x7C,0x4D,0x28,0x36};

  unsigned char* key1_c = (unsigned char*)&key1;
  unsigned char* key2_c = (unsigned char*)&key2;
  challenge[0] = key1_c[0];
  challenge[1] = key1_c[1];
  challenge[2] = key1_c[2];
  challenge[3] = key1_c[3];
  challenge[4] = key2_c[0];
  challenge[5] = key2_c[1];
  challenge[6] = key2_c[2];
  challenge[7] = key2_c[3];
  challenge[8] = key3[0];
  challenge[9] = key3[1];
  challenge[10] = key3[2];
  challenge[11] = key3[3];
  challenge[12] = key3[4];
  challenge[13] = key3[5];
  challenge[14] = key3[6];
  challenge[15] = key3[7];

  palMD5 md5;
  md5.Reset();
  md5.Update(&challenge[0], 16);
  md5.Finalize();
  char response[16];
  md5.GetMD5((unsigned char*)&response[0]);

#if 0
  palPrintf("IN : ");
  for (int i = 0; i < 16; i++) {
    int ch = challenge[16-i];
    palPrintf("%02x", ch);
  }
  palPrintf("\n");
  palPrintf("OUT: ");
  for (int i = 0; i < 16; i++) {
    int ch = response[i];
    palPrintf("%02x", ch);
  }
#endif

  palDynamicString server_response;

  server_response.Append("HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
  server_response.Append("Upgrade: WebSocket\r\n");
  server_response.Append("Connection: Upgrade\r\n");
  server_response.Append("Sec-WebSocket-Location: ws://");
  server_response.Append(host_name.C());
  server_response.Append(resource_name.C());
  server_response.Append("\r\n");
  server_response.Append("Sec-WebSocket-Origin: ");
  server_response.Append(origin_name.C());
  server_response.Append("\r\n");
  server_response.Append("\r\n");
  server_response.Append(&response[0], 16);

  //printf("Client:\n%s\n", s);
  //printf("Reply:\n%s\n", server_response.C());
  client_.Send((unsigned char*)server_response.C(), server_response.GetLength());
  return 0;
}
