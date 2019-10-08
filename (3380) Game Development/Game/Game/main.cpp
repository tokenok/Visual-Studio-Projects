#include <Windows.h>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
//#include <gdiplus.h>
//#pragma comment (lib,"gdiplus.lib")
#include <iostream>

#include "game.h"
#include "resource.h"
#include "C:/CPPlibs/common/common.h"

using namespace std;
//using namespace Gdiplus;

int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#ifdef _DEBUG
	SHOW_CONSOLE(true);
#endif

	/*GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);*/

	LoadLibrary(L"riched32.dll");//load dll for rich edit
	InitCommonControls();

	Game game;
	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)Game::StaticGameProc, (LPARAM)&game);//start up host window for game

	//Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;
}

