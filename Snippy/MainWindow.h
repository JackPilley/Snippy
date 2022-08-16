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
	// Create an instance of a pip window
	HWND CreateMainWindow(HINSTANCE hInstance);
}