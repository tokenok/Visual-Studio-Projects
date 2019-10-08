#include <iostream>
#include <utility>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>

using namespace std;

#define TPI 6.2831853071794
#define INTEGRAL_ITERATIONS 1000000

int nharmonics = 1500;

struct dpair { double x, y; };

string int_to_str(int n) {
	stringstream s;
	s << n;
	return s.str();
}

enum funcmode {	MODE_NORMAL, MODE_SIN, MODE_COS };
 
#define MODECHECK * (mode == MODE_SIN ? sin(TPI * n * t) : mode == MODE_COS ? cos(TPI * n * t) : 1)
#define FPARAMS double t, double n = 0, int mode = MODE_NORMAL

double f1(FPARAMS) {
	return t MODECHECK;
}
double f2(FPARAMS) {
	return (-t / 4 + 1) MODECHECK;
}
double f3(FPARAMS) {
	static string s = "01100010";
	return (s[(int)((t > 1.0 ? .9999 : t < 0 ? 0 : t) * 8)] - 48) MODECHECK;
}

#undef FPARAMS
#undef MODECHECK

double integral(double(*func)(double, double, int), double n = 0, int mode = MODE_NORMAL, double sx = 0, double ex = 1, int r = INTEGRAL_ITERATIONS) {
	double w = (ex - sx) / r;
	double sum = 0;
	for (int x = 0; x < r; x++)
		sum += func(sx + x * w, n, mode) * w;
	return sum;
}

vector<dpair> calc_fourier_series(double(*func)(double, double, int), vector<dpair> *coeffs, double *c) {
	*c = 2 * integral(func);
	for (int n = 1; n <= nharmonics; n++) {
		coeffs->push_back({2 * integral(func, n, MODE_SIN), 2 * integral(func, n, MODE_COS)});
	}

	vector<dpair> ret;
	for (double t = 0; t <= 1.01; t += .01) {
		double asum = 0, bsum = 0;
		for (unsigned int n = 1; n < coeffs->size() + 1; n++) {
			asum += coeffs->at(n - 1).x * sin(TPI * (double)n * t);
			bsum += coeffs->at(n - 1).y * cos(TPI * (double)n * t);
		}
		ret.push_back({t, (.5 * *c) + asum + bsum});
	}

	return ret;
}

int main() {
#define F(f) vector<dpair> f##coeffs; double f##c; funcs.push_back(make_pair(calc_fourier_series(f, &f##coeffs, &f##c), f));
	vector<pair<vector<dpair>, double(*)(double, double, int)>> funcs;
	F(f1) F(f2) F(f3)
#undef F

	ofstream file;
	file.open(int_to_str(nharmonics) + " harmonics.txt");	
	unsigned int i = 0; 
	for (unsigned int j = 0; j < funcs[i].first.size();) {
		file << funcs[i].first[j].x << '\t' << funcs[i].first[j].y << '\t' << funcs[i].second(funcs[i].first[j].x, 0, MODE_NORMAL);
		if (++i > funcs.size() - 1) {
			i = 0;
			j++;
			file << '\n';
		}
		else file << '\t';
	}	
	file.close();	

	file.open(int_to_str(nharmonics) + " coefficients.txt");
	file << f1c << '\n';
	for (int i = 0; i < f1coeffs.size(); i++) 
		file << i + 1 << '\t' << f1coeffs[i].x << '\t' << f1coeffs[i].y << '\n';
	file << '\n';

	file << f2c << '\n';
	for (int i = 0; i < f2coeffs.size(); i++) 
		file << i + 1 << '\t' << f2coeffs[i].x << '\t' << f2coeffs[i].y << '\n';
	file << '\n';

	file << f3c << '\n';
	for (int i = 0; i < f3coeffs.size(); i++) 
		file << i + 1 << '\t' << f3coeffs[i].x << '\t' << f3coeffs[i].y << '\n';
	file << '\n';

	file.close();

	return 0;
}