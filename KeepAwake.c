/*
 * Copytight (2015) CM Jarquin  c.m.jarquin@gmail.com
 */

#include <windows.h>
#include <tchar.h>
#include <dbt.h>
#include "utilities.h"
//#include "powermgmt.h"
#include "resource.h"

/* Name of the mutex object. */
#define MUTEX_INSTANCE_NAME _T("KeepAwakeMutexObj")

/* Message handler for the main application. */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/* Message handler for the "About..." dialog box. */
BOOL CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

/* Checks the status of the screensaver. */
BOOL IsSystemScreenSaverSet(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASSEX wcex = {0};
	HWND hwndWindow;

	TCHAR szErrMsg[1024] = {0};

    /* Handle for named mutex. */
	HANDLE hMutexInstObj = NULL;

	/* Window creation settings. */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICONMAINAPP));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = SZAPPNAME;

	RegisterClassEx(&wcex);

	/* 
	 * Only allow one (1) instance to run. 
	 */
	if(!(hMutexInstObj = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_INSTANCE_NAME)))
	{
		hMutexInstObj = CreateMutex(NULL, TRUE, MUTEX_INSTANCE_NAME);
	}
	else
	{
		MessageBox(NULL, _T("Another instance of this program is already running. PID:"),
			       SZAPPTITLE, MB_OK | MB_ICONINFORMATION);
		return -1;
	}

	/*
	 * Create the window.
	 */
	hwndWindow = CreateWindowEx(WS_EX_TOOLWINDOW, 
		                        SZAPPNAME, _T("Keep Awake"),
		                        WS_POPUPWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
								0, 0, NULL, NULL, hInstance, NULL); //(w: 264, h:108)

	if(hwndWindow == NULL)
	{
		wsprintf(szErrMsg, _T("Failed to create window. Error: 0x%08X"), GetLastError());
		MessageBox(NULL, szErrMsg, SZAPPNAME, MB_ICONERROR);

		return -1;
	}

	ShowWindow(hwndWindow, nCmdShow);
	UpdateWindow(hwndWindow);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	/* Release the mutex object. */
	CloseHandle(hMutexInstObj);
	ReleaseMutex(hMutexInstObj);

	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInst;
	RECT wrc, crc;
	TCHAR szSysMsg[1024] = {0};

	static HMENU hCtxMenu, hTrackPopUpMenu;
	POINT pt;
	HICON hIcon;

	/*
	 * Screenver specific variables.
	 */
	SCREENSAVERINFO ssi;
	TCHAR szScreenSaverMsg[1024] = {0};

	UINT uMenuItemState = 0UL;

	switch(uMsg)
	{
	   case WM_CREATE:
		   
		   /* 
		    * Check to see if the screensaver is set. 
			*/
		   /*
		   if(!IsSystemScreenSaverSet())
		   {
			   MessageBox(hwnd, 
				          _T("No screensaver is set for this system."), 
				          SZAPPTITLE, MB_OK | MB_ICONINFORMATION);
			   LoadSystemTrayIcon((HINSTANCE)GetModuleHandle(NULL), 
				                  hwnd, NULL, TRAY_ICON_RESOURCE_ID, 
								  WM_SYSTEMTRAYICONMSG, NULL, 
								  LSTI_STATECHANGEEXIT);
			   
			   return -1;
		   }
		   */

		   hInst = ((LPCREATESTRUCT)lParam)->hInstance;

		   GetWindowRect(GetDesktopWindow(), &wrc);
		   GetWindowRect(hwnd, &crc);

		   /* 
		    * Move the main window to the right bottom corner. 
			*/
		   SetWindowPos(hwnd, HWND_TOP, 
			            wrc.right - (crc.right - crc.left) - 6, 
			            wrc.bottom - (crc.bottom - crc.top) - 44, 
						0, 0, SWP_NOSIZE);

		   hCtxMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_CONTEXTMENU));
		   hTrackPopUpMenu = GetSubMenu(hCtxMenu, 0);

		   GetScreenSaverInfo(&ssi);
		   //DeviceIsRunningOnBatteries();

		   /* 
		    * Minimize to the system notification area.
			*/
		   LoadSystemTrayIcon(hInst, hwnd, NULL, TRAY_ICON_RESOURCE_ID, 
			                  WM_SYSTEMTRAYICONMSG, SZAPPTITLE, 
							  LSTI_STATEINITIALIZE);
	   break;

	   case WM_COMMAND:
		   switch(LOWORD(wParam))
		   {
			   /* 
			    * Pop up menu selection... 
			    */
			   case ID_POPUPMENU_KEEPSYSTEMAWAKE:
			   {
				   uMenuItemState = GetMenuState(hTrackPopUpMenu, ID_POPUPMENU_KEEPSYSTEMAWAKE,
												 MF_BYCOMMAND);

				   GetScreenSaverInfo(&ssi);

				   if((uMenuItemState & MF_UNCHECKED) == MF_UNCHECKED)
				   {
					   CheckMenuItem(hTrackPopUpMenu, ID_POPUPMENU_KEEPSYSTEMAWAKE, 
									 MF_BYCOMMAND | MF_CHECKED);
					   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONSYSTRAYACTIVE));

					   wsprintf(szSysMsg, _T("Keep system awake..."));

					   if(ssi.bIsScreensSaverActive && (ssi.dwScreenSaverTimeOut > 0UL))
					   {
						   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, &ssi.bIsScreensSaverActive, SPIF_UPDATEINIFILE);
						   //SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, FALSE, &ssi.dwScreenSaverTimeOut, SPIF_UPDATEINIFILE); 
					   }

					   LoadSystemTrayIcon(hInst, hwnd, hIcon, TRAY_ICON_RESOURCE_ID, 
			                              WM_SYSTEMTRAYICONMSG, szSysMsg, LSTI_STATECHANGEUPDATE);
				   }

				   if((uMenuItemState & MF_CHECKED) == MF_CHECKED)
				   {
					   CheckMenuItem(hTrackPopUpMenu, ID_POPUPMENU_KEEPSYSTEMAWAKE, 
									 MF_BYCOMMAND | MF_UNCHECKED);
					   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONMAINAPP));

					   wsprintf(szSysMsg, _T("System set to sleep..."));
					   

					   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, &ssi.bIsScreensSaverActive, SPIF_UPDATEINIFILE);
					   //SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, TRUE, &ssi.dwScreenSaverTimeOut, SPIF_UPDATEINIFILE);

					   LoadSystemTrayIcon(hInst, hwnd, hIcon, TRAY_ICON_RESOURCE_ID, 
			                              WM_SYSTEMTRAYICONMSG, szSysMsg, LSTI_STATECHANGEUPDATE);
				   }

			   }
			   break;

			   case ID_SETTINGS_RUNONSYSTEMSTARTUP:
			   {
				   /*
				    * Autorun on windows startup registry settings.
					*/
				   HKEY hRegSysRun = NULL;
				   long lregRetVal = 0;
				   DWORD dwcbData = 0;
				   TCHAR szModuleName[MAX_PATH] = {0};
				   TCHAR szRegAutoRunKey[256] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

				   uMenuItemState = GetMenuState(hTrackPopUpMenu,ID_SETTINGS_RUNONSYSTEMSTARTUP,
												 MF_BYCOMMAND);

				   GetModuleFileName(NULL, szModuleName, MAX_PATH);

				   if((uMenuItemState & MF_UNCHECKED) == MF_UNCHECKED)
				   {
					   CheckMenuItem(hTrackPopUpMenu, 
						             ID_SETTINGS_RUNONSYSTEMSTARTUP, MF_BYCOMMAND | MF_CHECKED);

					   lregRetVal = RegCreateKey(HKEY_CURRENT_USER,
						                         szRegAutoRunKey, &hRegSysRun);
					   RegSetValueEx(hRegSysRun, SZAPPTITLE, 0, REG_SZ, (LPBYTE)szModuleName, 
						   (_tcslen(szModuleName) + 1) * 2);
					   RegCloseKey(hRegSysRun);
				   }

				   if((uMenuItemState & MF_CHECKED) == MF_CHECKED)
				   {
					  CheckMenuItem(hTrackPopUpMenu, 
						            ID_SETTINGS_RUNONSYSTEMSTARTUP, MF_BYCOMMAND | MF_UNCHECKED);
					  
					  lregRetVal = RegOpenKeyEx(HKEY_CURRENT_USER, szRegAutoRunKey, 
								                0, KEY_QUERY_VALUE, &hRegSysRun);
					  
					  if(lregRetVal == ERROR_SUCCESS)
					  {
						  lregRetVal = RegQueryValueEx(hRegSysRun, SZAPPTITLE, NULL, NULL, 
							                           NULL, &dwcbData);
						  
						  if(lregRetVal == ERROR_SUCCESS && dwcbData > 0)
						  {
							  lregRetVal = RegOpenKeyEx(HKEY_CURRENT_USER, szRegAutoRunKey, 
														0, KEY_SET_VALUE, &hRegSysRun);

							  if(lregRetVal == ERROR_SUCCESS)
							  {
								  RegDeleteValue(hRegSysRun, SZAPPTITLE);
								  RegCloseKey(hRegSysRun);
							  }
						  }
					  }
				   }
			   }
			   break;

			   case ID_POPUPMENU_EXIT:
			   {
		           GetScreenSaverInfo(&ssi);
				   uMenuItemState = GetMenuState(hTrackPopUpMenu, 
					                             ID_POPUPMENU_KEEPSYSTEMAWAKE,
												 MF_BYCOMMAND);
				   
				   wsprintf(szScreenSaverMsg, _T("screensaver active: %d, timeout: %lu secs"), 
					        ssi.bIsScreensSaverActive, ssi.dwScreenSaverTimeOut);
				   MessageBox(hwnd, szScreenSaverMsg, SZAPPTITLE, MB_OK);
				   

				   if((uMenuItemState & MF_CHECKED) == MF_CHECKED)
				   {
					   if(MessageBox(hwnd,
						             _T("The setting for keeping the system awake is active. Disable it and exit?"), 
									 SZAPPTITLE, MB_ICONWARNING | MB_YESNO) == IDYES)
					   {
				           LoadSystemTrayIcon(hInst, hwnd, NULL, TRAY_ICON_RESOURCE_ID, 
			                                  WM_SYSTEMTRAYICONMSG, NULL, LSTI_STATECHANGEEXIT);
				           
						   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, &ssi.bIsScreensSaverActive, SPIF_UPDATEINIFILE);
						   //SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, TRUE, &ssi.dwScreenSaverTimeOut, SPIF_UPDATEINIFILE);

				           PostQuitMessage(0);
						   //DestroyWindow(hwnd);
					   }
				   }
				   else
				   {
					   LoadSystemTrayIcon(hInst, hwnd, NULL, TRAY_ICON_RESOURCE_ID, 
			                              WM_SYSTEMTRAYICONMSG, NULL, LSTI_STATECHANGEEXIT);
				       
					   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, &ssi.bIsScreensSaverActive, SPIF_UPDATEINIFILE);
					   //SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, TRUE, &ssi.dwScreenSaverTimeOut, SPIF_UPDATEINIFILE);
					   
		               PostQuitMessage(0);
				   }
			   }
			   break;

			   case ID_POPUPMENU_ABOUT:
				   DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUTWINDOW), 
					         hwnd, AboutDialogProc);
			   break;
		   }
	   break;

       case WM_SYSTEMTRAYICONMSG:
		   switch(lParam)
		   {
	          case WM_LBUTTONDBLCLK:
		      case WM_LBUTTONUP:
		      case WM_RBUTTONUP:
			  case WM_CONTEXTMENU:
				  GetCursorPos(&pt);
				  SetForegroundWindow(hwnd);
				  TrackPopupMenu(hTrackPopUpMenu, TPM_RIGHTBUTTON, 
					             pt.x, pt.y, 0, hwnd, NULL);
			  break;
		   }
	   return 0;

	   /*
	   case WM_CLOSE:   
		   GetScreenSaverInfo(&ssi);
		   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, &ssi.bIsScreensSaverActive, SPIF_UPDATEINIFILE);
		   //SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, TRUE, &ssi.dwScreenSaverTimeOut, SPIF_UPDATEINIFILE);
		   LoadSystemTrayIcon(hInst, hwnd, NULL, TRAY_ICON_RESOURCE_ID, 
			                  WM_SYSTEMTRAYICONMSG, NULL, LSTI_STATECHANGEEXIT);
		   DestroyWindow(hwnd);
		   CloseWindow(hwnd);
	   break;
	   */

	   case WM_DESTROY:
		   GetScreenSaverInfo(&ssi);
		   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, &ssi.bIsScreensSaverActive, SPIF_UPDATEINIFILE);
		   //SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, TRUE, &ssi.dwScreenSaverTimeOut, SPIF_UPDATEINIFILE);

		   LoadSystemTrayIcon(hInst, hwnd, NULL, TRAY_ICON_RESOURCE_ID, 
			                  WM_SYSTEMTRAYICONMSG, NULL, 
							  LSTI_STATECHANGEEXIT);

		   CloseWindow(hwnd);
		   DestroyWindow(hwnd);
	  break;

	  
	   case WM_POWERBROADCAST:
	   {
		   switch(wParam)
		   {
		       case PBT_APMPOWERSTATUSCHANGE:
			   {
				   MessageBox(hwnd, _T("Power status has changed."), SZAPPTITLE, MB_OK);
				   return TRUE;
			   }

			   case PBT_POWERSETTINGCHANGE:
			   {
				   POWERBROADCAST_SETTING *pps = (POWERBROADCAST_SETTING *)(lParam);
				   MessageBox(hwnd, _T("A power setting change event has been received."),
					          SZAPPTITLE, MB_OK);
			
				   if(memcmp(&GUID_ACDC_POWER_SOURCE, &pps->PowerSetting, sizeof(GUID)) == 0)
				   {
					   if(*(DWORD *)(&pps->Data) == PoAc)
						   MessageBox(hwnd, _T("Powered by AC power source"), 
							          SZAPPTITLE, MB_OK);
					   else if(*(DWORD *)(&pps->Data) == PoDc)
						   MessageBox(hwnd, _T("Powered by onboard battery."),
							          SZAPPTITLE, MB_OK);
					   else if(*(DWORD *)(&pps->Data) == PoHot)
						   MessageBox(hwnd, _T("Powered by a short-term source / UPS device."),
							          SZAPPTITLE, MB_OK);
					   else
					   {
						   MessageBox(hwnd, _T("Unknown."), SZAPPTITLE, MB_OK);
					   }
				   }
		       }
			   return TRUE;
		   }

	   return TRUE;
	}

	   default:
		   return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL IsSystemScreenSaverSet(void)
{
	/*
	 * Checks to see if the screensaver is set.
	 *
	 * Code adopted/taken from <http://support2.microsoft.com/kb/318781>
	 *
	 */

	HKEY hKeyScreenSaver = NULL;
	LONG lResult = 0L;
	LONG lScreenSaver = 0L;
	DWORD dwScreenSaverResult = 0L;
	BOOL bScreenSaverSet = FALSE;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, 
		                   TEXT("Control Panel\\Desktop"),
		                   0, KEY_QUERY_VALUE, &hKeyScreenSaver);
	
	if(lResult == ERROR_SUCCESS)
	{
		lScreenSaver = RegQueryValueEx(hKeyScreenSaver, 
			                           TEXT("SCRNSAVE.EXE"),
			                           NULL, NULL, NULL, 
									   &dwScreenSaverResult);


		if(lScreenSaver == ERROR_SUCCESS)
			bScreenSaverSet = TRUE;
		else
			bScreenSaverSet = FALSE;
	}

	RegCloseKey(hKeyScreenSaver);

	return bScreenSaverSet;
}

