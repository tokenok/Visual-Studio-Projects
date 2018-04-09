#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <random>
#include <fstream>
#include <algorithm>
#include <time.h>

using namespace std;

#define PI 3.14159265

const int T = 86400;

class transmission_station {
	public:
	vector<int> queue;
	int wait = 0;
	int c = 0;
	static int sid;
	int id = -1;

	transmission_station() {
		id = sid++;
	}
};

int transmission_station::sid = 0;

struct Random {	int Next(int min, int max) { return min + rand() % (max - min); } };
Random rng;	

void non_persistant_P(int &sent, int &collisions, int &unsent, vector<transmission_station> &ts, int end = -1) {
	bool in_use = false;
	int talker_id = -1;

	collisions = 0;
	sent = 0;
	unsent = 0;

	for (int f = 0; f < T; f++) {
		bool frame_used = false;
		for (unsigned int i = 0; i < (end == - 1 ? ts.size() : (end - 1)); i++) {
			if (rng.Next(1, 100) <= 20)
				ts[i].queue.push_back(1);
			if (ts[i].queue.size() > 0 && ts[i].wait == 0 && (!in_use || talker_id == ts[i].id) && !frame_used) {
				in_use = true;
				talker_id = ts[i].id;
				ts[i].queue[0]--;
				sent++;
				if (ts[i].queue[0] == 0) {
					ts[i].queue.erase(ts[i].queue.begin());
					in_use = false;
					talker_id = -1;
					frame_used = true;
				}
			}
			else if (ts[i].queue.size() > 0) {
				if (ts[i].wait == 0) {
					ts[i].wait = rng.Next(1, 100);
					collisions++;
				}
				else
					ts[i].wait--;
			}
		}
	//	if (f % 100 == 0) shuffle(ts.begin(), ts.end(), std::default_random_engine(clock()));
	}
	for (unsigned int i = 0; i < ts.size(); i++) {
		for (unsigned int j = 0; j < ts[i].queue.size(); j++) {
			unsent += ts[i].queue[j];
		}
	}
}

void one_persistant_BEB(int &sent, int &collisions, int &unsent, vector<transmission_station> &ts, int end = -1) {
	bool in_use = false;
	int talker_id = -1;

	collisions = 0;
	sent = 0;
	unsent = 0;

	for (int f = 0; f < T; f++) {
		bool frame_used = false;
		for (unsigned int i = 0; i < (end == -1 ? ts.size() : (end - 1)); i++) {
			if (rng.Next(1, 100) <= 20)
				ts[i].queue.push_back(1);
			if (ts[i].queue.size() > 0 && ts[i].wait == 0 && (!in_use || talker_id == ts[i].id) && !frame_used) {
				in_use = true;
				talker_id = ts[i].id;
				ts[i].queue[0]--;
				ts[i].c = 0;
				sent++;
				if (ts[i].queue[0] == 0) {
					ts[i].queue.erase(ts[i].queue.begin());
					in_use = false;
					talker_id = -1;
					frame_used = true;
				}
			}
			else if (ts[i].queue.size() > 0) {
				if (ts[i].wait == 0) {
					ts[i].wait = ts[i].c < 10 ? (int)pow(2, ts[i].c) : ts[i].c > 16 ? -1, ts[i].c = 0 : ts[i].wait;
					ts[i].c++;
					if (ts[i].wait == -1) {
						cout << "Error\n";
					}
					collisions++;
				}
				else
					ts[i].wait--;
			}
		}
	//	if (f % 100 == 0) shuffle(ts.begin(), ts.end(), std::default_random_engine(clock()));
	}
	for (unsigned int i = 0; i < ts.size(); i++) {
		for (unsigned int j = 0; j < ts[i].queue.size(); j++) {
			unsent += ts[i].queue[j];
		}
	}
}

int main() {
	srand(clock());

	ofstream file;
	file.open("thru nonp.txt");

	file << "0\t0\t0\t0\t0\t0\t0\t0\n";

	for (int N = 1; N <= 25; N++) {
		int sent = 0, collisions = 0, unsent = 0;
		vector<transmission_station> ts(N);
		vector<transmission_station> ts1(N);
		non_persistant_P(sent, collisions, unsent, ts);
		file << N << "\t" << (double)sent / (double)T * 100 << "\t" << ((double)sent / (double)(sent + unsent)) * 100 << '\t' << collisions << "\t";
		one_persistant_BEB(sent, collisions, unsent, ts1);
		file << N << "\t" << (double)sent / (double)T * 100 << "\t" << ((double)sent / (double)(sent + unsent)) * 100 << '\t' << collisions << '\n';
		cout << N << '\n';
	}
	file.close();

	//////////////
	vector<int> v;
	int max = 0;
	for (int i = 0; i < 100; i += 10) {
		int val = (int)(10 * sin(PI / 50 * i) + 15);
		v.push_back(val);
		max = val > max ? val : max;
	}
	vector<transmission_station> ts(max);
	vector<transmission_station> ts1(max);

	file.open("sim.txt");
	for (unsigned int i = 0; i < v.size(); i++) {
		int sent, collisions, unsent;

		non_persistant_P(sent, collisions, unsent, ts, v[i]);
		file << i << '\t' << v[i] << "\t" << (double)sent / (double)T << '\t' << collisions << "\t\t=1/(MAX(B:B)-MIN(B:B))*(B" << i + 1 << "-MAX(B:B))+1\t=1/(MAX(C:C)-MIN(C:C))*(C" << i + 1 << "-MAX(C:C))+1\t=1/(MAX(D:D)-MIN(D:D))*(D" << i + 1 << "-MAX(D:D))+1\t";

		one_persistant_BEB(sent, collisions, unsent, ts1, v[i]);
		file << i << '\t' << v[i] << "\t" << (double)sent / (double)T << '\t' << collisions << "\t\t=1/(MAX(J:J)-MIN(J:J))*(J" << i + 1 << "-MAX(J:J))+1\t=1/(MAX(K:K)-MIN(K:K))*(K" << i + 1 << "-MAX(K:K))+1\t=1/(MAX(L:L)-MIN(L:L))*(L" << i + 1 << "-MAX(L:L))+1\n";;

		cout << i << '\n';
	}
	file.close();

	return 0;
}

