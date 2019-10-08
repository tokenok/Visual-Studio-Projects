#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <ctime>
#include <thread>
#include <algorithm>

using namespace std;

struct dat {
	double a, b, c, d;
	char variable;
	vector<double> probs;
};

struct cent {
	double a, b, c, d;
	vector<dat> pts;
};

struct range {
	double min = 1.7976931348623158e+308;
	double max = 2.2250738585072014e-308;
};

double frand(double min, double max) {
	return min + (double)rand() / RAND_MAX * (max - min);
}

double get_dist(const cent& a, const dat& b) {
	return sqrt(pow(a.a - b.a, 2) + pow(a.b - b.b, 2) + pow(a.c - b.c, 2) + pow(a.d - b.d, 2));
}

string int_to_str(int num, bool is_hex = false) {
	stringstream out;
	out << (is_hex ? hex : dec) << num;
	string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}

vector<cent> kmeans(vector<dat>& data, int k, int reps, double* BEST_WCSSE, vector<double>* WCSSEs = NULL) {
	vector<cent> solution;

	//get data ranges
	vector<range> ranges(4);
	enum {a1, b1, c1, d1};
	for (UINT i = 0; i < data.size(); i++) {
		ranges[a1].min = data[i].a < ranges[a1].min ? data[i].a : ranges[a1].min;
		ranges[b1].min = data[i].b < ranges[b1].min ? data[i].b : ranges[b1].min;
		ranges[c1].min = data[i].c < ranges[c1].min ? data[i].c : ranges[c1].min;
		ranges[d1].min = data[i].d < ranges[d1].min ? data[i].d : ranges[d1].min;
		ranges[a1].max = data[i].a > ranges[a1].max ? data[i].a : ranges[a1].max;
		ranges[b1].max = data[i].b > ranges[b1].max ? data[i].b : ranges[b1].max;
		ranges[c1].max = data[i].c > ranges[c1].max ? data[i].c : ranges[c1].max;
		ranges[d1].max = data[i].d > ranges[d1].max ? data[i].d : ranges[d1].max;
	}

	*BEST_WCSSE = 1.7976931348623158e+308;

	for (UINT r = 0; r < reps; r++) {

		//generate random centroids 
		vector<cent> centroids;
		for (UINT i = 0; i < k; i++) {
			cent centroid;
			centroid.a = frand(ranges[a1].min, ranges[a1].max);
			centroid.b = frand(ranges[b1].min, ranges[b1].max);
			centroid.c = frand(ranges[c1].min, ranges[c1].max);
			centroid.d = frand(ranges[d1].min, ranges[d1].max);
			centroids.push_back(centroid);
		}

		while (true) {

			vector<cent> prev = centroids;

			//assign points to centroid based on distance
			for (UINT d = 0; d < data.size(); d++) {
				double min_dist = 1.7976931348623158e+308;
				int min_index = -1;
				for (UINT c = 0; c < centroids.size(); c++) {
					double dist = get_dist(centroids[c], data[d]);
					if (dist <= min_dist) {
						min_dist = dist;
						min_index = c;
					}
				}
				centroids[min_index].pts.push_back(data[d]);
			}

			//check for empty centroid, move a point from populated centroid to empty if found
			for (UINT i = 0; i < centroids.size(); i++) {
				if (!centroids[i].pts.size()) {
					for (UINT j = 0; j < centroids.size(); j++) {
						if (centroids[j].pts.size() > 1) {
							centroids[i].pts.push_back(centroids[j].pts[centroids[j].pts.size() - 1]);
							centroids[j].pts.pop_back();
							break;
						}
					}
				}
			}

			//move centroids to average of points
			for (UINT i = 0; i < centroids.size(); i++) {
				dat t = {0, 0, 0, 0};

				if (!centroids[i].pts.size()) {
					continue;
				}

				for (UINT j = 0; j < centroids[i].pts.size(); j++) {
					t.a += centroids[i].pts[j].a;
					t.b += centroids[i].pts[j].b;
					t.c += centroids[i].pts[j].c;
					t.d += centroids[i].pts[j].d;
				}
				centroids[i].a = t.a / centroids[i].pts.size();
				centroids[i].b = t.b / centroids[i].pts.size();
				centroids[i].c = t.c / centroids[i].pts.size();
				centroids[i].d = t.d / centroids[i].pts.size();
			}
			
			//check to see if centroids moved
			bool exit = true;
			for (UINT i = 0; i < prev.size(); i++) {
				if (prev[i].a != centroids[i].a || prev[i].b != centroids[i].b || prev[i].c != centroids[i].c || prev[i].d != centroids[i].d) {
					for (UINT c = 0; c < centroids.size(); c++)
						centroids[c].pts.clear();
					exit = false;
					break;
				}
			}			
			if (!exit)
				continue;

			//WCSSE	
			double WCSSE = 0;
			for (UINT c = 0; c < centroids.size(); c++) {
				for (UINT d = 0; d < centroids[c].pts.size(); d++)
					WCSSE += pow(get_dist(centroids[c], centroids[c].pts[d]), 2);				
			}
			WCSSE = sqrt(WCSSE);
			if (WCSSEs)
				WCSSEs->push_back(WCSSE);

			if (WCSSE < *BEST_WCSSE) {
				*BEST_WCSSE = WCSSE;
				solution = centroids;
			}

			if (exit)
				break;
		}
	}

	return solution;
}

