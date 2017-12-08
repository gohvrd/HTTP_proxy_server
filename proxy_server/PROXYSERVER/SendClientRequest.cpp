#include "stdafx.h"
#include <string>
#include <winsock2.h>

#define BUFF_SIZE 1024

#include "Statistic.hpp"

/*
 * Перенаправление сообщения клиента на сервер, получение и перенаправление ответа сервера клиенту, а также сбор статистических данных
 */
int SendClientRequest(SOCKET &server_sock, SOCKET &client_sock, std::string &request, char *ip, u_short port, Statistic &statistic)
{
	//	подключаем сокет к внешнему серверу (на который обращался клиент)

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

	//	отправляем запрос клиента внешнему серверу

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

	//	пока нет идентификатора конца запроса, получаем данные от внешнего сервера

	while (answerStr.find("\r\n\r\n") == std::string::npos)
	{
		if (recv(server_sock, buff, BUFF_SIZE, 0) < 0)
		{
			printf("Receive from remote server error!\n");
			return -1;
		}

		if (strlen(buff) == 0)
			continue;

		answerStr.append(buff);
		statistic.CountReceiveDate += answerStr.length();
	}

	totalsent = 0;
	buff = new char[BUFF_SIZE];
	memset(buff, 0, BUFF_SIZE);
	strcpy(buff, answerStr.c_str());

	//	перенаправляем ответ внешнего сервера клиенту

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
