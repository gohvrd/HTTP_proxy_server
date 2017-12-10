#include "stdafx.h"

#include "Statistic.hpp"

std::string GetCurrTime()
{
	SYSTEMTIME st;
	std::string time;
	char timeBuff[5];

	memset(timeBuff, 0, 5);

	GetLocalTime(&st);

	if (st.wHour < 10)
		sprintf(timeBuff, "0%d", st.wHour);
	else
		sprintf(timeBuff, "%d", st.wHour);
	time = timeBuff;
	time += ":";

	memset(timeBuff, 0, 4);
	if (st.wMinute < 10)
		sprintf(timeBuff, "0%d", st.wMinute);
	else
		sprintf(timeBuff, "%d", st.wMinute);
	time += timeBuff;
	time += ":";

	memset(timeBuff, 0, 4);
	if (st.wSecond < 10)
		sprintf(timeBuff, "0%d", st.wSecond);
	else
		sprintf(timeBuff, "%d", st.wSecond);
	time += timeBuff;
	time += " ";

	memset(timeBuff, 0, 4);
	sprintf(timeBuff, "%d", st.wDay);
	time += timeBuff;
	time += ".";

	memset(timeBuff, 0, 4);
	sprintf(timeBuff, "%d", st.wMonth);
	time += timeBuff;
	time += ".";

	memset(timeBuff, 0, 4);
	sprintf(timeBuff, "%d", st.wYear);
	time += timeBuff;

	return time;
}

bool WriteStructInFstream(Statistic oneStruct, char* mode = "a+")
{
	FILE *file;

	if ((file = fopen(FILE_NAME, mode)) == NULL)
		return false;

	fprintf(file, "%s", "{\n");
	fprintf(file, "%s", oneStruct.InternetProtocol);
	fprintf(file, "%s", "\n");
	fprintf(file, "%s", oneStruct.ConnectTime);
	fprintf(file, "%s", "\n");
	fprintf(file, "%d", oneStruct.CountReceiveDate);
	fprintf(file, "%s", "\n");
	fprintf(file, "%d", oneStruct.CountSendDate);
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

	char buff[23];

	for (unsigned int i(0); i < structCount; ++i)
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
		fprintf(file, "%d", statistic[i].CountReceiveDate);
		fprintf(file, "%s", "\n");
		fprintf(file, "%d", statistic[i].CountSendDate);
		fprintf(file, "%s", "\n}\n");
	}

	if (fclose(file) == EOF)
		return false;

	return true;


}

bool PutInDatabase(Statistic oneStruct)
{
	unsigned int structCount = StructCount();

	if (structCount == 0)
	{
		WriteStructInFstream(oneStruct);

		return true;
	}

	Statistic *statistic = ReadStructFromDatabase();

	for (unsigned int i(0); i < structCount; ++i)
	{
		if ((std::string(oneStruct.InternetProtocol) == std::string(statistic[i].InternetProtocol)))
		{
			strcpy(statistic[i].ConnectTime, oneStruct.ConnectTime);
			statistic[i].CountReceiveDate += oneStruct.CountReceiveDate;
			statistic[i].CountSendDate += oneStruct.CountSendDate;

			PutAllInDatabase(statistic, structCount);

			return true;
		}
	}

	WriteStructInFstream(oneStruct);

	return true;
}

std::string ReadStringFromDatabase()
{
	std::string resultString, buffString;

	Statistic* statistic = ReadStructFromDatabase();

	unsigned int structCount = StructCount();

	for (unsigned int i(0); i < structCount; ++i)
		resultString = resultString + std::string(statistic[i].InternetProtocol) + "?" + std::string(statistic[i].ConnectTime) + "?" + std::to_string(statistic[i].CountReceiveDate) + "?" + std::to_string(statistic[i].CountSendDate) + "!";

	return resultString;
}

unsigned int CountPersonalReceiveDate(const char* internetProtocol)
{
	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (unsigned int i(0); i < structCount; ++i)
	{
		if (strcmp(internetProtocol, statistic[i].InternetProtocol) == 0)
			return statistic[i].CountReceiveDate;
	}

	return false;
}

unsigned int CountAllReceiveDate()
{
	unsigned int countAllReceiveDate(0);

	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (unsigned int i(0); i < structCount; ++i)
		countAllReceiveDate += statistic[i].CountReceiveDate;

	return countAllReceiveDate;
}

unsigned int CountPersonalSendDate(const char* internetProtocol)
{
	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (unsigned int i(0); i < structCount; ++i)
	{
		if (strcmp(internetProtocol, statistic[i].InternetProtocol) == 0)
			return statistic[i].CountSendDate;
	}

	return false;
}

unsigned int CountAllSendDate()
{
	unsigned int countAllSendDate(0);

	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (unsigned int i(0); i < structCount; ++i)
		countAllSendDate += statistic[i].CountSendDate;

	return countAllSendDate;
}

char* PersonalConnectTime(const char* internetProtocol)
{
	Statistic* statistic = ReadStructFromDatabase();
	unsigned int structCount = StructCount();

	for (unsigned int i(0); i < structCount; ++i)
	{
		if (strcmp(internetProtocol, statistic[i].InternetProtocol) == 0)
			return statistic[i].ConnectTime;
	}

	return false;
}

std::string GetPersonalStatistic(const char* IP)
{
	std::string result;
	char buff[10];
	unsigned int count;
	if ((count = CountPersonalReceiveDate(IP)) == 0)
		sprintf(buff, "null");
	else
		sprintf(buff, "%d", CountPersonalReceiveDate(IP));
	std::string recieve(buff);

	if ((count = CountPersonalSendDate(IP)) == 0)
		sprintf(buff, "null");
	else
		sprintf(buff, "%d", CountPersonalSendDate(IP));
	std::string send(buff);
	result += std::string(IP) + "?" + PersonalConnectTime(IP) + "?" + recieve + "?" + send + "!";

	return result;
}