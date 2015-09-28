/*
 * Copytight (2015) CM Jarquin  c.m.jarquin@gmail.com
 */

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <shellapi.h>
#include "resource.h"

/* Application version information. */
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


/* Screensaver information. */
typedef struct 
{
	/* Screensaver status. */
	BOOL bIsScreensSaverActive;

	/* Number of seconds of inactivity
	 * before the system launches the
	 * screensaver. */
	DWORD dwScreenSaverTimeOut;
}SCREENSAVERINFO, *LPSCREENSAVERINFO;

/* Try builing the current version according to
 * date/time when the specified file was last
 * modified. */
void GetAppVersionFromSourceFile(const char *, LPVERSION_INFORMATION);

/* Modified the system tray icon according to the current settings
 * of the application. */
void LoadSystemTrayIcon(HINSTANCE, HWND, HICON, UINT, UINT, LPTSTR, DWORD);


/* Save the user settings to the registry. */
void SaveSettings(HWND, UINT);

/* The current status of the screensaver. */
void GetScreenSaverInfo(LPSCREENSAVERINFO);
