#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <thread>
#include <algorithm>
#include <fstream>
#include <conio.h>
#include <random>
#include <Commctrl.h>
#include <Richedit.h>
#pragma comment(lib, "Comctl32.lib")

#include "common/common.h"
#include "resource.h"
#include "AVLTree.h"

//#define CONSOLE

using namespace std;

HWND g_hwnd;

map<int, AvlTree> words;

BOOL CALLBACK CountdownProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

map<string, int> match(AvlTree& list, string& letters) {
	map<string, int> ret;
	do {
		if (list.search(letters))
			ret[letters] = letters.length();
	} while (next_permutation(letters.begin(), letters.end()));
	return ret;
}
void nCr(string in, string out, int start, int end, int index, int r, vector<string>& res);
void nCr(string in, string out, int start, int end, int index, int r, vector<string>& res) {
	if (index == r) {
		res.push_back(out);		
		return;
	}

	for (int i = start; i <= end && end - i + 1 >= r - index; i++) {
		out[index] = in[i];
		nCr(in, out, i + 1, end, index + 1, r, res);
	}
}
map<string, int> nCr(int n, int r, string in) {
	map<string, int> ret;

	string t;
	for (int i = 0; i < r; i++)
		t += ' ';

	vector<string> out;
	nCr(in, t, 0, n - 1, 0, r, out);

	for (UINT i = 0; i < out.size(); i++) {
		sort(out[i].begin(), out[i].end());
		ret[out[i]] = out[i].size();
	}

	return ret;
}

