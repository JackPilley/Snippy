// Copyright 2022 Jack Pilley
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#pragma once

#include <Windows.h>
#include <unordered_set>

#define WM_NOTIF_ICON_MSG WM_APP + 1
#define MENU_EXIT 1

namespace MainWindow
{
	struct MainData
	{
		HINSTANCE hInstance;
		
		std::unordered_set<HWND> pips;

		bool mouseDown;

		int originX;
		int originY;

		int previousX;
		int previousY;
		
		bool windowShown;
		HBITMAP image;
		HBITMAP buffer;
	};

	const wchar_t CLASS_NAME[] = L"MainClass";

	// Registers the window class with the OS. Should be called only once.
	void InitClass(HINSTANCE hInstance);
	// Create an instance of the main window. There should really only be one instance of this window
	HWND CreateMainWindow(HINSTANCE hInstance);
}