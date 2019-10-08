/*

Main.cpp

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

Team Midgard  (Josh Urmann, Simon Cecere, Ben Mason)

This Program implements a basic flight reservation system that is controlled by a text based user interface

Menu Options:

1. Add a new plane to the g_fleet.
2. Add a new flight to the schedule.
3. Add a new passenger to the list of passengers.
4. Add a new passenger to a flight.
5. Change the plane assigned to a flight.
6. Print the upcoming flights.
7. Print a list of passengers on a given flight.
8. Save the current collection of passengers, planes and schedules to the appropriate files.
*/

#include <Windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <time.h>
#include <sstream>

#include "resource.h"
#include "common.h"
#include "seats.h"
#include "passenger.h"
#include "plane.h"
#include "flight.h"

//this makes window controls use the windows 7 style instead of windows xp
#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;

//global window handle
HWND g_hwnd;

//prototypes for dialog proceduers
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AddPlaneProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AddFlightProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AddPassengerProc(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK DisplayPassengersProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DisplayFlightsProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DisplayPassengerProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DisplayFlightProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DisplayPlanesProc(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK TempWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TempWndProc2(HWND, UINT, WPARAM, LPARAM);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////load database info/////////////////////////////////////////////

// parse the data from the passengers.txt file
void loadPassengers(const string& filename) {
	ifstream file;
	file.open(filename);
	if (file.is_open()) {
		while (file.good()) {
			string line;
			getline(file, line);
			vector<string> p = split_str(line, "|", -1);

			if (p.size() < 2)
				continue;

			Passenger* passenger = new Passenger;

			passenger->set_id(str_to_int(p[0]));
			passenger->set_name(p[1]);
			for (UINT i = 2; i < p.size(); i++) {
				int flightNumber = str_to_int(p[i]);

				for (UINT j = 0; j < g_flights.size(); j++) {
					if (flightNumber == g_flights[j]->get_flightNumber()) {
						g_flights[j]->add_passenger(passenger);
						passenger->add_flight(g_flights[j]);
					}
				}
			}

			g_passengers.push_back(passenger);
		}
	}
	else
		cout << "unable to open " << filename << "\n";
	file.close();
}

//parse the data from the flights.txt file
void loadFlights(const string& filename) {
	ifstream file;
	file.open(filename);
	if (file.is_open()) {
		while (file.good()) {
			string line;
			getline(file, line);
			vector<string> p = split_str(line, "|", -1);

			///If plane has wrong input from File
			if (p.size() != 7)
				continue;

			Flight* flight = new Flight;

			flight->set_flightNumber(str_to_int(p[0]));
			flight->set_destination(p[1]);
			flight->set_departureLocation(p[2]);
			flight->set_depTime(string_time_to_struct_time(p[3]));
			flight->set_arrivalTime(string_time_to_struct_time(p[4]));
			int pid = str_to_int(p[5]);
			for (UINT i = 0; i < g_fleet.size(); i++) {
				if (g_fleet[i]->get_id() == pid) {
					flight->set_plane(g_fleet[i]);
				}
			}
			flight->set_miles(str_to_int(p[6]));


			//TODO load passengers

			g_flights.push_back(flight);
		}
	}
	else
		cout << "unable to open " << filename << "\n";
	file.close();
}

//parse the data from the planes.txt file
void loadPlanes(const string& filename) {
	ifstream file;
	file.open(filename);
	if (file.is_open()) {
		while (file.good()) {
			string line;
			getline(file, line);

			vector<string> p = split_str(line, "|", -1);

			if (p.size() != 7)
				continue;

			Plane* plane = new Plane;

			plane->set_id(str_to_int(p[0]));

			plane->set_firstClass_rows(str_to_int(p[2]));
			plane->set_firstClass_cols(str_to_int(p[1]));

			plane->set_economyPlus_rows(str_to_int(p[4]));
			plane->set_economyPlus_cols(str_to_int(p[3]));

			plane->set_economy_rows(str_to_int(p[6]));
			plane->set_economy_cols(str_to_int(p[5]));

			g_fleet.push_back(plane);
		}
	}
	else
		cout << "unable to open " << filename << "\n";
	file.close();
}

// Loads in all the flight data into the flight reservation system from the Text Files
//Planes.txt:     File Format |ID |First class Colums |First Class Rows |Economy Plus Colums |Economy plus Class Rows  |Economy Class Colums |Economy Class Rows |
//flights.txt:    File Format |Flight Number| Destination | Departure Location | Departure Date | Arrivial Date | Plane ID| Miles Scheduled |
//Passengers.txt: File Format | Passenger ID | Passenger Name | Flight Number | Flight Number
//A Passenger can have more than 1 flight number, EG. a passenger books a flight in January and another one in July
void load_airline_data() {
	loadPlanes("planes.txt");
	loadFlights("flights.txt");
	loadPassengers("passengers.txt");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////listview functions///////////////////////////////////////////////

//sorting function for listview contorl columns for passengers (all columns)
int CALLBACK ListViewPassengerCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	static LPARAM lastsort;
	if (lParamSort == -1)
		lParamSort = lastsort;
	lastsort = lParamSort;
	int ret = 0;
	Passenger* passenger1 = (Passenger*)lParam1;
	Passenger* passenger2 = (Passenger*)lParam2;

	if (!passenger1 || !passenger2)
		return 0;
	std::string s1, s2;

	switch (lParamSort) {
		case -2:
		case 0:{
			ret = (passenger1->get_name() < passenger2->get_name() ? -1 : 1);
			break;
		}
	}

	return lParamSort < -1 ? ret * -1 : ret;
}
//sorting function for listview control columns for flights (all columns)
int CALLBACK ListViewFlightCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	static LPARAM lastsort;
	if (lParamSort == -1)
		lParamSort = lastsort;
	lastsort = lParamSort;
	int ret = 0;
	Flight* flight1 = (Flight*)lParam1;
	Flight* flight2 = (Flight*)lParam2;
	if (!flight1 || !flight2)
		return 0;
	std::string s1, s2;

	switch (lParamSort) {
		case -8:
		case 0:{
			ret = flight1->get_flightNumber() < flight2->get_flightNumber() ? -1 : 1;
			break;
		}
		case -7:
		case 1:{
			ret = flight1->get_destination().compare(flight2->get_destination());
			break;
		}
		case -6:
		case 2:{
			ret = flight1->get_departureLocation().compare(flight2->get_departureLocation());
			break;
		}
		case -5:
		case 3:{
			ret = compare_time(&flight1->get_depTime(), &flight2->get_depTime()) ? 1 : -1;
			break;
		}
		case -4:
		case 4:{
			ret = compare_time(&flight1->get_arrivalTime(), &flight2->get_arrivalTime()) ? 1 : -1;
			break;
		}
		case -3:
		case 5:{
			ret = flight1->get_miles() > flight2->get_miles() ? -1 : 1;
			break;
		}
		case -2:
		case 6:{
			ret = flight1->get_plane() && flight2->get_plane() ? flight1->get_plane()->get_id() > flight2->get_plane()->get_id() ? 1 : -1 : -1;
			break;
		}
	}

	return lParamSort < -1 ? ret * -1 : ret;
}
//sorting function for listive view contorl columns for planes (id and flight no.)
int CALLBACK ListViewPlaneCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	static LPARAM lastsort;
	if (lParamSort == -1)
		lParamSort = lastsort;
	lastsort = lParamSort;
	int ret = 0;
	Plane* plane1 = (Plane*)lParam1;
	Plane* plane2 = (Plane*)lParam2;
	if (!plane1 || !plane2)
		return 0;
	std::string s1, s2;

	switch (lParamSort) {
		case -7:
		case 0:{
			ret = plane1->get_id() > plane2->get_id() ? 1 : -1;
			break;
		}
		case -6:
		case 1:{
			int plane1flightno = -1;
			int plane2flightno = -1;

			for (UINT j = 0; j < g_flights.size(); j++) {
				if (g_flights[j]->get_plane() && g_flights[j]->get_plane()->get_id() == plane1->get_id()) 
					plane1flightno = g_flights[j]->get_flightNumber();
				if (g_flights[j]->get_plane() && g_flights[j]->get_plane()->get_id() == plane2->get_id())
					plane2flightno = g_flights[j]->get_flightNumber();				
			}		
			ret = plane1flightno > plane2flightno ? 1 : -1;
			break;
		}
	}
	return lParamSort < -1 ? ret * -1 : ret;
}

