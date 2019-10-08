#pragma once

#include "iTunesCOMInterface.h"

#include <atlbase.h>

class CiTunesEventHandler: public _IiTunesEvents {
	private:
	long m_dwRefCount;
	CComPtr<ITypeInfo> m_pITypeInfo; // Pointer to type information.
	
	public:
	CiTunesEventHandler();
	~CiTunesEventHandler();

	HRESULT OnPlayerPlayEvent();
	HRESULT OnPlayerStopEvent();
	HRESULT OnSoundVolumeChangedEvent();
	HRESULT OnAboutToPromptUserToQuitEvent();
	//...and all the functions you want to use

	long getREF() { return m_dwRefCount; }

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *) { return E_NOTIMPL; };
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo **) { return E_NOTIMPL; };
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(const IID &, LPOLESTR *, UINT, LCID, DISPID *) { return E_NOTIMPL; };
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispidMember, REFIID, LCID, WORD, DISPPARAMS* pdispparams, VARIANT*, EXCEPINFO*, UINT*);
};