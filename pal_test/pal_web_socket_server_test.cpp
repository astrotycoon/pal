#include "libpal/libpal.h"
#include "pal_web_socket_server_test.h"

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

  while (true) {
    r = server.Update();
    if (r < 0) {
      printf("An error occured: %d\n", r);
      break;
    }
  }

  printf("Finished web socket test.\n");

  return true;
}