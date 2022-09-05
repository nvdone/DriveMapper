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
#include "mainwindow.hpp"
#include "itemdialog.hpp"

ItemDialog *itemDialog;

BOOL CALLBACK ItemDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return itemDialog->DialogProc(hWnd, message, wParam, lParam);
}

MainWindow::MainWindow(HINSTANCE p_hInstance, HWND p_hParent, const wchar_t *p_windowName, const RECT *p_windowRect, const list<wstring> *p_options, int p_lockfloppy, int p_persistence) : Window(p_hInstance, p_hParent, p_windowName, p_windowRect)
{
	hDrivesGroupBox = hButtonsGroupBox = hDrivesListView = hEditButton = hCloseButton = NULL;
	buttonsGroupBoxWidth = 0;
	dpi = 0;
	font = NULL;

	options = p_options;
	lockFloppy = p_lockfloppy;
	persistence = p_persistence;

	driveMap = new Drivemap(lockFloppy);
	
	HDC dc = GetDC(NULL);
	dpi = GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSX) / 96.0;
	ReleaseDC(NULL, dc);

	buttonsGroupBoxWidth = (int) (100.0 * dpi);
	
	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	font = CreateFontIndirect(&ncm.lfCaptionFont);
}

MainWindow::~MainWindow()
{
	if (driveMap)
		delete driveMap;
	DeleteObject(font);
}

