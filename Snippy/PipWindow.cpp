#include "PipWindow.h"
#include <windowsx.h>
#include <iostream>

HCURSOR PipWindow::hArrowCursor{ 0 };
bool PipWindow::s_classInitialized{ false };
const wchar_t PipWindow::CLASS_NAME[] = L"PipClass";
int PipWindow::s_instanceCount{ 0 };
ATOM PipWindow::s_classAtom{ 0 };
HINSTANCE PipWindow::s_hInstance{ 0 };

void PipWindow::InitClass(HINSTANCE hInstance)
{
	PipWindow::s_hInstance = hInstance;

	WNDCLASS wc = {};
	wc.lpfnWndProc = PipWindow::PipWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	PipWindow::s_classAtom = RegisterClass(&wc);

	if(PipWindow::s_classAtom == 0)
		std::cout << "Error registering class: " << GetLastError() << '\n';

	// It's fine if we load the cursor multiple times. The OS will just return the same handle each time
	PipWindow::hArrowCursor = LoadCursor(NULL, MAKEINTRESOURCE(32512));

	PipWindow::s_classInitialized = true;
}

void PipWindow::CleanupClass()
{
	if (!UnregisterClass((LPCWSTR)PipWindow::s_classAtom, PipWindow::s_hInstance))
		std::cout << "Error unregistering class: " << GetLastError() << '\n';

	PipWindow::s_hInstance = NULL;

	PipWindow::s_classAtom = NULL;

	PipWindow::s_classInitialized = false;
}

PipWindow::PipWindow(HINSTANCE hInstance)
{
	if (!PipWindow::s_classInitialized)
		InitClass(hInstance);
	++PipWindow::s_instanceCount;

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Pip Window",
		//WS_POPUP,
		WS_OVERLAPPEDWINDOW,

		100,
		100,
		800,
		600,

		NULL,
		NULL,
		hInstance,
		this
	);

	if (hwnd == 0)
	{
		std::cout << "Error creating window: " << GetLastError() << '\n';
		PostQuitMessage(0);
		return;
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	tme = {};
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = hwnd;

	TrackMouseEvent(&tme);
}


PipWindow::~PipWindow()
{
	if (--PipWindow::s_instanceCount == 0)
		PipWindow::CleanupClass();
}

LRESULT CALLBACK PipWindow::PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PipWindow* window;

	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<PipWindow*>(create->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
	}
	else
	{
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window = reinterpret_cast<PipWindow*>(ptr);
	}

	switch (uMsg)
	{
	case WM_DESTROY:
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

		return 0;
	}
	case WM_MOUSELEAVE:
	{
		std::cout << "Out!\n";
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}