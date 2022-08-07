#include <iostream>

#include "MainWindow.h"
#include "PipWindow.h"
#include <windowsx.h>

void MainWindow::InitClass(HINSTANCE hInstance)
{
	WNDCLASS wc = {};
	wc.lpfnWndProc = MainWindow::MainWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);
}

HWND MainWindow::CreateMainWindow(HINSTANCE hInstance)
{
	MainData* data = new MainData{};

	data->hInstance = hInstance;

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Main Window",
		// Borderless and non-resizeable
		WS_POPUP | WS_SYSMENU,

		0,
		0,
		0,
		0,

		NULL,
		NULL,
		hInstance,
		data
	);

	return hwnd;
}

HBITMAP MainWindow::CopyAndDisplayScreen(HWND hwnd)
{
	int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	SetWindowPos(
		hwnd,
		NULL,
		x, y,
		w, h,
		SWP_SHOWWINDOW
	);

	// Save a copy of the current screen image
	HDC screen = GetDC(NULL);
	HDC window = GetDC(hwnd);

	HDC memDC = CreateCompatibleDC(window);

	HBITMAP bitmap = CreateCompatibleBitmap(screen, w, h);

	HGDIOBJ old = SelectObject(memDC, bitmap);

	BitBlt(
		memDC,
		0, 0,
		w, h,
		screen,
		0, 0,
		SRCCOPY
	);

	SelectObject(memDC, old);
	DeleteObject(memDC);

	ReleaseDC(NULL, screen);
	ReleaseDC(hwnd, window);

	return bitmap;
}



LRESULT CALLBACK MainWindow::MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MainData* data{};

	// If the window is being created, associate the persistent data with it.
	// Otherwise retrieve the data.
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		data = reinterpret_cast<MainData*>(create->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)data);
	}
	else
	{
		LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		data = reinterpret_cast<MainData*>(ptr);
	}

	switch (uMsg)
	{
	case WM_DESTROY:
	{
		delete data;
		return 0;
	}
	case WM_HOTKEY:
	{
		//POINT pos = {};
		//GetCursorPos(&pos);
		//PipWindow::CreatePip(data->hInstance, pos.x, pos.y, 800, 600);

		if (data->windowShown) return 0;

		data->image = CopyAndDisplayScreen(hwnd);

		data->windowShown = true;
		//This should already be false, but it's convenient to set it again here in case something went wrong previously
		data->mouseDown = false;

		return 0;
	}
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
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case MENU_EXIT:
		{
			PostQuitMessage(0);
			break;
		}
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		
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

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		data->mouseDown = true;
		data->originX = GET_X_LPARAM(lParam);
		data->originY = GET_Y_LPARAM(lParam);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		data->mouseDown = false;

		int originX = data->originX;
		int originY = data->originY;

		int endX = GET_X_LPARAM(lParam);
		int endY = GET_Y_LPARAM(lParam);

		int temp;

		//Make sure the origin is the top left corner
		if (endX < originX)
		{
			temp = endX;
			endX = originX;
			originX = temp;
		}

		if (endY < originY)
		{
			temp = endY;
			endY = originY;
			originY = temp;
		}

		//Minimum is 20x20 pixels so there's room for the close button
		int w = max(endX - originX, 20);
		int h = max(endY - originY, 20);

		HDC window = GetDC(hwnd);

		HDC memDC = CreateCompatibleDC(window);
		HDC cropDC = CreateCompatibleDC(window);

		HBITMAP bitmap = CreateCompatibleBitmap(window, w, h);

		HGDIOBJ oldMem = SelectObject(memDC, data->image);
		HGDIOBJ oldCrop = SelectObject(cropDC, bitmap);

		BitBlt(
			cropDC,
			0, 0,
			w, h,
			memDC,
			originX, originY,
			SRCCOPY
		);

		SelectObject(memDC, oldMem);
		SelectObject(cropDC, oldCrop);
		DeleteObject(memDC);
		DeleteObject(cropDC);

		ReleaseDC(hwnd, window);

		ShowWindow(hwnd, SW_HIDE);
		DeleteObject(data->image);
		data->windowShown = false;

		PipWindow::CreatePip(data->hInstance, originX, originY, w, h, bitmap);

		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		ShowWindow(hwnd, SW_HIDE);
		DeleteObject(data->image);
		data->windowShown = false;
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void MainWindow::ShowContextMenu(HWND hwnd, POINT pt)
{
	HMENU hMenu = CreatePopupMenu();
	if (hMenu)
	{
		AppendMenu(hMenu, MF_ENABLED | MF_STRING, MENU_EXIT, L"Exit");

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