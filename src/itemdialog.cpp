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

BOOL ItemDialog::DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wchar_t *drive, *path;
	list<wstring>::const_iterator it;
	int present = 0;
	DWORD persistent = 0;

	switch (message)
	{
		case WM_INITDIALOG:

			hWndDriveLabel = GetDlgItem(hWnd, IDC_DRIVELABEL);
			hWndDriveBox = GetDlgItem(hWnd, IDC_DRIVEBOX);
			hWndPathLabel = GetDlgItem(hWnd, IDC_PATHLABEL);
			hWndPathCombo = GetDlgItem(hWnd, IDC_PATHCOMBO);
			hWndPersistCheck = GetDlgItem(hWnd, IDC_PERSISTENTCHECK);
			hWndSetButton = GetDlgItem(hWnd, IDC_SETBTN);
			hWndCloseButton = GetDlgItem(hWnd, IDCANCEL); 
			
			SendMessage(hWndDriveLabel, WM_SETFONT, (WPARAM)font, TRUE);
			SendMessage(hWndDriveBox, WM_SETFONT, (WPARAM)font, TRUE);
			SendMessage(hWndPathLabel, WM_SETFONT, (WPARAM)font, TRUE);
			SendMessage(hWndPathCombo, WM_SETFONT, (WPARAM)font, TRUE);
			SendMessage(hWndPersistCheck, WM_SETFONT, (WPARAM)font, TRUE);
			SendMessage(hWndSetButton, WM_SETFONT, (WPARAM)font, TRUE);
			SendMessage(hWndCloseButton, WM_SETFONT, (WPARAM)font, TRUE);

			SetDlgItemText(hWnd, IDC_DRIVEBOX, item->Letter.c_str());

			SendMessage(hWndPathCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"");

			for (it = options->begin(); it != options->end(); it++)
			{
				SendMessage(hWndPathCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)it->c_str());
				if (!present && StringEqualCI(it->c_str(), item->Path.c_str()))
					present = 1;
			}

			if(!present && item->Path.length())
				SendMessage(hWndPathCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)item->Path.c_str());

			if(item->Path.length() > 0)
			{
				SendMessage(hWndPathCombo, (UINT)CB_SELECTSTRING, (WPARAM)0, (LPARAM)item->Path.c_str());
				SendMessage(hWndPathCombo, (UINT)CB_SETEDITSEL, (WPARAM)0, MAKELPARAM(0, 0));
			}

			if (item->Persistent)
				SendMessage(hWndPersistCheck, BM_SETCHECK, BST_CHECKED, 0);

			if (item->Locked)
			{
				EnableWindow(hWndPathCombo, FALSE);
				EnableWindow(hWndPersistCheck, FALSE);
				EnableWindow(hWndSetButton, FALSE);
			}

			if (persistenceEnforced)
				EnableWindow(hWndPersistCheck, FALSE);

			SendMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)hWndCloseButton, TRUE);

		break;
			
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_PATHCOMBO:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						if (persistenceEnforced)
							SendMessage(hWndPersistCheck, BM_SETCHECK, BST_CHECKED, 0);

						LRESULT index = SendMessage(hWndPathCombo, CB_GETCURSEL, 0, 0);
						if (!SendMessage(hWndPathCombo, CB_GETLBTEXTLEN, index, 0))
							SendMessage(hWndPersistCheck, BM_SETCHECK, BST_UNCHECKED, 0);
					}
					if (HIWORD(wParam) == CBN_EDITCHANGE)
					{
						if (persistenceEnforced)
							SendMessage(hWndPersistCheck, BM_SETCHECK, BST_CHECKED, 0);

						if (!GetWindowTextLength(hWndPathCombo))
							SendMessage(hWndPersistCheck, BM_SETCHECK, BST_UNCHECKED, 0);
					}
				break;

				case IDC_PERSISTENTCHECK:
					if (!GetWindowTextLength(hWndPathCombo))
						SendMessage(hWndPersistCheck, BM_SETCHECK, BST_UNCHECKED, 0);
				break;

				case IDC_SETBTN:
					drive = new wchar_t[BUF_SIZE];
					memset(drive, 0, BUF_SIZE * sizeof(wchar_t));
					GetDlgItemText(hWnd, IDC_DRIVEBOX, drive, BUF_SIZE - 1);
					drive[2] = 0;
					
					path = new wchar_t[BUF_SIZE];
					memset(path, 0, BUF_SIZE * sizeof(wchar_t));
					GetDlgItemText(hWnd, IDC_PATHCOMBO, path, BUF_SIZE - 1);

					if (!wcscmp(path, L"about:drivemapper"))
					{
						MessageBox(NULL, L"NVD Drivemapper\r\n\r\n(c) 2022, Nikolay Dudkin\r\nnvd@nvd.one", L"About", MB_OK | MB_ICONINFORMATION);
						delete[]path;
						delete[]drive;
						break;
					}

					if (wcslen(path) > 0 && !PathFileExists(path))
					{
						MessageBox(NULL, L"Path does not exist or you do not have required access permissions.", L"NVD DriveMapper error", MB_ICONSTOP | MB_OK);
						delete[]path;
						delete[]drive;
						break;
					}

					NETRESOURCE netResource;
					memset(&netResource, 0, sizeof(NETRESOURCE));
					netResource.dwType = RESOURCETYPE_DISK;
					netResource.lpLocalName = (LPWSTR) drive;
					netResource.lpRemoteName = (LPWSTR) path;

					persistent = 1;
					if (IsDlgButtonChecked(hWnd, IDC_PERSISTENTCHECK) == BST_UNCHECKED)
						persistent = 0;

					WNetCancelConnection2(netResource.lpLocalName, CONNECT_UPDATE_PROFILE, TRUE);

					if (wcslen(path) > 0 && WNetAddConnection2(&netResource, NULL, NULL, persistent ? CONNECT_UPDATE_PROFILE : 0) != NO_ERROR)
					{
						MessageBox(NULL, L"Failed to map drive.", L"NVD DriveMapper error", MB_ICONSTOP | MB_OK);
						delete[]path;
						delete[]drive;
						break;
					}
					
					item->Update(path, persistent);

					delete[]path;
					delete[]drive;
					
					EndDialog(hWnd, 0);

				break;
				
				case IDCANCEL:
					EndDialog(hWnd, 1);
				break;
				
				default:
				break;
			}
		break;
		
		default:
		break;
	}

	return Window::DialogProc(hWnd, message, wParam, lParam);
}
