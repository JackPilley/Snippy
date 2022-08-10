#include "PipWindow.h"
#include <windowsx.h>
#include <iostream>

HCURSOR PipWindow::hArrowCursor = {};

void PipWindow::InitClass(HINSTANCE hInstance)
{
	hArrowCursor = LoadCursor(NULL, IDC_ARROW);

	WNDCLASS wc = {};
	wc.lpfnWndProc = PipWindow::PipWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = hArrowCursor;

	RegisterClass(&wc);
}

HWND PipWindow::CreatePip(HINSTANCE hInstance, int x, int y, int w, int h, HBITMAP image)
{
	// This will be deleted when the window receives the destroy message
	PipData* data = new PipData{};

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Pip Window",
		// Borderless and non-resizeable
		WS_POPUP|WS_SYSMENU,
		//WS_OVERLAPPEDWINDOW,

		x, y,
		w, h,

		NULL,
		NULL,
		hInstance,
		data
	);

	if (hwnd == 0)
	{
		std::cout << "Error creating window: " << GetLastError() << '\n';
		return hwnd;
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
	data->image = image;

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
		HGDIOBJ old = SelectObject(memDC, data->image);

		RECT rect = {};

		GetWindowRect(hwnd, &rect);
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		BitBlt(
			hdc,
			0, 0,
			w, h,
			memDC,
			0, 0,
			SRCCOPY
		);

		SelectObject(memDC, old);
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