#include "stdafx.h"

#include "ProxyServer.h"
#include "Statistic.hpp"

DWORD WINAPI ClientThread(void* threadData);

int nclients = 0;

int start_server()
{
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

	//	"привязка сокета" к адресу и порту

	if (bind(listen_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)))
	{
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(listen_sock);
		WSACleanup();

		return -1;
	}

	//	устанавливаем сокет в режим прослушивания

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

	while (true)
	{
		//	создание сокета для каждого подключившегося клиента

		client_socket = accept(listen_sock, (sockaddr *)&client_addr, &client_addr_size);
		nclients++;
		printf("\n\nNEW CLIENT CONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);

		ThreadData data;
		data.socket = client_socket;
		data.addr = client_addr;

		//	создание отдельного потока для обработки запросов каждого клиента

		DWORD thID;
		CreateThread(NULL, NULL, ClientThread, (void*)&data, NULL, &thID);
	}
}

DWORD WINAPI ClientThread(void* threadData)
{
	ThreadData *data = (ThreadData*)threadData;
	SOCKET client;
	client = data->socket;

	char *buff = new char[BUFF_SIZE];
	u_short port = 80;
	char ip[16];

	memset(buff, '\0', 300);
	memset(ip, '\0', 10);

	while (true)
	{
		std::string request;
		Statistic statistic;

		if (GetClientRequest(client, data, request, nclients, statistic) < 0)
		{
			nclients--;
			printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
			closesocket(client);
			return -1;
		}
		
		int parse_result = ParseClientRequest(client, request, ip, port, nclients);

		if (parse_result < 0)
		{
			nclients--;
			printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
			closesocket(client);
			return -1;
		}

		if (parse_result == 1)
			continue;

		SOCKET serverSock;
		serverSock = socket(AF_INET, SOCK_STREAM, 0);

		if (serverSock == -1)
		{
			printf("Error socket %d\n", WSAGetLastError());
			WSACleanup();

			return -1;
		}		

		if (SendClientRequest(serverSock, request, ip, port) < 0)
		{
			nclients--;
			printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
			closesocket(client);
			closesocket(serverSock);

			return -1;
		}

		std::string answer;

		if (GetServerAnswer(serverSock, answer, statistic) < 0)
		{
			nclients--;
			printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
			closesocket(client);
			closesocket(serverSock);

			return -1;
		}

		if (SendServerAnswer(client, answer) < 0)
		{
			nclients--;
			printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
			closesocket(client);
			closesocket(serverSock);

			return -1;
		}

		PutInDatabase(statistic);		

		closesocket(serverSock);
	}
}