#include <Windows.h>
#include <objidl.h>
#include <Psapi.h>
#include "shlobj.h"
#include "Shlwapi.h"

#pragma comment(lib, "Shlwapi.lib")

#include "common.h"

using namespace std;

string int_to_str(int num, bool is_hex /*= false*/) {
	cout.clear();
	stringstream out;
	out << (is_hex ? hex : dec) << num;
	string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}
string uint_to_str(unsigned int num, bool is_hex /*= false*/) {
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
unsigned int str_to_uint(string str) {
	cout.clear();
	stringstream out(str);
	unsigned int num;
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
BSTR wstr_to_bstr(wstring ws) {
	BSTR bs = SysAllocStringLen(ws.data(), ws.size());
	return bs;
}
BSTR str_to_bstr(string s) {
	return wstr_to_bstr(str_to_wstr(s));
}
wstring bstr_to_wstr(BSTR bs) {
	wstring ws(bs, SysStringLen(bs));
	return ws;
}
string bstr_to_str(BSTR bs) {
	return wstr_to_str(bstr_to_wstr(bs));
}
string LPVOID_to_str(LPVOID data, int len) {
	wstring wsdata(static_cast<const wchar_t *>(data), len);
	return wstr_to_str(wsdata);
}
//LEAKS + warning
LPWSTR str_to_LPWSTR(std::string s) {
	LPWSTR m = new wchar_t[s.size() + 1];
	copy(s.begin(), s.end(), m);
	m[s.size()] = '\0';
	return m;
}
string LPWSTR_to_str(LPWSTR wc, UINT size) {
	char* ch = new char[size];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wc, -1, ch, size, &DefChar, NULL);
	string text(ch, size);
	delete[] ch;
	return text;
}
string wastr_to_str(wchar_t *wc, UINT size) {
	char* ch = new char[size];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wc, -1, ch, size, &DefChar, NULL);
	string text(ch);
	delete[] ch;
	return text;
}

std::string FormatWithCommas(unsigned int num) {
	std::stringstream out;
	out.imbue(std::locale(""));
	out << std::fixed << num;
	return out.str();
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

void SHOW_CONSOLE(bool show, bool noclose) {
	static bool show_state = false;
	if (show && !show_state) {
		std::cout.clear();
		FILE *stream;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&stream, "CONOUT$", "w", stdout);
		if (noclose) 
			EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}
	if (!show)
		FreeConsole();
	show_state = show;
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
	int len = GetWindowTextLength(wnd) + 1;
	wchar_t* text = new wchar_t[len];
	GetWindowText(wnd, text, len);
	string ret = wstr_to_str(text);
	delete[] text;
	return ret;
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

//browse for folder
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
void GetFilesInDirectory(const std::string &directory, const std::string &filetype, std::vector<std::string> *out) {
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

		out->push_back(full_file_name);
	}
	while (FindNextFile(dir, &file_data));

	FindClose(dir);
}
std::string getexepath() {
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	return wstr_to_str(path);
}
std::string getexepath(HWND hwnd) {
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!handle)
		return "";
	wchar_t wpath[MAX_PATH];
	GetModuleFileNameEx(handle, NULL, wpath, MAX_PATH);
	CloseHandle(handle);
	return wstr_to_str(wpath);
}
std::string getexedir() {
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	string dir = wstr_to_str(path);
	return dir.substr(0, dir.rfind('\\'));
}
DWORD createfolder(std::string path) {
	if (!PathFileExists(str_to_wstr(path).c_str()))
		CreateDirectory(str_to_wstr(path).c_str(), NULL);
	else
		return 0;
	return GetLastError();
}

string browseforfile(HWND parent, bool open/* = true*/, LPCWSTR title/* = "Browse for file"*/, LPCWSTR filter/* = "All\0*.*\0"*/) {
	OPENFILENAME ofn;
	char File[MAX_PATH];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = (LPWSTR)File;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;// TEXT("exe\0*.exe*\0All\0*.*\0Shortcut\0*.lnk*\0Text\0*.TXT\0");
	ofn.lpstrTitle = title;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS | (open ? NULL : OFN_OVERWRITEPROMPT);

	string path;
	string type = "";

	if (open) {
		GetOpenFileName(&ofn);
		path = wstr_to_str(ofn.lpstrFile);
	}
	else {
		GetSaveFileName(&ofn);
		path = wstr_to_str(ofn.lpstrFile);
		if (path.size() && path.substr(path.rfind("\\"), string::npos).rfind(".") == string::npos) {
			for (UINT i = 0, pos = 0;; i++) {
				if (ofn.lpstrFilter[i] == '\0') {
					if (type.size() > 0)
						break;
					pos++;
					continue;
				}
				if ((pos + 1) / 2 == ofn.nFilterIndex) {
					type += ofn.lpstrFilter[i];
				}
			}
			if (type.size() > 1)
				type = type.substr(1, type.size() - 2);
		}
	}

	return path + type;
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

void ssystem(std::string command, bool pause /*= true*/) {
	ShellExecute(NULL, L"open", L"C:\\WINDOWS\\system32\\cmd.exe", str_to_wstr("/c " + command + (pause ? " & pause" : "")).c_str(), NULL, SW_SHOW);
}

///get appdata path and create a folder for it
/*
TCHAR curdir[MAX_PATH];
GetModuleFileName(NULL, curdir, MAX_PATH);

TCHAR tpath[MAX_PATH];
SHGetFolderPath(g_hwnd, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, tpath);

string appdatapath = wstr_to_str(tpath);
string dllpath = appdatapath + "\\Annihilus Launcher";

if (PathFileExists(str_to_wstr(appdatapath).c_str())) {
	if (!PathFileExists(str_to_wstr(dllpath).c_str())) {
		if (CreateDirectory(str_to_wstr(dllpath).c_str(), NULL)) {
			//move dll files to dllpath
		}
		else
			MessageBox(NULL, L"Unable to create folder\n", L"ERROR", MB_OK);
	}
	else {
		cout << "already exists" << '\n';
	}
}
else {
	MessageBox(NULL, L"%APPDATA% folder does not exist", L"ERROR", MB_OK);
}
*/


///ENABLE DEBUG PRIVALIDGE
//void EnableDebugPriv(void) {
//	HANDLE              hToken;
//	LUID                SeDebugNameValue;
//	TOKEN_PRIVILEGES    TokenPrivileges;
//
//	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
//		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &SeDebugNameValue)) {
//			TokenPrivileges.PrivilegeCount = 1;
//			TokenPrivileges.Privileges[0].Luid = SeDebugNameValue;
//			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
//
//			if (AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
//				CloseHandle(hToken);
//			}
//			else {
//				CloseHandle(hToken);
//				throw std::exception("Couldn't adjust token privileges!");
//			}
//		}
//		else {
//			CloseHandle(hToken);
//			throw std::exception("Couldn't look up privilege value!");
//		}
//	}
//	else {
//		throw std::exception("Couldn't open process token!");
//	}
//}

