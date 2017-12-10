#include "stdafx.h"

#include "ProxyServer.h"

int GetClientRequest(SOCKET &client, ThreadData *data, std::string &request, int &nclients, Statistic &statistic)
{
	std::string ip_port;
	ip_port = inet_ntoa(data->addr.sin_addr);
	ip_port += ":";
	ip_port += std::to_string(data->addr.sin_port);
	strcpy(statistic.InternetProtocol, ip_port.c_str());
	std::string time = GetCurrTime();
	strcpy(statistic.ConnectTime, time.c_str());

	char buff[1024];
	memset(buff, '\0', BUFF_SIZE);

	// пока не найден символ конца запроса, продолжаем принимать данные от клиента

	while (request.find("\r\n\r\n") == std::string::npos)
	{
		int lenght;

		if ((lenght = recv(client, buff, BUFF_SIZE, 0)) < 0)
			return -1;
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

	return 0;
}

/*
* ѕолучение IP адреса по доменному имени
*/
char* getIp(std::string &svrName)
{
	struct addrinfo hints, *res;
	char ipstr[16];
	char host[100];

	memset(host, '\0', 30);
	strcpy_s(host, 30, svrName.c_str());

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, NULL, &hints, &res) != 0) {
		return "";
	}

	void *addr;

	if (res->ai_family == AF_INET)
	{
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
		addr = &(ipv4->sin_addr);
	}
	else
	{
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
		addr = &(ipv6->sin6_addr);
	}

	inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);

	freeaddrinfo(res);

	return ipstr;
}

/*
* ѕроверка корректности IP адреса
*/
bool IsCorrectIP(std::string &svrName)
{
	int dotCount = 3, maxIpValue = 255;
	size_t valueBegin = 0, valueLength = 0;
	size_t portBegin;

	for (size_t i = 0; i < svrName.length(); ++i)
	{
		if (svrName[i] == '.' || i == svrName.length() - 1)
		{
			if (i != svrName.length() - 1)
			{
				valueLength = i - valueBegin;
				--dotCount;
			}
			else
				valueLength = i - valueBegin + 1;


			std::string ipValue = svrName.substr(valueBegin, valueLength);
			valueBegin = i + 1;

			if (atoi(ipValue.c_str()) > maxIpValue)
				return false;
		}
	}

	return dotCount == 0;
}

/*
* «аполн€ем данными переменные port и ip
*/
int getCharUrl(std::string &svrUrl, u_short &port, char* ip)
{
	char host[30];
	strcpy_s(host, 30, svrUrl.c_str());

	std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::string svrName;
	size_t posParserOld;

	//	выдел€ем IP(возможно доменное им€) и порт

	if ((posParserOld = svrUrl.find_first_of(":")) != std::string::npos)
	{
		svrName = svrUrl.substr(0, posParserOld);
		port = atoi(svrUrl.substr(posParserOld + 1).c_str());
	}
	else
	{
		svrName = svrUrl;
		port = 80;
	}

	//	определ€ем что находитс€ в имени сервера: IP или доменное им€

	bool isIp = (strpbrk(svrName.c_str(), alphabet.c_str()) == NULL);

	//	если это IP, провер€ем его корректность
	if (isIp)
	{
		if (IsCorrectIP(svrName))
			strcpy_s(ip, 16, svrName.c_str());
		else
		{
			printf("Invalid IP address\n");
			return -1;
		}
	}
	else
	{
		strcpy_s(ip, 16, getIp(svrName));
	}

	if (ip == "")
		return -1;

	return 0;
}


int ParseClientRequest(SOCKET &client, std::string request, char* ip, u_short& port, int &nclients)
{
	if (request.length() != 0)
	{
		printf("\nRequest:\n%s\n", request.c_str());

		//функци€ разбора запроса

		size_t parseCursorOld = 0;
		size_t parseCursorNew = request.find_first_of(" ", parseCursorOld);

		//	выделение метода запроса
		std::string httpMethod = request.substr(parseCursorOld, parseCursorNew - parseCursorOld);

		// запрос на представление данных о всех пользовател€х из базы

		if (httpMethod == "GETALLSTAT")
		{
			std::string dataFromBase = ReadStringFromDatabase();
			char buffClients[20];
			memset(buffClients, 0, 20);
			sprintf(buffClients, "\*%d\r\n\r\n", nclients);
			dataFromBase += buffClients;

			send(client, dataFromBase.c_str(), dataFromBase.length(), 0);

			return 1;
		}

		std::string url;
		parseCursorOld = parseCursorNew + 1;
		parseCursorNew = request.find_first_of(" ", parseCursorOld);
		url = request.substr(parseCursorOld, parseCursorNew - parseCursorOld);

		if (getCharUrl(url, port, ip) < 0)
			return -1;

		printf("IP: %s, PORT: %d\n", ip, port);		

		return 0;
	}

	return -1;
}

int SendClientRequest(SOCKET &server_sock, std::string &request, char *ip, u_short port)
{
	sockaddr_in adr;
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = inet_addr(ip);
	adr.sin_port = htons(port);

	char *buff = new char[BUFF_SIZE];
	strcpy(buff, request.c_str());

	if (connect(server_sock, (sockaddr*)&adr, sizeof(adr)) < 0)
		return -1;

	size_t totalsent = 0;
	int senteach;

	//	отправл€ем запрос клиента внешнему серверу

	while (totalsent < BUFF_SIZE)
	{
		if ((senteach = send(server_sock, buff + totalsent, BUFF_SIZE - totalsent, 0)) < 0)
			return -2;

		totalsent += senteach;
	}

	return 0;
}