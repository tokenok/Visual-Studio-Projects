#include <iostream>
#include <sstream>
#include <string>
#include <sapi.h>
#include <fstream>

ISpVoice *Voice = NULL;
HRESULT hRes;

std::wstring str_to_wstr(std::string s) {
	std::wstring w(s.begin(), s.end());
	return w;
}

void init_voice_com() {
	CoInitialize(0);
	hRes = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&Voice);
}
void voice_release() {
	if (Voice) {
		Voice->Release();
		Voice = NULL;
		CoUninitialize();
	}
}

int main() {
	init_voice_com();

	std::string s = "";
	while (std::getline(std::cin, s)) {
		Voice->Speak(str_to_wstr(s + "\0").c_str(), 0, 0);
	}

	voice_release();
}
