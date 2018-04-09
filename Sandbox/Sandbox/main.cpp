#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <conio.h>
#include <iomanip>

#include "c:\CPPlibs\common\f\common.h"

using namespace std;

int main() {
	
	int n; 
	std::cin >> n;
	for (; n-->0;) { 
		double t, s = 0, i = -1;
		std::cin >> t;
		for (; i++<t-1;) { 
			s += (pow(-1, i) / (2 * i + 1)); 
		}
		std::cout << std::setprecision(15) << s << '\n'; 
	}

	_getch();
	return 0;
}