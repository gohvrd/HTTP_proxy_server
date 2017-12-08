#pragma once
#include <winsock2.h>
#include <string>
#include "Statistic.hpp"

int SendClientRequest(SOCKET &server_sock, SOCKET &client_sock, std::string &request, char *ip, u_short port, Statistic &statistic);