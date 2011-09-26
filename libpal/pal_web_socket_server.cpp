
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
#include "libpal/pal_memory_stream.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_string.h"
#include "libpal/pal_endian.h"
#include "libpal/pal_sha1.h"
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



static int ParseKey(palTokenizer* tokenizer, unsigned char* key) {  
  palDynamicString key_as_string;

  bool r = tokenizer->ReadRestOfLineAsString(&key_as_string);
  if (r == false) {
    return -1;
  }

  //key_as_string.Set("dGhlIHNhbXBsZSBub25jZQ==");
  key_as_string.Append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

  key_as_string.TrimWhiteSpaceFromStart();

  palPrintf("Key: **%s**\n", key_as_string.C());

  {
    palSHA1 sha1;
    sha1.Reset();
    sha1.Update((unsigned char*)key_as_string.C(), key_as_string.GetLength());
    sha1.Finalize();
    sha1.GetSHA1(key);
  }

  palPrintf("Dumping SHA-1: ");
  for (int i = 0; i < 20; i++) {
    palPrintf("%02x ", key[i]);
  }
  palPrintf("\n");

  return 0;
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
#define kKW_key 12
#define kKW_WebSocketOriginColon 13
#define kKW_WebSocketProtocolColon 14
#define kKW_WebSocketVersionColon 15

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
  {"Sec-WebSocket-Key:", kKW_key},
  {"Sec-WebSocket-Origin:", kKW_WebSocketOriginColon },
  {"Sec-WebSocket-Protocol:", kKW_WebSocketProtocolColon },
  {"Sec-WebSocket-Version:", kKW_WebSocketVersionColon },
  { NULL, -1 }
};

bool palWebSocketServer::HasCompleteHandshake(const char* s, int s_len) {
  // super simple test for a complete handshake
  const char* first = s;
  palPrintf("Checking handshake: %s\n", s);
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

  const char* final_bytes = &s[s_len-4];
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

#if 0
static char base64_encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                      'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                      'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                      'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                      '4', '5', '6', '7', '8', '9', '+', '/'};
static int base64_mod_table[] = {0, 2, 1};

int base64_encode(const char *data, size_t input_length, char* output, size_t *output_length) {
  palAssert(input_length == 20);
  *output_length = 28;
  //*output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));

  char* encoded_data = output;

  for (unsigned int i = 0, j = 0; i < input_length;) {
    uint32_t octet_a = i < input_length ? data[i++] : 0;
    uint32_t octet_b = i < input_length ? data[i++] : 0;
    uint32_t octet_c = i < input_length ? data[i++] : 0;

    uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    encoded_data[j++] = base64_encoding_table[(triple >> 3 * 6) & 0x3F];
    encoded_data[j++] = base64_encoding_table[(triple >> 2 * 6) & 0x3F];
    encoded_data[j++] = base64_encoding_table[(triple >> 1 * 6) & 0x3F];
    encoded_data[j++] = base64_encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (int i = 0; i < base64_mod_table[input_length % 3]; i++) {
    encoded_data[*output_length - 1 - i] = '=';
  }

  return 0;
}

#endif

static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

int base64_encode(const char* data, size_t input_length, char* target, size_t* output_length) {
  size_t datalength = 0;
  u_char input[3];
  u_char output[4];
  size_t i;
  size_t srclength = input_length;
  palAssert(input_length == 20);
  *output_length = 28;
  const char* src = data;

  while (2 < srclength) {
    input[0] = *src++;
    input[1] = *src++;
    input[2] = *src++;
    srclength -= 3;

    output[0] = input[0] >> 2;
    output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
    output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
    output[3] = input[2] & 0x3f;
    palAssert(output[0] < 64);
    palAssert(output[1] < 64);
    palAssert(output[2] < 64);
    palAssert(output[3] < 64);

    if (datalength + 4 > *output_length)
      return (-1);

    target[datalength++] = Base64[output[0]];
    target[datalength++] = Base64[output[1]];
    target[datalength++] = Base64[output[2]];
    target[datalength++] = Base64[output[3]];
  }

  /* Now we worry about padding. */
  if (0 != srclength) {
    /* Get what's left. */
    input[0] = input[1] = input[2] = '\0';
    for (i = 0; i < srclength; i++)
      input[i] = *src++;

    output[0] = input[0] >> 2;
    output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
    output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
    palAssert(output[0] < 64);
    palAssert(output[1] < 64);
    palAssert(output[2] < 64);

    if (datalength + 4 > *output_length)
      return (-1);
    target[datalength++] = Base64[output[0]];
    target[datalength++] = Base64[output[1]];
    if (srclength == 1)
      target[datalength++] = Pad64;
    else
      target[datalength++] = Base64[output[2]];
    target[datalength++] = Pad64;
  }
  return 0;
}


int palWebSocketServer::ProcessHandshake(const char* s, int s_len) {
  palMemBlob blob((void*)s, (uint64_t)s_len);
  palMemoryStream ms;
  int res;

  palPrintf("Process Handshake %s\n", s);

  res = ms.Create(blob, false);
  if (res != 0) {
    return -2;
  }
  palTokenizer tokenizer;
  if (s_len < 0xb0) {
    // not likely
    return -1;
  }
  res = tokenizer.UseStream(&ms);
  if (res != 0) {
    return -3;
  }
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
  unsigned char key[20];
  bool key_processed = false;
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
      case kKW_key:
        ParseKey(&tokenizer, &key[0]);
        key_processed = true;
      break;
      case kKW_WebSocketOriginColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_WebSocketProtocolColon:
        tokenizer.SkipRestOfLine();
      break;
      case kKW_WebSocketVersionColon:
        tokenizer.SkipRestOfLine();
      break;
    }
  } while (key_processed == false);
  

  char accept_key[29];
  size_t accept_key_length;
  base64_encode((const char*)&key[0], 20, &accept_key[0], &accept_key_length);
  accept_key[28] = '\0';
  palDynamicString server_response;
  server_response.Append("HTTP/1.1 101 Switching Protocols\r\n");
  server_response.Append("Upgrade: websocket\r\n");
  server_response.Append("Connection: Upgrade\r\n");
  server_response.AppendPrintf("Sec-WebSocket-Accept: %s\r\n", &accept_key[0]);
  //server_response.Append("Sec-WebSocket-Protocol: \r\n");
#if 0
  server_response.Append("Sec-WebSocket-Location: ws://");
  server_response.Append(host_name.C());
  server_response.Append(resource_name.C());
  server_response.Append("\r\n");

  server_response.Append("Sec-WebSocket-Origin: ");
  server_response.Append(origin_name.C());
#endif
  server_response.Append("\r\n");
  server_response.Append("\r\n");
  
  palPrintf("Response:\n%s\n", server_response.C());
  client_.Send((unsigned char*)server_response.C(), server_response.GetLength());
  return 0;
}
