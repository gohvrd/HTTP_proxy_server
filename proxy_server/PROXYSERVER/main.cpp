#include "stdafx.h"
#include "ProxyServer.h"

int main()
{
	WSADATA ws;

	printf("PROXY SERVER DEMO\n\n");

	if (WSAStartup(0x0202, &ws))
	{
		printf("Error WSAStartup %d\n", WSAGetLastError());

		return -1;
	}

	start_server();
}
<<<<<<< HEAD
=======

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

	//	���� �������� � ��������� ��������� �������� � �������� �� �������

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

		// ���� �� ������ ������ ����� �������, ���������� ��������� ������ �� �������

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

			// ���������� ������ ���������� � ����

			statistic.CountSendDate += request.length();
		}

		// ��������� �������

		if (request.length() != 0)
		{
			printf("\nRequest:\n%s\n", request.c_str());

			//������� ������� �������

			std::string pResult = ParseClientRequest(request);

			// ������ �� ������������� ������ � ���� ������������� �� ����

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

			//	������ �� ������������� ������ � ���������� ������������ �� ����

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

			//	���� ��� �� ������ ����������, �� �������� IP �������� ������� � ��� ����

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

			//	�������� ������ ��� ��������� � �������� �������, ��������� �� ���� ������ � ��������� ������ �������

			SendClientRequest(serverSock, client, request, ip, port, statistic);

			//	������ ���������� � ���� ������
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
>>>>>>> c6339e7786b9baee09bc0c9f7ff055d1be0bab94
