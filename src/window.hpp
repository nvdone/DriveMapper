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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "stdafx.h"

class Window
{
	protected:

		HINSTANCE hInstance;
		HWND hParent;
		wchar_t *className;
		wchar_t *windowName;
		RECT windowRect;

		HICON hMainIcon;
		HICON hMainIconSmall;

		virtual void CreateChildComponents();
		virtual void LoadResources();
		virtual void UnloadResources();

	public:

		HWND hWindow;

		Window();
		Window(HINSTANCE p_hInstance, HWND p_hParent, const wchar_t *p_windowName, const RECT *p_windowRect);
		virtual ~Window();

		int Create(WNDPROC WndProc, DWORD style, DWORD exStyle, int cmdShow);
		INT_PTR Create(DLGPROC dialogProc, LPCTSTR dialogTemplate);
		void MessageLoop();

		virtual LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual BOOL DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
