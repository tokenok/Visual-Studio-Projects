#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <conio.h>

using namespace std;

typedef unsigned int uint;

string int_to_str(int n) {
	stringstream s;
	s << n;
	return s.str();
}

vector<vector<int>> chips = {
	{-1, 1, -1, 1, -1, 1, -1, 1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, 1, 1, -1, -1, 1, 1, -1},
	{-1, -1, 1, 1, -1, -1, 1, 1}
};

int dotprod(const vector<int>& a, const vector<int>& b) {	
	int sum = 0;
	for (uint i = 0; i < a.size(); i++) 
		sum += a[i] * b[i];
	return sum;
}

string printchip(const vector<int>& chip) {
	string ret = "(";
	for (uint i = 0; i < chip.size(); i++)
		ret += int_to_str(chip[i]) + (string)((i == (chip.size()) - 1) ? "" : ", ");
	return ret + ")";
}

vector<int> addchip(const vector<int>& a, const vector<int>& b) {
	vector<int> ret;

	for (uint i = 0; i < a.size(); i++) {
		ret.push_back(a[i] + b[i]);
	}

	return ret;
}

void testorthogonal() {
	for (uint i = 0; i < chips.size(); i++) {
		for (uint j = i + 1; j < chips.size(); j++) {
			int res = dotprod(chips[i], chips[j]);
			cout << printchip(chips[i]) << " " << (char)249 << " " << printchip(chips[j]) << " = " << res << '\n';
		}
	}
}

class transmitter {
	public:
	vector<int> chip_s;
	vector<int> ichip_s;

	vector<int> m;

	transmitter() {}
	transmitter(vector<int> chip) {
		chip_s = chip;
		for (uint i = 0; i < chip.size(); i++)
			ichip_s.push_back((chip[i] == 1) ? -1 : 1);
	}
};

class receiver {
	public:
	vector<int> chip_s;

	receiver() {}
	receiver(vector<int> chip) {
		chip_s = chip;
	}
	
	int decode(vector<int> chip) {
		return dotprod(chip, chip_s) / 8;
	}
};

vector<int> getmessage() {
	vector<int> v;
	char in;
	do {
		in = _getch();
		if (in == '0' || in == '1') {
			v.push_back((int)(in - 48));
			cout << in;
		}
		else if (in == '\b') {
			if (v.size()) {
				cout << "\b \b";
				v.pop_back();
			}
		}
	} while (in != '\r');
	return v;
}

int main() {
	//testorthogonal();

	ofstream f;
	f.open("out.txt");
	for (double i = 0; i < 8; i += .01) { 
		f <<// i << '\t' << chips[0][(int)i] << "\t\t" <<
			//i << '\t' << chips[1][(int)i] << "\t\t" <<
			//i << '\t' << chips[2][(int)i] << "\t\t" <<
			i << '\t' << chips[3][(int)i] << 
			'\n';
	}
	f.close();
	_getch();


	transmitter t0(chips[0]), t1(chips[1]), t2(chips[2]), t3(chips[3]);
	receiver r0(chips[0]), r1(chips[1]), r2(chips[2]), r3(chips[3]);

	while (true) {
		char in;

		cout << "Enter bit sequence for transmitter 0: ";
		t0.m = getmessage();

		cout << "\nEnter bit sequence for transmitter 1: ";
		t1.m = getmessage();

		cout << "\nEnter bit sequence for transmitter 2: ";
		t2.m = getmessage();

		cout << "\nEnter bit sequence for transmitter 3: ";
		t3.m = getmessage();

		cout << "\n\nJoining messages...\n\n";
		
		vector<vector<int>> encodes;
		for (uint i = 0;; i++) {
			vector<int> combined(8);
			int c = 0;
			if (i < t0.m.size()) {
				combined = addchip(combined, t0.m[i] ? t0.chip_s : t0.ichip_s);
				c++;
			}
			if (i < t1.m.size()) {
				combined = addchip(combined, t1.m[i] ? t1.chip_s : t1.ichip_s);
				c++;
			}
			if (i < t2.m.size()) {
				combined = addchip(combined, t2.m[i] ? t2.chip_s : t2.ichip_s);
				c++;
			}
			if (i < t3.m.size()) {
				combined = addchip(combined, t3.m[i] ? t3.chip_s : t3.ichip_s);
				c++;
			}
			if (!c)
				break;

			encodes.push_back(combined);

			cout << "encoded for bit #" << i + 1 << ": " << printchip(combined) << '\n';
		}

		cout << "\nDecoding...\n";

		cout << "\nReceiver 0 decoded: ";
		for (uint i = 0; i < encodes.size(); i++) {
			cout << r0.decode(encodes[i]);
		}

		cout << "\nReceiver 1 decoded: ";
		for (uint i = 0; i < encodes.size(); i++) {
			cout << r1.decode(encodes[i]);
		}

		cout << "\nReceiver 2 decoded: ";
		for (uint i = 0; i < encodes.size(); i++) {
			cout << r2.decode(encodes[i]);
		}

		cout << "\nReceiver 3 decoded: ";
		for (uint i = 0; i < encodes.size(); i++) {
			cout << r3.decode(encodes[i]);
		}

		cout << '\n';

		_getch();
	//	system("cls");
	}


	_getch();
	return 0;
}
