#include "stdafx.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "SendClientRequest.h"
#include "GetClientRequest.h"
#include "Statistic.hpp"

#define MY_PORT 1380
#define sHELLO "Hello, I'm demo TCP-echo SERVER\r\n"
#define BUFF_SIZE 1024

std::string request;

struct ThreadData
{
	SOCKET socket;
	sockaddr_in addr;
};

void print_users(int nclients)
{
	if (nclients)
		printf("%d users on-line\n", nclients);
	else 
		printf("0 users on-line\n");
}

DWORD WINAPI ClientTHread(void* threadData);

int nclients = 0;

int main()
{
	WSADATA ws;

	printf("PROXY SERVER DEMO\n\n");

	if (WSAStartup(0x0202, &ws))
	{
		printf("Error WSAStartup %d\n", WSAGetLastError());

		return -1;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_sock == -1)
	{
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup();

		return -1;
	}

	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)))
	{
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(listen_sock);
		WSACleanup();

		return -1;
	}

	if (listen(listen_sock, 5))
	{
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(listen_sock);
		WSACleanup();

		return -1;
	}

	SOCKET client_socket;
	sockaddr_in client_addr;

	int client_addr_size = sizeof(client_addr);

	while (1)
	{
		client_socket = accept(listen_sock, (sockaddr *)&client_addr, &client_addr_size);
		nclients++;
		printf("\n\nNEW CLIENT CONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);

		ThreadData data;
		data.socket = client_socket;
		data.addr = client_addr;

		DWORD thID;
		CreateThread(NULL, NULL, ClientTHread, (void*)&data, NULL, &thID);
	}

	return 0;
}

DWORD WINAPI ClientTHread(void* threadData)
{
	ThreadData *data = (ThreadData*)threadData;
	Statistic *statistic = new Statistic[1];
	
	strcpy(statistic[0].InternetProtocol, inet_ntoa(data->addr.sin_addr));
	strcpy(statistic[0].ConnectTime, "TIMETOSTRFROMSTRCT");
		
	SOCKET client;
	client = data->socket;

	char *buff = new char[BUFF_SIZE];
	char host[30];
	u_short port = 80;
	char ip[16];

	memset(buff, '\0', 300);
	memset(host, '\0', 30);
	memset(ip, '\0', 10);

	while (1)
	{
		std::string request;
		memset(buff, '\0', BUFF_SIZE);

		while (request.find("\r\n\r\n") == std::string::npos)
		{		
			int lenght;

			if ((lenght = recv(client, buff, BUFF_SIZE, 0)) < 0)
			{
				PutInDatabase(statistic, 1);
				nclients--;
				printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
				closesocket(client);
				return -1;
			}
			else
			{
				if (lenght == 0)
				{
					break;
				}
				else
				{
					if (strlen(buff) == 0)
						continue;

					request.append(buff);
				}
			}

			statistic[0].CountReceiveDate += request.length();
		}

		if (request.length() != 0)
		{
			printf("\nRequest:\n%s\n", request.c_str());

			if (getCharUrl(ParseClientRequest(request), host, port, ip) < 0)
			{
				nclients--;
				printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
				closesocket(client);
				return -1;
			}

			printf("IP: %s, PORT: %d\n", ip, port);

			SOCKET serverSock;
			serverSock = socket(AF_INET, SOCK_STREAM, 0);

			if (serverSock == -1)
			{
				printf("Error socket %d\n", WSAGetLastError());
				WSACleanup();

				return -1;
			}

			SendClientRequest(serverSock, client, request, ip, port, statistic[0]);
		}
	}
}