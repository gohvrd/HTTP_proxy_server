#pragma once

struct Statistic
{
	char InternetProtocol[17];					//XXX.XXX.XXX.XXX.
	char ConnectTime[20];						//DD.MM.YYYY HH:MM:SS
	unsigned int CountReceiveDate = 0;
	unsigned int CountSendDate = 0;
};

bool PutInDatabase(Statistic *statistic, unsigned int count);