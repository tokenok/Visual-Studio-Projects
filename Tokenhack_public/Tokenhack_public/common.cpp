#include <Windows.h>
#include <sstream>
#include <iostream>
#include <objidl.h>
#include "shlobj.h"

#include "common.h"

using namespace std;

string int_to_str(int num, bool is_hex /*= false*/) {
	cout.clear();
	stringstream out;
	out << (is_hex ? hex : dec) << num;
	string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}
string char_to_str(char c) {
	cout.clear();
	stringstream out;
	out << c;
	string return_value = out.str();
	return return_value;
}
int str_to_int(string str) {
	cout.clear();
	stringstream out(str);
	int num;
	out >> (str.find("0x") != string::npos ? hex : dec) >> num;
	return num;
}
wstring str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}
string wstr_to_str(wstring ws) {
	string ret(ws.begin(), ws.end());
	return ret;
}

vector<vector<string>> split_vec(const vector<string>& vec, const string& delimiter) {
	vector<vector<string>> ret;
	vector<string> token;
	for (UINT i = 0; i < vec.size(); i++) {
		if (token.size() > 0 && vec[i] == delimiter) {
			ret.push_back(token);
			token.erase(token.begin(), token.end());
			continue;
		}
		if (vec[i] != delimiter)
			token.push_back(vec[i]);
	}
	if (token.size() > 0)
		ret.push_back(token);
	return ret;
}

vector<string> split_str(string str, const string& delimiter, int minlen /*= -1*/) {
	vector<string> ret;
	UINT pos = 0;
	string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		if ((int)token.size() >= minlen)
			ret.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	if ((int)str.size() > 0)
		ret.push_back(str);
	return ret;
}

string error_code_to_text(DWORD error_code) {
	string ret;
	LPTSTR buf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buf,
		0, NULL);
	return wstr_to_str(buf) + '(' + int_to_str((int)error_code) + ')';
}


void CONSOLE_ON() {
	cout.clear();
	FILE *stream;
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen_s(&stream, "CONOUT$", "w", stdout);
	HWND hwnd = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(hwnd, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

string getwindowclassname(HWND hwnd) {
	size_t i;
	TCHAR clsss[15];
	char a[15];
	GetClassName((HWND)hwnd, clsss, 15);
	wcstombs_s(&i, a, (size_t)15, clsss, 15);
	return string(a);
}
string getwindowtext(HWND wnd) {
	wchar_t text[256];
	GetWindowText(wnd, text, 256);
	return wstr_to_str(text);
}
RECT getwindowrect(HWND wnd) {
	RECT ret;
	GetWindowRect(wnd, &ret);
	return ret;
}
RECT getclientrect(HWND wnd) {
	RECT ret;
	GetClientRect(wnd, &ret);
	return ret;
}
RECT getmappedclientrect(HWND wndFrom, HWND wndTo /*= NULL*/) {
	RECT ret = getclientrect(wndFrom);
	MapWindowPoints(wndFrom, wndTo, (LPPOINT)&ret, 2);
	return ret;
}
void displayrect(RECT& rc) {
	cout << "left: " << rc.left << " right: " << rc.right << " top: " << rc.top << " bottom: " << rc.bottom << '\n';
}
POINT getclientcursorpos(HWND hwnd_parent) {
	RECT temprect;
	GetClientRect(hwnd_parent, &temprect);
	MapWindowPoints(hwnd_parent, NULL, (LPPOINT)&temprect, 2);
	POINT temppoint;
	GetCursorPos(&temppoint);
	POINT client_cursor_pos;
	client_cursor_pos.x = temppoint.x - temprect.left;
	client_cursor_pos.y = temppoint.y - temprect.top;
	return client_cursor_pos;
}
POINT getcursorpos() {
	POINT ret;
	GetCursorPos(&ret);
	return ret;
}
bool is_cursor_in_region(RECT region, POINT cursor_pos) {
	return (cursor_pos.x > region.left && cursor_pos.x < region.right && cursor_pos.y < region.bottom && cursor_pos.y > region.top);
}

bool GetFolder(std::string& folderpath, const wchar_t* szCaption, HWND hOwner) {
	bool retVal = false;

	// The BROWSEINFO struct tells the shell 
	// how it should display the dialog.
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.ulFlags = BIF_USENEWUI;
	bi.hwndOwner = hOwner;
	bi.lpszTitle = szCaption;

	// must call this if using BIF_USENEWUI
	::OleInitialize(NULL);

	// Show the dialog and get the itemIDList for the 
	// selected folder.
	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if (pIDL != NULL) {
		// Create a buffer to store the path, then 
		// get the path.
		wchar_t buffer[_MAX_PATH] = {'\0'};
		if (::SHGetPathFromIDList(pIDL, buffer) != 0) {
			// Set the string value.
			folderpath = wstr_to_str((wstring)buffer);
			retVal = true;
		}

		// free the item id list
		CoTaskMemFree(pIDL);
	}

	::OleUninitialize();

	return retVal;
}
void GetFilesInDirectory(std::vector<string> &out, const string &directory, const string &filetype) {
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile(str_to_wstr((directory + "/*." + filetype)).c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const string file_name = wstr_to_str(file_data.cFileName);
		const string full_file_name = directory + "\\" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		out.push_back(full_file_name);
	}
	while (FindNextFile(dir, &file_data));

	FindClose(dir);
}

HKEY OpenRegKey(HKEY hRootKey, wchar_t* strKey) {
	HKEY hKey;
	LONG nError = RegOpenKeyEx(hRootKey, strKey, NULL, KEY_ALL_ACCESS, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND) {
		cout << "Creating registry key: " << strKey << endl;
		nError = RegCreateKeyEx(hRootKey, strKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if (nError)
		cout << "Error: " << nError << " Could not find or create " << strKey << endl;

	return hKey;
}
void SetRegVal(HKEY hKey, LPCTSTR lpValue, DWORD data) {
	LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_DWORD, (LPBYTE)&data, sizeof(DWORD));

	if (nError)
		cout << "Error: " << nError << " Could not set registry value: " << (char*)lpValue << endl;
}
DWORD GetRegVal(HKEY hKey, LPCTSTR lpValue) {
	DWORD data;		DWORD size = sizeof(data);	DWORD type = REG_DWORD;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
		data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		cout << "Error: " << nError << " Could not get registry value " << (char*)lpValue << endl;

	return data;
}

string milliseconds_to_hms(int milliseconds) {
	string milli = int_to_str(milliseconds % 1000);
	string seconds = int_to_str((milliseconds / 1000) % 60);
	string minutes = int_to_str(((milliseconds / (1000 * 60)) % 60));
	string hours = int_to_str(((milliseconds / (1000 * 60 * 60)) % 24));
	if (seconds.size() < 2)
		seconds = '0' + seconds;
	if (minutes.size() < 2)
		minutes = '0' + minutes;
	return (hours + ':' + minutes + ':' + seconds + '.' + milli);
}

