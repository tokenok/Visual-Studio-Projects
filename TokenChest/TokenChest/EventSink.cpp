// EventSink.cpp
#include "eventsink.h"

#include <thread>

#include "common.h"
#include "inject.h"
#include "../TokenChestDll/Dll.h"

ULONG EventSink::AddRef() {
	return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release() {
	LONG lRef = InterlockedDecrement(&m_lRef);
	if (lRef == 0)
		delete this;
	return lRef;
}

HRESULT EventSink::QueryInterface(REFIID riid, void** ppv) {
	if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
		*ppv = (IWbemObjectSink *) this;
		AddRef();
		return WBEM_S_NO_ERROR;
	}
	else return E_NOINTERFACE;
}


HRESULT EventSink::Indicate(long lObjectCount,
	IWbemClassObject **apObjArray) {
//	HRESULT hres = S_OK;
	**apObjArray;// removes compiler warning: 4100 unreferenced formal parameter
	//for (int i = 0; i < lObjectCount; i++) {
		//IWbemClassObject *pObj = apObjArray[i];
		//BSTR text;
		//pObj->GetObjectText(WBEM_FLAG_NO_FLAVORS, &text);
		//
		//string s = bstr_to_str(text);
		//cout << s << '\n'; //TargetInstance.Handle
	if (lObjectCount) {
		this_thread::sleep_for(chrono::milliseconds(3000));

		EnumWindows(EnumWindowsProc, INJECT_DLL);
	}
		
	//}

	return WBEM_S_NO_ERROR;
}

HRESULT EventSink::SetStatus(
	/* [in] */ LONG lFlags,
	/* [in] */ HRESULT hResult,
	/* [in] */ BSTR strParam,
	/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
	) {
	strParam; *pObjParam; // removes compiler warning: 4100 unreferenced formal parameter
	printf("Status: %d", lFlags);
	if (lFlags == WBEM_STATUS_COMPLETE) {
		printf("Call complete. hResult = 0x%X\n", hResult);
	}
	else if (lFlags == WBEM_STATUS_PROGRESS) {
		printf("Call in progress.\n");
	}

	return WBEM_S_NO_ERROR;
}    // end of EventSink.cpp