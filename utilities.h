#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <shellapi.h>
#include "resource.h"


typedef struct
{
	/* Application version */
	short majorVersion;
	short minorVersion;
	short buildNumber;

	/* Build date */
	int year;
	int month;
	int mday;
	long int pdate;
}VERSION_INFORMATION, *LPVERSION_INFORMATION;


typedef struct 
{
	BOOL bIsScreensSaverActive;
	DWORD dwScreenSaverTimeOut;
}SCREENSAVERINFO, *LPSCREENSAVERINFO;

void GetAppVersionFromSourceFile(const char *, LPVERSION_INFORMATION);
void LoadSystemTrayIcon(HINSTANCE, HWND, HICON, UINT, UINT, LPTSTR, DWORD);

void SaveSettings(HWND, UINT);

void GetScreenSaverInfo(LPSCREENSAVERINFO);