LRESULT MainWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD windowColor = 0;

	switch (message)
	{
		case WM_SETFOCUS:
			SetFocus(hDrivesListView);
			break;

		case WM_SIZE:
			RECT rect;
			GetWindowRect(hWindow, &rect);
			if (rect.right - rect.left < WINDOW_WIDTH_MIN)
				MoveWindow(hWindow, rect.left, rect.top, WINDOW_WIDTH_MIN, rect.bottom - rect.top, TRUE);
			if (rect.bottom - rect.top < WINDOW_HEIGHT_MIN)
				MoveWindow(hWindow, rect.left, rect.top, rect.right - rect.left, WINDOW_HEIGHT_MIN, TRUE);
			GetClientRect(hWindow, &rect);
			MoveWindow(hDrivesGroupBox, 1, 1, rect.right - buttonsGroupBoxWidth - 1, rect.bottom - 1, TRUE);
			MoveWindow(hButtonsGroupBox, rect.right - buttonsGroupBoxWidth, 1, buttonsGroupBoxWidth, rect.bottom - 1, TRUE);
			MoveWindow(hEditButton, rect.right - buttonsGroupBoxWidth + (int)(4 * dpi), (int)(20 * dpi), buttonsGroupBoxWidth - (int)(8 * dpi), (int)(25 * dpi), TRUE);
			MoveWindow(hCloseButton, rect.right - buttonsGroupBoxWidth + (int)(4 * dpi), (int)(45 * dpi), buttonsGroupBoxWidth - (int)(8 * dpi), (int)(25 * dpi), TRUE);
			GetClientRect(hDrivesGroupBox, &rect);
			MoveWindow(hDrivesListView, (int)(4 * dpi), (int)(20 * dpi), rect.right -(int)(6 * dpi), rect.bottom - (int)(24 * dpi), TRUE);
			GetWindowRect(hDrivesListView, &rect);
			ListView_SetColumnWidth(hDrivesListView, 1, (LPARAM)(rect.right - rect.left - ListView_GetColumnWidth(hDrivesListView, 0)));
			break;

		case WM_NOTIFY:
			switch (LOWORD(wParam))
			{
				case ID_APPLISTVIEW:
					if (((LPNMHDR)lParam)->code == NM_DBLCLK)
					{
						EditSelectedItem();
					}
					if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
					{
						switch (((LPNMLVCUSTOMDRAW)lParam)->nmcd.dwDrawStage)
						{
							case CDDS_PREPAINT:
								return CDRF_NOTIFYITEMDRAW;
							case CDDS_ITEMPREPAINT:
								windowColor = GetSysColor(COLOR_WINDOW);

								if (driveMap->Get((int)((LPNMLVCUSTOMDRAW)lParam)->nmcd.dwItemSpec)->Locked)
								{
									((LPNMLVCUSTOMDRAW)lParam)->clrTextBk = windowColor & 0x00f00000 && windowColor & 0x0000f000 && windowColor & 0x0000000c ? windowColor - 0x00101004 : windowColor;
								}
								else
								{
									if (((int)((LPNMLVCUSTOMDRAW)lParam)->nmcd.dwItemSpec % 2)==0)
									{
										((LPNMLVCUSTOMDRAW)lParam)->clrTextBk = windowColor & 0x00080000 && windowColor & 0x00000800 && windowColor & 0x00000008 ? windowColor - 0x00080808 : windowColor;
									}
									else
									{
										((LPNMLVCUSTOMDRAW)lParam)->clrTextBk = windowColor;
									}
								}
								return CDRF_NEWFONT;

								default:
								break;
						}
					}
				break;

				default:
					return Window::WindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_EDITBUTTON:
					EditSelectedItem();
					break;

				case ID_CLOSEBUTTON:
					return DestroyWindow(hWindow);

				default:
					return Window::WindowProc(hWnd, message, wParam, lParam);
			}

		default:
			return Window::WindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void MainWindow::CreateChildComponents()
{
	RECT rect;

	GetClientRect(hWindow, &rect);

	hDrivesGroupBox = CreateWindow(L"BUTTON", L"Drive mapping:", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 1, 1, rect.right - buttonsGroupBoxWidth - 1, rect.bottom - 1, hWindow, (HMENU)-1, hInstance, NULL);
	SendMessage(hDrivesGroupBox, WM_SETFONT, (WPARAM)font, TRUE);

	hButtonsGroupBox = CreateWindow(L"BUTTON", L"Actions:", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, rect.right - buttonsGroupBoxWidth, 1, buttonsGroupBoxWidth, rect.bottom - 1, hWindow, (HMENU)-1, hInstance, NULL);
	SendMessage(hButtonsGroupBox, WM_SETFONT, (WPARAM)font, TRUE);

	hEditButton = CreateWindow(L"BUTTON", L"&Edit", WS_CHILD | WS_VISIBLE, rect.right - buttonsGroupBoxWidth + (int)(4 * dpi), (int)(20 * dpi), buttonsGroupBoxWidth - (int)(8 * dpi), (int)(25 * dpi), hWindow, (HMENU)ID_EDITBUTTON, hInstance, NULL);
	SendMessage(hEditButton, WM_SETFONT, (WPARAM)font, TRUE);

	hCloseButton = CreateWindow(L"BUTTON", L"&Close", WS_CHILD | WS_VISIBLE, rect.right - buttonsGroupBoxWidth + (int)(4 * dpi), (int)(45 * dpi), buttonsGroupBoxWidth - (int)(8 * dpi), (int)(25 * dpi), hWindow, (HMENU)ID_CLOSEBUTTON, hInstance, NULL);
	SendMessage(hCloseButton, WM_SETFONT, (WPARAM)font, TRUE);

	hDrivesListView = CreateWindow(WC_LISTVIEW, NULL, LVS_REPORT | WS_CHILD | WS_VISIBLE | LVS_SINGLESEL, (int)(4.0 * dpi), (int)(20.0 * dpi), rect.right - buttonsGroupBoxWidth, rect.bottom - (int)(24.0 * dpi), hWindow, (HMENU)ID_APPLISTVIEW, hInstance, NULL);
	ListView_SetExtendedListViewStyleEx(hDrivesListView, LVS_EX_GRIDLINES, LVS_EX_GRIDLINES);
	ListView_SetExtendedListViewStyleEx(hDrivesListView, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	LV_COLUMN lvColumn;
	memset(&lvColumn, 0, sizeof(LV_COLUMN));

	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvColumn.fmt = LVCFMT_LEFT;

	lvColumn.pszText = (LPWSTR)L"Drive";
	lvColumn.cx = (int)(60 * dpi);
	ListView_InsertColumn(hDrivesListView, 0, &lvColumn);

	lvColumn.pszText = (LPWSTR)L"Path";
	GetClientRect(hDrivesListView, &rect);
	lvColumn.cx = rect.right - rect.left - ListView_GetColumnWidth(hDrivesListView, 0);
	ListView_InsertColumn(hDrivesListView, 1, &lvColumn);

	SendMessage(hDrivesListView, WM_SETFONT, (WPARAM)font, TRUE);

	driveMap->ShowInListView(hDrivesListView);
}

void MainWindow::EditSelectedItem()
{
	int itemId = ListView_GetNextItem(hDrivesListView, -1, LVNI_FOCUSED);

	if (itemId == -1)
		return;

	Drive *item = driveMap->Get(itemId);
	if (!item)
		throw L"Failed to get AppMenuItem!";

	itemDialog = new ItemDialog(hInstance, hWindow, (wchar_t*)L"Edit item", NULL, item, font, options, persistence);
	if (!itemDialog)
		throw L"Failed to create new ItemDialog!";

	if (!itemDialog->Create((DLGPROC)ItemDialogProc, MAKEINTRESOURCE(IDD_ITEMDLG)))
	{
		driveMap->ShowInListView(hDrivesListView);
	}

	delete itemDialog;
}