vector<cent> fuzzy(vector<dat>& data, int k, int reps, double* BEST_WCSSE, vector<double>* WCSSEs = NULL) {
	vector<cent> solution;

	//get data ranges
	vector<range> ranges(4);
	enum { a1, b1, c1, d1 };
	for (UINT i = 0; i < data.size(); i++) {
		ranges[a1].min = data[i].a < ranges[a1].min ? data[i].a : ranges[a1].min;
		ranges[b1].min = data[i].b < ranges[b1].min ? data[i].b : ranges[b1].min;
		ranges[c1].min = data[i].c < ranges[c1].min ? data[i].c : ranges[c1].min;
		ranges[d1].min = data[i].d < ranges[d1].min ? data[i].d : ranges[d1].min;
		ranges[a1].max = data[i].a > ranges[a1].max ? data[i].a : ranges[a1].max;
		ranges[b1].max = data[i].b > ranges[b1].max ? data[i].b : ranges[b1].max;
		ranges[c1].max = data[i].c > ranges[c1].max ? data[i].c : ranges[c1].max;
		ranges[d1].max = data[i].d > ranges[d1].max ? data[i].d : ranges[d1].max;
	}

	for (UINT r = 0; r < reps; r++) {

		//generate random centroids 
		vector<cent> centroids;
		for (UINT i = 0; i < k; i++) {
			cent centroid;
			centroid.a = frand(ranges[a1].min, ranges[a1].max);
			centroid.b = frand(ranges[b1].min, ranges[b1].max);
			centroid.c = frand(ranges[c1].min, ranges[c1].max);
			centroid.d = frand(ranges[d1].min, ranges[d1].max);
			centroids.push_back(centroid);
		}

		//E-Step
		vector<double> dists(k);
		vector<double> probs(k);

		while (true) {
			vector<cent> prev = centroids;

			//calculate distance weights
			for (UINT d = 0; d < data.size(); d++) {
				double denom = 0;

				for (UINT c = 0; c < centroids.size(); c++)
					dists[c] = 1.0 / (pow(get_dist(centroids[c], data[d]), 2));

				for (UINT i = 0; i < dists.size(); i++)
					denom += dists[i];

				for (UINT i = 0; i < probs.size(); i++)
					probs[i] = dists[i] / denom;

				data[d].probs = probs;
			}

			//M-Step
			vector<double> na, nb, nc, nd;
			for (UINT p = 0; p < probs.size(); p++) {
				double num = 0, den = 0;
				for (UINT i = 0; i < data.size(); i++) {
					double pa = pow(data[i].probs[p], 2);
					num += (pa * data[i].a);
					den += pa;
				}
				na.push_back(num / den);
			}

			for (UINT p = 0; p < probs.size(); p++) {
				double num = 0, den = 0;
				for (UINT i = 0; i < data.size(); i++) {
					double pa = pow(data[i].probs[p], 2);
					num += (pa * data[i].b);
					den += pa;
				}
				nb.push_back(num / den);
			}

			for (UINT p = 0; p < probs.size(); p++) {
				double num = 0, den = 0;
				for (UINT i = 0; i < data.size(); i++) {
					double pa = pow(data[i].probs[p], 2);
					num += (pa * data[i].c);
					den += pa;
				}
				nc.push_back(num / den);
			}

			for (UINT p = 0; p < probs.size(); p++) {
				double num = 0, den = 0;
				for (UINT i = 0; i < data.size(); i++) {
					double pa = pow(data[i].probs[p], 2);
					num += (pa * data[i].d);
					den += pa;
				}
				nd.push_back(num / den);
			}

			for (UINT i = 0; i < centroids.size(); i++) {
				centroids[i].a = na[i];
				centroids[i].b = nb[i];
				centroids[i].c = nc[i];
				centroids[i].d = nd[i];
			}

			bool exit = false;
			for (UINT i = 0; i < centroids.size(); i++) {
				if (abs(centroids[i].a - prev[i].a) < .0001
					&& abs(centroids[i].b - prev[i].b) < .0001
					&& abs(centroids[i].c - prev[i].c) < .0001
					&& abs(centroids[i].d - prev[i].d) < .0001) {
					exit = true;
					break;
				}
			}
			if (!exit)
				continue;

			for (UINT i = 0; i < data.size(); i++) {
				int bi = -1;
				double b = -100;
				for (UINT j = 0; j < data[i].probs.size(); j++) {
					if (data[i].probs[j] > b) {
						b = data[i].probs[j];
						bi = j;
					}
				}
				centroids[bi].pts.push_back(data[i]);
			}

			//WCSSE	
			double WCSSE = 0;
			for (UINT c = 0; c < centroids.size(); c++) {
				for (UINT d = 0; d < centroids[c].pts.size(); d++)
					WCSSE += pow(get_dist(centroids[c], centroids[c].pts[d]), 2);
			}
			WCSSE = sqrt(WCSSE);
			if (WCSSEs)
				WCSSEs->push_back(WCSSE);

			if (WCSSE < *BEST_WCSSE) {
				*BEST_WCSSE = WCSSE;
				solution = centroids;
			}

			if (exit)
				break;
		}
	}

	return solution;
}

