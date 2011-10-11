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

#include "libpal/pal_platform.h"
#include "libpal/pal_debug.h"
#if defined(PAL_PLATFORM_WINDOWS)
#include <winsock2.h>
#else
#endif
#include <stdio.h>
#include "libpal/pal_socket.h"

static int TranslateSocketError(int error) {
  switch (error) {
  case WSAEWOULDBLOCK:
    return PAL_SOCKET_ERROR_WOULDBLOCK;
  break;
  case WSAECONNABORTED:
    return PAL_SOCKET_ERROR_CONNECTION_ABORTED;
  break;
  default:
    palPrintf("Untranslated socket error: %d\n", error);
  break;
  }
  return error;
}

void palSocketPrintErrorNo() {
  int error = WSAGetLastError();
  palPrintf("Socket error: %d (0x%08x)\n", error, error);
}

void palSocketInit() {
#if defined(PAL_PLATFORM_WINDOWS)
  // low word = major, highword = minor
  WSADATA wsaData = {0};
  WORD wVer = MAKEWORD(2,2);

  int nRet = WSAStartup( wVer, &wsaData );
  if (nRet < 0) {
    palSocketPrintErrorNo();
  } else {
    //printf("Winsock 2.2 initialized\n");
  }
#endif
}

void palSocketFini() {
#if defined(PAL_PLATFORM_WINDOWS)
  WSACleanup();
#endif
}

bool palSocketIsValid(palSocket socket) {
  return socket != INVALID_SOCKET;
}

int palSocketGetErrorNumber() {
  return TranslateSocketError(WSAGetLastError());
}

int palSocketGetLocalName(palSocket socket, palIpv4Address* local_ip, palIPPort* local_port) {
  sockaddr_in saLocal;
  int size = sizeof(saLocal);

  int r = getsockname(socket, (sockaddr*)&saLocal, &size);

  *local_ip = saLocal.sin_addr.S_un.S_addr;
  *local_port = saLocal.sin_port;

  return r;
}

int palSocketGetRemoteName(palSocket socket, palIpv4Address* remote_ip, palIPPort* remote_port) {
  sockaddr_in saRemote;
  int size = sizeof(saRemote);

  int r = getpeername(socket, (sockaddr*)&saRemote, &size);

  *remote_ip = saRemote.sin_addr.S_un.S_addr;
  *remote_port = saRemote.sin_port;

  return r;
}

palSocket palSocketOpenStream() {
  palSocket s;
  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s == kpalSocketInitializer) {
    palSocketPrintErrorNo();
  }
  return s;
}

int palSocketAvailableBytes(palSocket socket) {
  int available_bytes;
  int r = ioctlsocket(socket, FIONREAD, (u_long*)&available_bytes);
  if (r < 0) {
    palSocketPrintErrorNo();
  }
  if (r <= 0) {
    return r;
  }
  return available_bytes;
}

int palSocketControlBlocking(palSocket socket, bool blocking) {
  int non_blocking = blocking == false;
  int r;
  r = ioctlsocket(socket, FIONBIO, (u_long*)&non_blocking);
  if (r < 0) {
    palSocketPrintErrorNo();
  }
  return r;
}

int palSocketBind(palSocket socket, palIpv4Address local_ip, palIPPort local_port) {
  int r;

  sockaddr_in saBind;
  saBind.sin_family = AF_INET;
  saBind.sin_port = htons(local_port);
  saBind.sin_addr.S_un.S_addr = htonl(local_ip);

  r = bind(socket, (sockaddr*)&saBind, sizeof(saBind));
  if (r < 0) {
    palSocketPrintErrorNo();
  }
  return r;
}

int palSocketListen(palSocket socket, int backlog) {
  return listen(socket, backlog);
}

palSocket palSocketAccept(palSocket listener_socket) {
  palSocket client_socket;

  sockaddr_in saAccept;
  int saAcceptLen = sizeof(saAccept);
  client_socket = accept(listener_socket, (sockaddr*)&saAccept, &saAcceptLen);

  if (client_socket == kpalSocketInitializer) {
    palSocketPrintErrorNo();
  }
  return client_socket;
}

int palSocketConnect(palSocket socket, palIpv4Address remote_ip, palIPPort remote_port) {
  sockaddr_in saConnect;

  saConnect.sin_family = AF_INET;
  saConnect.sin_addr.S_un.S_addr = htonl(remote_ip);
  saConnect.sin_port = htons(remote_port);

  int r = connect(socket, (sockaddr*)&saConnect, sizeof(saConnect));

  if (r < 0) {
    palSocketPrintErrorNo();
  }
  return r;
}

int palSocketSend(palSocket socket, const unsigned char* buf, int* bytes_to_send) {
  int r;

  r = send(socket, (char*)buf, *bytes_to_send, 0);
  if (r < 0) {
    palPrintf("socket=%d . bytes_to_send=%d\n", socket, *bytes_to_send);
    palSocketPrintErrorNo();
    return r;
  }
  if (r > 0) {
    *bytes_to_send = r;
    return 0;
  } else {
    *bytes_to_send = 0;
    return r;
  }
}

int palSocketReceive(palSocket socket, unsigned char* buf, int* bytes_to_read) {
  int r;

  r = recv(socket, (char*)buf, *bytes_to_read, 0);
  if (r > 0) {
    *bytes_to_read = r;
    return 0;
  } else if (r < 0) {
    *bytes_to_read = 0;
    palSocketPrintErrorNo();
    return r;
  } else {
    *bytes_to_read = 0;
    return r;
  }
}

int palSocketClose(palSocket socket) {
  if (socket != kpalSocketInitializer) {
    shutdown(socket, SD_BOTH);
    return closesocket(socket);
  }
  return 0;
}

void palSocketSet::Clear() {
  FD_ZERO(&set_);
}

void palSocketSet::Add(palSocket socket) {
  FD_SET(socket, &set_);
}

void palSocketSet::Remove(palSocket socket) {
  FD_CLR(socket, &set_);
}

int palSocketSet::IsMember(palSocket socket) const {
  return FD_ISSET(socket, &set_);
}

int palSocketSetSelectImmediate(palSocket max_socket, palSocketSet* can_read, palSocketSet* can_write, palSocketSet* has_error) {
  timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  fd_set* readable = can_read ? &can_read->set_ : NULL;
  fd_set* writable = can_write ? &can_write->set_ : NULL;
  fd_set* errorable = has_error ? &has_error->set_ : NULL;
  int r = select(max_socket, readable, writable, errorable, &tv); 
  if (r < 0) {
    palSocketPrintErrorNo();
  }
  return r;
}


int palSocketSetSelectBlock(palSocket max_socket, palSocketSet* can_read, palSocketSet* can_write, palSocketSet* has_error) {
  fd_set* readable = can_read ? &can_read->set_ : NULL;
  fd_set* writable = can_write ? &can_write->set_ : NULL;
  fd_set* errorable = has_error ? &has_error->set_ : NULL;
  int r = select(max_socket, readable, writable, errorable, NULL); 
  if (r < 0) {
    palSocketPrintErrorNo();
  }
  return r;
}
