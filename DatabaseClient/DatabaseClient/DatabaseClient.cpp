#include "stdafx.h"
#include "resource1.h"
#include <vector>
#include <string>
#include <winsock2.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <commctrl.h>

#define MAX_LOADSTRING 100
#define MAX_LISTITEM 20
#define MAX_LISTSUBITEM 5
#define MAX_TABNUM 2

#define PORT 1380
#define SERVERADDR "127.0.0.1"
#define BUFF_SIZE 10000

SOCKET my_sock;
std::string answer;
int recieveVal = 0, sendVal = 0, online = 0;
bool is_con = false;

MSG msg;
HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
static HWND hTableList;

BOOL InitInstance(HINSTANCE, int);
BOOL CALLBACK	TableProc(HWND, UINT, WPARAM, LPARAM);
LRESULT TableDraw(LPARAM lParam);


void InitTableList(HWND);
void InsertTableList(HWND);
void InitTableDlg(HWND);

struct parse_result
{
	std::vector<std::string> _ipArr;
	std::vector<int> _countRecieveDataArr;
	std::vector<int> _countSendDataArr;
	std::vector<std::string> _timeArr;
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	return (int)msg.wParam;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	DialogBox(hInst, (LPCTSTR)IDD_TABLEDLG, NULL, (DLGPROC)TableProc);
	return TRUE;
}

