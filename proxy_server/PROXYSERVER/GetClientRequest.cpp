#include "stdafx.h"
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>

std::string ParseClientRequest(char* buff)
{
	std::string request = buff;
	std::string url;

	size_t parseCursorOld = 0;
	size_t parseCursorNew = request.find_first_of(" ", parseCursorOld);;
	std::string httpMethod, httpProtocol, requestHeader;
	std::string requestHeaderName, requestHeaderContent;

	httpMethod = request.substr(parseCursorOld, parseCursorNew - parseCursorOld);
	parseCursorOld = parseCursorNew + 1;

	parseCursorNew = request.find_first_of(" ", parseCursorOld);
	url = request.substr(parseCursorOld, parseCursorNew - parseCursorOld);

	return url;
}

char* getIp(std::string svrName)
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

int getCharUrl(std::string svrUrl, char* host, u_short &port, char* ip)
{
	strcpy_s(host, 30, svrUrl.c_str());

	size_t posParserOld = 0, posParserNew = 0;
	std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::string svrName;

	if ((posParserOld = svrUrl.find_first_of(":")) != std::string::npos)
	{
		svrName = svrUrl.substr(0, posParserOld);
		port = atoi(svrUrl.substr(posParserOld + 1).c_str());
	}
	else
	{
		svrName = svrUrl;
	}

	bool isIp = (strpbrk(svrName.c_str(), alphabet.c_str()) == NULL);

	if (isIp){
		strcpy_s(ip, 16, svrName.c_str());
	}
	else{
		strcpy_s(ip, 16, getIp(svrName));
	}

	if (ip == "")
		return -1;

	return 0;
}