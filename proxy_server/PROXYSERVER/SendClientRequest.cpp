#include "stdafx.h"
#include <string>
#include <winsock2.h>

#define BUFF_SIZE 1024

#include "Statistic.hpp"

int SendClientRequest(SOCKET server_sock, SOCKET client_sock, std::string request, char *ip, u_short port, Statistic &statistic)
{
	sockaddr_in adr;
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = inet_addr(ip);
	adr.sin_port = htons(port);

	char *buff = new char[BUFF_SIZE];
	strcpy(buff, request.c_str());

	if (connect(server_sock, (sockaddr*)&adr, sizeof(adr)) < 0)
	{
		printf("Connect to remote server error!\n");
		return -1;
	}

	size_t totalsent = 0;
	int senteach;

	while (totalsent < BUFF_SIZE)
	{
		if ((senteach = send(server_sock, buff + totalsent, BUFF_SIZE - totalsent, 0)) < 0)
		{
			printf("Send to remote server error!\n");
			return -1;
		}

		totalsent += senteach;
	}

	std::string answerStr;
	memset(buff, 0, BUFF_SIZE);

	while (answerStr.find("\r\n\r\n") == std::string::npos)
	{
		int lenght;

		if ((lenght = recv(server_sock, buff, BUFF_SIZE, 0)) < 0)
		{
			printf("Receive from remote server error!\n");
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

				answerStr.append(buff);
			}
		}

		statistic.CountReceiveDate += answerStr.length();	
	}

	totalsent = 0;
	buff = new char[BUFF_SIZE];
	memset(buff, 0, BUFF_SIZE);
	strcpy(buff, answerStr.c_str());

	while (totalsent < BUFF_SIZE)
	{
		if ((senteach = send(client_sock, buff + totalsent, BUFF_SIZE - totalsent, 0)) < 0)
		{
			printf("Send answer to client error!\n");
			return -1;
		}

		totalsent += senteach;
	}

	closesocket(server_sock);

	return 0;
}
