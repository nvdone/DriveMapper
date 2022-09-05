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

Drive::Drive()
{
	Letter = L"";
	Path = L"";
	Persistent = 0;
	Locked = 0;
}

Drive::Drive(const wchar_t *p_letter, const wchar_t *p_path, DWORD p_persistent, DWORD p_locked)
{
	Letter = p_letter;
	Path = p_path;
	Persistent = p_persistent; 
	Locked = p_locked;
}

Drive::~Drive()
{
}

void Drive::Update(const wchar_t *p_path, DWORD p_persistent)
{
	Path = p_path;
	Persistent = p_persistent;
}

void Drive::ShowInListView(HWND hAppLV, int itemId)
{
	LV_ITEM lv_item;
	memset(&lv_item, 0, sizeof(LV_ITEM));
	lv_item.mask = LVIF_TEXT;
	lv_item.iItem = itemId;
	lv_item.iSubItem = 0;
	lv_item.pszText = (wchar_t*)Letter.c_str();
	ListView_InsertItem(hAppLV, &lv_item);
	ListView_SetItemText(hAppLV, itemId, 1, (wchar_t*)Path.c_str());
}
