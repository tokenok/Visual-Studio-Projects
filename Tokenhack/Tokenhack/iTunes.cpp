#include <string>
#include <iostream>
#include "iTunesCOMInterface.h"
#include "iTunes.h"

IiTunes *iITunes = 0;
IITTrack *iITrack = 0;
HRESULT ihRes;

void init_itunes_com(){
	::CoInitialize(0);
	ihRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);
}
void itunes_release(){
	if (ihRes == S_OK && iITunes) 
		iITunes->Release();
	if (iITunes)
		CoUninitialize();
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
std::string Igetcursong(){
	std::wstring temp;
	BSTR bt = 0;
	if (ihRes == S_OK && iITunes){
		iITunes->get_CurrentTrack(&iITrack);
		iITrack->get_Name((BSTR *)&bt);
		temp = bt;
	}
	else{ std::cout << "failed" << '\n'; }
	return std::string(temp.begin(), temp.end());
}
std::string Igetcurartist(){
	std::wstring temp;
	BSTR bt = 0;
	if (ihRes == S_OK && iITunes){
		iITunes->get_CurrentTrack(&iITrack);
		iITrack->get_Artist((BSTR *)&bt);
		temp = bt;
	}
	else{ std::cout << "failed" << '\n'; }
	return std::string(temp.begin(), temp.end());
}
//void test() {
//	HRESULT hres;
//	DWORD d;
//	IConnectionPoint * icp;
//	IConnectionPointContainer * icpc;
//	hres = iITunes->QueryInterface(IID_IConnectionPointContainer, (PVOID *)&icpc);
//	hres = icpc->FindConnectionPoint(DIID__IiTunesEvents, &icp);
//	 eventSink = new ITunesEventSink();
//	hres = icp->Advise((IUnknown*)&eventSink, &d);
//}
/*


IConnectionPointContainer  * pCPC;

hr=spITunesApp->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);

IConnectionPoint * pCP;

_IiTunesEvents * pITEvent;
hr=pCPC->FindConnectionPoint (__uuidof(pITEvent),&pCP);
pCPC->Release();

IUnknown *pSinkUnk;
CITunesEventSink * pSink;
pSink=new CITunesEventSink;
hr = pSink->QueryInterface (IID_IUnknown,(void **)&pSinkUnk);

DWORD dwAdvise;
hr = pCP->Advise(pSinkUnk,&dwAdvise);
*/

