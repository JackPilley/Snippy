#include "PipWindow.h"
#include <windowsx.h>
#include <iostream>

HCURSOR PipWindow::hArrowCursor = {};

void PipWindow::InitClass(HINSTANCE hInstance)
{
	WNDCLASS wc = {};
	wc.lpfnWndProc = PipWindow::PipWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hArrowCursor = LoadCursor(NULL, MAKEINTRESOURCE(32512));
}

HWND PipWindow::CreatePip(HINSTANCE hInstance)
{
	// This will be deleted when the window receives the destroy message
	PipData* data = new PipData{};

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Pip Window",
		// Borderless and non-resizeable
		WS_POPUP|WS_SYSMENU,

		100,
		100,
		800,
		600,

		NULL,
		NULL,
		hInstance,
		data
	);

	if (hwnd == 0)
	{
		std::cout << "Error creating window: " << GetLastError() << '\n';
		PostQuitMessage(0);
		return 0;
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	//Track when the mouse leaves the window
	TRACKMOUSEEVENT tme = {};
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = hwnd;

	TrackMouseEvent(&tme);

	data->hwnd = hwnd;
	data->tme = tme;

	return hwnd;
}

LRESULT CALLBACK PipWindow::PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PipData* data{};

	// If the window is being created, associate the persistent data with it.
	// Otherwise retrieve the data.
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		data = reinterpret_cast<PipData*>(create->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)data);

		HDC screen = GetDC(NULL);
		HDC window = GetDC(hwnd);

		HDC memDC = CreateCompatibleDC(window);

		HBITMAP bitmap = CreateCompatibleBitmap(screen, 800, 600);

		SelectObject(memDC, bitmap);

		BitBlt(
			memDC,
			0, 0,
			800, 600,
			screen,
			0, 0,
			SRCCOPY
		);

		data->image = bitmap;

		DeleteObject(memDC);

		ReleaseDC(NULL, screen);
		ReleaseDC(hwnd, window);

	}
	else
	{
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		data = reinterpret_cast<PipData*>(ptr);
	}

	switch (uMsg)
	{
	case WM_DESTROY:
		DeleteObject(data->image);
		delete data;
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, data->image);

		BitBlt(
			hdc, 0, 0, 800, 600, memDC, 0, 0, SRCCOPY
		);

		DeleteObject(memDC);

		EndPaint(hwnd, &ps);
	}
		return 0;
	case WM_MOUSEMOVE:
	{
		// If the mouse is entering the window after leaving it
		if (data->mouseOver == false)
		{
			data->mouseOver = true;
			TrackMouseEvent(&data->tme);
		}

		if (data->mouseDown)
		{
			RECT windowRect;
			GetWindowRect(hwnd, &windowRect);

			POINT mousePoint;
			GetCursorPos(&mousePoint);

			SetWindowPos(hwnd, HWND_TOPMOST, mousePoint.x - data->moveOffsetX, mousePoint.y - data->moveOffsetY, 0, 0, SWP_NOSIZE);
		}

		SetCursor(hArrowCursor);

		return 0;
	}
	case WM_MOUSELEAVE:
	{
		data->mouseOver = false;
		// Stops the window from getting weirdly stuck to the cursor if they move it too fast
		// and the cursor is near the edge of the window
		data->mouseDown = false;
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		RECT windowRect;
		GetWindowRect(hwnd, &windowRect);

		POINT mousePoint;
		GetCursorPos(&mousePoint);

		data->moveOffsetX = mousePoint.x - windowRect.left;
		data->moveOffsetY = mousePoint.y - windowRect.top;
		data->mouseDown = true;

		return 0;
	}
	case WM_LBUTTONUP:
	{
		data->mouseDown = false;
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		DestroyWindow(hwnd);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}