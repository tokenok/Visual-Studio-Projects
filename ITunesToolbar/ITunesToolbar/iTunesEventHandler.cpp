#include "iTunesEventHandler.h"
#include <iostream>
#include <atlbase.h>

using namespace std;

CiTunesEventHandler::CiTunesEventHandler() {
	m_dwRefCount = 0;
	CComPtr<ITypeLib> pITypeLib = NULL;
	HRESULT	hr = ::LoadRegTypeLib(LIBID_iTunesLib, 1, 5, 0x00, &pITypeLib);
	// Get type information for the interface of the object.
	hr = pITypeLib->GetTypeInfoOfGuid(DIID__IiTunesEvents, &m_pITypeInfo);
	//pITypeLib->Release();
}

CiTunesEventHandler::~CiTunesEventHandler() {}

HRESULT STDMETHODCALLTYPE CiTunesEventHandler::QueryInterface(REFIID iid, void **ppvObject) {
	if ((iid == IID_IDispatch) || (iid == DIID__IiTunesEvents)) {
		m_dwRefCount++;
		*ppvObject = this;//(_IiTunesEvents *)this;
		cout << "IiTunesEvents *ppvObject = this\n";
		return S_OK;
	}
	if (iid == IID_IUnknown) {
		m_dwRefCount++;
		*ppvObject = this;//(IUnknown *)this;
		cout << "IID_IUnknown *ppvObject = this\n";
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CiTunesEventHandler::AddRef() {
	InterlockedIncrement(&m_dwRefCount);
	return m_dwRefCount;
}

ULONG STDMETHODCALLTYPE CiTunesEventHandler::Release() {
	InterlockedDecrement(&m_dwRefCount);
	if (m_dwRefCount == 0) {
		delete this;
		return 0;
	}
	return m_dwRefCount;
}

HRESULT STDMETHODCALLTYPE CiTunesEventHandler::Invoke(DISPID dispidMember, REFIID, LCID, WORD, DISPPARAMS* pdispparams, VARIANT*, EXCEPINFO*, UINT*) {
	switch (dispidMember) {
		case ITEventPlayerPlay: {
			this->OnPlayerPlayEvent();
			break;
		}
		case ITEventPlayerStop: {
			this->OnPlayerStopEvent();
			break;
		}
		case ITEventAboutToPromptUserToQuit: {
			this->OnAboutToPromptUserToQuitEvent();
			break;
		}
		case ITEventSoundVolumeChanged: {
			this->OnSoundVolumeChangedEvent();
			break;
		}
		default: break;
	}
	return S_OK;
} 