//get the index of the currently selected item of a listview
int ListView_GetCurSel(HWND hList) {
	return ListView_GetNextItem(hList, -1, LVNI_SELECTED);
}
//get the lParam of specific listview item index
LPARAM ListView_GetItemParam(HWND hList, int item) {
	LVITEM lvi = {0};
	lvi.iItem = item;
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	ListView_GetItem(hList, &lvi);
	return lvi.lParam;
}

void PopulatePassengersListView(HWND listview) {
	ListView_DeleteAllItems(listview);

	//loop through each passenger adding their name to the list
	for (UINT i = 0; i < g_passengers.size(); i++) {
		LVITEM lvi = {0};
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.cchTextMax = 240;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.lParam = (LPARAM)g_passengers[i];
		lvi.pszText = str_to_LPWSTR(g_passengers[i]->get_name());

		SendMessage(listview, LVM_INSERTITEM, 0, (LPARAM)&lvi);
		delete[] lvi.pszText;
	}
}
void PopulateFlightsListView(HWND listview) {
	ListView_DeleteAllItems(listview);

	//get current time
	time_t t = time(0);
	tm* now = localtime(&t);
	now->tm_mon++;
	now->tm_year += 1900;

	//loop throught flights adding them to the list, each attribute gettings its own column
	for (UINT i = 0; i < g_flights.size(); i++) {
		if (compare_time(now, &g_flights[i]->get_depTime())) {
			LVITEM lvi = {0};
			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			lvi.cchTextMax = 240;
			lvi.iItem = 0;
			lvi.iSubItem = 0;
			lvi.lParam = (LPARAM)g_flights[i];
			lvi.pszText = str_to_LPWSTR(int_to_str(g_flights[i]->get_flightNumber()));

			int pos = SendMessage(listview, LVM_INSERTITEM, 0, (LPARAM)&lvi);
			delete[] lvi.pszText;

			lvi.pszText = str_to_LPWSTR(g_flights[i]->get_destination());
			ListView_SetItemText(listview, pos, 1, lvi.pszText);
			delete[] lvi.pszText;

			lvi.pszText = str_to_LPWSTR(g_flights[i]->get_departureLocation());
			ListView_SetItemText(listview, pos, 2, lvi.pszText);
			delete[] lvi.pszText;

			lvi.pszText = str_to_LPWSTR(struct_time_to_string_time(g_flights[i]->get_depTime()));
			ListView_SetItemText(listview, pos, 3, lvi.pszText);
			delete[] lvi.pszText;

			lvi.pszText = str_to_LPWSTR(struct_time_to_string_time(g_flights[i]->get_arrivalTime()));
			ListView_SetItemText(listview, pos, 4, lvi.pszText);
			delete[] lvi.pszText;

			lvi.pszText = str_to_LPWSTR(int_to_str(g_flights[i]->get_miles()));
			ListView_SetItemText(listview, pos, 5, lvi.pszText);
			delete[] lvi.pszText;

			//check if flight has a plane, show its id if it does, NULL if it doesn't
			if (g_flights[i]->get_plane()) {
				lvi.pszText = str_to_LPWSTR(int_to_str(g_flights[i]->get_plane()->get_id()));
				ListView_SetItemText(listview, pos, 6, lvi.pszText);
				delete[] lvi.pszText;
			}
			else {
				lvi.pszText = L"NULL";
				ListView_SetItemText(listview, pos, 6, lvi.pszText);
			}
		}
	}
}
void PopulatePlanesListView(HWND listview) {
	ListView_DeleteAllItems(listview);

	for (UINT i = 0; i < g_fleet.size(); i++) {
		LVITEM lvi = {0};
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.cchTextMax = 240;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.lParam = (LPARAM)g_fleet[i];
		lvi.pszText = str_to_LPWSTR(int_to_str(g_fleet[i]->get_id()));

		int pos = SendMessage(listview, LVM_INSERTITEM, 0, (LPARAM)&lvi);
		delete[] lvi.pszText;

		//check if plane has flights that are using it, display all of them, or null if it has none
		vector<int> flight_ids;
		for (UINT j = 0; j < g_flights.size(); j++) {
			if (g_flights[j]->get_plane() && g_flights[j]->get_plane()->get_id() == g_fleet[i]->get_id()) {
				flight_ids.push_back(g_flights[j]->get_flightNumber());
			}
		}
		if (!flight_ids.size()) {
			lvi.pszText = L"NULL";
			ListView_SetItemText(listview, pos, 1, lvi.pszText);
		}
		else {
			//append all flight ids, except the last, to a string separated by commas
			string line = "";
			for (UINT j = 0; j < flight_ids.size() - 1; j++) {
				line += (int_to_str(flight_ids[j]) + ",");
			}
			//add last flight id to string (to prevent trailing comma)
			line += int_to_str(flight_ids[flight_ids.size() - 1]);

			lvi.pszText = str_to_LPWSTR(line);
			ListView_SetItemText(listview, pos, 1, lvi.pszText);
			delete[] lvi.pszText;
		}

		lvi.pszText = str_to_LPWSTR("{" + int_to_str(g_fleet[i]->get_firstClass_rows()) + "," + int_to_str(g_fleet[i]->get_firstClass_cols()) + "}");
		ListView_SetItemText(listview, pos, 2, lvi.pszText);
		delete[] lvi.pszText;

		lvi.pszText = str_to_LPWSTR("{" + int_to_str(g_fleet[i]->get_economyPlus_rows()) + "," + int_to_str(g_fleet[i]->get_economyPlus_cols()) + "}");
		ListView_SetItemText(listview, pos, 3, lvi.pszText);
		delete[] lvi.pszText;

		lvi.pszText = str_to_LPWSTR("{" + int_to_str(g_fleet[i]->get_economy_rows()) + "," + int_to_str(g_fleet[i]->get_economy_cols()) + "}");
		ListView_SetItemText(listview, pos, 4, lvi.pszText);
		delete[] lvi.pszText;
	}
}

