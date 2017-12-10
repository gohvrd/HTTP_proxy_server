#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#define MY_PORT 1380
#define BUFF_SIZE 1024

#define FILE_NAME "Statistic.json"

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>

struct ThreadData
{
	SOCKET socket;
	sockaddr_in addr;
};