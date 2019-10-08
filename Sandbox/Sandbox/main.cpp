#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <conio.h>
#include <iomanip>
#include <Windows.h>

#include "c:\CPPlibs\common\f\common.h"

using namespace std;

int main() {

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess(NULL, L"cmd /c START /MIN \"\" cmd /c \"/D \"C:\\Users\\Josh\\Documents\\Visual Studio 2017\\Projects\\Tokenhack\Release\"\" Tokenhack.exe", NULL, NULL, NULL, NULL, NULL, NULL, &si, &pi);
	
	cout << getlasterror() << '\n';

	/*int n; 
	std::cin >> n;
	for (; n-->0;) { 
		double t, s = 0, i = -1;
		std::cin >> t;
		for (; i++<t-1;) { 
			s += (pow(-1, i) / (2 * i + 1)); 
		}
		std::cout << std::setprecision(15) << s << '\n'; 
	}*/

	return 0;
}