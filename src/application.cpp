//NVD DriveMapper
//Copyright © 2022, Nikolay Dudkin

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License
//along with this program.If not, see<https://www.gnu.org/licenses/>.

#include "stdafx.h"

MainWindow *mainWindow;

void loadOptions(const wchar_t *server, list<wstring> *options, int showAdminShares);
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifndef __WATCOMC__
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int)
#endif
{
	CmdLine *cmdLine = new CmdLine(lpCmdLine);

	if (cmdLine->HasParam(L"-help", 1) || cmdLine->HasParam(L"-?", 1))
	{
		MessageBox(NULL, L"NVD DriveMapper\r\n\r\n(c) 2022, Nikolay Dudkin\r\nnvd@nvd.one\r\n\r\nUsage: drivemapper.exe <ServerNamesToMapDrivesFromSeparatedBySpace> [-?] [-help] [-persistence:enforce] [-lockfloppy:yes] [-showadminshares:yes]", L"About", MB_OK | MB_ICONINFORMATION);
		return 0;
	}

	int lockFloppy = 0;
	if (cmdLine->GetOptionValue(L"-lockfloppy", 1, 1))
	{
		if (!wcscmp(cmdLine->GetOptionValue(L"-lockfloppy", 1, 1), L"yes"))
			lockFloppy = 1;
		else
			lockFloppy = 0;
	}

	int persistence = 0;
	if (cmdLine->GetOptionValue(L"-persistence", 1, 1))
	{
		if (!wcscmp(cmdLine->GetOptionValue(L"-persistence", 1, 1), L"enforce"))
			persistence = 1;
		else
			persistence = 0;
	}

	int showAdminShares = 0;
	if (cmdLine->GetOptionValue(L"-showadminshares", 1, 1))
	{
		if (!wcscmp(cmdLine->GetOptionValue(L"-showadminshares", 1, 1), L"yes"))
			showAdminShares = 1;
		else
			showAdminShares = 0;
	}

	list<wstring> options;
	if (cmdLine->CountCommands() > 0)
	{
		for (int i = 0; i < cmdLine->CountCommands(); i++)
		{
			loadOptions(cmdLine->GetCommand(i)->GetName(0), &options, showAdminShares);
		}
	}

	delete cmdLine;

	mainWindow = NULL;

	RECT rect = { 0, 0, 0, 0 };

	try
	{
		INITCOMMONCONTROLSEX icc;
		memset(&icc, 0, sizeof(INITCOMMONCONTROLSEX));
		icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icc.dwICC = ICC_LISTVIEW_CLASSES;
		if (!InitCommonControlsEx(&icc))
			throw L"InitCommonControlsEx failed!";

		HDC dc = GetDC(NULL);
		double dpi = GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSX) / 96.0;
		ReleaseDC(NULL, dc);

		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		rect.left = rect.left + (rect.right - rect.left - (int)(WINDOW_WIDTH_MIN * dpi))/2;
		if (rect.left < 0)
			rect.left = 0;
		rect.top = rect.top + (rect.bottom - rect.top - (int)(WINDOW_HEIGHT_MIN * dpi))/2;
		if (rect.top < 0)
			rect.top = 0;
		rect.right = rect.left + (int)(WINDOW_WIDTH_MIN * dpi);
		rect.bottom = rect.top + (int)(WINDOW_HEIGHT_MIN * dpi);
		
		mainWindow = (MainWindow *)new MainWindow(hInstance, NULL, (wchar_t *)WINDOW_NAME, &rect, &options, lockFloppy, persistence);
		mainWindow->Create(MainWindowProc, WS_OVERLAPPEDWINDOW, 0, SW_SHOW);
		mainWindow->MessageLoop();
	}
	catch(wchar_t *message)
	{
		MessageBox(NULL, message, L"NVD DriveMapper exception", MB_OK | MB_ICONSTOP);
		if (mainWindow)
			delete mainWindow;
		return 1;
	}

	if (mainWindow)
		delete mainWindow;

	return 0;
}

void loadOptions(const wchar_t *server, list<wstring> *options, int showAdminShares)
{
	list<wstring>::iterator it;
	wchar_t *buf = new wchar_t[BUF_SIZE];
	DWORD result = 0;
	
	PSHARE_INFO_1 shareInfos = NULL;
	DWORD shareInfosRead = 0;
	DWORD shareInfosTotal = 0;
	DWORD netShareInfoResumeHandle = 0;

	do
	{
		result = NetShareEnum((LPWSTR)server, 1, (LPBYTE*)&shareInfos, MAX_PREFERRED_LENGTH, &shareInfosRead, &shareInfosTotal, &netShareInfoResumeHandle);
		if (result == ERROR_SUCCESS || result == ERROR_MORE_DATA)
		{
			for (DWORD i = 0; i < shareInfosRead; i++)
			{
				if ((shareInfos[i].shi1_type & STYPE_DISKTREE) == STYPE_DISKTREE)
				{
					if (showAdminShares || (shareInfos[i].shi1_type & STYPE_SPECIAL) != STYPE_SPECIAL)
					{
						memset(buf, 0, BUF_SIZE * sizeof(wchar_t));
						swprintf_s(buf, (BUF_SIZE - 1), L"\\\\%s\\%s", server, shareInfos[i].shi1_netname);

						int present = 0;
						for (it = options->begin(); it != options->end(); it++)
						{
							if (StringEqualCI(it->c_str(), buf))
							{
								present = 1;
								break;
							}
						}
						if(!present)
							options->push_back(buf);
					}
				}
			}
			NetApiBufferFree(shareInfos);
		}
		else
		{
			break;
		}
	} while (result == ERROR_MORE_DATA);

	delete[]buf;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return mainWindow->WindowProc(hWnd, message, wParam, lParam);
}
