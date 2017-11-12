#pragma once

struct Statistic
{
	wchar_t InternetProtocol[17];					//XXX.XXX.XXX.XXX.
	wchar_t ConnectTime[20];						//DD.MM.YYYY HH:MM:SS
	unsigned int CountReceiveDate;
	unsigned int CountSendDate;

};

bool PutInDatabase(Statistic *statistic, unsigned int count);
Statistic *ReadOnDatabase();