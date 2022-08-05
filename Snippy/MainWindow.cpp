#include "MainWindow.h"
#include "PipWindow.h"

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
		0,

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
		PipWindow::CreatePip(data->hInstance);
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
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);

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