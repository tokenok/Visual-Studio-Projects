#include <string>
#include <iostream>
#include <thread>

#include "Voice.h"

using namespace std;

wstring str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}

int main() {
	string s;

	while (cin >> s) {
		thread t(
			[&]() {
				const wchar_t* c = str_to_wstr(s).c_str();
				Voice->Speak(c, 0, 0);
			}
		);
		t.detach();
	}

	return 0;
}