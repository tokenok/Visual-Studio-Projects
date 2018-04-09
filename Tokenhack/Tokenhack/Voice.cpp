#include "Voice.h"

ISpVoice *Voice = NULL;
HRESULT hRes;

void init_voice_com() {
	::CoInitialize(0);
	hRes = ::CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&Voice);
}
void voice_release() {
	if (Voice) {
		Voice->Release();
		Voice = NULL;
		::CoUninitialize();
	}
}