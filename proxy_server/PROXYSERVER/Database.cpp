#include "stdafx.h"
#include <iostream>

#include "Statistic.h"

#define FILE_NAME "Statistic.json"

//bool PutInDatabase(Statistic *statistic, unsigned int count)
//{
//	FILE *file;
//
//	if ((file = fopen("text.txt", "w")) == NULL)
//		return false;
//
//	for (unsigned int i(0); i < count; ++i)
//	{
//		fwprintf(file, L"%s", L"{\n");
//		fwprintf(file, L"%s", statistic[i].InternetProtocol);
//		fwprintf(file, L"%s", L"\n");
//		fwprintf(file, L"%s", statistic[i].ConnectTime);
//		fwprintf(file, L"%s", L"\n");
//		fwprintf(file, L"%d", statistic[i].CountSendDate);
//		fwprintf(file, L"%s", L"\n");
//		fwprintf(file, L"%d", statistic[i].CountReceiveDate);
//		fwprintf(file, L"%s", L"\n}\n");
//	}
//
//	if (fclose(file) == EOF)
//		return false;
//
//	return true;
//
//
//}
//
//Statistic *ReadOnDatabase()
//{
//	FILE *file;
//
//	if ((file = fopen(FILE_NAME, "r")) == NULL)
//		return NULL;
//
//	unsigned int structCount(0);
//	char c;
//
//	while (!feof(file))
//	if ((c = fgetc(file)) == '{' || c == EOF)
//		structCount++;
//
//	/*
//	unsigned int structCount(0);
//
//	if (stringCount != 0)
//	structCount = (unsigned int)((stringCount - 1) / 6);
//	*/
//	Statistic *statistic = new Statistic[structCount];
//
//	wchar_t buff[5];
//
//	for (unsigned int i(0); i < structCount; ++i)
//	{
//		fwscanf(file, L"%s", buff);
//		std::cout << buff;
//		std::cin >> c;
//	}
//
//	return NULL;
//}