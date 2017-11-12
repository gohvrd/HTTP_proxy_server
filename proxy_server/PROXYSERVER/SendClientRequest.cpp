#include "stdafx.h"
#include <winsock2.h>

int SendClientRequest(SOCKET server_sock, char *buff, char *ip, u_short port, char* answer)
{
	sockaddr_in adr;

	adr.sin_family = AF_INET;
	adr.sin_addr.S_un.S_addr = inet_addr(ip);
	adr.sin_port = htons(port);

	if (SOCKET_ERROR == connect(server_sock, (sockaddr*)&adr, sizeof (adr)))
	{
		return -1;
	}

	if (SOCKET_ERROR == send(server_sock, (char*)&buff, 300, 0))
	{
		return -1;
	}

	int len;
	if (SOCKET_ERROR == (len = recv(server_sock, (char *)&answer, 300, 0)))
		return -1;

	//printf("\nAnswer:\n\n%s\n", buff);

	if (SOCKET_ERROR == closesocket(server_sock))
	{
		return -1;
	}

	return 1;
}
