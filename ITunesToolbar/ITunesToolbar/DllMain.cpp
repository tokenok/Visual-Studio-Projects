#include <strsafe.h> // for StringCchXXX functions
#include <olectl.h> // for SELFREG_E_CLASS
#include <shlobj.h> // for ICatRegister
#include <atlbase.h>
#include "ClassFactory.h" // for the class factory

#include <iostream>

// {46B3D3EF-071A-4b7e-8AA2-E560810DAB35}
CLSID CLSID_DeskBand = {0x46b3d3ef, 0x71a, 0x4b7e, {0x8a, 0xa2, 0xe5, 0x60, 0x81, 0xd, 0xab, 0x35}};

HINSTANCE   g_hInst = NULL;
long        g_cDllRef = 0;

STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void *) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		g_hInst = hInstance;
		DisableThreadLibraryCalls(hInstance);
	}
	return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) {
	HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

	if (IsEqualCLSID(CLSID_DeskBand, rclsid)) {
		hr = E_OUTOFMEMORY;

	//	if (FindWindowW(NULL, L"iTunes") != NULL) {
			CComPtr<CClassFactory> pClassFactory = new CClassFactory();
			if (pClassFactory) {
				hr = pClassFactory->QueryInterface(riid, ppv);
				//pClassFactory->Release();
			}
	//	}
	}

	return hr;
}

STDAPI DllCanUnloadNow() {
	std::cout << "g_cDllRef " << g_cDllRef << '\n';
	return g_cDllRef > 0 ? S_FALSE : S_OK;
}

HRESULT RegisterServer() {
	WCHAR szCLSID[MAX_PATH];
	StringFromGUID2(CLSID_DeskBand, szCLSID, ARRAYSIZE(szCLSID));

	WCHAR szSubkey[MAX_PATH];
	HKEY hKey;

	HRESULT hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
	if (SUCCEEDED(hr)) {
		hr = E_FAIL;
		if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CLASSES_ROOT, szSubkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL)) {
			WCHAR const szName[] = L"ITunes Toolbar";
			if (ERROR_SUCCESS == RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)szName, sizeof(szName))) {
				hr = S_OK;
			}
			else {
				MessageBox(NULL, L"RegSetValueExW failed", L"Error", MB_OK);
			}
		
			RegCloseKey(hKey);
		}
		else {
			MessageBox(NULL, L"RegCreateKeyExW failed", L"Error" , MB_OK);
		}
	}
	else
		MessageBox(NULL, L"StringCchPrintfW failed", L"Error", MB_OK);

	if (SUCCEEDED(hr)) {
		hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s\\InprocServer32", szCLSID);
		if (SUCCEEDED(hr)) {
			hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, szSubkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));
			if (SUCCEEDED(hr)) {
				WCHAR szModule[MAX_PATH];
				if (GetModuleFileNameW(g_hInst, szModule, ARRAYSIZE(szModule))) {
					DWORD cch = lstrlen(szModule);
					hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)szModule, cch * sizeof(szModule[0])));
				}
				else
					MessageBox(NULL, L"GetModuleFileNameW failed", L"Error", MB_OK);

				if (SUCCEEDED(hr)) {
					WCHAR const szModel[] = L"Apartment";
					hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ, (LPBYTE)szModel, sizeof(szModel)));
				}
				else
					MessageBox(NULL, L"HRESULT_FROM_WIN32(RegSetValueExW,...) failed", L"Error", MB_OK);

				RegCloseKey(hKey);
			}
			else
				MessageBox(NULL, L"RegCreateKeyExW failed", L"Error", MB_OK);
		}
		else
			MessageBox(NULL, L"StringCchPrintfW failed", L"Error", MB_OK);
	}
	else
		MessageBox(NULL, L"failed", L"Error", MB_OK);

	return hr;
}

HRESULT RegisterComCat() {
	CComPtr<ICatRegister> pcr;
	HRESULT hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pcr));
	if (SUCCEEDED(hr)) {
		CATID catid = CATID_DeskBand;
		hr = pcr->RegisterClassImplCategories(CLSID_DeskBand, 1, &catid);
		//pcr->Release();
	}
	return hr;
}

STDAPI DllRegisterServer() {
	::CoInitialize(0);
	// Register the deskband object.
	HRESULT hr = RegisterServer();
	
	if (SUCCEEDED(hr)) {
		// Register the component category for the deskband object.
		hr = RegisterComCat();
	}

	if (!SUCCEEDED(hr))
		MessageBox(NULL, L"failed\n", L"falied", MB_OK);

	return SUCCEEDED(hr) ? S_OK : SELFREG_E_CLASS;
}

STDAPI DllUnregisterServer() {
	WCHAR szCLSID[MAX_PATH];
	StringFromGUID2(CLSID_DeskBand, szCLSID, ARRAYSIZE(szCLSID));

	WCHAR szSubkey[MAX_PATH];
	HRESULT hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
	if (SUCCEEDED(hr)) {
		if (ERROR_SUCCESS != RegDeleteTreeW(HKEY_CLASSES_ROOT, szSubkey)) {
			hr = E_FAIL;
		}
	}

	return SUCCEEDED(hr) ? S_OK : SELFREG_E_CLASS;
}

