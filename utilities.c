#include "utilities.h"

void GetAppVersionFromSourceFile(const char * lpszFileDate, LPVERSION_INFORMATION vi)
{
	char szMonth[5] = {0};
	strncpy_s(szMonth, 5, lpszFileDate, 3);

	if(strcmp(szMonth, "Jan") == 0)
		vi->month = 1;
	if(strcmp(szMonth, "Feb") == 0)
		vi->month = 2;
	if(strcmp(szMonth, "Mar") == 0)
		vi->month = 3;
	if(strcmp(szMonth, "Apr") == 0)
		vi->month = 4;
	if(strcmp(szMonth, "May") == 0)
		vi->month = 5;
	if(strcmp(szMonth, "Jun") == 0)
		vi->month = 6;
	if(strcmp(szMonth, "Jul") == 0)
		vi->month = 7;
	if(strcmp(szMonth, "Aug") == 0)
		vi->month = 8;
	if(strcmp(szMonth, "Sep") == 0)
		vi->month = 9;
	if(strcmp(szMonth, "Oct") == 0)
		vi->month = 10;
	if(strcmp(szMonth, "Nov") == 0)
		vi->month = 11;
	if(strcmp(szMonth, "Dec") == 0)
		vi->month = 12;

	vi->year = atoi(lpszFileDate + 7);
	vi->mday = (int)atoi(lpszFileDate + 4);

	vi->majorVersion = 1;
	vi->minorVersion = 0;
	vi->buildNumber = 160;
	vi->pdate = (((vi->year << 0x04) | vi->month) << 0x05) | vi->mday;

	return;
}


void LoadSystemTrayIcon(HINSTANCE hInst, HWND hWnd, HICON hIcon, 
	                    UINT uTrayIconID, UINT uCallbackMessage, 
						LPTSTR lpszTextMsg, DWORD dwEventType)
{
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = uTrayIconID;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;// | NIF_INFO;

	nid.uCallbackMessage = uCallbackMessage;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONMAINAPP));
	nid.uVersion = NOTIFYICON_VERSION;

	lstrcpy(nid.szTip, SZAPPTITLE);
	
	if(dwEventType == LSTI_STATEINITIALIZE)
	{
		Shell_NotifyIcon(NIM_ADD, &nid);
		Shell_NotifyIcon(NIM_SETVERSION, &nid);
	}

	if(dwEventType == LSTI_STATECHANGEUPDATE)
	{
		nid.hIcon = hIcon; // LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONSYSTRAYACTIVE));
		nid.uFlags |= NIF_INFO;
		nid.uTimeout = 1000;
		nid.dwInfoFlags = NIIF_INFO;
		nid.hBalloonIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONMAINAPP));
	    
		lstrcpy(nid.szInfoTitle, SZAPPTITLE);
	    lstrcpy(nid.szInfo, lpszTextMsg);
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}

	if(dwEventType == LSTI_STATECHANGEEXIT)
	{
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}

	return;
}

void GetScreenSaverInfo(LPSCREENSAVERINFO lpssi)
{
	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &lpssi->bIsScreensSaverActive, 0);
	SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &lpssi->dwScreenSaverTimeOut, 0);

	return;
}