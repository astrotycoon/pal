#include "libpal/libpal.h"
#include "pal_web_socket_server_test.h"

void PrintMessages(palMemBlob msg, bool binary_message) {
  palPrintf("Message handler\n");
  if (binary_message == false) {
    uint32_t len = (uint32_t)msg.GetBufferSize();
    palPrintf("%.*s\n", len, msg.GetPtr<const char>());
  } else {
    palPrintf("Got a binary message with %lld bytes\n", msg.GetBufferSize());
  }
}

struct MsgCounter {
  int counter;
  MsgCounter() : counter(0) {}
  void CountMessages(palMemBlob msg, bool binary_message) {
    counter++;
  }
};

bool palWebSocketServerTest() {
#define BUFFER_SIZE 10*1024
  unsigned char* incoming_buffer = new unsigned char[BUFFER_SIZE];
  unsigned char* outgoing_buffer = new unsigned char[BUFFER_SIZE];
  unsigned char* message_buffer = new unsigned char[BUFFER_SIZE];
  palWebSocketServer server;

  server.SetIncomingBuffer(incoming_buffer, BUFFER_SIZE);
  server.SetOutgoingBuffer(outgoing_buffer, BUFFER_SIZE);
  server.SetMessageBuffer(message_buffer, BUFFER_SIZE);

  int r;
  
  r = server.Startup(8080);
  if (r != 0) {
    palPrintf("Could not start websocket server: %d\n", r);
    return false;
  }

  palPrintf("Started server on port: %d\n", 8080);
  int threshold = 2;
  while (true) {
    server.Update();
    if (server.HasOpen()) {
      palPrintf("Connection was opened\n");
    }
    if (server.HasClose()) {
      palPrintf("Connection was closed\n");
    }
    if (server.HasError()) {
      palPrintf("Connection had error\n");
    }

    if (server.Connected() == true) {
      //server.SendPing();
    }
    
    if (server.PendingMessageCount() > threshold) {
      threshold = 0;
      MsgCounter counter;
      server.ProcessMessages(palWebSocketServer::OnMessageDelegate(&counter, &MsgCounter::CountMessages));
      palPrintf("Received %d messages\n", counter.counter);
      server.ProcessMessages(PrintMessages);
      server.ClearMessages(); // clear messages
      server.SendMessage("MSG0");
#if 0
      server.SendMessage("MSG1");
      server.SendMessage("MSG2");
      server.SendMessage("MSG3");
      server.SendMessage("MSG4");
      server.SendMessage("MSG5");
      server.SendMessage("MSG6");
      server.SendMessage("MSG7");
      server.SendMessage("MSG8");
      server.SendMessage("MSG9");
      server.SendMessage("MSG10");
#endif
    }
  }

  palPrintf("Finished web socket test.\n");

  return true;
}