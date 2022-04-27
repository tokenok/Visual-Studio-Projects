#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "resource.h"

#include "Game.h"


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

	Game* game = new Game(hInstance);

	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return 0;
}

