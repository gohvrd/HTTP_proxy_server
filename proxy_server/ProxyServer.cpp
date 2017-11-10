#include "ProxyServer.h"

#include <WS2tcpip.h>
#include <winsock2.h>
#include <winsock.h>
#include <thread>

#include <iostream>

#define WINSOCK_DEPRECATED_NO_WARNINGS
#define sHELLO "Hello, I'm demo TCP-echo SERVER\r\n"


ProxyServer::ProxyServer()
{	
}

ProxyServer::~ProxyServer()
{
	closesocket(listeningSocket);
	closesocket(clientSocket);
	closesocket(serverSocket);
}

HANDLE ProxyServer::StartThread(LPTHREAD_METHOD pMethod, LPVOID pParam,
	LPDWORD pdwThreadID /* = NULL */,
	LPSECURITY_ATTRIBUTES pSecurity /* = NULL */,
	DWORD dwStackSize /* = 0 */,
	DWORD dwFlags /* = 0 */)
{
	// Создаем структуру и упаковываем данные для статической функции.
	LPSTARTUP_PARAM pStartup = new STARTUP_PARAM;
	pStartup->pClass = this;
	pStartup->pMethod = pMethod;
	pStartup->pParam = pParam;

	// Создаем новый поток.
	return CreateThread(pSecurity, dwStackSize, (LPTHREAD_START_ROUTINE)StartFunc, pStartup, dwFlags, pdwThreadID);
}

DWORD ProxyServer::StartFunc(LPSTARTUP_PARAM pStartup)
{
	// Распаковываем данные в новом потоке.
	// Получаем указатель на класс и на метод класса.
	ProxyServer* pClass = pStartup->pClass;
	LPTHREAD_METHOD pMethod = pStartup->pMethod;
	LPVOID pParam = pStartup->pParam;

	// Запускаем метод класса в новом потоке.
	DWORD dwResult = (pClass->*pMethod)(pParam);

	// Удаляем временные данные и возвращаем код возврата из нового потока.
	delete pStartup;
	return dwResult;
}

int ProxyServer::turnOnLine()
{
	if (InitListeningSocket())
	{
		printf("Failed to initialize socket for client\n");
		return -1;
	}

	while (1)
	{
		clientAddrSize = sizeof(clientAddr);

		if (clientSocket = accept(listeningSocket, (sockaddr *)&clientAddr, &clientAddrSize) < 0)
		{
			printf("Accept call failed\n");
			return -1;
		}

		DWORD thID;
		/*if (CreateThread(NULL, NULL, ProcessingRequest, this, NULL, &thID))
		{
			printf("Error thread creation\n");
			return -1;
		}*/

		while (1)
		{
			ProcessingRequest(0);
		}

		//StartThread(&ProxyServer::ProcessingRequest, 0, &thID);

		closesocket(clientSocket);
	}
}

int ProxyServer::InitListeningSocket()
{
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listeningSocket == -1)
	{
		printf("Error socket %d\n", WSAGetLastError());
		return -1;
	}

	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listeningSocket, (struct sockaddr*)&local_addr, sizeof(local_addr)))
	{
		printf("Error bind %d\n", WSAGetLastError());
		return -1;
	}

	if (listen(listeningSocket, 5))
	{
		printf("Error listen %d\n", WSAGetLastError());
		return -1;
	}

	return 0;
}

DWORD ProxyServer::ProcessingRequest(LPVOID pParam)
{
	if (RecvClientRequest() < 0)
	{
		//printf("Receiving from client error\n");
		return -1;
	}

	if (ParseClientRequest() < 0)
	{
		printf("Parse client request error\n");
		return -1;
	}

	PrintClientRequest();

	if (SendClientRequest() < 0)
	{
		printf("Sending client request to server error\n");
		return -1;
	}

	if (RecvServerAnswer() < 0)
	{
		printf("Receiving from server error\n");
		return -1;
	}

	if (SendServerAnswer() < 0)
	{
		printf("Sending server answer error\n");
		return -1;
	}

	return 0;
}

