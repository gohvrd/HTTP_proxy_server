#pragma once

struct Statistic
{
	char InternetProtocol[23];					//XXX.XXX.XXX.XXX.
	char ConnectTime[22];						//DD.MM.YYYY HH:MM:SS
	unsigned int CountReceiveDate = 0;
	unsigned int CountSendDate = 0;
};

std::string GetCurrTime();

bool PutInDatabase(Statistic oneStruct);
std::string ReadStringFromDatabase();
std::string GetPersonalStatistic(const char* IP);