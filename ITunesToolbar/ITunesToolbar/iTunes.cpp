#include <string>
#include <iostream>
#include <atlbase.h>

#include "iTunes.h"

#include "iTunesCOMInterface.h"
#include "iTunesEventHandler.h"

using namespace std;

IiTunes *iITunes;
HRESULT ihRes;

CComPtr<CiTunesEventHandler> eventHandler;
IConnectionPoint* icp;

DWORD dwAdvise;

CCoInitialize ci;

void init_itunes_com(){
	/*FILE *stream;
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen_s(&stream, "CONOUT$", "w", stdout);*/

	//if (FindWindowW(NULL, L"iTunes") != NULL) {
	ci.init(NULL, &ihRes);
	ihRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);
	
	eventHandler = new CiTunesEventHandler();
	CComPtr<IConnectionPointContainer> icpc;
	iITunes->QueryInterface(IID_IConnectionPointContainer, (void **)&icpc);
	if (icpc) {
		icpc->FindConnectionPoint(DIID__IiTunesEvents, &icp);
		if (icp) {
			icp->Advise(eventHandler, &dwAdvise);
		}
		else
			cout << "failed to get ConnectionPoint\n";
	}
	else
		cout << "failed to get ConnectionPointContainer\n";
	//}
}
void Iplaypause(){
	if (ihRes == S_OK && iITunes)
		iITunes->PlayPause();
	else{ std::cout << "failed" << '\n'; }
}
void Inext(){
	if (ihRes == S_OK && iITunes)
		iITunes->NextTrack();
	else{ std::cout << "failed" << '\n'; }
}
void Iprev(){
	if (ihRes == S_OK && iITunes)
		iITunes->PreviousTrack();
	else{ std::cout << "failed" << '\n'; }
}
void Ivolu(){
	long vol;
	if (ihRes == S_OK && iITunes){
		iITunes->get_SoundVolume(&vol);
		iITunes->put_SoundVolume(vol + 5);
	}
	else{ std::cout << "failed" << '\n'; }
}
void Ivold(){
	long vol;
	if (ihRes == S_OK && iITunes){
		iITunes->get_SoundVolume(&vol);
		iITunes->put_SoundVolume(vol - 5);
	}
	else{ std::cout << "failed" << '\n'; }
}
long Igetvol() {
	long vol = -1;
	if (ihRes == S_OK && iITunes)
		iITunes->get_SoundVolume(&vol);
	else { std::cout << "failed" << '\n'; }
	return vol;
}
void Isetvol(long vol) {
	if (ihRes == S_OK && iITunes)
		iITunes->put_SoundVolume(vol);
	else { std::cout << "failed" << '\n'; }
}
std::string Igetcursong(){
	std::wstring temp;
	BSTR bt = 0;
	if (ihRes == S_OK && iITunes){
		CComPtr<IITTrack> iITrack;
		iITunes->get_CurrentTrack(&iITrack);
		iITrack->get_Name((BSTR *)&bt);
		temp = bt;
		//iITrack->Release();
	}
	else{ std::cout << "failed" << '\n'; }
	return std::string(temp.begin(), temp.end());

	
}
std::string Igetcurartist(){
	std::wstring temp;
	BSTR bt = 0;
	if (ihRes == S_OK && iITunes){
		CComPtr<IITTrack> iITrack;
		iITunes->get_CurrentTrack(&iITrack);
		iITrack->get_Artist((BSTR *)&bt);
		temp = bt;
	//	iITrack->Release();
	}
	else{ std::cout << "failed" << '\n'; }
	return std::string(temp.begin(), temp.end());
}
void Imaximize() {
	if (ihRes == S_OK && iITunes) {	
		CComPtr<IITBrowserWindow> browserWindow;
		if (SUCCEEDED(iITunes->get_BrowserWindow(&browserWindow))) {
			browserWindow->put_Minimized(VARIANT_FALSE);			
		}
	}
}
bool Iisminimized() {	
	if (ihRes == S_OK && iITunes) {
		CComPtr<IITBrowserWindow> browserWindow;
		if (SUCCEEDED(iITunes->get_BrowserWindow(&browserWindow))) {
			VARIANT_BOOL b;
			browserWindow->get_Minimized(&b);
			return b == VARIANT_TRUE;
		}
	}	
	return true;
}
bool Iisplay() {
	if (ihRes == S_OK && iITunes) {
		ITPlayerState ps;
		iITunes->get_PlayerState(&ps);
		return ps == ITPlayerStatePlaying;
	}
	return false;
}

void Icleanup() {
	if (icp) {
		cout << "icp->Unadvise(dwAdvise) " << icp->Unadvise(dwAdvise) << '\n';
		cout << "icp->Release() " << icp->Release() << '\n';
		//icp = 0;	

		//cout << "eventHandler->Release() " << dec << eventHandler->Release() << '\n';
		//eventHandler = 0;
	}
	if (iITunes) {
	//	cout << "iITunes->Quit() " << iITunes->Quit() << '\n';
		cout << "iITunes->Release() " << iITunes->Release() << '\n';
		//iITunes = 0;
		cout << "CoUninitialize()" << '\n';
		CoUninitialize();
	}
}

