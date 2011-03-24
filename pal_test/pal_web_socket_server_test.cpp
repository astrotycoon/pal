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
    printf("Could not start websocket server: %d\n", r);
  }

  int threshold = 2;
  while (true) {
    server.Update();
    if (server.HasOpen()) {
      printf("Connection was opened\n");
    }
    if (server.HasClose()) {
      printf("Connection was closed\n");
    }
    if (server.HasError()) {
      printf("Connection had error\n");
    }
    if (server.PendingMessageCount() > threshold) {
      threshold = 0;
      MsgCounter counter;
      server.ProcessMessages(palWebSocketServer::OnMessageDelegate(&counter, &MsgCounter::CountMessages));
      printf("Received %d messages\n", counter.counter);
      server.ProcessMessages(PrintMessages);
      server.ClearMessages(); // clear messages
      server.SendMessage("MSG");
    }
  }

  printf("Finished web socket test.\n");

  return true;
}