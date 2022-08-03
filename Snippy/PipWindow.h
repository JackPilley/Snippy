#pragma once

#include <Windows.h>

class PipWindow {
private:

	// Static data that is shared across instances.
	// Has the window class been registered with the OS? Note this is different from a C++ class
	static bool s_classInitialized;
	static const wchar_t CLASS_NAME[];
	//Handle to the module that created the window
	static HINSTANCE s_hInstance;
	//Unique ATOM used to identify the window class
	static ATOM s_classAtom;
	// How many instances of our class exist. If this reaches zero then the window class gets cleaned up
	static int s_instanceCount;

	// Handle to the standard arrow pointer cursor. Note this doesn't get unloaded when the class is cleaned up because
	// it's not associated with the window class, and also there's no way to unload a cursor.
	static HCURSOR hArrowCursor;

	// Register the window class with the OS
	static void InitClass(HINSTANCE hInstance);
	// Clean up the window class
	static void CleanupClass();

	// Handle to the window
	HWND hwnd;
	TRACKMOUSEEVENT tme;

	int mousedownX;
	int mousedownY;
	bool mouseOver;

public:
	// We must have an HINSTANCE to create our window
	PipWindow() = delete;
	PipWindow(HINSTANCE hInstance);
	~PipWindow();

	static LRESULT CALLBACK PipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};