BOOL CALLBACK TableProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int iIndex;
	LPNMLISTVIEW pnm;
	TCHAR *pVarName = NULL;
	POINT pt;
	static RECT lstRect;
	hTableList = GetDlgItem(hDlg, IDC_TABLELIST);
	char zero[] = "";

	switch (message)
	{
	case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETREDRAW, FALSE, 0);

		InitTableList(hTableList);

		
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_GETSTAT:
			if (!is_con)
			{
				MessageBox(hDlg, "Вы не подключены к серверу", "Ошибка подключения!", MB_OK);
				return TRUE;
			}

			recieveVal = 0;
			sendVal = 0;

			send(my_sock, "GETALLSTAT \r\n\r\n", 16, 0);

			char buff[BUFF_SIZE];

			while (answer.find("\r\n\r\n") == std::string::npos)
			{
				memset(buff, '\0', BUFF_SIZE);

				int lenght;

				if ((lenght = recv(my_sock, buff, BUFF_SIZE, 0)) < 0)
				{
					closesocket(my_sock);
					return FALSE;
				}
				else
				{
					if (lenght == 0)
					{
						break;
					}
					else
					{
						if (strlen(buff) == 0)
							continue;

						answer.append(buff);
					}
				}
			}

			hTableList = GetDlgItem(hDlg, IDC_TABLELIST);
			ListView_DeleteAllItems(hTableList);
			InsertTableList(hTableList);

			char value[10];
			memset(value, 0, 10);
			sprintf(value, "%d", recieveVal);
			SetDlgItemText(hDlg, IDC_RECIEVE, value);
			sprintf(value, "%d", sendVal);
			SetDlgItemText(hDlg, IDC_SEND, value);
			sprintf(value, "%d", online);
			SetDlgItemText(hDlg, IDC_ONLINE, value);

			UpdateWindow(hDlg);
			answer = "";
			return TRUE;
		case IDC_DIS:
			closesocket(my_sock);
			is_con = false;
			SetDlgItemText(hDlg, IDC_STATUS, "Вы не подключены");
			return TRUE;

		case IDC_CLEAR:
			ListView_DeleteAllItems(hTableList);
			
			SetDlgItemText(hDlg, IDC_RECIEVE, zero);
			SetDlgItemText(hDlg, IDC_SEND, zero);
			SetDlgItemText(hDlg, IDC_ONLINE, zero);
			return TRUE;

		case IDC_CON:
			WSADATA ws;

			if (WSAStartup(0x202, &ws))
			{
				return FALSE;
			}

			my_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (my_sock == -1)
			{
				return FALSE;
			}

			sockaddr_in dest_addr;
			dest_addr.sin_family = AF_INET;
			dest_addr.sin_port = htons(PORT);
			HOSTENT *hst;

			if (inet_addr(SERVERADDR) != INADDR_NONE)
				dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
			else

				if (hst = gethostbyname(SERVERADDR))
					((unsigned long *)&dest_addr.sin_addr)[0] = ((unsigned long **)hst->h_addr_list)[0][0];
				else
				{
					closesocket(my_sock);
					WSACleanup();

					return FALSE;
				}

			if (connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr)))
			{
				return FALSE;
			}

			is_con = true;

			SetDlgItemText(hDlg, IDC_STATUS, "Вы подключены");
			return TRUE;
		}

		


		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			PostQuitMessage(0);
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	case WM_NCHITTEST:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (pt.x >= lstRect.left && pt.x <= lstRect.right &&
			pt.y >= lstRect.top && pt.y <= lstRect.right)
		{
			return (LRESULT)HTERROR;
		}
		break;
	case WM_NOTIFY:
		switch (LOWORD(wParam))
		{
		case IDC_TABLELIST:
			pnm = (LPNMLISTVIEW)lParam;
			if (pnm->hdr.hwndFrom == hTableList &&pnm->hdr.code == NM_CUSTOMDRAW)
			{
				SetWindowLong(hDlg, DWL_MSGRESULT, (LONG)TableDraw(lParam));
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

int countOfUsers()
{
	int len = answer.length();
	int count = 0;

	for (int i = 0; i < len; ++i)
	{
		if (answer[i] == '!')
			++count;
	}

	return count;
}

parse_result ParseAnswer(int &recieveData, int &sendData, int &online)
{
	parse_result result;

	int currentUser = 0;
	int startPos = 0;
	int endPos;

	while (answer.find_first_of("!") != std::string::npos)
	{
		endPos = answer.find_first_of("!");
		std::string user = answer.substr(startPos, endPos - startPos);
		answer = answer.substr(endPos + 1, answer.length() - endPos - 1);

		endPos = user.find_first_of("?");
		result._ipArr.push_back(user.substr(0, endPos));

		user = user.substr(endPos + 1, user.length() - endPos - 1);

		endPos = user.find_first_of("?");
		result._timeArr.push_back(user.substr(0, endPos));

		user = user.substr(endPos + 1, user.length() - endPos - 1);

		endPos = user.find_first_of("?");
		int recieveV = atoi(user.substr(0, endPos).c_str());
		recieveData += recieveV;
		result._countRecieveDataArr.push_back(recieveV);

		user = user.substr(endPos + 1, user.length() - endPos - 1);

		endPos = user.find_first_of("?");
		int sendV = atoi(user.substr(0, endPos).c_str());
		sendData += sendV;
		result._countSendDataArr.push_back(sendV);

		++currentUser;
	}

	startPos = answer.find_first_of("*") + 1;
	endPos = answer.find_first_of("\r");
	int onlineCount = atoi(answer.substr(startPos, endPos - startPos).c_str());

	online = onlineCount;

	return result;
}

void InsertTableList(HWND hTableList)
{
	LVITEM listItem;
	memset(&listItem, 0, sizeof(LVITEM));
	listItem.mask = LVIF_IMAGE | LVIF_TEXT;
	listItem.cchTextMax = 256;
	TCHAR Temp[MAX_PATH] = { 0 };

	int userCount = countOfUsers();

	parse_result result = ParseAnswer(recieveVal, sendVal, online);

	for (int j = 0; j < userCount; j++)
	{
		listItem.iItem = j;
		listItem.iSubItem = 0;
		sprintf((char*)Temp, "%s", result._ipArr[j].c_str());
		listItem.pszText = Temp;
		SendMessage(hTableList, LVM_SETITEMSTATE, j, (LPARAM)&listItem);
		SendMessage(hTableList, LVM_INSERTITEM, 0, (LPARAM)&listItem);
		memset(Temp, 0, MAX_PATH);
		for (int i = 1; i <= 3; i++)
		{
			listItem.iSubItem = i;
			switch (i)
			{
			case 1:
				sprintf((char*)Temp, "%d", result._countRecieveDataArr[j]);
				break;
			case 2:
				sprintf((char*)Temp, "%d", result._countSendDataArr[j]);
				break;
			case 3:
				sprintf((char*)Temp, "%s", result._timeArr[j].c_str());
				break;
			}
			listItem.pszText = Temp;
			SendMessage(hTableList, LVM_SETITEM, 0, (LPARAM)&listItem);
		}
	}
	for (int i = 0; i <= MAX_LISTITEM; i++)
	{
		ListView_SetItemState(hTableList, i, 0, LVIS_SELECTED | LVIS_FOCUSED);
	}
}

void InitTableList(HWND hTableList)
{
	LVCOLUMN listCol;
	memset(&listCol, 0, sizeof(LVCOLUMN));
	listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	listCol.pszText = "IP";
	SendMessage(hTableList, LVM_INSERTCOLUMN, 0, (LPARAM)&listCol); 
	listCol.pszText = "Count Recieve Data";                           
	SendMessage(hTableList, LVM_INSERTCOLUMN, 1, (LPARAM)&listCol);
	listCol.pszText = "Count Send Data";                            
	SendMessage(hTableList, LVM_INSERTCOLUMN, 2, (LPARAM)&listCol); 
	listCol.pszText = "Connect time";                            
	SendMessage(hTableList, LVM_INSERTCOLUMN, 3, (LPARAM)&listCol); 
	
	ListView_SetColumnWidth(hTableList, 0, 80);
	ListView_SetColumnWidth(hTableList, 1, 100);
	ListView_SetColumnWidth(hTableList, 2, 100);
	ListView_SetColumnWidth(hTableList, 3, 120);
}

LRESULT TableDraw(LPARAM lParam)
{
	int iRow;
	LPNMLVCUSTOMDRAW pListDraw = (LPNMLVCUSTOMDRAW)lParam;
	switch (pListDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);
	case CDDS_ITEMPREPAINT:
		iRow = (int)pListDraw->nmcd.dwItemSpec;
		if (iRow % 2 == 0)
		{
			pListDraw->clrTextBk = RGB(202, 221, 250);
			return CDRF_NEWFONT;
		}
	default:
		break;
	}
	return CDRF_DODEFAULT;
}