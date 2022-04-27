#pragma once

#include <Windows.h>
#include <map>

#include "eventsink.h"

#define INJECT_DLL 1

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

class WMIclss;
extern WMIclss* WMIstart;
extern std::map<HWND, HHOOK> injectedHWNDs;

class WMIclss {
	public:
	HRESULT hres;
	IWbemLocator *pLoc = NULL;
	IWbemServices *pSvc = NULL;
	IUnsecuredApartment* pUnsecApp = NULL;
	EventSink* pSink;
	IUnknown* pStubUnk = NULL;
	IWbemObjectSink* pStubSink = NULL;

	WMIclss();
	~WMIclss();
};

BOOL isInjected(HWND wnd);
BOOL InjectDll(HWND target);
void AutoInjectOn(bool on);
void EnableDebugPriv(void);

