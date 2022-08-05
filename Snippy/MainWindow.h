#pragma once

#include <Windows.h>

#define WM_NOTIF_ICON_MSG WM_APP + 1
#define MENU_EXIT 1

namespace MainWindow
{
	struct MainData
	{
		HINSTANCE hInstance;
	};

	const wchar_t CLASS_NAME[] = L"MainClass";
	//We need to set the cursor when it enters the window
	extern HCURSOR hArrowCursor;

	// Registers the window class with the OS. Should be called only once.
	void InitClass(HINSTANCE hInstance);
	// Create an instance of a pip window
	HWND CreateMainWindow(HINSTANCE hInstance);

	// Message handling for the main window
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ShowContextMenu(HWND hwnd, POINT pt);
}