#include "stdafx.h"

#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define PORT 1380
#define SERVERADDR "127.0.0.1"

int main()
{
	WSADATA ws;
	printf("PROXY DEMO CLIENT\n\n");

	if (WSAStartup(0x202, &ws))
	{
		printf("WSAStart error %d\n", WSAGetLastError());

		return -1;
	}

	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock == -1)
	{
		printf("Socket() error %d\n", WSAGetLastError());

		return -1;
	}

	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else

	if (hst = gethostbyname(SERVERADDR))
		// hst->h_addr_list содержит не массив адресов, а массив указателей на адреса
		((unsigned long *)&dest_addr.sin_addr)[0] = ((unsigned long **)hst->h_addr_list)[0][0];
	else
	{
		printf("Invalid address %s\n", SERVERADDR);
		closesocket(my_sock);
		WSACleanup();

		return -1;
	}

	if (connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());

		return -1;
	}

	printf("Connection to the %s was successfully installed!\nType \"quit\" for disconnect\n\n", SERVERADDR);


	/*while (recv(my_sock, &buff[0], sizeof(buff), 0) > 0)
	{
	printf("\nS=>C:%s", buff);

	printf("\nS<=C:");

	fgets(&buff[0], sizeof(buff), stdin);

	if (!strcmp(&buff[0], "quit\n"))
	{
	printf("\nDisconnected - OK!\n");
	closesocket(my_sock);
	WSACleanup();

	return 0;
	}

	send(my_sock, &buff[0], sizeof(buff), 0);

	memset(buff, '\0', sizeof(buff));

	}*/

	

	while (1)
	{
		char* buff = new char[1040];
		memset(buff, 0, 1040);

		printf("\nS<=C:");

		fgets(buff, 1040, stdin);

		if (!strcmp(buff, "quit\n"))
		{
			printf("\nDisconnected - OK!\n");
			closesocket(my_sock);
			WSACleanup();

			return 0;
		}

		for (int i = 0; i < strlen(buff); ++i)
		{
			if (buff[i] == '\n')
			{
				buff[i] = '\r';
				buff[i+1] = '\n';
				buff[i+2] = '\r';
				buff[i+3] = '\n';
				break;
			}
		}

		send(my_sock, buff, 1040, 0);
	}


	printf("Receiving error %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();

	return 0;
}