#ifndef TokenDLL
#define TokenDLL

#include <Windows.h>
#include <string>

#define DLL_NAME L"TokenChestDll.dll"

#define WM_DUMPED 1

#define CREG_PATH L"SOFTWARE\\TokenChest\\"
#define CREG_ROOTKEY HKEY_LOCAL_MACHINE
#define CREG_KEY_AUTOLOG L"Auto Log"
#define CREG_KEY_DLLWARN L"dllwarn"
#define CREG_KEY_TOKENCHEST_PATH L"path"

struct WndInfo {
	DWORD pid;
	HWND hwnd;
};

extern WndInfo g_info;
extern HWND g_TokenChest;
extern std::string g_TokenChestPath;

#define DLL_API __declspec(dllexport)

extern "C"{
	DLL_API LRESULT CALLBACK msgretProc(int code, WPARAM wParam, LPARAM lParam);
	DLL_API int DUMP();
}

#endif