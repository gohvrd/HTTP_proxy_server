#include "stdafx.h"

#include "ProxyServer.h"

int GetServerAnswer(SOCKET &server, std::string &answer, Statistic &statistic)
{
	char buff[1024];

	memset(buff, 0, BUFF_SIZE);

	//	пока нет идентификатора конца запроса, получаем данные от внешнего сервера

	while (answer.find("\r\n\r\n") == std::string::npos)
	{
		if (recv(server, buff, BUFF_SIZE, 0) < 0)
		{
			printf("Receive from remote server error!\n");
			return -1;
		}

		if (strlen(buff) == 0)
			continue;

		answer.append(buff);
		statistic.CountReceiveDate += answer.length();
	}

	return 0;
}

int SendServerAnswer(SOCKET &client, std::string answer)
{
	char buff[BUFF_SIZE];
	memset(buff, 0, BUFF_SIZE);
	strcpy(buff, answer.c_str());

	size_t totalsent = 0;
	int senteach;

	//	перенаправляем ответ внешнего сервера клиенту

	while (totalsent < BUFF_SIZE)
	{
		if ((senteach = send(client, buff + totalsent, BUFF_SIZE - totalsent, 0)) < 0)
		{
			printf("Send answer to client error!\n");
			return -1;
		}

		totalsent += senteach;
	}

	return 0;
}