void PopulateFlightListView(Flight* flight, HWND hwnd) {
	HWND listview = GetDlgItem(hwnd, IDC_LIST_FLIGHT);

	//clear the list first
	ListView_DeleteAllItems(listview);

	//loop through flights passengers adding them to the list
	for (UINT i = 0; i < flight->get_passengers().size(); i++) {
		LVITEM lvi = {0};
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.cchTextMax = 240;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.lParam = (LPARAM)flight->get_passenger(i);
		lvi.pszText = str_to_LPWSTR(flight->get_passenger(i)->get_name());

		SendMessage(listview, LVM_INSERTITEM, 0, (LPARAM)&lvi);
		delete[] lvi.pszText;
	}
}
void PopulatePassengerListView(Passenger* passenger, HWND hwnd) {
	HWND listview = GetDlgItem(hwnd, IDC_LIST_PASSENGER);

	//clear the list first
	ListView_DeleteAllItems(listview);

	//loop through passengers flights and add them to the list (each attribute gets its own column)
	for (UINT i = 0; i < passenger->get_flights().size(); i++) {
		const Flight* flight = passenger->get_flight(i);

		LVITEM lvi = {0};
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.cchTextMax = 240;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.lParam = (LPARAM)flight;
		lvi.pszText = str_to_LPWSTR(int_to_str(flight->get_flightNumber()));

		int pos = SendMessage(listview, LVM_INSERTITEM, 0, (LPARAM)&lvi);
		delete[] lvi.pszText;

		lvi.pszText = str_to_LPWSTR(flight->get_destination());
		ListView_SetItemText(listview, pos, 1, lvi.pszText);
		delete[] lvi.pszText;

		lvi.pszText = str_to_LPWSTR(flight->get_departureLocation());
		ListView_SetItemText(listview, pos, 2, lvi.pszText);
		delete[] lvi.pszText;

		lvi.pszText = str_to_LPWSTR(struct_time_to_string_time(flight->get_depTime()));
		ListView_SetItemText(listview, pos, 3, lvi.pszText);
		delete[] lvi.pszText;

		lvi.pszText = str_to_LPWSTR(struct_time_to_string_time(flight->get_arrivalTime()));
		ListView_SetItemText(listview, pos, 4, lvi.pszText);
		delete[] lvi.pszText;

		lvi.pszText = str_to_LPWSTR(int_to_str(flight->get_miles()));
		ListView_SetItemText(listview, pos, 5, lvi.pszText);
		delete[] lvi.pszText;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////window procedures/////////////////////////////////////////////

//program entry point
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
#ifdef _DEBUG
	SHOW_CONSOLE(true);
#endif
	//start main menu dialog
	return DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLG_MAIN_MENU), NULL, (DLGPROC)DialogProc, 0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////main menu (handle button presses)////////////////////////////////////////

//main menu window procedure
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	switch (message) {
		case WM_INITDIALOG:{
			//set global window for root parent (g_hwnd)
			g_hwnd = hwnd;

			//load the plane data
			load_airline_data();

			//init common controls (for listview control mostly)
			InitCommonControls();

			break;
		}
		case WM_COMMAND:{
			//handle button presses
			switch (LOWORD(wParam)) {
				case IDC_BUTTON1:{
					//handle add plane button press
					DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLG_ADD_PLANE), NULL, (DLGPROC)AddPlaneProc, 0);
				
					break;
				}
				case IDC_BUTTON2:{
					//handle add flight button press
					DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLG_ADD_FLIGHT), NULL, (DLGPROC)AddFlightProc, 0);
					
					break;
				}
				case IDC_BUTTON3:{
					//handle add passenger button press
					DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLG_ADD_PASSENGER), NULL, (DLGPROC)AddPassengerProc, 0);

					break;
				}
				case IDC_BUTTON4:{
					//handle adding passenger to flight button press
					CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG_TEMP), NULL, (DLGPROC)TempWndProc);

					break;
				}
				case IDC_BUTTON5:{
					//handle assigning plane to flight button press
					CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG_TEMP), NULL, (DLGPROC)TempWndProc2);

					break;
				}
				case IDC_BUTTON6:{
					//handle save button press
					//////////////////////////////////////////save planes///////////////////////////////////////////////////
					ofstream fplane;
					fplane.open("planes.txt");
					if (!fplane.is_open()) {
						cout << "Unable to open planes.txt\n";
						break;
					}
					for (UINT i = 0; i < g_fleet.size(); i++) {
						fplane << g_fleet[i]->get_id() << "|"
							<< g_fleet[i]->get_firstClass_cols() << "|"
							<< g_fleet[i]->get_firstClass_rows() << "|"
							<< g_fleet[i]->get_economyPlus_cols() << "|"
							<< g_fleet[i]->get_economyPlus_rows() << "|"
							<< g_fleet[i]->get_economy_cols() << "|"
							<< g_fleet[i]->get_economy_rows() << '\n';
					}
					fplane.close();

					//////////////////////////////////////////save flights///////////////////////////////////////////////////
					ofstream fflight, flightlog;
					fflight.open("flightsSaved.txt");
					flightlog.open("flights.txt");
					if (!fflight.is_open()) {
						cout << "Unable to open flights.txt";
						break;
					}

					time_t t = time(0);
					tm* now = localtime(&t);
					now->tm_mon++;
					now->tm_year += 1900;

					for (UINT i = 0; i < g_flights.size(); i++) {
						if (compare_time(now, &g_flights[i]->get_depTime())) {
							fflight << g_flights[i]->get_flightNumber() << "|"
								<< g_flights[i]->get_destination() << "|"
								<< g_flights[i]->get_departureLocation() << "|"
								<< struct_time_to_string_time(g_flights[i]->get_depTime()) << "|"
								<< struct_time_to_string_time(g_flights[i]->get_arrivalTime()) << "|"
								<< (g_flights[i]->get_plane() ? g_flights[i]->get_plane()->get_id() : 0) << "|"
								<< g_flights[i]->get_miles() << '\n';
						}
						flightlog << g_flights[i]->get_flightNumber() << "|"
							<< g_flights[i]->get_destination() << "|"
							<< g_flights[i]->get_departureLocation() << "|"
							<< struct_time_to_string_time(g_flights[i]->get_depTime()) << "|"
							<< struct_time_to_string_time(g_flights[i]->get_arrivalTime()) << "|"
							<< (g_flights[i]->get_plane() ? g_flights[i]->get_plane()->get_id() : 0) << "|"
							<< g_flights[i]->get_miles() << '\n';
					}
					fflight.close();
					flightlog.close();

					//////////////////////////////////////////save passengers///////////////////////////////////////////////
					ofstream pfile;
					pfile.open("passengers.txt");
					if (!pfile.is_open()) {
						cout << "Unable to open passengers.txt";
						break;
					}
					for (UINT i = 0; i < g_passengers.size(); i++) {
						pfile << g_passengers[i]->get_id() << "|"
							<< g_passengers[i]->get_name();
						for (UINT j = 0; j < g_passengers[i]->get_flights().size(); j++) {
							pfile << "|" << g_passengers[i]->get_flight(j)->get_flightNumber();
						}
						pfile << '\n';
					}
					pfile.close();

					break;
				}
				case IDC_BUTTON8:{
					//handle display flights button press
					DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_FLIGHTS_DISPLAY), NULL, (DLGPROC)DisplayFlightsProc, 0);

					break;
				}
				case IDC_BUTTON9:{
					//handle display passengers button press
					DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_PASSENGERS_DISPLAY), NULL, (DLGPROC)DisplayPassengersProc, 0);

					break;
				}
				case IDC_BUTTON10:{
					//handle display planes button press
					DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_PLANES_DISPLAY), NULL, (DLGPROC)DisplayPlanesProc, 0);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////creating new entries for planes, flights, or passengers/////////////////////////////

