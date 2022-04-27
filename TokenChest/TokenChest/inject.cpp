#include "inject.h"

#include "common.h"
#include "../TokenChestDll/Dll.h"
#include "include.h"

#define INJECT_DLL 1

WMIclss* WMIstart = NULL;
map<HWND, HHOOK> injectedHWNDs;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	switch (lParam) {
		case INJECT_DLL:{
			if (getwindowtext(hwnd) == "Diablo II") {
				string path = getexepath(hwnd);
				if (path == "")	MessageBox(hwnd, str_to_wstr("Could not get Diablo II exe path.\n" + error_code_to_text(GetLastError())).c_str(), L"ERROR", MB_OK);
				string exename = path.substr(path.rfind("\\") + 1, path.size() - path.rfind("\\"));
				if (exename == "Game.exe") {
					if (!isInjected(hwnd)) {
						if (!InjectDll(hwnd)) {
							MessageBox(hwnd, L"Injection failed", L"ERROR", MB_OK);
							break;
						}
					}
				}
			}
			break;
		}
	}
	return TRUE;
}

WMIclss::WMIclss() {
	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		MessageBox(NULL, str_to_wstr("Failed to initialize COM library. Error code = " + (int_to_str((int)hres, true))).c_str(), L"ERROR", MB_OK);
		return;                 // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM negotiates service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);


	if (FAILED(hres)) {
		MessageBox(NULL, str_to_wstr("Failed to initialize security. Error code = " + (int_to_str((int)hres, true))).c_str(), L"ERROR", MB_OK);
		pSink = NULL;
		CoUninitialize();
		return;                 // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------


	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		MessageBox(NULL, str_to_wstr("Failed to create IWbemLocator object. Err code = " + (int_to_str((int)hres, true))).c_str(), L"ERROR", MB_OK);
		CoUninitialize();
		return;                 // Program has failed.
	}

	// Step 4: ---------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method


	// Connect to the local root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
		);

	if (FAILED(hres)) {
		MessageBox(NULL, str_to_wstr("Could not connect.Error code = " + (int_to_str((int)hres, true))).c_str(), L"ERROR", MB_OK);
		pLoc->Release(); pLoc = NULL;
		CoUninitialize();
		return;
	}

	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres)) {
		MessageBox(NULL, str_to_wstr("Could not set proxy blanket. Error code = " + (int_to_str((int)hres, true))).c_str(), L"ERROR", MB_OK);
		pSvc->Release(); pSvc = NULL;
		pLoc->Release(); pLoc = NULL;
		CoUninitialize();
		return;         // Program has failed.
	}

	// Step 6: -------------------------------------------------
	// Receive event notifications -----------------------------

	// Use an unsecured apartment for security


	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL,
		CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
		(void**)&pUnsecApp);

	pSink = new EventSink;
	pSink->AddRef();


	pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

	pStubUnk->QueryInterface(IID_IWbemObjectSink,
		(void **)&pStubSink);

	// The ExecNotificationQueryAsync method will call
	// The EventQuery::Indicate method when an event occurs
	hres = pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"),
		_bstr_t("SELECT * "
		"FROM __InstanceCreationEvent WITHIN 1 "
		"WHERE TargetInstance ISA 'Win32_Process' AND TargetInstance.Name = 'game.exe'"),
		WBEM_FLAG_SEND_STATUS,
		NULL,
		pStubSink);

	// Check for errors.
	if (FAILED(hres)) {
		MessageBox(NULL, str_to_wstr("ExecNotificationQueryAsync failed with error = " + (int_to_str((int)hres, true))).c_str(), L"ERROR", MB_OK);
		pSvc->Release(); pSvc = NULL;
		pLoc->Release(); pLoc = NULL;
		pUnsecApp->Release(); pUnsecApp = NULL;
		pStubUnk->Release(); pStubUnk = NULL;
		pSink->Release(); pSink = NULL;
		pStubSink->Release(); pStubSink = NULL;
		CoUninitialize();
	}
}
WMIclss::~WMIclss() {
	if (pStubSink && pSvc) {
		hres = pSvc->CancelAsyncCall(pStubSink);
		if (FAILED(hres)) 
			MessageBox(NULL, str_to_wstr("CancelAsyncCall failed with error = " + (int_to_str((int)hres, true))).c_str(), L"ERROR", MB_OK);
	}

	if (pSvc) pSvc->Release(); pSvc = NULL;
	if (pLoc) pLoc->Release(); pLoc = NULL;
	if (pUnsecApp) pUnsecApp->Release(); pUnsecApp = NULL;
	if (pStubUnk) pStubUnk->Release(); pStubUnk = NULL;
	if (pSink) pSink->Release(); pSink = NULL;
	if (pStubSink) pStubSink->Release(); pStubSink = NULL;
	CoUninitialize();
}

BOOL isInjected(HWND wnd) {
	return (injectedHWNDs[wnd] != NULL);
}
BOOL InjectDll(HWND target) {
	HMODULE hDll = LoadLibrary(DLL_NAME);
	HOOKPROC cbtProcAddr = (HOOKPROC)GetProcAddress(hDll, "_msgretProc@12");
	if (!cbtProcAddr) {
		MessageBox(NULL, str_to_wstr("Unable to Find Proc Address of: _msgretProc@12\n" + error_code_to_text(GetLastError())).c_str(), L"ERROR", MB_OK);
		return FALSE;
	}
	HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROCRET, cbtProcAddr, hDll, GetWindowThreadProcessId(target, NULL));
	if (!hook) {
		MessageBox(NULL, str_to_wstr("Injection failed." + error_code_to_text(GetLastError())).c_str(), L"ERROR", MB_OK);
		return FALSE;
	}
	injectedHWNDs[target] = hook;

	return TRUE;
}
void AutoInjectOn(bool on) {
	if (on) {
		WMIstart = new WMIclss();
	}
	else {
		if (WMIstart) {
			delete WMIstart;
			WMIstart = NULL;
		}
		for (auto a : injectedHWNDs) 
			UnhookWindowsHookEx(a.second);
		injectedHWNDs.clear();
	}
}
void EnableDebugPriv(void) {
	HANDLE hToken;
	LUID SeDebugNameValue;
	TOKEN_PRIVILEGES TokenPrivileges;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &SeDebugNameValue)) {
			TokenPrivileges.PrivilegeCount = 1;
			TokenPrivileges.Privileges[0].Luid = SeDebugNameValue;
			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if (AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
				CloseHandle(hToken);
			}
			else {
				CloseHandle(hToken);
				MessageBox(NULL, L"Couldn't adjust token privileges!", L"ERROR", MB_OK);
			}
		}
		else {
			CloseHandle(hToken);
			MessageBox(NULL, L"Couldn't look up privilege value!", L"ERROR", MB_OK);
		}
	}
	else {
		MessageBox(NULL, L"Couldn't open process token!", L"ERROR", MB_OK);
	}
}

