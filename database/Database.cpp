#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>

#include "Statistic.hpp"

#define FILE_NAME "Statistic.json"

bool PutInDatabase(Statistic *statistic, unsigned int count)
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

	return structCount;
}

Statistic *ReadStructFromDatabase()
{
	unsigned int structCount = StructCount();
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

std::string ReadStringFromDatabase()
{
	std::string resultString, buffString;


	Statistic* statistic = ReadStructFromDatabase();

	unsigned int structCount = StructCount();

	for (int i(0); i < structCount; ++i)
		resultString = resultString + std::string(statistic[i].InternetProtocol) + "?" + std::string(statistic[i].ConnectTime) + "?" + std::to_string(statistic[i].CountReceiveDate) + "?" + std::to_string(statistic[i].CountSendDate) + "!!!";
	
	return resultString;
}


