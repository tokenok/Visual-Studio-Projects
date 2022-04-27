#ifndef common_header_guard
#define common_header_guard

#include <Windows.h>
#include <string>
#include <vector>
#include <locale>
#include <sstream>

std::string int_to_str(int num, bool is_hex = false);
std::string uint_to_str(unsigned int num, bool is_hex = false);
std::string char_to_str(char c);
int str_to_int(std::string str);
std::wstring str_to_wstr(std::string s);
std::string wstr_to_str(std::wstring ws);
BSTR wstr_to_bstr(std::wstring ws);
BSTR str_to_bstr(std::string s);
std::wstring bstr_to_wstr(BSTR bs);
std::string bstr_to_str(BSTR bs);
std::string LPVOID_to_str(LPVOID data, int len);
LPWSTR str_to_LPWSTR(std::string s);
std::string LPWSTR_to_str(LPWSTR wc, UINT size);
std::string wastr_to_str(wchar_t *wc, UINT size);

template<class T>
std::string FormatWithCommas(T value) {
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << value;
	return ss.str();
}

std::vector<std::vector<std::string>> split_vec(const std::vector<std::string>& vec, const std::string& delimiter);

std::vector<std::string> split_str(std::string str, const std::string& delimiter, int minlen = -1);

std::string error_code_to_text(DWORD error_code);

void SHOW_CONSOLE(bool show = true, bool noclose = false);

std::string getwindowclassname(HWND hwnd);
std::string getwindowtext(HWND wnd);
RECT getwindowrect(HWND wnd);
RECT getclientrect(HWND wnd);
RECT getmappedclientrect(HWND wndFrom, HWND wndTo = NULL);
void displayrect(RECT& rc);
POINT getclientcursorpos(HWND hwnd_parent);
POINT getcursorpos();
bool is_cursor_in_region(RECT region, POINT cursor_pos);

void GetFilesInDirectory(const std::string &directory, const std::string &filetype, std::vector<std::string> *out);
std::string getexepath();
std::string getexepath(HWND hwnd);
std::string getexedir();
DWORD createfolder(std::string path);

std::string milliseconds_to_hms(int milliseconds);

void ssystem(std::string command, bool pause = true);

#endif

