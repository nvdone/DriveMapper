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

#ifndef ITEMDIALOG_HPP
#define ITEMDIALOG_HPP

#include "stdafx.h"

class ItemDialog : public Window
{
	private:

		HWND hWndDriveLabel, hWndDriveBox, hWndPathLabel, hWndPathCombo, hWndPersistCheck, hWndSetButton, hWndCloseButton;

	protected:

		Drive *item;
		HFONT font;

		const list<wstring> *options;
		int persistenceEnforced;

	public:

		ItemDialog(HINSTANCE p_hInstance, HWND p_hParent, const wchar_t *p_windowName, const RECT *p_windowRect, Drive *p_item, HFONT p_font, const list<wstring> *p_options, int p_persistenceEnforced)
			: Window(p_hInstance, p_hParent, p_windowName, p_windowRect)
		{
			hWndDriveLabel = hWndDriveBox = hWndPathLabel = hWndPathCombo = hWndPersistCheck = hWndSetButton = hWndCloseButton = NULL;
			item = p_item;
			font = p_font;
			options = p_options;
			persistenceEnforced = p_persistenceEnforced;
		}

		~ItemDialog(){}

		BOOL DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
#endif