void AppendTextToEditControl(HWND edit, string text) {
	SetWindowText(edit, str_to_wstr(getwindowtext(edit) + text).c_str());
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#ifdef CONSOLE
	SHOW_CONSOLE(true);	
#endif
	LoadLibrary(L"riched20.dll");

	InitCommonControls();

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)CountdownProc, 0);

	return 0;
}
BOOL CALLBACK CountdownProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			g_hwnd = hwnd;

			ifstream file;
			file.open("words.txt");
			if (file.is_open()) {
				while (file.good()) {
					string line;
					getline(file, line);
					words[line.length()].push(line);
				}
			}

			break;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case IDC_LETTERBTN:{
					auto getwords = [](HWND hwnd) {
						EnableWindow(GetDlgItem(hwnd, IDC_LETTERBTN), FALSE);

						SetWindowText(GetDlgItem(g_hwnd, IDC_LOUT), L"");

						HWND letters = GetDlgItem(hwnd, IDC_LETTERS);
						string text = getwindowtext(letters);
						sort(text.begin(), text.end());

						for (UINT l = text.size(); l > 0; l--) {
							map<string, int> combinations = nCr(text.size(), l, text);
							for (auto const &c : combinations) {
								map<string, int> list = match(words[l], string(c.first));
								for (auto const &i : list) {
									AppendTextToEditControl(GetDlgItem(g_hwnd, IDC_LOUT), (i.first + " (" + static_cast<char>(i.second + 48) + ")\r\n"));
								}
							}
						}

						EnableWindow(GetDlgItem(hwnd, IDC_LETTERBTN), TRUE);
					};

					thread a(getwords, hwnd);
					a.detach();					

					break;
				}
				case IDC_NUMBERBTN:{
					auto calc = [](HWND hwnd) {
						EnableWindow(GetDlgItem(hwnd, IDC_NUMBERBTN), FALSE);

						SetWindowText(GetDlgItem(g_hwnd, IDC_NOUT), L"");

						vector<int> numbers = split_stri(getwindowtext(GetDlgItem(hwnd, IDC_NUMBERS)), " ");

					//	numbers = {100, 75, 50, 25, 6, 3, 952};

					//	2 4 7 8 50 100 719 -> has a solution


						/*
						https://github.com/jes/cntdn/blob/master/cntdn.js			
						8 + 4 = 12
						50 - 12 = 38
						38 / 2 = 19
						100 * 7 = 700
						700 + 19 = 719


						100 * 7 


						*/

						int lowest = 11;

						if (numbers.size() == 0) {
							vector<int> numbs = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 25, 50, 75, 100};

							shuffle(numbs.begin(), numbs.end(), std::default_random_engine(chrono::system_clock::now().time_since_epoch().count()));
							
							copy(numbs.begin(), numbs.begin() + 6, std::back_inserter(numbers));

							random_device rd;
							mt19937 gen(rd());
							numbers.push_back(gen() % 900 + 100);

							sort(numbers.begin(), numbers.end());

							string text = "";
							for (UINT i = 0; i < numbers.size(); i++)
								text += int_to_str(numbers[i]) + " ";

							SetWindowTextW(GetDlgItem(g_hwnd, IDC_NUMBERS), str_to_wstr(text).c_str());
						}
						if (numbers.size() == 7) {
							sort(numbers.begin(), numbers.end());

							int target = numbers[numbers.size() - 1];
							numbers.pop_back();

							vector<string> combinations = {"*****", "****+", "****-", "****/", "***+*", "***++", "***+-", "***+/", "***-*", "***-+", "***--", "***-/",
								"***/*", "***/+", "***/-", "***//", "**+**", "**+*+", "**+*-", "**+*/", "**++*", "**+++", "**++-", "**++/", "**+-*", "**+-+", "**+--",
								"**+-/", "**+/*", "**+/+", "**+/-", "**+//", "**-**", "**-*+", "**-*-", "**-*/", "**-+*", "**-++", "**-+-", "**-+/", "**--*", "**--+",
								"**---", "**--/", "**-/*", "**-/+", "**-/-", "**-//", "**/**", "**/*+", "**/*-", "**/*/", "**/+*", "**/++", "**/+-", "**/+/", "**/-*",
								"**/-+", "**/--", "**/-/", "**//*", "**//+", "**//-", "**///", "*+***", "*+**+", "*+**-", "*+**/", "*+*+*", "*+*++", "*+*+-", "*+*+/",
								"*+*-*", "*+*-+", "*+*--", "*+*-/", "*+*/*", "*+*/+", "*+*/-", "*+*//", "*++**", "*++*+", "*++*-", "*++*/", "*+++*", "*++++", "*+++-",
								"*+++/", "*++-*", "*++-+", "*++--", "*++-/", "*++/*", "*++/+", "*++/-", "*++//", "*+-**", "*+-*+", "*+-*-", "*+-*/", "*+-+*", "*+-++",
								"*+-+-", "*+-+/", "*+--*", "*+--+", "*+---", "*+--/", "*+-/*", "*+-/+", "*+-/-", "*+-//", "*+/**", "*+/*+", "*+/*-", "*+/*/", "*+/+*",
								"*+/++", "*+/+-", "*+/+/", "*+/-*", "*+/-+", "*+/--", "*+/-/", "*+//*", "*+//+", "*+//-", "*+///", "*-***", "*-**+", "*-**-", "*-**/",
								"*-*+*", "*-*++", "*-*+-", "*-*+/", "*-*-*", "*-*-+", "*-*--", "*-*-/", "*-*/*", "*-*/+", "*-*/-", "*-*//", "*-+**", "*-+*+", "*-+*-",
								"*-+*/", "*-++*", "*-+++", "*-++-", "*-++/", "*-+-*", "*-+-+", "*-+--", "*-+-/", "*-+/*", "*-+/+", "*-+/-", "*-+//", "*--**", "*--*+",
								"*--*-", "*--*/", "*--+*", "*--++", "*--+-", "*--+/", "*---*", "*---+", "*----", "*---/", "*--/*", "*--/+", "*--/-", "*--//", "*-/**",
								"*-/*+", "*-/*-", "*-/*/", "*-/+*", "*-/++", "*-/+-", "*-/+/", "*-/-*", "*-/-+", "*-/--", "*-/-/", "*-//*", "*-//+", "*-//-", "*-///",
								"*/***", "*/**+", "*/**-", "*/**/", "*/*+*", "*/*++", "*/*+-", "*/*+/", "*/*-*", "*/*-+", "*/*--", "*/*-/", "*/*/*", "*/*/+", "*/*/-",
								"*/*//", "*/+**", "*/+*+", "*/+*-", "*/+*/", "*/++*", "*/+++", "*/++-", "*/++/", "*/+-*", "*/+-+", "*/+--", "*/+-/", "*/+/*", "*/+/+",
								"*/+/-", "*/+//", "*/-**", "*/-*+", "*/-*-", "*/-*/", "*/-+*", "*/-++", "*/-+-", "*/-+/", "*/--*", "*/--+", "*/---", "*/--/", "*/-/*",
								"*/-/+", "*/-/-", "*/-//", "*//**", "*//*+", "*//*-", "*//*/", "*//+*", "*//++", "*//+-", "*//+/", "*//-*", "*//-+", "*//--", "*//-/",
								"*///*", "*///+", "*///-", "*////", "+****", "+***+", "+***-", "+***/", "+**+*", "+**++", "+**+-", "+**+/", "+**-*", "+**-+", "+**--",
								"+**-/", "+**/*", "+**/+", "+**/-", "+**//", "+*+**", "+*+*+", "+*+*-", "+*+*/", "+*++*", "+*+++", "+*++-", "+*++/", "+*+-*", "+*+-+",
								"+*+--", "+*+-/", "+*+/*", "+*+/+", "+*+/-", "+*+//", "+*-**", "+*-*+", "+*-*-", "+*-*/", "+*-+*", "+*-++", "+*-+-", "+*-+/", "+*--*",
								"+*--+", "+*---", "+*--/", "+*-/*", "+*-/+", "+*-/-", "+*-//", "+*/**", "+*/*+", "+*/*-", "+*/*/", "+*/+*", "+*/++", "+*/+-", "+*/+/",
								"+*/-*", "+*/-+", "+*/--", "+*/-/", "+*//*", "+*//+", "+*//-", "+*///", "++***", "++**+", "++**-", "++**/", "++*+*", "++*++", "++*+-",
								"++*+/", "++*-*", "++*-+", "++*--", "++*-/", "++*/*", "++*/+", "++*/-", "++*//", "+++**", "+++*+", "+++*-", "+++*/", "++++*", "+++++",
								"++++-", "++++/", "+++-*", "+++-+", "+++--", "+++-/", "+++/*", "+++/+", "+++/-", "+++//", "++-**", "++-*+", "++-*-", "++-*/", "++-+*",
								"++-++", "++-+-", "++-+/", "++--*", "++--+", "++---", "++--/", "++-/*", "++-/+", "++-/-", "++-//", "++/**", "++/*+", "++/*-", "++/*/",
								"++/+*", "++/++", "++/+-", "++/+/", "++/-*", "++/-+", "++/--", "++/-/", "++//*", "++//+", "++//-", "++///", "+-***", "+-**+", "+-**-",
								"+-**/", "+-*+*", "+-*++", "+-*+-", "+-*+/", "+-*-*", "+-*-+", "+-*--", "+-*-/", "+-*/*", "+-*/+", "+-*/-", "+-*//", "+-+**", "+-+*+",
								"+-+*-", "+-+*/", "+-++*", "+-+++", "+-++-", "+-++/", "+-+-*", "+-+-+", "+-+--", "+-+-/", "+-+/*", "+-+/+", "+-+/-", "+-+//", "+--**",
								"+--*+", "+--*-", "+--*/", "+--+*", "+--++", "+--+-", "+--+/", "+---*", "+---+", "+----", "+---/", "+--/*", "+--/+", "+--/-", "+--//",
								"+-/**", "+-/*+", "+-/*-", "+-/*/", "+-/+*", "+-/++", "+-/+-", "+-/+/", "+-/-*", "+-/-+", "+-/--", "+-/-/", "+-//*", "+-//+", "+-//-",
								"+-///", "+/***", "+/**+", "+/**-", "+/**/", "+/*+*", "+/*++", "+/*+-", "+/*+/", "+/*-*", "+/*-+", "+/*--", "+/*-/", "+/*/*", "+/*/+",
								"+/*/-", "+/*//", "+/+**", "+/+*+", "+/+*-", "+/+*/", "+/++*", "+/+++", "+/++-", "+/++/", "+/+-*", "+/+-+", "+/+--", "+/+-/", "+/+/*",
								"+/+/+", "+/+/-", "+/+//", "+/-**", "+/-*+", "+/-*-", "+/-*/", "+/-+*", "+/-++", "+/-+-", "+/-+/", "+/--*", "+/--+", "+/---", "+/--/",
								"+/-/*", "+/-/+", "+/-/-", "+/-//", "+//**", "+//*+", "+//*-", "+//*/", "+//+*", "+//++", "+//+-", "+//+/", "+//-*", "+//-+", "+//--",
								"+//-/", "+///*", "+///+", "+///-", "+////", "-****", "-***+", "-***-", "-***/", "-**+*", "-**++", "-**+-", "-**+/", "-**-*", "-**-+",
								"-**--", "-**-/", "-**/*", "-**/+", "-**/-", "-**//", "-*+**", "-*+*+", "-*+*-", "-*+*/", "-*++*", "-*+++", "-*++-", "-*++/", "-*+-*",
								"-*+-+", "-*+--", "-*+-/", "-*+/*", "-*+/+", "-*+/-", "-*+//", "-*-**", "-*-*+", "-*-*-", "-*-*/", "-*-+*", "-*-++", "-*-+-", "-*-+/",
								"-*--*", "-*--+", "-*---", "-*--/", "-*-/*", "-*-/+", "-*-/-", "-*-//", "-*/**", "-*/*+", "-*/*-", "-*/*/", "-*/+*", "-*/++", "-*/+-",
								"-*/+/", "-*/-*", "-*/-+", "-*/--", "-*/-/", "-*//*", "-*//+", "-*//-", "-*///", "-+***", "-+**+", "-+**-", "-+**/", "-+*+*", "-+*++",
								"-+*+-", "-+*+/", "-+*-*", "-+*-+", "-+*--", "-+*-/", "-+*/*", "-+*/+", "-+*/-", "-+*//", "-++**", "-++*+", "-++*-", "-++*/", "-+++*",
								"-++++", "-+++-", "-+++/", "-++-*", "-++-+", "-++--", "-++-/", "-++/*", "-++/+", "-++/-", "-++//", "-+-**", "-+-*+", "-+-*-", "-+-*/",
								"-+-+*", "-+-++", "-+-+-", "-+-+/", "-+--*", "-+--+", "-+---", "-+--/", "-+-/*", "-+-/+", "-+-/-", "-+-//", "-+/**", "-+/*+", "-+/*-",
								"-+/*/", "-+/+*", "-+/++", "-+/+-", "-+/+/", "-+/-*", "-+/-+", "-+/--", "-+/-/", "-+//*", "-+//+", "-+//-", "-+///", "--***", "--**+",
								"--**-", "--**/", "--*+*", "--*++", "--*+-", "--*+/", "--*-*", "--*-+", "--*--", "--*-/", "--*/*", "--*/+", "--*/-", "--*//", "--+**",
								"--+*+", "--+*-", "--+*/", "--++*", "--+++", "--++-", "--++/", "--+-*", "--+-+", "--+--", "--+-/", "--+/*", "--+/+", "--+/-", "--+//",
								"---**", "---*+", "---*-", "---*/", "---+*", "---++", "---+-", "---+/", "----*", "----+", "-----", "----/", "---/*", "---/+", "---/-",
								"---//", "--/**", "--/*+", "--/*-", "--/*/", "--/+*", "--/++", "--/+-", "--/+/", "--/-*", "--/-+", "--/--", "--/-/", "--//*", "--//+",
								"--//-", "--///", "-/***", "-/**+", "-/**-", "-/**/", "-/*+*", "-/*++", "-/*+-", "-/*+/", "-/*-*", "-/*-+", "-/*--", "-/*-/", "-/*/*",
								"-/*/+", "-/*/-", "-/*//", "-/+**", "-/+*+", "-/+*-", "-/+*/", "-/++*", "-/+++", "-/++-", "-/++/", "-/+-*", "-/+-+", "-/+--", "-/+-/",
								"-/+/*", "-/+/+", "-/+/-", "-/+//", "-/-**", "-/-*+", "-/-*-", "-/-*/", "-/-+*", "-/-++", "-/-+-", "-/-+/", "-/--*", "-/--+", "-/---",
								"-/--/", "-/-/*", "-/-/+", "-/-/-", "-/-//", "-//**", "-//*+", "-//*-", "-//*/", "-//+*", "-//++", "-//+-", "-//+/", "-//-*", "-//-+",
								"-//--", "-//-/", "-///*", "-///+", "-///-", "-////", "/****", "/***+", "/***-", "/***/", "/**+*", "/**++", "/**+-", "/**+/", "/**-*",
								"/**-+", "/**--", "/**-/", "/**/*", "/**/+", "/**/-", "/**//", "/*+**", "/*+*+", "/*+*-", "/*+*/", "/*++*", "/*+++", "/*++-", "/*++/",
								"/*+-*", "/*+-+", "/*+--", "/*+-/", "/*+/*", "/*+/+", "/*+/-", "/*+//", "/*-**", "/*-*+", "/*-*-", "/*-*/", "/*-+*", "/*-++", "/*-+-",
								"/*-+/", "/*--*", "/*--+", "/*---", "/*--/", "/*-/*", "/*-/+", "/*-/-", "/*-//", "/*/**", "/*/*+", "/*/*-", "/*/*/", "/*/+*", "/*/++",
								"/*/+-", "/*/+/", "/*/-*", "/*/-+", "/*/--", "/*/-/", "/*//*", "/*//+", "/*//-", "/*///", "/+***", "/+**+", "/+**-", "/+**/", "/+*+*",
								"/+*++", "/+*+-", "/+*+/", "/+*-*", "/+*-+", "/+*--", "/+*-/", "/+*/*", "/+*/+", "/+*/-", "/+*//", "/++**", "/++*+", "/++*-", "/++*/",
								"/+++*", "/++++", "/+++-", "/+++/", "/++-*", "/++-+", "/++--", "/++-/", "/++/*", "/++/+", "/++/-", "/++//", "/+-**", "/+-*+", "/+-*-",
								"/+-*/", "/+-+*", "/+-++", "/+-+-", "/+-+/", "/+--*", "/+--+", "/+---", "/+--/", "/+-/*", "/+-/+", "/+-/-", "/+-//", "/+/**", "/+/*+",
								"/+/*-", "/+/*/", "/+/+*", "/+/++", "/+/+-", "/+/+/", "/+/-*", "/+/-+", "/+/--", "/+/-/", "/+//*", "/+//+", "/+//-", "/+///", "/-***",
								"/-**+", "/-**-", "/-**/", "/-*+*", "/-*++", "/-*+-", "/-*+/", "/-*-*", "/-*-+", "/-*--", "/-*-/", "/-*/*", "/-*/+", "/-*/-", "/-*//",
								"/-+**", "/-+*+", "/-+*-", "/-+*/", "/-++*", "/-+++", "/-++-", "/-++/", "/-+-*", "/-+-+", "/-+--", "/-+-/", "/-+/*", "/-+/+", "/-+/-",
								"/-+//", "/--**", "/--*+", "/--*-", "/--*/", "/--+*", "/--++", "/--+-", "/--+/", "/---*", "/---+", "/----", "/---/", "/--/*", "/--/+",
								"/--/-", "/--//", "/-/**", "/-/*+", "/-/*-", "/-/*/", "/-/+*", "/-/++", "/-/+-", "/-/+/", "/-/-*", "/-/-+", "/-/--", "/-/-/", "/-//*",
								"/-//+", "/-//-", "/-///", "//***", "//**+", "//**-", "//**/", "//*+*", "//*++", "//*+-", "//*+/", "//*-*", "//*-+", "//*--", "//*-/",
								"//*/*", "//*/+", "//*/-", "//*//", "//+**", "//+*+", "//+*-", "//+*/", "//++*", "//+++", "//++-", "//++/", "//+-*", "//+-+", "//+--",
								"//+-/", "//+/*", "//+/+", "//+/-", "//+//", "//-**", "//-*+", "//-*-", "//-*/", "//-+*", "//-++", "//-+-", "//-+/", "//--*", "//--+",
								"//---", "//--/", "//-/*", "//-/+", "//-/-", "//-//", "///**", "///*+", "///*-", "///*/", "///+*", "///++", "///+-", "///+/", "///-*",
								"///-+", "///--", "///-/", "////*", "////+", "////-", "/////"};

							map<string, int> old;

							do {
								for (UINT i = 0; i < combinations.size(); i++) {
									string ret = int_to_str(numbers[0]);
									int res = numbers[0];
									for (UINT j = 0; j < combinations[i].size(); j++) {
										if (combinations[i][j] == '+')
											res += numbers[j + 1];
										else if (combinations[i][j] == '-' && res - numbers[j + 1] > 0)
											res -= numbers[j + 1];
										else if (combinations[i][j] == '*' && numbers[j + 1] != 1 && res != 1)
											res *= numbers[j + 1];
										else if (combinations[i][j] == '/' && numbers[j + 1] != 1 && res % numbers[j + 1] == 0)
											res /= numbers[j + 1];
										else
											continue;

										ret.insert(0, "(");
										ret += " ";
										ret += combinations[i][j];
										ret += " ";
										ret += int_to_str(numbers[j + 1]);
										ret += ")";
										int val = abs(res - target);
										if (val <= 10 && (val < lowest || res == target)) {
											lowest = val;
											ret += " = " + int_to_str(res);
											if (old[ret] == 0) {
												old[ret]++;

												if (res == target) {
													SetWindowText(GetDlgItem(g_hwnd, IDC_NOUT), str_to_wstr(ret + "\r\n" + getwindowtext(GetDlgItem(g_hwnd, IDC_NOUT))).c_str());
													//goto end;
												}
												else
													SetWindowText(GetDlgItem(g_hwnd, IDC_NOUT), str_to_wstr(ret + "\r\n" + getwindowtext(GetDlgItem(g_hwnd, IDC_NOUT))).c_str());
											}

											break;
										}
									}
								}
							} while (next_permutation(numbers.begin(), numbers.end()));
						}
						end:

						EnableWindow(GetDlgItem(hwnd, IDC_NUMBERBTN), TRUE);
					};

					thread a(calc, hwnd);
					a.detach();

					break;
				}
			}
			switch (HIWORD(wParam)) {
				case 0:{
					if (GetAsyncKeyState(VK_RETURN) < 0) {
						HWND focus = GetFocus();

						BOOL disabled;

						if (focus == GetDlgItem(hwnd, IDC_LETTERS)) {
							disabled = EnableWindow(GetDlgItem(hwnd, IDC_LETTERBTN), FALSE);
							if (disabled == FALSE) EnableWindow(GetDlgItem(hwnd, IDC_LETTERBTN), disabled);

							if (!disabled)
								SendMessage(hwnd, WM_COMMAND, IDC_LETTERBTN, NULL);
						}
						else if (focus == GetDlgItem(hwnd, IDC_NUMBERS)) {
							disabled = EnableWindow(GetDlgItem(hwnd, IDC_NUMBERBTN), FALSE);
							if (disabled == FALSE) EnableWindow(GetDlgItem(hwnd, IDC_NUMBERBTN), disabled);

							if (!disabled) 								
								SendMessage(hwnd, WM_COMMAND, IDC_NUMBERBTN, NULL);
						}

						break;
					}

					break;
				}
				case EN_UPDATE:{
					switch (LOWORD(wParam)) {
						case IDC_LETTERS:{
							if (GetAsyncKeyState(VK_CONTROL) < 0 || GetAsyncKeyState(VK_LBUTTON) < 0)
								break;

							HWND letters = GetDlgItem(hwnd, IDC_LETTERS);
							string text = getwindowtext(letters);

							if (text.size() > 9) {
								text = text.substr(0, 9);
								SetWindowText(letters, str_to_wstr(text).c_str());
							}

							string temp = "";
							bool diff = false;
							for (UINT i = 0; i < text.size(); i++) {
								if (text[i] >= 'a' && text[i] <= 'z') {
									temp += text[i];
									continue;
								}
								if (text[i] >= 'A' && text[i] <= 'Z')
									temp += text[i] + 32;
								diff = true;
									
							}
							if (diff) {
								text = temp;
								SetWindowText(letters, str_to_wstr(text).c_str());								
							}

							if (text.find(' ') != string::npos) {
								text.erase(remove(text.begin(), text.end(), ' '), text.end());
								SetWindowText(letters, str_to_wstr(text).c_str());
							}
							
							SendMessage(letters, EM_SETSEL, text.size(), text.size());
							
							break;
						}
						case IDC_NUMBERS:{
							if (GetAsyncKeyState(VK_CONTROL) < 0 || GetAsyncKeyState(VK_LBUTTON) < 0)
								break;

							HWND numbers = GetDlgItem(hwnd, IDC_NUMBERS);
							string text = getwindowtext(numbers);

							string t = "";
							for (UINT i = 0; i < text.size(); i++) {
								if (text[i] >= 48 && text[i] < 58 || text[i] == 32)
									t += text[i];
							}

							SetWindowText(numbers, STW(t));

							SendMessage(numbers, EM_SETSEL, text.size(), text.size());

							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
			break;
		}
	}
	return FALSE;
}