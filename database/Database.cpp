#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>

#include "Statistic.hpp"

#define FILE_NAME "Statistic.json"

bool WriteStructInFstream(FILE* file, Statistic oneStruct, char* mode = "a+")
{
	if ((file = fopen(FILE_NAME, mode)) == NULL)
		return false;

	fprintf(file, "%s", "{\n");
	fprintf(file, "%s", oneStruct.InternetProtocol);
	fprintf(file, "%s", "\n");
	fprintf(file, "%s", oneStruct.ConnectTime);
	fprintf(file, "%s", "\n");
	fprintf(file, "%d", oneStruct.CountSendDate);
	fprintf(file, "%s", "\n");
	fprintf(file, "%d", oneStruct.CountReceiveDate);
	fprintf(file, "%s", "\n}\n");

	if (fclose(file) == EOF)
		return false;

	return true;
}

unsigned int StructCount()
{
	FILE *file;

	if ((file = fopen(FILE_NAME, "rt")) == NULL)
		return NULL;

	unsigned int structCount(0);
	char c;

	while (!feof(file))
		if ((c = fgetc(file)) == '{')
			structCount++;

	if (fclose(file) == EOF)
		return false;

	return structCount;
}

Statistic *ReadStructFromDatabase()
{
	unsigned int structCount = StructCount();

	if (structCount == 0)
		return NULL;

	Statistic *statistic = new Statistic[structCount];

	std::ifstream file(FILE_NAME);

	char buff[20];

	for (int i(0); i < structCount; ++i)
	{
		file >> buff >> statistic[i].InternetProtocol >> statistic[i].ConnectTime >> buff;

		statistic[i].ConnectTime[8] = ' ';

		for (int j(0); j < 8; ++j)
			statistic[i].ConnectTime[j + 9] = buff[j];

		statistic[i].ConnectTime[17] = '\0';

		file >> statistic[i].CountReceiveDate >> statistic[i].CountSendDate >> buff;
	}

	return statistic;
}

bool PutAllInDatabase(Statistic *statistic, unsigned int count)
{
	FILE *file;

	if ((file = fopen(FILE_NAME, "w")) == NULL)
		return false;

	for (unsigned int i(0); i < count; ++i)
	{
		fprintf(file, "%s", "{\n");
		fprintf(file, "%s", statistic[i].InternetProtocol);
		fprintf(file, "%s", "\n");
		fprintf(file, "%s", statistic[i].ConnectTime);
		fprintf(file, "%s", "\n");
		fprintf(file, "%d", statistic[i].CountSendDate);
		fprintf(file, "%s", "\n");
		fprintf(file, "%d", statistic[i].CountReceiveDate);
		fprintf(file, "%s", "\n}\n");
	}

	if (fclose(file) == EOF)
		return false;

	return true;


}

bool PutInDatabase(Statistic oneStruct)
{
	FILE *file;

	unsigned int structCount = StructCount();

	if (structCount == 0)
	{
		WriteStructInFstream(file, oneStruct);

		return true;
	}

	Statistic *statistic = ReadStructFromDatabase();

	for (int i(0); i < structCount; ++i)
		if ((std::string(oneStruct.InternetProtocol) == std::string(statistic[i].InternetProtocol)))
		{
			statistic[i] = oneStruct;

			PutAllInDatabase(statistic, structCount);

			return true;
		}
		
	WriteStructInFstream(file, oneStruct);

	return true;
}

std::string ReadStringFromDatabase()
{
	std::string resultString, buffString;


	Statistic* statistic = ReadStructFromDatabase();

	unsigned int structCount = StructCount();

	for (int i(0); i < structCount; ++i)
		resultString = resultString + std::string(statistic[i].InternetProtocol) + "?" + std::string(statistic[i].ConnectTime) + "?" + std::to_string(statistic[i].CountReceiveDate) + "?" + std::to_string(statistic[i].CountSendDate) + "!!!";
	
	return resultString;
}

unsigned int CountPersonalReceiveDate(char* internetProtocol)
{
	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (int i(0); i < structCount; ++i)
		if (internetProtocol == statistic[i].InternetProtocol)
			return statistic[i].CountReceiveDate;

	return false;
}

unsigned int CountAllReceiveDate()
{
	unsigned int countAllReceiveDate(0);

	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (int i(0); i < structCount; ++i)
		countAllReceiveDate += statistic[i].CountReceiveDate;

	return countAllReceiveDate;
}

unsigned int CountPersonalSendDate(char* internetProtocol)
{
	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (int i(0); i < structCount; ++i)
		if (internetProtocol == statistic[i].InternetProtocol)
			return statistic[i].CountSendDate;

	return false;
}

unsigned int CountAllSendDate()
{
	unsigned int countAllSendDate(0);

	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (int i(0); i < structCount; ++i)
		countAllSendDate += statistic[i].CountSendDate;

	return countAllSendDate;
}

char* PersonalConnectTime(char* internetProtocol)
{
	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (int i(0); i < structCount; ++i)
		if (internetProtocol == statistic[i].InternetProtocol)
			return statistic[i].ConnectTime;

	return false;
}
