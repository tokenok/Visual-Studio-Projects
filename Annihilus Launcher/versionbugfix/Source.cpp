#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <string>

#include "Registry.h"

using namespace std;

wstring str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}
string wstr_to_str(wstring ws) {
	string ret(ws.begin(), ws.end());
	return ret;
}
void SHOW_CONSOLE(bool show) {
	static bool show_state = false;
	if (show && !show_state) {
		std::cout.clear();
		FILE *stream;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&stream, "CONOUT$", "w", stdout);
	}
	if (!show)
		FreeConsole();
	show_state = show;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR args, int /*iCmdShow*/) {
	SHOW_CONSOLE(true);
	CRegistry hregkey;
	//if (!hregkey.Open(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", HKEY_LOCAL_MACHINE))
	if (!hregkey.Open(L"Software\\Annihilus Launcher"), HKEY_CURRENT_USER)
		MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);

	TCHAR wpath[MAX_PATH];
	GetModuleFileName(NULL, wpath, MAX_PATH);
	string path = wstr_to_str(wpath);
	path = path.substr(0, path.rfind('\\') + 1);
	path += "Diablo II.exe";

	cout << path << '\n';
	
	LPCTSTR key = str_to_wstr(path).c_str();
	LPCTSTR val = str_to_wstr("DisableNXShowUI").c_str();

	hregkey[key] = val;

	hregkey.Close();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}