//procedure for create new plane dialog
BOOL CALLBACK AddPlaneProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	switch (message) {
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case IDOK:{
					//get plane seating info from input edit controls
					string fcr = getwindowtext(GetDlgItem(hwnd, IDC_ADD_PLANE_EDC_FIRST_ROWS));
					string fcc = getwindowtext(GetDlgItem(hwnd, IDC_ADD_PLANE_EDC_FIRST_COLS));
					string epr = getwindowtext(GetDlgItem(hwnd, IDC_ADD_PLANE_EDC_ECONOMYPLUS_ROWS));
					string epc = getwindowtext(GetDlgItem(hwnd, IDC_ADD_PLANE_EDC_ECONOMYPLUS_COLS));
					string ecr = getwindowtext(GetDlgItem(hwnd, IDC_ADD_PLANE_EDC_ECONOMY_ROWS));
					string ecc = getwindowtext(GetDlgItem(hwnd, IDC_ADD_PLANE_EDC_ECONOMY_COLS));

					//check if all inputs are set
					if (!fcr.size() || !fcc.size() || !epr.size() || !epc.size() || !ecr.size() || !ecc.size()) {
						MessageBox(hwnd, L"Must set all seat class row count and column count", L"Error", MB_OK);
						break;
					}

					//create new plane
					Plane* plane = new Plane;

					//initialize plane with user input
					plane->set_id(g_fleet.size() + 1);
					plane->set_firstClass_rows(str_to_int(fcr));
					plane->set_firstClass_cols(str_to_int(fcc));
					plane->set_economyPlus_rows(str_to_int(epr));
					plane->set_economyPlus_cols(str_to_int(epc));
					plane->set_economy_rows(str_to_int(ecr));
					plane->set_economy_cols(str_to_int(ecc));

					//add new plane to global list of planes
					g_fleet.push_back(plane);

					//end input dialog
					EndDialog(hwnd, 0);

					break;
				}
				case IDCANCEL:{
					EndDialog(hwnd, 0);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}

//procedure for create new flight dialog
BOOL CALLBACK AddFlightProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	switch (message) {
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case IDOK:{	
					//get input from edit controls
					string flightNumber = getwindowtext(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_FLIGHTNUMBER));
					string destination = getwindowtext(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_DESTINATION));
					string departureLocation = getwindowtext(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_DEPARTURE));
					string depTime = getwindowtext(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_DEPTIME));
					string arrivalTime = getwindowtext(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_ARRIVALTIME));
					string miles = getwindowtext(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_MILES));

					////////////////////////test the inputs for validity///////////////////////////////////////////
					//check for empty inputs
					if (!flightNumber.size() || !departureLocation.size() || !destination.size() || !depTime.size() || !arrivalTime.size() || !miles.size()) {
						MessageBox(hwnd, L"Must provide all flight information", L"Error", MB_OK | MB_ICONASTERISK);
						break;
					}						

					//check if input flight number already exists (found), if it does prompt user to change it
					bool found = false;
					for (UINT i = 0; i < g_flights.size(); i++) {
						if (g_flights[i]->get_flightNumber() == str_to_int(flightNumber)) {
							found = true;
							break;
						}
					}
					if (found) {
						MessageBox(hwnd, L"Flight Number already exists", L"Error", MB_OK | MB_ICONASTERISK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_FLIGHTNUMBER), 0, -1);
						break;
					}
					//test validity of departure time input
					if (!string_time_to_struct_time(depTime).tm_year) {
						MessageBox(hwnd, L"Invalid input for Departure Time\nFormat must be: MM/DD/YYYY HH:MM:SS", L"Error", MB_OK | MB_ICONASTERISK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_DEPTIME), 0, -1);
						break;
					}
					//test validity of arrival time input
					if (!string_time_to_struct_time(arrivalTime).tm_year) {
						MessageBox(hwnd, L"Invalid input for Arrival Time\nFormat must be: MM/DD/YYYY HH:MM:SS", L"Error", MB_OK | MB_ICONASTERISK);
						Edit_SetSel(GetDlgItem(hwnd, IDC_ADD_FLIGHT_EDC_ARRIVALTIME), 0, -1);
						break;
					}

					//check if departure times and arrival times are valid (in right order)
					tm tm_arrivalTime = string_time_to_struct_time(arrivalTime);
					tm tm_depTime = string_time_to_struct_time(depTime);
					if (compare_time(&tm_arrivalTime, &tm_depTime)) {
						//arrival time is earlier than teptime, which isn't possible
						MessageBox(hwnd, L"Cannot have an arrival time that comes before departure time!", L"Error", MB_OK | MB_ICONASTERISK);
						break;
					}

					////////////////////////all inputs valid, make new flight///////////////////////////////////////////
					//create a new flight and initialize it with the users input
					Flight* flight = new Flight;
					flight->set_flightNumber(str_to_int(flightNumber));
					flight->set_destination(destination);
					flight->set_departureLocation(departureLocation);
					flight->set_depTime(tm_depTime);
					flight->set_arrivalTime(tm_arrivalTime);
					flight->set_miles(str_to_int(miles));
					flight->set_plane(NULL);
								
					//add new flight to list of all flights
					g_flights.push_back(flight);

					//exit input dialog
					EndDialog(hwnd, 0);

					break;
				}
				case IDCANCEL:{
					EndDialog(hwnd, 0);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}

