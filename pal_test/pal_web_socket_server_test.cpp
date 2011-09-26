#include "libpal/libpal.h"
#include "pal_web_socket_server_test.h"

void PrintMessages(const char* msg, int msg_len) {
  palPrintf("%.*s\n", msg_len, msg);
}

struct MsgCounter {
  int counter;
  MsgCounter() : counter(0) {}
  void CountMessages(const char* msg, int msg_len) {
    counter++;
  }
};

bool palWebSocketServerTest() {
#define BUFFER_SIZE 10*1024
  unsigned char* incoming_buffer = new unsigned char[BUFFER_SIZE];
  unsigned char* outgoing_buffer = new unsigned char[BUFFER_SIZE];

  palWebSocketServer server(NULL, 0, NULL, 0);

  server.SetIncomingBuffer(incoming_buffer, BUFFER_SIZE);
  server.SetOutgoingBuffer(outgoing_buffer, BUFFER_SIZE);

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
    
    if (server.PendingMessageCount() > threshold) {
      threshold = 0;
      MsgCounter counter;
      server.ProcessMessages(palWebSocketServer::OnMessageDelegate(&counter, &MsgCounter::CountMessages));
      palPrintf("Received %d messages\n", counter.counter);
      server.ProcessMessages(PrintMessages);
      server.ClearMessages(); // clear messages
      server.SendMessage("MSG0");
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
    }
  }

  palPrintf("Finished web socket test.\n");

  return true;
}