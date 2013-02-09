#ifndef EXCDR_GLOBAL_H
#define EXCDR_GLOBAL_H

#ifdef _WIN32
  #include <windows.h>
  #include <direct.h>
#else
  #include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <ctime>

using namespace std;

#include <SimplyFlat.h>
#include "Helpers.h"

#ifndef VK_RETURN
#define VK_RETURN  0x0D
#define VK_SPACE   0x20
#define VK_CONTROL 0x11
#define VK_SHIFT   0x10
#endif

#ifdef _WIN32
 #include <winsock.h>

 #define SOCK SOCKET
 #define ADDRLEN int

 #define SOCKETWOULDBLOCK WSAEWOULDBLOCK
 #define SOCKETCONNRESET  WSAECONNRESET
 #define LASTERROR() WSAGetLastError()
#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 #include <fcntl.h>

 #define SOCK int
 #define ADDRLEN socklen_t

 #define INVALID_SOCKET -1

 #define SOCKETWOULDBLOCK EAGAIN
// Is this right? TODO: find proper value for linux
 #define SOCKETCONNRESET  10054L
 #define LASTERROR() errno
#endif

#endif
