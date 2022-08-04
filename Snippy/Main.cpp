#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <WinUser.h>

#include <iostream>

#include "PipWindow.h"

#define WM_NOTIF_ICON_MSG WM_APP + 1


HINSTANCE g_hInstance;


// Windows uses different entry points depending on the subsystem being used. We use the console subsystem for debugging
// which uses the main(...) entry point. Otherwise there's no difference for our purposes so we just immediately call
// the other entry point.
int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowContextMenu(HWND hwnd, POINT pt);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	const wchar_t CLASS_NAME[] = L"MainClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = MainWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Main Window",
		0,

		0,
		0,
		0,
		0,

		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == 0)
	{
		std::cout << "Error creating window: " << GetLastError() << '\n';
		PostQuitMessage(0);
		return 0;
	}

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

void ShowContextMenu(HWND hwnd, POINT pt)
{
	HMENU hMenu = CreatePopupMenu();
	if (hMenu)
	{
		AppendMenu(hMenu, MF_ENABLED | MF_STRING, 1, L"Exit");

		// our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
		SetForegroundWindow(hwnd);

		// respect menu drop alignment
		UINT uFlags = TPM_RIGHTBUTTON;
		if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
		{
			uFlags |= TPM_RIGHTALIGN;
		}
		else
		{
			uFlags |= TPM_LEFTALIGN;
		}

		TrackPopupMenuEx(hMenu, uFlags, pt.x, pt.y, hwnd, NULL);

		DestroyMenu(hMenu);
	}
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_HOTKEY:
		std::cout << "Pressed\n";
		return 0;
	case WM_NOTIF_ICON_MSG:
		switch (LOWORD(lParam))
		{
		case WM_CONTEXTMENU:
		{
			POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
			ShowContextMenu(hwnd, pt);
		}
		break;
		}
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}