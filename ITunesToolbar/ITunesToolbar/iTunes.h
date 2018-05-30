#ifndef itunes_header_guard
#define itunes_header_guard

#include "iTunesEventHandler.h"

#include <string>
#include <vector>
#include <iostream>

void init_itunes_com();
void Iplaypause();
void Inext();
void Iprev();
void Ivolu();
void Ivold();
long Igetvol();
void Isetvol(long vol);
std::string Igetcursong();
std::string Igetcurartist();
void Imaximize();
bool Iisminimized();
bool Iisplay();
void Icleanup();

class CCoInitialize {
	private:
	HRESULT m_hr;
	public:
	CCoInitialize():m_hr(E_NOT_SET) {}
	CCoInitialize(PVOID pReserved, HRESULT *hr)
		: m_hr(E_UNEXPECTED) {
		*hr = m_hr = CoInitialize(pReserved);
	}
	~CCoInitialize() { if (SUCCEEDED(m_hr)) { std::cout << "CoUninitialize()\n"; CoUninitialize(); } }

	void init(PVOID pReserved, HRESULT *hr) {
		if (m_hr == E_NOT_SET) {
			m_hr = CoInitialize(pReserved);
		}
		*hr = m_hr;
	}
};

#endif

