#include "ProxyServer.h"
#include <iostream>
#include <winsock2.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA ws;

	printf("TCP SERVER DEMO\n\n");

	if (WSAStartup(0x0202, &ws))
	{
		printf("Error WSAStartup %d\n", WSAGetLastError());
		return -1;
	}

	ProxyServer* httpProxy = new ProxyServer;

	if (httpProxy->turnOnLine())
	{
		printf("Error starting HTTPProxy\n");
		return -1;
	}

	return 0;
}
