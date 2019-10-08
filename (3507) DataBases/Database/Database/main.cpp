#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <conio.h>
#include <time.h>
#include <sstream>

#include <boost/network/protocol/http/client.hpp>

#include "resource.h"
#include "C:/CPPLibs/common/common.h"

using namespace std;

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

struct flight {
	string date;
	string time;
	string depairline;
	string depflightno;
	string retairline;
	string retflightno;
	string fare;
	string comment;
};

void print(ostream& out, flight& f, string delimeter) {
	out << f.date << delimeter;
	out << f.time << delimeter;
	out << f.depairline << delimeter;
	out << f.depflightno << delimeter;
	out << f.retairline << delimeter;
	out << f.retflightno << delimeter;
	out << f.fare << delimeter;
	out << f.comment;
}

void addentries(string entries) {
	if (entries.size() >= 2 && entries[entries.size() - 1] == '\n' && entries[entries.size() - 2] == '\r')
		entries = entries.substr(0, entries.size() - 2);
	ofstream file;
	file.open("data.txt", ios::app);
	file << entries;
	file.close();
}
void writeout() {
	ifstream file;
	file.open("data.txt");
	if (!file.is_open())
		exit(1);

	vector<flight> flights;

	while (file.good()) {
		string line;
		getline(file, line);
		flight f;
		vector<string> e = split_str(line, "\t", -1);
		if (e.size() == 8) {
			f.date = e[0];
			f.time = e[1];
			f.depairline = e[2];
			f.depflightno = e[3];
			f.retairline = e[4];			
			f.retflightno = e[5];
			f.fare = e[6];
			f.comment = e[7];

			flights.push_back(f);
		}
		else if (line.size() != 0)
			cout << "failed: " <<  line << "\n";
	}
	file.close();

	ofstream f;
	f.open("tab.txt");
	for (auto a : flights) {
		print(f, a, "\t");
		f << '\n';
	}
	f.close();
	f.clear();

	f.open("fares.CSV");
	for (auto a : flights) {
		vector<string> mdy = split_str(a.date, "/");
		a.date = mdy[2] + "-" + mdy[0] + "-" + mdy[1];
		print(f, a, ",");
		f << '\n';
	}
	f.close();
}

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
	SHOW_CONSOLE(true);


	//boost::network::http::client client;
	//boost::network::http::client::request request("https://www.google.com/flights/#search;f=MSP;t=DFW,DAL;d=2015-11-17;r=2015-11-22;sel=MSPDFW0DL5821,DFWMSP0DL3682");
	//request << boost::network::header("Connection", "close");
	//boost::network::http::client::response response = client.get(request);

	//std::cout << body(response);

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc, 0);

	return 0;
}
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	switch (message) {
		case WM_INITDIALOG:{
			
			break;
		}
		case WM_COMMAND:{
			switch (wParam) {
				case IDC_BUTTON1:{
					string dfwfare = getwindowtext(GetDlgItem(hwnd, IDC_EDIT1));
					string seafare = getwindowtext(GetDlgItem(hwnd, IDC_EDIT2));
					string miafare = getwindowtext(GetDlgItem(hwnd, IDC_EDIT3));

					time_t t = time(0);
					struct tm* now = localtime(&t);
					string date = //"\'" +
						int_to_str(now->tm_mon + 1) + "/" +
						int_to_str(now->tm_mday) + "/" +
						int_to_str(now->tm_year + 1900);
					string time = //"\'" +
						int_to_str(now->tm_hour) + ":" +
						int_to_str(now->tm_min) + ":" +
						int_to_str(now->tm_sec);					
					
					string delimeter = (Button_GetState(GetDlgItem(hwnd, IDC_CHECK1)) == BST_CHECKED) ? "," : "\t";
					string out = "";
					
					if (dfwfare.size() > 0) {
						flight dfw;
						dfw.date = date;
						dfw.time = time;
						dfw.depairline = "DL";
						dfw.depflightno = "5821";
						dfw.retairline = "DL";
						dfw.retflightno = "3682";
						dfw.fare = dfwfare;
						dfw.comment = "MSP-DFW";

						stringstream s;
						print(s, dfw, delimeter);
						out += s.str() + "\r\n";
					}
					if (seafare.size() > 0) {
						flight sea;
						sea.date = date;
						sea.time = time;
						sea.depairline = "AS";
						sea.depflightno = "39";
						sea.retairline = "AS";
						sea.retflightno = "28";
						sea.fare = seafare;
						sea.comment = "MSP-SEA";

						stringstream s;
						print(s, sea, delimeter);
						out += s.str() + "\r\n";
					}
					if (miafare.size() > 0) {
						flight mia;
						mia.date = date;
						mia.time = time;
						mia.depairline = "AA";
						mia.depflightno = "309";
						mia.retairline = "AA";
						mia.retflightno = "1621";
						mia.fare = miafare;
						mia.comment = "MSP-MIA";

						stringstream s;
						print(s, mia, delimeter);
						out += s.str() + "\r\n";
					}

					SetWindowText(GetDlgItem(hwnd, IDC_EDIT4), str_to_wstr(out).c_str());

					SetWindowText(GetDlgItem(hwnd, IDC_EDIT1), L"");
					SetWindowText(GetDlgItem(hwnd, IDC_EDIT2), L"");
					SetWindowText(GetDlgItem(hwnd, IDC_EDIT3), L"");

					addentries("\r\n" + out);
					writeout();

					break;
				}
			}
			switch (HIWORD(wParam)) {
				case EN_UPDATE:{
					switch (LOWORD(wParam)) {
						case IDC_EDIT1:
						case IDC_EDIT2:
						case IDC_EDIT3:{
							HWND flight = GetDlgItem(hwnd, LOWORD(wParam));
							string fare = getwindowtext(flight);

							string temp = "";
							bool diff = false;
							for (UINT i = 0; i < fare.size(); i++) {
								if (fare[i] >= '0' && fare[i] <= '9') {
									temp += fare[i];
									continue;
								}
								diff = true;

							}
							if (diff) {
								fare = temp;
								SetWindowText(flight, str_to_wstr(fare).c_str());
							}

							if (fare.find(' ') != string::npos) {
								fare.erase(remove(fare.begin(), fare.end(), ' '), fare.end());
								SetWindowText(flight, str_to_wstr(fare).c_str());
							}

							SendMessage(flight, EM_SETSEL, fare.size(), fare.size());

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