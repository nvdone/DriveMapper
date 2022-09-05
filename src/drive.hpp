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

#ifndef DRIVE_HPP
#define DRIVE_HPP

#include "stdafx.h"

class Drive
{
	public:

		wstring Letter;
		wstring Path;
		DWORD Persistent;
		DWORD Locked;

		Drive();
		Drive(const wchar_t *p_letter, const wchar_t *p_path, DWORD p_persistent, DWORD p_locked);
		~Drive();

		void Update(const wchar_t *p_path, DWORD p_persistent);

		void ShowInListView(HWND hAppLV, int itemId);
};

#endif
