#pragma once
#include <winsock2.h>

int SendClientRequest(SOCKET server_sock, char *buff, char *host, u_short port, char* answer);