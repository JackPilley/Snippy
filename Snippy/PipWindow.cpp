#include "PipWindow.h"
#include <windowsx.h>
#include <iostream>

#include "resource.h"

namespace PipWindow
{
	HCURSOR hArrowCursor = {};
	// Message handling for pip windows
	LRESULT CALLBACK PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void InitClass(HINSTANCE hInstance)
	{
		hArrowCursor = LoadCursor(NULL, IDC_ARROW);

		WNDCLASS wc = {};
		wc.lpfnWndProc = PipWindowProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;
		wc.hCursor = hArrowCursor;
		wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

		RegisterClass(&wc);
	}

	HWND CreatePip(HINSTANCE hInstance, HWND mainWindow, int x, int y, int w, int h, HBITMAP image)
	{
		// This will be deleted when the window receives the destroy message
		PipData* data = new PipData{};

		HWND hwnd = CreateWindowEx(
			0,
			CLASS_NAME,
			L"Snippy Pip",
			// Borderless and non-resizeable
			WS_POPUP | WS_SYSMENU,
			//WS_OVERLAPPEDWINDOW,

			x, y,
			w, h,

			mainWindow,
			NULL,
			hInstance,
			data
		);

		if (hwnd == 0)
		{
			std::cout << "Error creating window: " << GetLastError() << '\n';
			return hwnd;
		}

		ShowWindow(hwnd, SW_NORMAL);

		//Track when the mouse leaves the window
		TRACKMOUSEEVENT tme = {};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;

		TrackMouseEvent(&tme);

		data->mainWin = mainWindow;
		data->tme = tme;
		data->image = image;

		return hwnd;
	}

	LRESULT CALLBACK PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			//Let the main window know the pip is being destroyed
			SendMessage(data->mainWin, WM_PIP_DESTROYED, 0, (LPARAM)hwnd);

			DeleteObject(data->image);
			delete data;
			return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			HDC memDC = CreateCompatibleDC(hdc);
			HGDIOBJ old = SelectObject(memDC, data->image);

			RECT rect = {};

			GetClientRect(hwnd, &rect);
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;

			int srcW = GetDeviceCaps(memDC, HORZRES);
			int srcH = GetDeviceCaps(memDC, VERTRES);

			BITMAP bitmap = {};

			GetObject(data->image, sizeof(BITMAP), &bitmap);

			SetStretchBltMode(hdc, STRETCH_HALFTONE);

			StretchBlt(
				hdc,
				0, 0,
				w, h,
				memDC,
				0, 0,
				bitmap.bmWidth, bitmap.bmHeight,
				SRCCOPY
			);

			SelectObject(memDC, old);
			DeleteDC(memDC);

			EndPaint(hwnd, &ps);

			return 0;
		}
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* info = (MINMAXINFO*)lParam;
			info->ptMinTrackSize.y = MIN_SIZE;

			return 0;
		}
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
		case WM_SYSKEYUP:
		{
			if (wParam == VK_MENU || wParam == VK_F10)
			{
					// Skip checking keycodes if it's a double click event
		case WM_LBUTTONDBLCLK:
				if (data->titleBarShown)
				{
					SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_SYSMENU);
				}
				else
				{
					SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
				}

				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);

				data->titleBarShown = !data->titleBarShown;

			}
			return 0;
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}