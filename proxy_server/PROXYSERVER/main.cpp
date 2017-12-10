#include "stdafx.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "SendClientRequest.h"
#include "ParseClientRequest.h"
#include "Statistic.hpp"

#define MY_PORT 1380
#define BUFF_SIZE 1024

DWORD WINAPI ClientThread(void* threadData);

struct ThreadData
{
	SOCKET socket;
	sockaddr_in addr;
};

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
	
	//	создание слушающего сокета

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

	//	цикл принятия и обработки сообщений клиентов и отправки им ответов

	while (true)
	{
		Statistic statistic;
		std::string ip_port;
		ip_port = inet_ntoa(data->addr.sin_addr);
		ip_port += ":";
		ip_port += std::to_string(data->addr.sin_port);
		strcpy(statistic.InternetProtocol, ip_port.c_str());
		std::string time = GetCurrTime();
		strcpy(statistic.ConnectTime, time.c_str());

		std::string request;
		memset(buff, '\0', BUFF_SIZE);

		// пока не найден символ конца запроса, продолжаем принимать данные от клиента

		while (request.find("\r\n\r\n") == std::string::npos)
		{		
			int lenght;

			if ((lenght = recv(client, buff, BUFF_SIZE, 0)) < 0)
			{
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

			// добавление данных статистики в базу

			statistic.CountSendDate += request.length();
		}

		// обработка запроса

		if (request.length() != 0)
		{
			printf("\nRequest:\n%s\n", request.c_str());

			//функция разбора запроса

			std::string pResult = ParseClientRequest(request);

			// запрос на представление данных о всех пользователях из базы

			if (pResult == "GETALLSTAT")
			{
				std::string dataFromBase = ReadStringFromDatabase();
				char buffClients[20];
				memset(buffClients, 0, 20);
				sprintf(buffClients, "\*%d\r\n\r\n", nclients);
				dataFromBase += buffClients;
				
				send(client, dataFromBase.c_str(), dataFromBase.length(), 0);

				continue;
			}

			//	запрос на представление данных о конкретном пользователе из базы

			if (pResult.find("GETPERSONAL") != std::string::npos)
			{
				int begin = pResult.find("GETPERSONAL");
				std::string stat_ip = pResult.substr(begin + 11, pResult.length());
				std::string dataFromBase = GetPersonalStatistic(stat_ip.c_str());

				char buffClients[20];
				memset(buffClients, 0, 20);
				sprintf(buffClients, "\*%d\r\n\r\n", nclients);
				dataFromBase += buffClients;

				send(client, dataFromBase.c_str(), dataFromBase.length(), 0);

				continue;
			}

			//	если это не запрос статистики, то выделяем IP внешнего сервера и его порт

			if (getCharUrl(pResult, port, ip) < 0)
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

			//	создание сокета для обращения к внешнему серверу, получение от него ответа и пересылка данных клиенту

			SendClientRequest(serverSock, client, request, ip, port, statistic);

			//	запись статистики в базу данных
			PutInDatabase(statistic);
		}
		else
		{
			nclients--;
			printf("\n\nCLIENT DISCONNECTED!\nCOUNT OF CLIENTS: %d\n\n", nclients);
			closesocket(client);
			return -1;
		}
	}
}