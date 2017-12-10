#pragma once

struct Statistic
{
<<<<<<< HEAD
	char InternetProtocol[23];					//XXX.XXX.XXX.XXX:YYYYY
	char ConnectTime[20];						//DD.MM.YYYY HH:MM:SS
=======
	char InternetProtocol[23];					//XXX.XXX.XXX.XXX.
	char ConnectTime[22];						//DD.MM.YYYY HH:MM:SS
>>>>>>> c6339e7786b9baee09bc0c9f7ff055d1be0bab94
	unsigned int CountReceiveDate = 0;
	unsigned int CountSendDate = 0;
};

std::string GetCurrTime();

bool PutInDatabase(Statistic oneStruct);
std::string ReadStringFromDatabase();
std::string GetPersonalStatistic(const char* IP);