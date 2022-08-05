#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <WinUser.h>

#include <iostream>

#include "MainWindow.h"
#include "PipWindow.h"

// Windows uses different entry points depending on the subsystem being used. We use the console subsystem for debugging
// which uses the main(...) entry point. Otherwise there's no difference for our purposes so we just immediately call
// the other entry point.
int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	PipWindow::InitClass(hInstance);

	MainWindow::InitClass(hInstance);

	HWND hwnd = MainWindow::CreateMainWindow(hInstance);

	//To do, custom icon
	HICON icon = LoadIcon(NULL, IDI_QUESTION);

	NOTIFYICONDATA notifyIconData = {};
	notifyIconData.cbSize = sizeof(notifyIconData);
	notifyIconData.uID = 1;
	notifyIconData.hWnd = hwnd;
	notifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	notifyIconData.uCallbackMessage = WM_NOTIF_ICON_MSG;
	notifyIconData.hIcon = icon;
	notifyIconData.uVersion = NOTIFYICON_VERSION_4;

	Shell_NotifyIcon(NIM_ADD, &notifyIconData);
	Shell_NotifyIcon(NIM_SETVERSION, &notifyIconData);

	// Register hotkey Ctrl-Shift-W. Using C would make more sense but it's taken by gyazo.
	// 0x57 is the key code for W
	if (RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x57))
		std::cout << "Registered hotkey\n";

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Shell_NotifyIcon(NIM_DELETE, &notifyIconData);

	return 0;
}