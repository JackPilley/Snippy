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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	PipWindow window(hInstance);
	PipWindow window2(hInstance);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}