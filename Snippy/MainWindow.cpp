#include <iostream>

#include "MainWindow.h"
#include "PipWindow.h"
#include <windowsx.h>

HCURSOR MainWindow::hSelectCursor = {};

void MainWindow::InitClass(HINSTANCE hInstance)
{
	hSelectCursor = LoadCursor(NULL, IDC_CROSS);

	WNDCLASS wc = {};
	wc.lpfnWndProc = MainWindow::MainWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = hSelectCursor;

	RegisterClass(&wc);
}

HWND MainWindow::CreateMainWindow(HINSTANCE hInstance)
{
	MainData* data = new MainData{};

	data->hInstance = hInstance;

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Snippy",
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

void MainWindow::CopyAndDisplayScreen(HWND hwnd, MainData& data)
{
	int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// Save a copy of the current screen image
	HDC screen = GetDC(NULL);
	HDC window = GetDC(hwnd);

	HDC memDC = CreateCompatibleDC(window);

	HBITMAP bitmap = CreateCompatibleBitmap(screen, w, h);
	HBITMAP buffer = CreateCompatibleBitmap(screen, w, h);

	HGDIOBJ old = SelectObject(memDC, bitmap);

	BitBlt(
		memDC,
		0, 0,
		w, h,
		screen,
		0, 0,
		SRCCOPY
	);

	SelectObject(memDC, buffer);

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

	data.image = bitmap;
	data.buffer = buffer;


	SetWindowPos(
		hwnd,
		NULL,
		x, y,
		w, h,
		SWP_SHOWWINDOW
	);
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
		DeleteObject(data->image);
		DeleteObject(data->buffer);
		delete data;
		return 0;
	}
	case WM_HOTKEY:
	{
		if (data->windowShown) return 0;

		CopyAndDisplayScreen(hwnd, *data);

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
	/*case WM_PAINT:
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
	}*/
	case WM_MOUSEMOVE:
	{
		if (data->mouseDown)
		{
			int originX = data->originX;
			int originY = data->originY;

			int endX = data->previousX;
			int endY = data->previousY;

			int temp;

			int w = endX - originX;
			int h = endY - originY;

			w = w >= 0 ? max(w, PipWindow::MIN_SIZE) : min(w, -PipWindow::MIN_SIZE);
			h = h >= 0 ? max(h, PipWindow::MIN_SIZE) : min(h, -PipWindow::MIN_SIZE);

			HDC hdc = GetDC(hwnd);

			HDC imageDC = CreateCompatibleDC(hdc);
			HDC bufferDC = CreateCompatibleDC(hdc);
			HGDIOBJ oldImageSelection = SelectObject(imageDC, data->image);
			HGDIOBJ oldBufferSelection = SelectObject(bufferDC, data->buffer);

			BitBlt(
				bufferDC,
				originX, originY,
				w, h,
				imageDC,
				originX, originY,
				SRCCOPY
			);

			data->previousX = endX = GET_X_LPARAM(lParam);
			data->previousY = endY = GET_Y_LPARAM(lParam);

			//Make sure there's room for the close button
			w = endX - originX;
			h = endY - originY;

			w = w >= 0 ? max(w, PipWindow::MIN_SIZE) : min(w, -PipWindow::MIN_SIZE);
			h = h >= 0 ? max(h, PipWindow::MIN_SIZE) : min(h, -PipWindow::MIN_SIZE);

			BitBlt(
				bufferDC,
				originX, originY,
				w, h,
				imageDC,
				originX, originY,
				SRCINVERT
			);

			BitBlt(
				hdc,
				0,0,
				GetDeviceCaps(hdc, HORZRES), GetDeviceCaps(hdc, VERTRES),
				bufferDC,
				0,0,
				SRCCOPY
			);

			SelectObject(imageDC, oldImageSelection);
			DeleteObject(imageDC);

			SelectObject(bufferDC, oldBufferSelection);
			DeleteObject(bufferDC);
		}
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		data->mouseDown = true;
		data->originX = GET_X_LPARAM(lParam);
		data->originY = GET_Y_LPARAM(lParam);
		data->previousX = GET_X_LPARAM(lParam);
		data->previousY = GET_Y_LPARAM(lParam);
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

		//Make sure there's room for the close button
		int w = max(endX - originX, PipWindow::MIN_SIZE);
		int h = max(endY - originY, PipWindow::MIN_SIZE);

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