//procedure for create new passenger dialog
BOOL CALLBACK AddPassengerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	switch (message) {
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case IDOK:{
					string name = getwindowtext(GetDlgItem(hwnd, IDC_ADD_PASSENGER_EDC_NAME));

					//check input
					if (!name.size()) {
						MessageBox(hwnd, L"Must enter a name.", L"Error", MB_OK);
						break;
					}

					//make new Passenger
					Passenger* passenger = new Passenger;

					//initialize new passenger from input
					passenger->set_id(g_passengers.size() + 1);
					passenger->set_name(name);

					//add passenger to global list of passengers
					g_passengers.push_back(passenger);

					//exit input dialog
					EndDialog(hwnd, 0);

					break;
				}
				case IDCANCEL:{
					EndDialog(hwnd, 0);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////display all passengers, planes, or flights//////////////////////////////////////

//procedure for display passengers dialog (shows a list of all passengers)
BOOL CALLBACK DisplayPassengersProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			//check which instance of the dialog to initialize
			HWND listview = GetDlgItem(hwnd, IDC_LIST_PASSENGERS);
			if (!listview)
				listview = GetDlgItem(hwnd, IDC_LIST_PASSENGERS_CHILD);

			//initialize listview control to display passengers info
			SendMessage(listview, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			LV_COLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 500;
			lvc.pszText = L"Passenger Name";
			SendMessage(listview, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

			//fill the listview with ever passenger 
			PopulatePassengersListView(listview);

			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_LIST_PASSENGERS_CHILD:
				case IDC_LIST_PASSENGERS:{
					switch (lpnmh->code) {
						case LVN_COLUMNCLICK:{
							//sort column when clicked
							NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
							static int last = pListView->iSubItem;
							static bool flip = false;//store ordering
							if (last == pListView->iSubItem)
								flip = !flip;

							ListView_SortItems(GetDlgItem(hwnd, lpnmh->idFrom), ListViewPassengerCompareProc, flip ? pListView->iSubItem - 2 : pListView->iSubItem);

							last = pListView->iSubItem;//save last item

							break;
						}
						case NM_DBLCLK:{
							if (GetParent(hwnd)) {
							}
							else {
								if (ListView_GetCurSel(GetDlgItem(hwnd, lpnmh->idFrom)) >= 0) {
									ShowWindow(hwnd, SW_HIDE);
									//open selected passenger in new window on double click
									DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_PASSENGER_DISPLAY), NULL, (DLGPROC)DisplayPassengerProc,
										ListView_GetItemParam(GetDlgItem(hwnd, lpnmh->idFrom), ListView_GetCurSel(GetDlgItem(hwnd, lpnmh->idFrom))));
									EndDialog(hwnd, 0);
								}
							}

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
			if (!GetParent(hwnd))
				EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}

//procedure for display flights dialog (shows a list of all flights)
BOOL CALLBACK DisplayFlightsProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			//check which instance of the dialog to initialize
			HWND listview = GetDlgItem(hwnd, IDC_LIST_FLIGHTS);
			if (!listview)
				listview = GetDlgItem(hwnd, IDC_LIST_FLIGHTS_CHILD);

			//initialize listview control to display flight info
			SendMessage(listview, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			LV_COLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 90;
			lvc.pszText = L"Flight Number";
			SendMessage(listview, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);
			lvc.cx = 124;
			lvc.pszText = L"Destination";
			SendMessage(listview, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);
			lvc.cx = 124;
			lvc.pszText = L"Departure Location";
			SendMessage(listview, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);
			lvc.cx = 118;
			lvc.pszText = L"Departure Time";
			SendMessage(listview, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc);
			lvc.cx = 118;
			lvc.pszText = L"Arrival Time";
			SendMessage(listview, LVM_INSERTCOLUMN, 4, (LPARAM)&lvc);
			lvc.cx = 66;
			lvc.pszText = L"Flight Miles";
			SendMessage(listview, LVM_INSERTCOLUMN, 5, (LPARAM)&lvc);
			lvc.cx = 55;
			lvc.pszText = L"Plane ID";
			SendMessage(listview, LVM_INSERTCOLUMN, 6, (LPARAM)&lvc);

			//populate the new listview with all flights
			PopulateFlightsListView(listview);

			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_LIST_FLIGHTS_CHILD:
				case IDC_LIST_FLIGHTS:{
					switch (lpnmh->code) {
						case LVN_COLUMNCLICK:{
							//sort columns when you click on them, reversing order when clicked again
							NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
							static int last = pListView->iSubItem;
							static bool flip = false;
							if (last == pListView->iSubItem)
								flip = !flip;

							ListView_SortItems(GetDlgItem(hwnd, lpnmh->idFrom), ListViewFlightCompareProc, flip ? pListView->iSubItem - 8 : pListView->iSubItem);

							last = pListView->iSubItem;

							break;
						}
						case NM_DBLCLK:{
							if (GetParent(hwnd)) {
							}
							else {
								if (ListView_GetCurSel(GetDlgItem(hwnd, lpnmh->idFrom)) >= 0) {
									ShowWindow(hwnd, SW_HIDE);
									//open selected flight on double click in new window
									DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_FLIGHT_DISPLAY), NULL, (DLGPROC)DisplayFlightProc,
										ListView_GetItemParam(GetDlgItem(hwnd, lpnmh->idFrom), ListView_GetCurSel(GetDlgItem(hwnd, lpnmh->idFrom))));
									EndDialog(hwnd, 0);
								}
							}

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
			if (!GetParent(hwnd))
				EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}

//procedure for display planes dialog (shows a list of all planes)
BOOL CALLBACK DisplayPlanesProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			//check which instance of dialog is being used
			HWND listview = GetDlgItem(hwnd, IDC_LIST_PLANES);
			if (!listview)
				listview = GetDlgItem(hwnd, IDC_LIST_PLANES_CHILD);

			//initialize listview control to display flight info
			SendMessage(listview, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			LV_COLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 45;
			lvc.pszText = L"ID";
			SendMessage(listview, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);
			lvc.cx = 140;
			lvc.pszText = L"Flight No.";
			SendMessage(listview, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);
			lvc.cx = 180;
			lvc.pszText = L"First Class Rows/Columns";
			SendMessage(listview, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);
			lvc.cx = 180;
			lvc.pszText = L"Economy Plus Rows/Columns";
			SendMessage(listview, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc);
			lvc.cx = 180;
			lvc.pszText = L"Economy Class Rows/Columns";
			SendMessage(listview, LVM_INSERTCOLUMN, 4, (LPARAM)&lvc);			

			//populate the new listview with every plane
			PopulatePlanesListView(listview);

			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_LIST_PLANES_CHILD:
				case IDC_LIST_PLANES:{
					switch (lpnmh->code) {
						case LVN_COLUMNCLICK:{
							//sort planes columns when clicked on, flipping them when clicked again
							NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
							static int last = pListView->iSubItem;
							static bool flip = false;
							if (last == pListView->iSubItem)
							flip = !flip;

							ListView_SortItems(GetDlgItem(hwnd, lpnmh->idFrom), ListViewPlaneCompareProc, flip ? pListView->iSubItem - 7 : pListView->iSubItem);

							last = pListView->iSubItem;

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
			if (!GetParent(hwnd))
				EndDialog(hwnd, 0);

		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////display individual flight or passenger info////////////////////////////////////

//procedure for displaying a passenger dialog (list of their flights + passenger info)
BOOL CALLBACK DisplayPassengerProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			//get the passenger from lParam 
			Passenger* passenger = (Passenger*)lParam;

			HWND listview = GetDlgItem(hwnd, IDC_LIST_PASSENGER);

			//initialize listview control to display flight info
			SendMessage(listview, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			LV_COLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 174;
			lvc.pszText = L"Flight Number";
			SendMessage(listview, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);
			lvc.cx = 124;
			lvc.pszText = L"Destination";
			SendMessage(listview, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);
			lvc.cx = 124;
			lvc.pszText = L"Departure Location";
			SendMessage(listview, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);
			lvc.cx = 118;
			lvc.pszText = L"Departure Time";
			SendMessage(listview, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc);
			lvc.cx = 118;
			lvc.pszText = L"Arrival Time";
			SendMessage(listview, LVM_INSERTCOLUMN, 4, (LPARAM)&lvc);
			lvc.cx = 66;
			lvc.pszText = L"Flight Miles";
			SendMessage(listview, LVM_INSERTCOLUMN, 5, (LPARAM)&lvc);

			//populate the listview with the passengers flights
			PopulatePassengerListView(passenger, hwnd);

			//display the passengers name in an edit control
			SetWindowText(GetDlgItem(hwnd, IDC_DISPLAY_PASSENGER_EDC_NAME), STW(passenger->get_name()));

			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_LIST_PASSENGER:{
					switch (lpnmh->code) {
						case LVN_COLUMNCLICK:{
							//sort columns on click, reversing sorting order every time it is clicked again
							NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
							static int last = pListView->iSubItem;
							static bool flip = false;
							if (last == pListView->iSubItem)
								flip = !flip;

							ListView_SortItems(GetDlgItem(hwnd, IDC_LIST_PASSENGER), ListViewFlightCompareProc, flip ? pListView->iSubItem - 7 : pListView->iSubItem);

							last = pListView->iSubItem;

							break;
						}
						case NM_DBLCLK:{
							if (ListView_GetCurSel(GetDlgItem(hwnd, IDC_LIST_PASSENGER)) >= 0) {
								ShowWindow(hwnd, SW_HIDE);
								//display flight information when one of the passengers flights is double clicked
								DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_FLIGHT_DISPLAY), NULL, (DLGPROC)DisplayFlightProc,
									ListView_GetItemParam(GetDlgItem(hwnd, IDC_LIST_PASSENGER), ListView_GetCurSel(GetDlgItem(hwnd, IDC_LIST_PASSENGER))));
								EndDialog(hwnd, 0);
							}

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
		}
	}
	return FALSE;
}

//procedure for displaying a flight dialog (list of passengers + plane info + the flights info)
BOOL CALLBACK DisplayFlightProc(HWND hwnd, UINT message, WPARAM /*wParam*/, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:{
			//get flight from lParam
			Flight* flight = (Flight*)lParam;

			HWND listview = GetDlgItem(hwnd, IDC_LIST_FLIGHT);

			//initialize the listview for passengers
			SendMessage(listview, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			LV_COLUMN lvc = {0};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 300;
			lvc.pszText = L"Name";
			SendMessage(listview, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

			//populate the list with the flights passengers
			PopulateFlightListView(flight, hwnd);

			//display the flights assigned plane's info if it has one
			if (flight->get_plane()) {
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_FIRSTCLASS_ROWS), ITW(flight->get_plane()->get_firstClass_rows()));
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_FIRSTCLASS_COLS), ITW(flight->get_plane()->get_firstClass_cols()));

				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMYPLUS_ROWS), ITW(flight->get_plane()->get_economyPlus_rows()));
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMYPLUS_COLS), ITW(flight->get_plane()->get_economyPlus_cols()));

				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMY_ROWS), ITW(flight->get_plane()->get_economy_rows()));
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMY_COLS), ITW(flight->get_plane()->get_economy_cols()));
			}
			else {
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_FIRSTCLASS_ROWS), L"NULL");
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_FIRSTCLASS_COLS), L"NULL");

				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMYPLUS_ROWS), L"NULL");
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMYPLUS_COLS), L"NULL");

				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMY_ROWS), L"NULL");
				SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ECONOMY_COLS), L"NULL");
			}

			//display the flights info
			SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_FLIGHTNUMBER), ITW(flight->get_flightNumber()));
			SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_DESTINATION), STW(flight->get_destination()));
			SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_DEPARTURELOCATION), STW(flight->get_departureLocation()));
			SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_DEPTIME), STW(struct_time_to_string_time(flight->get_depTime())));
			SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_ARRIVALTIME), STW(struct_time_to_string_time(flight->get_arrivalTime())));
			SetWindowText(GetDlgItem(hwnd, IDC_FLIGHT_DISPLAY_EDC_MILES), ITW(flight->get_miles()));

			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			switch (lpnmh->idFrom) {
				case IDC_LIST_FLIGHT:{
					switch (lpnmh->code) {
						case LVN_COLUMNCLICK:{
							//swap sorting order when column is clicked
							NMLISTVIEW* pListView = (NMLISTVIEW*)lParam;
							static int last = pListView->iSubItem;
							static bool flip = false;
							if (last == pListView->iSubItem)
								flip = !flip;

							ListView_SortItems(GetDlgItem(hwnd, IDC_LIST_FLIGHT), ListViewPassengerCompareProc, flip ? pListView->iSubItem - 2 : pListView->iSubItem);

							last = pListView->iSubItem;

							break;
						}
						case NM_DBLCLK:{							
							if (ListView_GetCurSel(GetDlgItem(hwnd, IDC_LIST_FLIGHT)) >= 0) {
								ShowWindow(hwnd, SW_HIDE);
								//display passenger info when passenger is double clicked
								DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_PASSENGER_DISPLAY), NULL, (DLGPROC)DisplayPassengerProc,
									ListView_GetItemParam(GetDlgItem(hwnd, IDC_LIST_FLIGHT), ListView_GetCurSel(GetDlgItem(hwnd, IDC_LIST_FLIGHT))));
								EndDialog(hwnd, 0);
							}							

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
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////host windows for dialog clones///////////////////////////////////////////

//host window for selecting passengers to assign to flights
BOOL CALLBACK TempWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	static HWND df, dp;

	switch (message) {
		case WM_INITDIALOG:{
			ShowWindow(hwnd, SW_SHOW);

			//startup child windows for selecting a flight and a passenger
			df = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_FLIGHTS_DISPLAY_CHILD), hwnd, (DLGPROC)DisplayFlightsProc);
			ShowWindow(df, SW_SHOW);
			dp = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_PASSENGERS_DISPLAY_CHILD), hwnd, (DLGPROC)DisplayPassengersProc);
			ShowWindow(dp, SW_SHOW);
		
			break;
		}
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case IDC_TEMP_BTN_OK:{
					//get users flight and passenger selection when ok button is pressed
					Passenger* passenger = (Passenger*)ListView_GetItemParam(GetDlgItem(dp, IDC_LIST_PASSENGERS_CHILD), ListView_GetCurSel(GetDlgItem(dp, IDC_LIST_PASSENGERS_CHILD)));
					Flight* flight = (Flight*)ListView_GetItemParam(GetDlgItem(df, IDC_LIST_FLIGHTS_CHILD), ListView_GetCurSel(GetDlgItem(df, IDC_LIST_FLIGHTS_CHILD)));

					//check if both have been selected
					if (!passenger) {
						MessageBox(hwnd, L"Choose a passenger!", L"Error", MB_OK | MB_ICONASTERISK);
						break;
					}
					if (!flight) {
						MessageBox(hwnd, L"Choose a flight!", L"Error", MB_OK | MB_ICONASTERISK);
						break;
					}

					//check to see if the passenger is already on the flight
					bool found = false;
					for (UINT i = 0; i < passenger->get_flights().size(); i++) {
						if (passenger->get_flight(i)->get_flightNumber() == flight->get_flightNumber()) {
							MessageBox(hwnd, L"Passenger is already on this flight", L"Error", MB_OK);
							found = true;
							break;
						}
					}
					if (found)
						break;

					//inputs valid and passenger not already on flight so put them on the flight
					passenger->add_flight(flight);
					flight->add_passenger(passenger);

					//display success message box
					MessageBox(hwnd, STW("Successfully added " + passenger->get_name() + " to flight no. " + int_to_str(flight->get_flightNumber())), L"Success", MB_OK);

					//EndDialog(hwnd, 0);

					break;
				}
				case IDC_TEMP_BTN_CANCEL:{
					EndDialog(hwnd, 0);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}