/*
 * About dialog windows message handler.
 */
BOOL CALLBACK AboutDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hStaticCtrlVerionText;
	VERSION_INFORMATION vi = {0};

	TCHAR szModuleExeName[MAX_PATH] = {0};
	
	RECT wrec, srec;

	switch(uMsg)
	{
	    case WM_INITDIALOG:
			
			GetAppVersionFromSourceFile(__DATE__, &vi);

			hStaticCtrlVerionText = GetDlgItem(hDlg, 
				                               IDC_STATIC_APPNAMEVERSION);

			wsprintf(szModuleExeName, TEXT("%s %d.%d.%d.%ld"), 
				     SZAPPTITLE, vi.majorVersion, vi.minorVersion,
					 vi.buildNumber, vi.pdate);


			SendMessage(hStaticCtrlVerionText, WM_SETTEXT, 0, 
				        (LPARAM)(LPTSTR)szModuleExeName);

			GetWindowRect(hDlg, &wrec);
			GetWindowRect(GetDesktopWindow(), &srec);
			SetWindowPos(hDlg, HWND_TOP, 
				         srec.right - (wrec.right - wrec.left) - 6,
				         srec.bottom - (wrec.bottom - wrec.top) - 48, 
						 0, 0, SWP_NOSIZE);
		break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			    case IDOK:
					EndDialog(hDlg, IDCANCEL);
				break;
			}
	    break;

		case WM_CLOSE:
			EndDialog(hDlg, IDCANCEL);
	    break;
	}

	return FALSE;
}
