#pragma once
#include <WinUser.h>

class PipWindow {
public:
	int mousedownX;
	int mousedownY;

	TRACKMOUSEEVENT tme;
};