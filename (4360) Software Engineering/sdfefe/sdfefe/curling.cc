#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip> 
#include "curlingStatistics.h"

using namespace std;

string extractName(string s){
	return s.substr(0, s.find('\t'));
}

vector<int> extractScores(string s){
	vector<int> points;
	for (int i = 0; i < s.size(); i++)
		if(s[i] >= '0' && s[i] <= '9') 
			points.push_back(s[i] - '0');
	return points;
}

int main() {
	cout << "Welcome to the curling stat tracker!" << endl;
	vector<CurlingStatistics> team1;
	vector<CurlingStatistics> team2;
	vector<vector<CurlingStatistics>> teams {team1, team2};
	string teamnames[2];
	string teamname, player1;
	ifstream file;
	file.open("scoresMJW");
	for (int i = 0; i < 2; i++) {
		string line;
		getline(file, teamname);
		teamnames[i] = teamname;
		for (int j = 0; j < 4; j++){
			getline(file, player1);
			vector<int> vals = extractScores(player1);
			int totes = 0;
			for (int k = 0; k < vals.size(); k++){
				totes += vals[k];
			}
			CurlingStatistics player(extractName(player1), totes, vals.size());
			teams[i].push_back(player);
		}
	}
	ofstream out;
	out.open("resultsADC.txt");
	for (int i = 0; i < 2; i++){
		out << teamnames[i] << endl;
		for (int j = 0; j < 4; ++j) {
			out << setprecision(4) << " " << teams[i][j].getCurlerName() << " " << teams[i][j].calcAvg() << "%" << endl;
		}
	}

	out << "\n\nTop Statistics by position\n\n";

	vector<string> positions = {"Lead", "Second", "Vice Skip", "Skip"};
	for (int i = 0; i < 4; i++) 
		out << positions[i] << ": " << (teams[0][i].calcAvg() > teams[1][i].calcAvg() ? teams[0][i].getCurlerName() : teams[1][i].getCurlerName()) << '\n';

	out.close();
	return 0;
}