//procedure for host window for assigning a plane to a flight dialog
BOOL CALLBACK TempWndProc2(HWND hwnd, UINT message, WPARAM wParam, LPARAM /*lParam*/) {
	static HWND df, dp;

	switch (message) {
		case WM_INITDIALOG:{
			ShowWindow(hwnd, SW_SHOW);

			//parent dialog is recycled so change its title for a more accurate description
			SetWindowText(hwnd, L"Assign a plane to a flight");

			//startup child windows for selecting a flight and a plane
			df = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_FLIGHTS_DISPLAY_CHILD), hwnd, (DLGPROC)DisplayFlightsProc);
			ShowWindow(df, SW_SHOW);
			dp = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDC_DLG_PLANES_DISPLAY_CHILD), hwnd, (DLGPROC)DisplayPlanesProc);
			ShowWindow(dp, SW_SHOW);

			break;
		}
		case WM_COMMAND:{
			switch (LOWORD(wParam)) {
				case IDC_TEMP_BTN_OK:{
					//get flight and plane selections from each window's listview
					Plane* plane = (Plane*)ListView_GetItemParam(GetDlgItem(dp, IDC_LIST_PLANES_CHILD), ListView_GetCurSel(GetDlgItem(dp, IDC_LIST_PLANES_CHILD)));
					Flight* flight = (Flight*)ListView_GetItemParam(GetDlgItem(df, IDC_LIST_FLIGHTS_CHILD), ListView_GetCurSel(GetDlgItem(df, IDC_LIST_FLIGHTS_CHILD)));

					//check to see that both a flight and plane have been selected
					if (!plane) {
						MessageBox(hwnd, L"Must choose a plane!", L"Error", MB_OK | MB_ICONASTERISK);
						break;
					}
					if (!flight) {
						MessageBox(hwnd, L"Must choose a flight!", L"Error", MB_OK | MB_ICONASTERISK);
						break;
					}

					//set the selected plane to be the plane for the selected flight
					flight->set_plane(plane);

					//display success message box
					MessageBox(hwnd, STW("Successfully set plane id: " + int_to_str(plane->get_id()) + " to flight no. " + int_to_str(flight->get_flightNumber())), L"Success", MB_OK);

					break;
				}
				case IDC_TEMP_BTN_CANCEL:{
					EndDialog(hwnd, 0);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			EndDialog(hwnd, 0);
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

