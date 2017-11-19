#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <iostream>

#define MY_PORT 97
#define BUFF_SIZE 1024

struct ThreadData
{
	SOCKET socket;
	sockaddr_in addr;
};

DWORD WINAPI ClientTHread(void* threadData);

int main()
{
	WSADATA ws;

	printf("HTTP SERVER DEMO\n\n");

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

		printf("\n\nNEW CLIENT CONNECTED!\n\n");

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
	//Statistic statistic;

	//wcscpy_s(statistic.InternetProtocol, (wchar_t*)inet_ntoa(data.addr.sin_addr));
	//wcscpy_s(statistic.ConnectTime, L"TIMETOSTRFROMSTRCT");

	std::string request;
	SOCKET client;
	client = data->socket;

	char *buff = new char[BUFF_SIZE];
	char host[30];
	u_short port = 80;
	char ip[16];
	char answer[300];

	memset(buff, '\0', 300);
	memset(host, '\0', 30);
	memset(ip, '\0', 10);
	memset(answer, '\0', 300);

	while (request.find("\r\n\r\n") == std::string::npos)
	{
		memset(buff, '\0', BUFF_SIZE);

		int lenght;

		if ((lenght = recv(client, buff, BUFF_SIZE, 0)) < 0)
		{
			printf("\n\nCLIENT DISCONNECTED!\n\n");
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

		//	statistic.CountReceiveDate += sizeof(buff);	
	}

	printf("\nRECIEVE:\n%s\n\n", request.c_str());

	int totalsent = 0, senteach;
	int buffSize = request.length();
	buff = new char[buffSize];
	memset(buff, 0, buffSize);
	strcpy(buff, request.c_str());

	while (totalsent < buffSize)
	{
		if ((senteach = send(client, buff + totalsent, buffSize - totalsent, 0)) < 0)
		{
			return -1;
		}

		totalsent += senteach;
	}

	closesocket(client);

	return 0;
}