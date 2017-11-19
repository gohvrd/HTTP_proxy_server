#include "stdafx.h"
#include <iostream>

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


/* В разработке
Statistic *ReadOnDatabase()
{
	FILE *file;

	if ((file = fopen(FILE_NAME, "r")) == NULL)
		return NULL;

	unsigned int structCount(0);
	char c;

	while (!feof(file)) 
		if ((c = fgetc(file)) == '{' || c == EOF)
			structCount++;

	
	unsigned int structCount(0);

	if (stringCount != 0)
		structCount = (unsigned int)((stringCount - 1) / 6);
	
	Statistic *statistic = new Statistic[structCount];

	wchar_t buff[5];

	for (unsigned int i(0); i < structCount; ++i)
	{
		fwscanf(file, L"%s", buff);
		std::cout << buff;
		std::cin >> c;
	}

	return NULL;
}
*/




