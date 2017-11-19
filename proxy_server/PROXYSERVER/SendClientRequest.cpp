#include "stdafx.h"
#include <string>
#include <winsock2.h>

#include "Statistic.hpp"

int SendClientRequest(SOCKET server_sock, SOCKET client_sock, std::string request, char *ip, u_short port, Statistic &statistic)
{
	sockaddr_in adr;
	hostent* hst;

	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = inet_addr(ip);
	adr.sin_port = htons(port);


	size_t buffSize = request.length();
	char *buff = new char[buffSize];
	strcpy(buff, request.c_str());

	if (connect(server_sock, (sockaddr*)&adr, sizeof(adr)) < 0)
	{
		return -1;
	}

	size_t totalsent = 0;
	int senteach;

	while (totalsent < buffSize)
	{
		if ((senteach = send(server_sock, buff + totalsent, buffSize - totalsent, 0)) < 0)
		{
			return -1;
		}

		totalsent += senteach;
	}

	std::string answerStr;
	memset(buff, 0, buffSize);

	while (answerStr.find("\r\n\r\n") == std::string::npos)
	{
		int lenght;

		if ((lenght = recv(server_sock, buff, buffSize, 0)) < 0)
		{
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
	buffSize = answerStr.length();
	buff = new char[buffSize];
	memset(buff, 0, buffSize);
	strcpy(buff, answerStr.c_str());

	while (totalsent < buffSize)
	{
		if ((senteach = send(client_sock, buff + totalsent, buffSize - totalsent, 0)) < 0)
		{
			return -1;
		}

		totalsent += senteach;
	}

	closesocket(server_sock);

	return 0;
}
