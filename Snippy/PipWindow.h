#pragma once

#include <Windows.h>

namespace PipWindow
{
	// Data tracked per window
	struct PipData
	{
		// Handle of the window
		HWND hwnd;
		// Windows stops sending mouse leave events after the first one, so we need to use this struct
		// to re-enable those events when the mouse enters the window again
		TRACKMOUSEEVENT tme;

		// Mouse offset from the origin of the window when the left mouse button was pressed
		int moveOffsetX;
		int moveOffsetY;

		// Is the mouse over the window
		bool mouseOver;
		// Is the left mouse button down
		bool mouseDown;

		HBITMAP image;
	};

	const wchar_t CLASS_NAME[] = L"PipClass";
	const int MIN_SIZE = 50;
	//We need to set the cursor when it enters the window
	extern HCURSOR hArrowCursor;

	// Registers the window class with the OS. Should be called only once.
	void InitClass(HINSTANCE hInstance);
	// Create an instance of a pip window
	HWND CreatePip(HINSTANCE hInstance, int x, int y, int w, int h, HBITMAP image);
	// Message handling for pip windows
	LRESULT CALLBACK PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}