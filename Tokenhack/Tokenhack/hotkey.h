#ifndef hotkey_header_guard
#define hotkey_header_guard

#include <string>
#include <Windows.h>

std::string GetModName(int key);
void changebuttontext(HWND btn, DWORD w);
void fileedit(int btnid, DWORD hotkeykey);
void hotkeychange(HWND btn, int btnid, DWORD &hotkeykey, DWORD w);

#endif

