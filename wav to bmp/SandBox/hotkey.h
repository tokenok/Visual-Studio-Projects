#ifndef hotkey_header_guard
#define hotkey_header_guard

#include <string>
#include <Windows.h>
#include <vector>

extern std::vector<DWORD> hotkeys;
extern DWORD global_change_hotkey;

std::string GetModName(int key);
void changebuttontext(HWND btn, DWORD w);
void fileedit(UINT btnid, DWORD hotkeykey);
void hotkeychange(HWND btn, UINT btnid, DWORD &hotkeykey, DWORD w);

void update_hotkeykeys();


#endif

