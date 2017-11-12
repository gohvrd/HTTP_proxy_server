#pragma once
#include <string>
#include <winsock2.h>

std::string ParseClientRequest(char* buff);
int getCharUrl(std::string svrUrl, char* host, u_short &port, char* ip);