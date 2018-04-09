#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <time.h>
#include <sstream>
#include <Netfw.h>
#include <comutil.h>
#include <regex>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "comsuppw.lib")

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////

#pragma region function decs

HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);

wstring str_to_wstr(string s);
string wstr_to_str(wstring ws);
BSTR wstr_to_bstr(wstring ws);
BSTR str_to_bstr(string s);
wstring bstr_to_wstr(BSTR bs);
string bstr_to_str(BSTR bs);
string int_to_str(int num, bool is_hex = false);
int str_to_int(string str);

string error_code_to_text(DWORD error_code);

BSTR get_ips();
BOOL add_ip_to_rules(BSTR ips);

void SHOW_CONSOLE(bool show = true, bool noclose = false);

ATOM register_class(HINSTANCE hInstance);

#pragma endregion

///////////////////////////////////////////////////////////////////////////////////////////

#define VERSION L"Tokenhack2_v1.1"

const TCHAR className[] = TEXT("badguyblockerakalettherightonesin");

VOID CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND g_hwnd;

UINT timer_id = 1;

///////////////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*args*/, int /*iCmdShow*/) {
	SHOW_CONSOLE();
	SetConsoleTitle(VERSION);

	register_class(hInstance);

	g_hwnd = CreateWindow(className, VERSION, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 350, 150, 500, 400, NULL, NULL, hInstance, NULL);
	//ShowWindow(g_hwnd, SW_SHOW);

	SetTimer(g_hwnd, timer_id, 60000, timerProc);
	timerProc(g_hwnd, NULL, timer_id, 0);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CLOSE:
		case WM_DESTROY:{
			PostQuitMessage(0);
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
VOID CALLBACK timerProc(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR idEvent, DWORD /*dwTime*/) {
	if (idEvent == timer_id) {
		static int tic_count = 0;
		tic_count++;
		cout << "tic (" << tic_count << ")\n";

		add_ip_to_rules(get_ips());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

BSTR get_ips() {
	BSTR ips = L"";
	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		driver = get_driver_instance();
		con = driver->connect("localhost"/*"192.99.160.107"*/, "ipcheck", "tokenhack");
		con->setSchema("pvpgn");		

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT acct_lastlogin_ip FROM pvpgn_bnet WHERE acct_lastlogin_owner != \"zcc\" AND acct_lastlogin_owner != \"\"");
		int count = 0, ip_count = 0;
		_bstr_t str = L"";
		while (res->next()) {
			string t_ip = (res->getString(1));
			if (regex_match(t_ip, regex("(\\d{1,3}(\\.\\d{1,3}){3})"))) { 
				ip_count++;
				_bstr_t temp = str_to_bstr(t_ip);
				_bstr_t ending = L"/255.255.255.255,"; 
				str += (temp + ending);
			}
			else
				cout << "Invalid Ip entry: (" << t_ip << ") at pos = " << count << '\n';
			count++;
		}
		if (!ip_count)
			cout << "no ips found\n";
		ips = str;
		delete res;
		delete stmt;
		delete con;
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return ips;
}

BOOL add_ip_to_rules(BSTR ips) {
	BSTR bstrRuleName = SysAllocString(L"D2GSFilter");

	BOOL ret = FALSE;

	HRESULT hrComInit = S_OK;
	HRESULT hr = S_OK;

	INetFwPolicy2 *pNetFwPolicy2 = NULL;
	INetFwRules *rules = NULL;

	// Initialize COM.
	hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	if (hrComInit != RPC_E_CHANGED_MODE) {
		if (FAILED(hrComInit)) {
			printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
			cout << error_code_to_text(GetLastError()) << '\n';
			goto Cleanup;
		}
	}
	hr = WFCOMInitialize(&pNetFwPolicy2);
	if (FAILED(hr)) {
		cout << "failed\n";
		cout << error_code_to_text(GetLastError()) << '\n';
		goto Cleanup;
	}
	hr = pNetFwPolicy2->get_Rules(&rules);
	if (FAILED(hr)) {
		printf("get_Rules failed: 0x%08lx\n", hr);
		cout << error_code_to_text(GetLastError()) << '\n';
		goto Cleanup;
	}
	
	ret = TRUE;
	cout << "Success!\n";
		
	INetFwRule *rule;
	rules->Item(bstrRuleName, &rule);

	hr = rule->put_RemoteAddresses(ips);
	if (FAILED(hr)) {
		printf("Put Remote Address failed: 0x%08lx\n", hr);
		cout << error_code_to_text(GetLastError()) << '\n';
		goto Cleanup;
	}

Cleanup:
	// Free BSTR's
	SysFreeString(bstrRuleName);

	// Release the INetFwRule object
	if (rule != NULL) {
		rule->Release();
	}

	// Release the INetFwRules object
	if (rules != NULL) {
		rules->Release();
	}

	// Release the INetFwPolicy2 object
	if (pNetFwPolicy2 != NULL) {
		pNetFwPolicy2->Release();
	}

	// Uninitialize COM.
	if (SUCCEEDED(hrComInit)) {
		CoUninitialize();
	}
	return ret;
}

HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2) {
	HRESULT hr = S_OK;

	hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)ppNetFwPolicy2);

	if (FAILED(hr)) 
		printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);

	return hr;
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
string int_to_str(int num, bool is_hex /*= false*/) {
	cout.clear();
	stringstream out;
	out << (is_hex ? hex : dec) << num;
	string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}
int str_to_int(string str) {
	cout.clear();
	stringstream out(str);
	int num;
	out >> (str.find("0x") != string::npos ? hex : dec) >> num;
	return num;
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
		if (noclose) {
			HWND hwnd = GetConsoleWindow();
			HMENU hmenu = GetSystemMenu(hwnd, FALSE);
			EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
		}
	}
	else
		FreeConsole();
	show_state = show;
}

ATOM register_class(HINSTANCE hInstance) {
	WNDCLASSEX wnd = {0};
	wnd.hInstance = hInstance;
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = WndProc;
	wnd.style = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	return RegisterClassEx(&wnd);
}

///////////////////////////////////////////////////////////////////////////////////////////

