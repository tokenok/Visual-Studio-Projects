#include "Tokenhackdll.h"
#include "window.h"

using namespace std;

HWND tokenhackwnd = FindWindow(L"Tokenhack", VERSION);

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved) {
	switch (Reason) {
		case DLL_PROCESS_ATTACH:{
			if (!tokenhackwnd) {
				MessageBox(NULL, L"Tokenhack not found", TEXT("Error"), MB_OK);
				return TRUE;
			}

			FILE *stream;
			AllocConsole();
			AttachConsole(GetCurrentProcessId());
			freopen_s(&stream, "CONOUT$", "w", stdout);

			std::cout << "DLL attach function called" << '\n';
			break;
		}
		case DLL_PROCESS_DETACH:{
			std::cout << "DLL detach function called" << '\n';
			FreeConsole();
			break;
		}
		case DLL_THREAD_ATTACH:{
			std::cout << "DLL thread attach function called" << '\n';
			break;
		}
		case DLL_THREAD_DETACH:{

			std::cout << "DLL thread detach function called" << '\n';
			break;
		}
	}
	return TRUE;
}

DLL_API LRESULT CALLBACK dllcallwndProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0) {
		CWPSTRUCT cw = *((CWPSTRUCT*)lParam);
		cout << cw.message << " " << cw.wParam << " " << cw.lParam << '\n';
		switch (cw.message) {
			case WM_GETICON:{
				PostMessage(tokenhackwnd, WM_DLL_HOOKPROC, (WPARAM)cw.hwnd, NULL);
			}
		}
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

DLL_API LRESULT CALLBACK dllcallwndretProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0) {
		CWPRETSTRUCT cwr = *((CWPRETSTRUCT*)lParam);
		cout << "ret: " << cwr.message << " " << cwr.wParam << " " << cwr.lParam << " " << cwr.lResult << '\n';

	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

DLL_API LRESULT CALLBACK dllgetmessageProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0) {
		MSG msg = *((MSG*)lParam);
		cout << "msg: " << msg.message << " " << msg.wParam << " " << msg.lParam << '\n';
	}
	return CallNextHookEx(NULL, code, wParam, lParam);

}

