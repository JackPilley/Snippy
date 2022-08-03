#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include <iostream>

#include "PipWindow.h"

// Windows uses different entry points depending on the subsystem being used. We use the console subsystem for debugging
// which uses the main(...) entry point. Otherwise there's no difference for our purposes so we just immediately call
// the other entry point.
int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
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

	// Register hotkey Ctrl-Shift-W. Using C would make more sense but it's taken by gyazo.
	if (RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x57))
		std::cout << "Registered hotkey\n";

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_HOTKEY:
		std::cout << "Pressed\n";
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}