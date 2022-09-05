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

Drivemap::Drivemap(int lockFloppy)
{
	memcpy(alphabet, L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 * sizeof(wchar_t));
	drivesLoadBasicInfo(lockFloppy);
	drivesAppendSharedFoldersData();
	drivesSetPersistenceInfo();
}
	
Drivemap::~Drivemap()
{
	for (int i = 0; i < 26; i++)
	{
		if (drives[i])
			delete drives[i];
	}
}

Drive *Drivemap::Get(int pos)
{
	if (pos < 0 || pos > 25)
		throw("Drive index out of bounds!");

	return drives[pos];
}

void Drivemap::drivesLoadBasicInfo(int lockFloppy)
{
	DWORD logicalDrives = GetLogicalDrives();

	wchar_t *letter = new wchar_t[4];
	wchar_t *path = NULL;
	DWORD type = 0;
	DWORD persistent = 0;
	DWORD locked = 0;
	memset(letter, 0, 4 * sizeof(wchar_t));
	memcpy(letter, L"A:\\", wcslen(L"A:\\") * sizeof(wchar_t));

	for (int i = 0; i < 26; i++)
	{
		letter[0] = alphabet[i];

		if (logicalDrives & (1 << i))
		{
			type = GetDriveType(letter);
			path = NULL;
			persistent = 1;
			locked = 1;
			switch (type)
			{
			case DRIVE_UNKNOWN:
				path = (wchar_t*)L"<DRIVE_UNKNOWN>";
				break;
			case DRIVE_NO_ROOT_DIR:
				path = (wchar_t*)L"<DRIVE_NO_ROOT_DIR>";
				break;
			case DRIVE_REMOVABLE:
				path = (wchar_t*)L"<DRIVE_REMOVABLE>";
				break;
			case DRIVE_FIXED:
				path = (wchar_t*)L"<DRIVE_FIXED>";
				break;
			case DRIVE_REMOTE:
				path = (wchar_t*)L"<DRIVE_REMOTE>";
				persistent = 0;
				locked = 0;
				break;
			case DRIVE_CDROM:
				path = (wchar_t*)L"<DRIVE_CDROM>";
				break;
			case DRIVE_RAMDISK:
				path = (wchar_t*)L"<DRIVE_RAMDISK>";
				break;
			default:
				path = (wchar_t*)L"<EPIC_FAIL>";
				break;
			}
		}
		else
		{
			path = (wchar_t*)L"";
			persistent = 0;
			locked = 0;
		}

		if (i < 2 && !locked && lockFloppy)
		{
			if (!path || wcslen(path) == 0)
				path = (wchar_t*)L"<DRIVE_FLOPPY>";
			persistent = 1;
			locked = 1;
		}

		drives[i] = new Drive(letter, path, persistent, locked);
	}

	delete[]letter;
}

void Drivemap::drivesAppendSharedFoldersData()
{
	HANDLE enumResourceHandle = NULL;
	NETRESOURCE *netResources = NULL;
	DWORD netResourcesCount = -1;
	DWORD netResourcesBufSize = BUF_SIZE;

	netResources = (NETRESOURCE*)malloc(BUF_SIZE);
	if (!netResources)
		throw "Malloc failed!";

	wchar_t *buf = new wchar_t[BUF_SIZE];

	DWORD result = WNetOpenEnum(RESOURCE_CONNECTED, RESOURCETYPE_DISK, 0, NULL, &enumResourceHandle);
	if (result == NO_ERROR)
	{
		do
		{
			memset(netResources, 0, BUF_SIZE);
			result = WNetEnumResource(enumResourceHandle, &netResourcesCount, netResources, &netResourcesBufSize);

			if (result == ERROR_NO_MORE_ITEMS)
				break;

			if (result != NO_ERROR)
			{
				throw L"WNetEnumResource failed!";
			}

			for (DWORD i = 0; i < netResourcesCount; i++)
			{
				memset(buf, 0, BUF_SIZE * sizeof(wchar_t));
				swprintf_s(buf, BUF_SIZE - 1, L"%s", netResources[i].lpLocalName);
#ifndef __WATCOMC__
				_wcsupr_s(buf, BUF_SIZE - 1);
#else
				_wcsupr(buf);
#endif

				for (int j = 0; j < 26; j++)
				{
					if (alphabet[j] == buf[0])
					{
						drives[j]->Path = netResources[i].lpRemoteName;
					}
				}
			}
		} while (result == NO_ERROR);
	}

	delete[] buf;
	free(netResources);
}

void Drivemap::drivesSetPersistenceInfo()
{
	HKEY key;
	DWORD dwIndex = 0;
	DWORD lpcchName = BUF_SIZE;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Network", 0, KEY_READ, &key) != ERROR_SUCCESS)
		return;

	wchar_t* buf = new wchar_t[BUF_SIZE];
	memset(buf, 0, BUF_SIZE * sizeof(wchar_t));

	while (RegEnumKeyExW(key, dwIndex++, (LPWSTR)buf, &lpcchName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		for (int i = 0; i < 26; i++)
		{
			if (drives[i]->Letter.c_str()[0] == towupper(buf[0]) && drives[i]->Path.length())
			{
				drives[i]->Persistent = 1;
				break;
			}
		}

		lpcchName = BUF_SIZE;
		memset(buf, 0, BUF_SIZE * sizeof(wchar_t));
	}

	delete[]buf;
	RegCloseKey(key);
}


void Drivemap::ShowInListView(HWND hAppLV)
{
	ListView_DeleteAllItems(hAppLV);

	for (int i = 0; i < 26; i++)
	{
		drives[i]->ShowInListView(hAppLV, i);
	}
}
