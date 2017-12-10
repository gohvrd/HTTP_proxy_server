#pragma once

#include <winsock2.h>
#include <string>
#include "Statistic.hpp"

int start_server();

int GetClientRequest(SOCKET &client, ThreadData *data, std::string &request, int &nclients, Statistic &statistic);
int ParseClientRequest(SOCKET &client, std::string request, char* ip, u_short& port, int &nclients);
int SendClientRequest(SOCKET &server_sock, std::string &request, char *ip, u_short port);

int GetServerAnswer(SOCKET &server, std::string &answer, Statistic &statistic);
int SendServerAnswer(SOCKET &client, std::string answer);