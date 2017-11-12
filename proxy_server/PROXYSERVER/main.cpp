#include "stdafx.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "SendClientRequest.h"
#include "GetClientRequest.h"
#include "Statistic.h"

#define MY_PORT 1380
#define sHELLO "Hello, I'm demo TCP-echo SERVER\r\n"

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

DWORD WINAPI ClientTHread(LPVOID client_socket);

int nclients = 0;

int main()
{
	WSADATA ws;

	printf("TCP SERVER DEMO\n\n");

	if (WSAStartup(0x0202, &ws))
	{
		printf("Error WSAStartup %d\n", WSAGetLastError());

		return -1;
	}

	SOCKET listen_sock;

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
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

	printf("Waiting for new connections!\n\n");

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
		CreateThread(NULL, NULL, ClientTHread, &data, NULL, &thID);
	}

	return 0;
}

DWORD WINAPI ClientTHread(ThreadData data)
{
	//Statistic statistic;
	
	//wcscpy_s(statistic.InternetProtocol, (wchar_t*)inet_ntoa(data.addr.sin_addr));
	//wcscpy_s(statistic.ConnectTime, L"TIMETOSTRFROMSTRCT");

	std::string request;
	SOCKET client;
	client = data.socket;

	char buff[300];
	char host[30];
	u_short port = 80;
	char ip[16];
	char answer[300];

	memset(buff, '\0', 300);
	memset(host, '\0', 30);
	memset(ip, '\0', 10);
	memset(answer, '\0', 300);

	send(client, sHELLO, sizeof(sHELLO), 0);

	int lenght;
	do
	{
		memset(buff, '\0', 300);
		memset(host, '\0', 30);
		memset(ip, '\0', 10);
		memset(answer, '\0', 300);

		if (lenght = recv(client, (char *)&buff, 300, 0) < 0)
		{
			return -1;
		}

	//	statistic.CountReceiveDate += sizeof(buff);

		if (getCharUrl(ParseClientRequest(buff), host, port, ip) < 0)
		{
			return -1;
		}

		if (strlen(host) == 0)
			continue;

		printf("IP: %s, PORT: %d\n", ip, port);

		SOCKET serverSock;
		serverSock = socket(AF_INET, SOCK_STREAM, 0);

		if (serverSock == -1)
		{
			printf("Error socket %d\n", WSAGetLastError());
			WSACleanup();

			return -1;
		}

		//SendClientRequest(serverSock, buff, host, port, answer);
		send(client,(char*) &buff, 300, 0);

		//statistic.CountSendDate += sizeof(buff);

		//PutInDatabase(&statistic, 1);
	} while (lenght >= 0);

	nclients--;
	printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
	closesocket(client);

	return 0;
}