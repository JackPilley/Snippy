#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <WinUser.h>
#include <strsafe.h>

#include <iostream>

#include "resource.h"

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
	//Register window classes with Windows
	PipWindow::InitClass(hInstance);
	MainWindow::InitClass(hInstance);

	HWND hwnd = MainWindow::CreateMainWindow(hInstance);

	HICON icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	//Set up notification icon (tray icon)
	NOTIFYICONDATA notifyIconData = {};
	notifyIconData.cbSize = sizeof(notifyIconData);
	notifyIconData.uID = 1;
	notifyIconData.hWnd = hwnd;
	notifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_MESSAGE;
	notifyIconData.uCallbackMessage = WM_NOTIF_ICON_MSG;
	StringCchCopy(notifyIconData.szTip, sizeof(notifyIconData.szTip), L"Snippy");
	notifyIconData.hIcon = icon;
	notifyIconData.uVersion = NOTIFYICON_VERSION_4;

	// Todo: Make this only true on the first run
	bool showStartupMessage = true;

	if (showStartupMessage)
	{
		notifyIconData.uFlags |= NIF_INFO;
		notifyIconData.dwInfoFlags |= NIIF_NONE;
		StringCchCopy(notifyIconData.szInfoTitle, sizeof(notifyIconData.szInfoTitle), L"Snippy is ready!");
		StringCchCopy(notifyIconData.szInfo, sizeof(notifyIconData.szInfo), L"Press Ctrl-Shift-W to take a screen shot.\nSnippy will keep running in the background.");
	}

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