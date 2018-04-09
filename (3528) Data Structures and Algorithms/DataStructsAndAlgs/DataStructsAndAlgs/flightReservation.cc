/*
Josh Urmann
Program #2

this program reads plane seating charts from file which contains seats that are already taken
then prompts the user to enter seats that they wish to reserve
it will then save the modified seating chart to another file
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
//#include <vector>//pls

using namespace std;

void load(ifstream&, bool***);
void save(ofstream&, const bool* const*);
void printchart(ostream&, const bool* const*);
void getinput(bool***);
bool testseat(string&, int*, int*);

int str_to_int(string);
string int_to_str(int, bool = false);

int g_totalrows = -1;
int g_totalcolumns = -1;
int g_aisle = -1;

int main() {
	bool** seats;

	//load initial plane seating chart
	ifstream ifplane;
	ifplane.open("plane");
	if (ifplane.is_open())
		load(ifplane, &seats);
	else
		return 0;
	ifplane.close();

	//print the initial chart
	printchart(cout, seats);

	//get user input to modify chart
	getinput(&seats);

	//save those modifications
	ofstream ofplane;
	ofplane.open("newPlane");
	if (ofplane.is_open())
		save(ofplane, seats);
	else
		return 0;
	ofplane.close();

	return 0;
}

//tests range of inputs against size of array, also allows for lowercase characters to be used
//row and column will be used for the indices of array based on input
bool testseat(string& test, int* row, int* column) {
	if (test.size() < 2) {
		cout << "Invalid seat.\nMust be a number-letter pair\n";
		return false;
	}
	*row = str_to_int(test.substr(0, test.size() - 1));
	*column = test[test.size() - 1];
	if (*row > g_totalrows || *row <= 0) {
		cout << "Invalid seat.\nRow out of range.\n";
		return false;
	}
	if (*column >= 97 && *column <= 123) *column -= 32;//handle lowercase
	if (*column - 65 >= g_totalcolumns + 1 || *column - 65 < 0) {
		cout << "Invalid seat.\nColumn out of range.\n";
		return false;
	}
	test = int_to_str(*row) + static_cast<char>(*column);
	if (*column - 65 >= g_aisle)
		*column = *column - 1;
	*column -= 65;
	*row -= 1;
	return true;
}

//loads plane seating chart from file and stores it in array(bool) where 1 = seat taken, 0 = seat available
void load(ifstream& file, bool*** arr) {
	file >> g_totalrows;
	file >> g_totalcolumns;
	g_aisle = g_totalcolumns / 2 + g_totalcolumns % 2;
	*arr = new bool*[g_totalrows];
	for (int i = 0; i < g_totalrows; i++) {
		(*arr)[i] = new bool[g_totalcolumns];
	}

	string seats = "";
	while (file >> seats) {
		int row, column;
		if (testseat(seats, &row, &column)) {
			(*arr)[row][column] = 1;
		}
	}
}

//saves arr to file
void save(ofstream& file, const bool* const* arr) {
	file << g_totalrows << " ";
	file << g_totalcolumns << " \n";
	for (int i = 0; i < g_totalrows; i++) {
		for (int j = 0; j < g_totalcolumns; j++) {
			if (arr[i][j] == 1)
				file << i + 1
				<< static_cast<char>(j + 65 + (j >= g_aisle ? 1 : 0)) << " ";//adjust for aisle
		}
		file << '\n';
	}
}

//streams the current seating chart in grid form to "out" (std::cout)
void printchart(ostream& out, const bool* const* arr) {
	for (int i = 0; i < g_totalrows; i++) {
		out << left << setw(2) << i + 1 << ": ";
		for (int j = 0; j < g_totalcolumns; j++) {
			if (j == g_aisle)
				out << " ";//add space for aisle
			if (arr[i][j] == 1)
				out << "X ";//seat is taken
			else
				out << static_cast<char>(65 + j + (j >= g_aisle ? 1 : 0)) << " ";//adjust for aisle

		}
		out << '\n';
	}
}

void getinput(bool*** arr) {
	string input = "";
	cout << "Enter a seat you want to reserve: [1-" << g_totalrows << "][A-" << static_cast<char>(g_totalcolumns + 65) << "] (Enter 0 to Save and Exit.)\n";
	while (cin >> input) {
		if (input.size() == 1 && input[0] == '0') return;//exit when zero is entered
		int row, column;
		if (testseat(input, &row, &column)) {//test validity of input and get indices
			(*arr)[row][column] = 1;//if valid then reserve the seat
			printchart(cout, *arr);
		}
		cout << "Enter a seat you want to reserve: [1-" << g_totalrows << "][A-" << static_cast<char>(g_totalcolumns + 65) << "] (Enter 0 to Save and Exit.)\n";
	}
}

//been using these for years, very useful
//converts stl strings to int and vise versa
int str_to_int(string str) {
	stringstream out(str);
	int num;
	out >> (str.find("0x") != string::npos ? hex : dec) >> num;
	return num;
}
string int_to_str(int num, bool is_hex /*= false*/) {
	stringstream out;
	out << (is_hex ? hex : dec) << num;
	string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}