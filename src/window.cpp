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
#include "window.hpp"

Window::Window()
{
	hInstance = NULL;
	hParent = NULL;
	className = NULL;
	windowName = NULL;
	hMainIcon = NULL;
	hMainIconSmall = NULL;
	hWindow = NULL;
	windowRect.left = windowRect.right = windowRect.top = windowRect.bottom = 0;
}

Window::Window(HINSTANCE p_hInstance, HWND p_hParent, const wchar_t *p_windowName, const RECT *p_windowRect)
{
	hInstance = p_hInstance;
	hParent = p_hParent;

	windowName = new wchar_t[wcslen(p_windowName) + 1];
	memset(windowName, 0, (wcslen(p_windowName) + 1) * sizeof(wchar_t));

	className = new wchar_t[wcslen(p_windowName) + 8];
	memset(className, 0, (wcslen(p_windowName) + 8) * sizeof(wchar_t));

	#ifndef __WATCOMC__
		wcsncpy_s(windowName, wcslen(p_windowName) + 1, p_windowName, wcslen(p_windowName) + 1);
		wcsncpy_s(className, wcslen(p_windowName) + 1, p_windowName, wcslen(p_windowName) + 1);
		wcsncat_s(className, wcslen(p_windowName) + 7, L" class", wcslen(p_windowName) + 7);
	#else
		wcsncpy(windowName,  p_windowName, wcslen(p_windowName) + 1);
		wcsncpy(className, p_windowName, wcslen(p_windowName) + 1);
		wcsncat(className, L" class", wcslen(p_windowName) + 7);
	#endif
	
	if (!p_windowRect)
	{
		windowRect.left = CW_USEDEFAULT;
		windowRect.top = CW_USEDEFAULT;
		windowRect.right = CW_USEDEFAULT;
		windowRect.bottom = CW_USEDEFAULT;
	}
	else
	{
		windowRect.left = p_windowRect->left;
		windowRect.top = p_windowRect->top;
		windowRect.right = p_windowRect->right;
		windowRect.bottom = p_windowRect->bottom;
	}
		
	hMainIcon = NULL;
	hMainIconSmall = NULL;
}

Window::~Window()
{
	delete []className;
	delete []windowName;
}

INT_PTR Window::Create(DLGPROC dialogProc, LPCTSTR dialogTemplate)
{
	LoadResources();
	INT_PTR retVal = DialogBox(hInstance, dialogTemplate, hParent, dialogProc);
	UnloadResources();
	return retVal;
}

int Window::Create(WNDPROC WndProc, DWORD style, DWORD exStyle, int cmdShow)
{
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(WNDCLASSEX));

	LoadResources();

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hMainIcon;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)COLOR_BTNSHADOW;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= className;
	wcex.hIconSm		= hMainIcon;

	if (!RegisterClassEx(&wcex))
		throw L"RegisterClassEx failure";

	hWindow = CreateWindowEx(exStyle, className, windowName, style, windowRect.left, windowRect.top, windowRect.right-windowRect.left, windowRect.bottom - windowRect.top, hParent, NULL, hInstance, NULL);

	if (!hWindow)
		throw L"CreateWindowEx failure";

	CreateChildComponents();

	ShowWindow(hWindow, cmdShow);

	return 0;
}

void Window::MessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(className, hInstance);
	UnloadResources();
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_CLOSE:
			return DefWindowProc(hWnd, message, wParam, lParam);

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

BOOL Window::DialogProc(HWND hWnd, UINT message, WPARAM, LPARAM)
{
	switch (message)
	{
		case WM_DESTROY:
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;

		default:
			return FALSE;
	}
}

void Window::CreateChildComponents()
{
}

void Window::LoadResources()
{
	hMainIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	if (!hMainIcon)
		throw L"Failed to load main icon!";

	hMainIconSmall = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINSMALLICON));
	if (!hMainIconSmall)
		throw L"Failed to load small icon!";
}

void Window::UnloadResources()
{
	if (hMainIcon)
		DestroyIcon(hMainIcon);

	if (hMainIconSmall)
		DestroyIcon(hMainIconSmall);
}