int ProxyServer::RecvClientRequest()
{
	int recvLength;
	char* buf = new char[buf_sz];
	memset(buf, '\0', buf_sz);

	if (recvLength = recv(clientSocket, buf, buf_sz, 0) < 0)
	{
		//printf("Failed to receive request (blocking)\n");
		return -1;
	}

	clientRequest.append(buf, recvLength);

	while (1)
	{
		memset(buf, '\0', buf_sz);
		recvLength = recv(clientSocket, buf, buf_sz, 0);

		if (recvLength < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN){
				break;
			}
			else 
			{
				printf("Failed receiving request (nonblocking)\n");
				return -1;
			}
		}

		clientRequest.append(buf, recvLength);

		if (recvLength < buf_sz)
			break;
	}

	return 0;
}

int ProxyServer::ParseClientRequest()
{
	size_t parseCursorOld = 0;
	size_t parseCursorNew = clientRequest.find_first_of(" ", parseCursorOld);;
	std::string httpMethod, httpProtocol, requestHeader;
	std::string requestHeaderName, requestHeaderContent;

	httpMethod = clientRequest.substr(parseCursorOld, parseCursorNew - parseCursorOld);
	parseCursorOld = parseCursorNew + 1;

	parseCursorNew = clientRequest.find_first_of(" ", parseCursorOld);
	url = clientRequest.substr(parseCursorOld, parseCursorNew - parseCursorOld);

	return 0;
}

int ProxyServer::GetUrlAndPort()
{
	size_t posParserOld = 0, posParserNew = 0;
	std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::string svrName;

	if ((posParserOld = url.find_first_of(":")) != std::string::npos)
	{
		svrName = url.substr(0, posParserOld);
		port = atoi(url.substr(posParserOld + 1).c_str());
	}
	else
	{
		svrName = url;
	}

	bool isIp = (strpbrk(svrName.c_str(), alphabet.c_str()) == NULL);

	if (isIp)
	{
		ip = svrName;
	}
	else
	{
		GetIP(svrName);
	}

	if (ip == "")
		return -1;

	return 0;
}

int ProxyServer::GetIP(std::string svrName)
{
	struct addrinfo hints, *res;
	char ipstr[16];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(svrName.c_str(), NULL, &hints, &res) != 0) 
	{
		return -1;
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

	inet_ntop(res->ai_family, addr, ipstr, sizeof (ipstr));

	ip = ipstr;

	freeaddrinfo(res);

	return 0;
}

int ProxyServer::InitServerSocket()
{
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Failed to create socket\n");
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());


	if (connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		printf("Failed to connect\n");
		return -1;
	}

	return 0;
}

int ProxyServer::SendClientRequest()
{
	size_t requestSize = clientRequest.length();
	std::string* requestDataPtr = &clientRequest;

	char *buf = new char[requestSize];
	memset(buf, '\0', requestSize);
	memcpy(buf, requestDataPtr->data(), requestSize);

	if (send(serverSocket, buf, requestSize, 0) < 0)
	{
		printf("Sending request failed\n");
		return -1;
	}

	delete [] buf;

	return 0;
}

int ProxyServer::RecvServerAnswer()
{
	int recvLength;
	char* buf = new char[buf_sz];
	memset(buf, '\0', buf_sz);

	if (!(recvLength = recv(serverSocket, buf, buf_sz, 0)))
	{
		printf("Failed to receive request (blocking)\n");
		return -1;
	}

	serverAnswer.append(buf, recvLength);

	while (1)
	{
		memset(buf, '\0', buf_sz);
		recvLength = recv(serverSocket, buf, buf_sz, 0);

		if (recvLength < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
			{
				break;
			}

			printf("Failed receiving answer (nonblocking)\n");
			return -1;
		}

			serverAnswer.append(buf, recvLength);

			if (recvLength < buf_sz)
				break;
	}

	delete [] buf;

	return 0;
}

int ProxyServer::SendServerAnswer()
{
	size_t answerSize = serverAnswer.length();
	std::string* responseDataPtr = &serverAnswer;

	char *buf = new char[answerSize];
	memset(buf, '\0', answerSize);
	memcpy(buf, responseDataPtr->data(), answerSize);

	if (send(clientSocket, buf, answerSize, 0) < 0)
	{
		printf("Sending response failed\n");
		return -1;
	}

	delete [] buf;

	return 0;
}

int ProxyServer::PrintClientRequest()
{
	printf("Client Request:\n%s\n", clientRequest.c_str());
	printf("From Parse:\nHost's IP - %s\nPort - %d\nMethod - %s\n", ip.c_str(), port, method.c_str());
	return 0;
}
