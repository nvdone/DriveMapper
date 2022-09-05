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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "stdafx.h"

#define WINDOW_WIDTH_MIN 700
#define WINDOW_HEIGHT_MIN 590
#define WINDOW_NAME L"NVD DriveMapper"

class MainWindow : public Window
{
	private:

		HWND hDrivesGroupBox, hButtonsGroupBox, hDrivesListView, hEditButton, hCloseButton;
		double dpi;
		int buttonsGroupBoxWidth;
		HFONT font;

		Drivemap *driveMap;
		const list<wstring> *options;
		int lockFloppy;
		int persistence;

	protected:

		void CreateChildComponents();
		void EditSelectedItem();

	public:
	
		MainWindow(HINSTANCE p_hInstance, HWND p_hParent, const wchar_t *p_windowName, const RECT *p_windowRect, const list<wstring> *p_options, int p_lockFloppy, int p_persistence);
		~MainWindow();
		LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
