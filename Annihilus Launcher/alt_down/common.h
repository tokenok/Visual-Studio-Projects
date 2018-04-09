#ifndef common_header_guard
#define common_header_guard

#include <string>
#include <Windows.h>
#include <vector>

std::string int_to_str(int num, bool is_hex = false);
int str_to_int(std::string str);
std::wstring str_to_wstr(std::string s);
std::string wstr_to_str(std::wstring ws);

std::vector<std::vector<std::string>> split_vec(const std::vector<std::string>& vec, const std::string& delimiter);

std::vector<std::string> split_str(std::string& str, const std::string& delimiter);

std::string error_code_to_text(DWORD error_code);

void CONSOLE_ON();
std::string getwindowclassname(HWND hwnd);
std::string getwindowtext(HWND wnd);
RECT getwindowrect(HWND wnd);
RECT getclientrect(HWND wnd);
RECT getmappedclientrect(HWND wnd, HWND wndTo = NULL);
POINT getclientcursorpos(HWND hwnd_parent);
POINT getcursorpos();
bool cursor_in_region(RECT region, POINT cursor_pos);

void GetFilesInDirectory(std::vector<std::string> &out, const std::string &directory);
bool GetFolder(std::string& folderpath, const wchar_t* szCaption = NULL, HWND hOwner = NULL);

HKEY OpenRegKey(HKEY hRootKey, wchar_t* strKey);
void SetRegVal(HKEY hKey, LPCTSTR lpValue, DWORD data);
DWORD GetRegVal(HKEY hKey, LPCTSTR lpValue);

std::string milliseconds_to_hms(int milliseconds);


#endif

