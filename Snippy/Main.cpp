#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include <iostream>

#include "PipWindow.h"

HCURSOR hArrowCursor;

// Windows uses different entry points depending on the subsystem being used. We use the console subsystem for debugging
// which uses the main(...) entry point. Otherwise there's no difference for our purposes so we just immediately call
// the other entry point.
int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdShow);
}

LRESULT CALLBACK PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"PipClass";
	WNDCLASS wc = {};
	wc.lpfnWndProc = PipWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Pip Window",
		WS_POPUP,

		100, 
		100,
		800,
		600,

		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL)
		return 0;

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	TRACKMOUSEEVENT tme = {};
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = hwnd;

	TrackMouseEvent(&tme);

	hArrowCursor = LoadCursor(NULL, MAKEINTRESOURCE(32512));

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	}
		return 0;
	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		std::cout << x << " " << y << '\n';

		SetCursor(hArrowCursor);

		break;
	}
	case WM_MOUSELEAVE:
	{
		std::cout << "Out!\n";
		break;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}