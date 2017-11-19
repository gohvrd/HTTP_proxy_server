#pragma once
#include <winsock2.h>
#include <string>

; int getCharUrl(std::string svrUrl, char* host, u_short &port, char* ip);
std::string ParseClientRequest(std::string request);