vector<dat> column_shuffle(vector<dat>& data) {
	vector<double> a;
	vector<double> b;
	vector<double> c;
	vector<double> d;
	for (UINT i = 0; i < data.size(); i++) {
		a.push_back(data[i].a);
		b.push_back(data[i].b);
		c.push_back(data[i].c);
		d.push_back(data[i].d);
	}

	for (int i = 0; i < 10; i++)
		random_shuffle(a.begin(), a.end());
	
	for (int i = 0; i < 10; i++)
		random_shuffle(b.begin(), b.end());
	
	for (int i = 0; i < 10; i++)
		random_shuffle(c.begin(), c.end());
	
	for (int i = 0; i < 10; i++)
		random_shuffle(d.begin(), d.end());

	vector<dat> sdata(data.size());
	for (UINT i = 0; i < sdata.size(); i++) {
		sdata[i].a = a[i];
		sdata[i].b = b[i];
		sdata[i].c = c[i];
		sdata[i].d = d[i];
	}

	return sdata;
}

int main() {
	srand(time(NULL));

	vector<dat> data = {
		{3, 6.6, 4.4, 1.4},
		{3.2, 7, 4.7, 1.4},
		{2.9, 4.4, 1.4, 0.2},
		{3.4, 5, 1.6, 0.4},
		{2.8, 6.2, 4.8, 1.8},
		{2.2, 6, 5, 1.5},
		{3.6, 5, 1.4, 0.2},
		{2, 5, 3.5, 1},
		{3, 4.4, 1.3, 0.2},
		{2.9, 6.1, 4.7, 1.4},
		{2.9, 5.7, 4.2, 1.3},
		{3.2, 4.6, 1.4, 0.2},
		{3.5, 5.1, 1.4, 0.2},
		{3.6, 7.2, 6.1, 2.5},
		{3, 6.7, 5, 1.7},
		{2.8, 6.8, 4.8, 1.4},
		{3.9, 5.4, 1.7, 0.4},
		{3.4, 5, 1.5, 0.2},
		{2.9, 6.4, 4.3, 1.3},
		{2.3, 5, 3.3, 1},
		{2.9, 6.3, 5.6, 1.8},
		{3.2, 4.4, 1.3, 0.2},
		{2.8, 5.8, 5.1, 2.4},
		{2.5, 5.5, 4, 1.3},
		{3.5, 5, 1.6, 0.6},
		{2.7, 6, 5.1, 1.6},
		{2.3, 6.3, 4.4, 1.3},
		{2.5, 6.3, 4.9, 1.5},
		{2.8, 6.4, 5.6, 2.1},
		{2.9, 6.6, 4.6, 1.3},
		{2.7, 6.3, 4.9, 1.8},
		{3, 4.8, 1.4, 0.3},
		{3.4, 5.2, 1.4, 0.2},
		{3.5, 5.5, 1.3, 0.2},
		{4, 5.8, 1.2, 0.2},
		{4.4, 5.7, 1.5, 0.4},
		{3.7, 5.4, 1.5, 0.2},
		{3.7, 5.3, 1.5, 0.2},
		{3, 5.9, 5.1, 1.8},
		{3.1, 6.9, 5.4, 2.1},
		{3.4, 5.4, 1.5, 0.4},
		{2.7, 5.8, 4.1, 1},
		{3, 6.5, 5.8, 2.2},
		{3.4, 6.3, 5.6, 2.4},
		{2.8, 6.1, 4, 1.3},
		{3.5, 5.1, 1.4, 0.3},
		{2.6, 5.8, 4, 1.2},
		{3.2, 6.4, 5.3, 2.3},
		{3.3, 6.7, 5.7, 2.5},
		{2.5, 4.9, 4.5, 1.7},
		{2.6, 5.5, 4.4, 1.2},
		{3.1, 6.7, 4.7, 1.5},
		{3.2, 4.7, 1.3, 0.2},
		{3, 6.7, 5.2, 2.3},
		{3.5, 5, 1.3, 0.3},
		{3, 6.8, 5.5, 2.1},
		{3.1, 4.9, 1.5, 0.1},
		{2.8, 6.4, 5.6, 2.2},
		{3, 4.3, 1.1, 0.1},
		{3.6, 4.9, 1.4, 0.1},
		{2.8, 6.5, 4.6, 1.5},
		{2.7, 5.8, 5.1, 1.9},
		{2.8, 7.7, 6.7, 2},
		{3, 6.5, 5.2, 2},
		{3, 7.2, 5.8, 1.6},
		{3.2, 5.9, 4.8, 1.8},
		{3.2, 6.5, 5.1, 2},
		{3.1, 6.4, 5.5, 1.8},
		{3.4, 5.1, 1.5, 0.2},
		{2.4, 5.5, 3.7, 1},
		{3, 6, 4.8, 1.8},
		{3.4, 4.8, 1.9, 0.2},
		{3, 6.1, 4.6, 1.4},
		{2.3, 4.5, 1.3, 0.3},
		{3.3, 5, 1.4, 0.2},
		{2.8, 5.7, 4.1, 1.3},
		{3.1, 6.7, 4.4, 1.4},
		{2.3, 5.5, 4, 1.3},
		{4.2, 5.5, 1.4, 0.2},
		{2.6, 5.7, 3.5, 1},
		{3.4, 4.6, 1.4, 0.3},
		{3, 5.9, 4.2, 1.5},
		{2.9, 5.6, 3.6, 1.3},
		{3.7, 5.1, 1.5, 0.4},
		{3.8, 7.9, 6.4, 2},
		{3.2, 6.9, 5.7, 2.3},
		{2.7, 5.8, 3.9, 1.2},
		{2.7, 5.2, 3.9, 1.4},
		{3.3, 6.3, 6, 2.5},
		{2.5, 6.7, 5.8, 1.8},
		{2.8, 6.1, 4.7, 1.2},
		{2.6, 6.1, 5.6, 1.4},
		{3.2, 6.8, 5.9, 2.3},
		{3, 4.9, 1.4, 0.2},
		{3.8, 5.7, 1.7, 0.3},
		{3, 5, 1.6, 0.2},
		{3.4, 5.4, 1.7, 0.2},
		{3.3, 5.1, 1.7, 0.5},
		{3.1, 6.9, 5.1, 2.3},
		{2.8, 6.3, 5.1, 1.5},
		{3, 5.6, 4.1, 1.3},
		{3, 4.8, 1.4, 0.1},
		{2.2, 6, 4, 1},
		{2.9, 6.2, 4.3, 1.3},
		{3.4, 6.2, 5.4, 2.3},
		{2.7, 5.8, 5.1, 1.9},
		{3.1, 4.6, 1.5, 0.2},
		{3.6, 4.6, 1, 0.2},
		{3.9, 5.4, 1.3, 0.4},
		{3.3, 6.3, 4.7, 1.6},
		{3, 7.1, 5.9, 2.1},
		{2.5, 5.1, 3, 1.1},
		{4.1, 5.2, 1.5, 0.1},
		{3.2, 6.4, 4.5, 1.5},
		{2.9, 6, 4.5, 1.5},
		{2.5, 5.6, 3.9, 1.1},
		{3.1, 4.9, 1.5, 0.2},
		{2.7, 6.4, 5.3, 1.9},
		{3.2, 5, 1.2, 0.2},
		{3.4, 4.8, 1.6, 0.2}
	};

	/*ofstream fWCSSE;
	fWCSSE.open("WCSSE.txt");
	for (int k = 1; k <= 10; k++) {
		double WCSSE;
		vector<cent> solution = kmeans(data, k, 100, &WCSSE);

		fWCSSE << WCSSE << '\n';

		ofstream file;
		file.open("k_" + int_to_str(k) + ".txt");
		for (int i = 0; i < solution.size(); i++) {
			file << "centroid: "; 
			file << solution[i].a << ", "
				<< solution[i].b << ", " 
				<< solution[i].c << ", " 
				<< solution[i].d << '\n';

			for (int j = 0; j < solution[i].pts.size(); j++) {
				file << solution[i].pts[j].a << ", " 
					<< solution[i].pts[j].b << ", " 
					<< solution[i].pts[j].c << ", " 
					<< solution[i].pts[j].d << '\n';
			}
			file << '\n';
		}
		file.close();
	}
	fWCSSE.close();*/

	/*vector<vector<double>> shuffle_trials;
	for (int i = 0; i < 25; i++) {
		vector<dat> sdata = column_shuffle(data);
		double WCSSE;
		vector<double> trial;
		for (int j = 1; j <= 10; j++) {
			kmeans(sdata, j, 100, &WCSSE);
			trial.push_back(WCSSE);
		}
		shuffle_trials.push_back(trial);
	}

	ofstream shuffled;
	shuffled.open("shuffled.txt");
	for (UINT j = 0; j < shuffle_trials[0].size(); j++) {
		for (UINT i = 0; i < shuffle_trials.size(); i++) {
			shuffled << shuffle_trials[i][j] << "\t";
		}
		shuffled << '\n';
	}
	shuffled.close();*/

	/*double WCSSE;
	vector<double> WCSSEs;
	vector<cent> best_solution = kmeans(data, 3, 25, &WCSSE, &WCSSEs);

	ofstream WC;
	WC.open("WCSSEs.txt");
	sort(WCSSEs.begin(), WCSSEs.end());
	for (UINT i = 0; i < WCSSEs.size(); i++)
		WC << WCSSEs[i] << '\n';
	WC.close();
	
	ofstream c1;
	c1.open("cluster1.txt");
	c1 << best_solution[0].a << "\t" << best_solution[0].b << "\t" << best_solution[0].c << "\t" << best_solution[0].d << "\n\n";
	for (UINT i = 0; i < best_solution[0].pts.size(); i++) {
		c1 << best_solution[0].pts[i].a << "\t" << best_solution[0].pts[i].b << "\t" << best_solution[0].pts[i].c << "\t" << best_solution[0].pts[i].d << '\n';
	}
	c1.close();

	ofstream c2;
	c2.open("cluster2.txt");
	c2 << best_solution[1].a << "\t" << best_solution[1].b << "\t" << best_solution[1].c << "\t" << best_solution[1].d << "\n\n";
	for (UINT i = 0; i < best_solution[1].pts.size(); i++) {
		c2 << best_solution[1].pts[i].a << "\t" << best_solution[1].pts[i].b << "\t" << best_solution[1].pts[i].c << "\t" << best_solution[1].pts[i].d << '\n';
	}
	c2.close();

	ofstream c3;
	c3.open("cluster3.txt");
	c3 << best_solution[2].a << "\t" << best_solution[2].b << "\t" << best_solution[2].c << "\t" << best_solution[2].d << "\n\n";
	for (UINT i = 0; i < best_solution[2].pts.size(); i++) {
		c3 << best_solution[2].pts[i].a << "\t" << best_solution[2].pts[i].b << "\t" << best_solution[2].pts[i].c << "\t" << best_solution[2].pts[i].d << '\n';
	}
	c3.close();	*/

	vector<dat> data2 = {
		{2.3, 4.5, 1.3, 0.3, 'A'},
		{2.9, 4.4, 1.4, 0.2, 'A'},
		{3, 4.3, 1.1, 0.1, 'A'},
		{3, 4.4, 1.3, 0.2, 'A'},
		{3, 4.8, 1.4, 0.3, 'A'},
		{3, 4.8, 1.4, 0.1, 'A'},
		{3, 4.9, 1.4, 0.2, 'A'},
		{3, 5, 1.6, 0.2, 'A'},
		{3.1, 4.6, 1.5, 0.2, 'A'},
		{3.1, 4.9, 1.5, 0.1, 'A'},
		{3.1, 4.9, 1.5, 0.2, 'A'},
		{3.2, 4.4, 1.3, 0.2, 'A'},
		{3.2, 4.6, 1.4, 0.2, 'A'},
		{3.2, 4.7, 1.3, 0.2, 'A'},
		{3.2, 5, 1.2, 0.2, 'A'},
		{3.3, 5, 1.4, 0.2, 'A'},
		{3.3, 5.1, 1.7, 0.5, 'A'},
		{3.4, 4.6, 1.4, 0.3, 'A'},
		{3.4, 4.8, 1.9, 0.2, 'A'},
		{3.4, 4.8, 1.6, 0.2, 'A'},
		{3.4, 5, 1.6, 0.4, 'A'},
		{3.4, 5, 1.5, 0.2, 'A'},
		{3.4, 5.1, 1.5, 0.2, 'A'},
		{3.4, 5.2, 1.4, 0.2, 'A'},
		{3.4, 5.4, 1.5, 0.4, 'A'},
		{3.4, 5.4, 1.7, 0.2, 'A'},
		{3.5, 5, 1.6, 0.6, 'A'},
		{3.5, 5, 1.3, 0.3, 'A'},
		{3.5, 5.1, 1.4, 0.2, 'A'},
		{3.5, 5.1, 1.4, 0.3, 'A'},
		{3.5, 5.5, 1.3, 0.2, 'A'},
		{3.6, 4.6, 1, 0.2, 'A'},
		{3.6, 4.9, 1.4, 0.1, 'A'},
		{3.6, 5, 1.4, 0.2, 'A'},
		{3.7, 5.1, 1.5, 0.4, 'A'},
		{3.7, 5.3, 1.5, 0.2, 'A'},
		{3.7, 5.4, 1.5, 0.2, 'A'},
		{3.8, 5.7, 1.7, 0.3, 'A'},
		{3.9, 5.4, 1.7, 0.4, 'A'},
		{3.9, 5.4, 1.3, 0.4, 'A'},
		{4, 5.8, 1.2, 0.2, 'A'},
		{4.1, 5.2, 1.5, 0.1, 'A'},
		{4.2, 5.5, 1.4, 0.2, 'A'},
		{4.4, 5.7, 1.5, 0.4, 'A'},
		{2, 5, 3.5, 1, 'B'},
		{2.2, 6, 4, 1, 'B'},
		{2.3, 5, 3.3, 1, 'B'},
		{2.3, 5.5, 4, 1.3, 'B'},
		{2.3, 6.3, 4.4, 1.3, 'B'},
		{2.4, 5.5, 3.7, 1, 'B'},
		{2.5, 5.1, 3, 1.1, 'B'},
		{2.5, 5.5, 4, 1.3, 'B'},
		{2.5, 5.6, 3.9, 1.1, 'B'},
		{2.5, 6.3, 4.9, 1.5, 'B'},
		{2.6, 5.5, 4.4, 1.2, 'B'},
		{2.6, 5.7, 3.5, 1, 'B'},
		{2.6, 5.8, 4, 1.2, 'B'},
		{2.7, 5.2, 3.9, 1.4, 'B'},
		{2.7, 5.8, 4.1, 1, 'B'},
		{2.7, 5.8, 3.9, 1.2, 'B'},
		{2.7, 6, 5.1, 1.6, 'B'},
		{2.8, 5.7, 4.1, 1.3, 'B'},
		{2.8, 6.1, 4, 1.3, 'B'},
		{2.8, 6.1, 4.7, 1.2, 'B'},
		{2.8, 6.5, 4.6, 1.5, 'B'},
		{2.8, 6.8, 4.8, 1.4, 'B'},
		{2.9, 5.6, 3.6, 1.3, 'B'},
		{2.9, 5.7, 4.2, 1.3, 'B'},
		{2.9, 6, 4.5, 1.5, 'B'},
		{2.9, 6.1, 4.7, 1.4, 'B'},
		{2.9, 6.2, 4.3, 1.3, 'B'},
		{2.9, 6.4, 4.3, 1.3, 'B'},
		{2.9, 6.6, 4.6, 1.3, 'B'},
		{3, 5.6, 4.1, 1.3, 'B'},
		{3, 5.9, 4.2, 1.5, 'B'},
		{3, 6.1, 4.6, 1.4, 'B'},
		{3, 6.6, 4.4, 1.4, 'B'},
		{3, 6.7, 5, 1.7, 'B'},
		{3.1, 6.7, 4.7, 1.5, 'B'},
		{3.1, 6.7, 4.4, 1.4, 'B'},
		{3.2, 5.9, 4.8, 1.8, 'B'},
		{3.2, 6.4, 4.5, 1.5, 'B'},
		{3.2, 7, 4.7, 1.4, 'B'},
		{3.3, 6.3, 4.7, 1.6, 'B'},
		{2.2, 6, 5, 1.5, 'C'},
		{2.5, 4.9, 4.5, 1.7, 'C'},
		{2.5, 6.7, 5.8, 1.8, 'C'},
		{2.6, 6.1, 5.6, 1.4, 'C'},
		{2.7, 5.8, 5.1, 1.9, 'C'},
		{2.7, 5.8, 5.1, 1.9, 'C'},
		{2.7, 6.3, 4.9, 1.8, 'C'},
		{2.7, 6.4, 5.3, 1.9, 'C'},
		{2.8, 5.8, 5.1, 2.4, 'C'},
		{2.8, 6.2, 4.8, 1.8, 'C'},
		{2.8, 6.3, 5.1, 1.5, 'C'},
		{2.8, 6.4, 5.6, 2.1, 'C'},
		{2.8, 6.4, 5.6, 2.2, 'C'},
		{2.8, 7.7, 6.7, 2, 'C'},
		{2.9, 6.3, 5.6, 1.8, 'C'},
		{3, 5.9, 5.1, 1.8, 'C'},
		{3, 6, 4.8, 1.8, 'C'},
		{3, 6.5, 5.8, 2.2, 'C'},
		{3, 6.5, 5.2, 2, 'C'},
		{3, 6.7, 5.2, 2.3, 'C'},
		{3, 6.8, 5.5, 2.1, 'C'},
		{3, 7.1, 5.9, 2.1, 'C'},
		{3, 7.2, 5.8, 1.6, 'C'},
		{3.1, 6.4, 5.5, 1.8, 'C'},
		{3.1, 6.9, 5.4, 2.1, 'C'},
		{3.1, 6.9, 5.1, 2.3, 'C'},
		{3.2, 6.4, 5.3, 2.3, 'C'},
		{3.2, 6.5, 5.1, 2, 'C'},
		{3.2, 6.8, 5.9, 2.3, 'C'},
		{3.2, 6.9, 5.7, 2.3, 'C'},
		{3.3, 6.3, 6, 2.5, 'C'},
		{3.3, 6.7, 5.7, 2.5, 'C'},
		{3.4, 6.2, 5.4, 2.3, 'C'},
		{3.4, 6.3, 5.6, 2.4, 'C'},
		{3.6, 7.2, 6.1, 2.5, 'C'},
		{3.8, 7.9, 6.4, 2, 'C'}
	};

	vector<cent> centroids = {{3.4011, 5.0025, 1.45528, 0.250249},
		{3.02524, 6.57338, 5.4333, 2.01532},
		{2.72662, 5.87434, 4.23475, 1.3239}
	};

	for (UINT d = 0; d < data2.size(); d++) {
		double min_dist = 1.7976931348623158e+308;
		int min_index = -1;
		for (UINT c = 0; c < centroids.size(); c++) {
			double dist = get_dist(centroids[c], data2[d]);
			if (dist <= min_dist) {
				min_dist = dist;
				min_index = c;
			}
		}
		centroids[min_index].pts.push_back(data2[d]);
	}

	for (UINT c = 0; c < centroids.size(); c++) {
		cout << c + 1 << " ";
		int x, y, z;
		x = y = z = 0;
		for (UINT p = 0; p < centroids[c].pts.size(); p++) {
			if (centroids[c].pts[p].variable == 'A')
				x++;
			if (centroids[c].pts[p].variable == 'B')
				y++;
			if (centroids[c].pts[p].variable == 'C')
				z++;
		}
		cout << "A: " << x << " B: " << y << " C: " << z << '\n';
	}
	_getch();

	ofstream fuz;
	fuz.open("fuzzy.txt");
	
	vector<double> WCSSEs;
	for (UINT i = 0; i < 25; i++) {
		double WCSSE;
		
		vector<cent> best_solution = fuzzy(data, 3, &WCSSE, &WCSSEs);

		ofstream c1;
		c1.open("fuzzycluster1.txt");
		c1 << best_solution[0].a << "\t" << best_solution[0].b << "\t" << best_solution[0].c << "\t" << best_solution[0].d << "\n\n";
		for (UINT i = 0; i < best_solution[0].pts.size(); i++) {
			c1 << best_solution[0].pts[i].a << "\t" << best_solution[0].pts[i].b << "\t" << best_solution[0].pts[i].c << "\t" << best_solution[0].pts[i].d << '\n';
		}
		c1.close();

		ofstream c2;
		c2.open("fuzzycluster2.txt");
		c2 << best_solution[1].a << "\t" << best_solution[1].b << "\t" << best_solution[1].c << "\t" << best_solution[1].d << "\n\n";
		for (UINT i = 0; i < best_solution[1].pts.size(); i++) {
			c2 << best_solution[1].pts[i].a << "\t" << best_solution[1].pts[i].b << "\t" << best_solution[1].pts[i].c << "\t" << best_solution[1].pts[i].d << '\n';
		}
		c2.close();

		ofstream c3;
		c3.open("fuzzycluster3.txt");
		c3 << best_solution[2].a << "\t" << best_solution[2].b << "\t" << best_solution[2].c << "\t" << best_solution[2].d << "\n\n";
		for (UINT i = 0; i < best_solution[2].pts.size(); i++) {
			c3 << best_solution[2].pts[i].a << "\t" << best_solution[2].pts[i].b << "\t" << best_solution[2].pts[i].c << "\t" << best_solution[2].pts[i].d << '\n';
		}
		c3.close();	

		sort(best_solution.begin(), best_solution.end(), [](cent& l, cent& r) { return l.c < r.c; });
		for (UINT j = 0; j < best_solution.size(); j++) {
			fuz << best_solution[j].a << "\t" << best_solution[j].b << "\t" << best_solution[j].c << "\t" << best_solution[j].d << "\t\t";
		}
		fuz << '\n';
	}
	fuz.close();

	ofstream fwc;
	fwc.open("fuzwc.txt");
	sort(WCSSEs.begin(), WCSSEs.end());
	for (UINT j = 0; j < WCSSEs.size(); j++) {
		fwc << WCSSEs[j] << '\n';
	}
	fwc.close();

	return 0;
}

