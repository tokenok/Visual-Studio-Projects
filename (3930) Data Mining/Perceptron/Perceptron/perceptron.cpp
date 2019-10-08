#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <conio.h>

#include "C://CPPLibs/common//common.h"

using namespace std;

enum classes {
	CLS_VIRGINICA,
	CLS_VERSICOLOR,
	CLS_SETOSA
};

struct iris {
	double sepal_w, sepal_l, petal_l, petal_w;
	int cls;

	double b;
};

vector<iris> data = {
	{3, 5.9, 5.1, 1.8, CLS_VIRGINICA, 1},
	{2.6, 5.5, 4.4, 1.2, CLS_VERSICOLOR, 1},
	{2.8, 6.2, 4.8, 1.8, CLS_VIRGINICA, 1},
	{3.2, 4.7, 1.3, 0.2, CLS_SETOSA, 1},
	{2.2, 6, 4, 1, CLS_VERSICOLOR, 1},
	{3.3, 6.7, 5.7, 2.1, CLS_VIRGINICA, 1},
	{3.3, 6.7, 5.7, 2.5, CLS_VIRGINICA, 1},
	{3.4, 5.1, 1.5, 0.2, CLS_SETOSA, 1},
	{3.5, 5, 1.6, 0.6, CLS_SETOSA, 1},
	{3.4, 4.8, 1.9, 0.2, CLS_SETOSA, 1},
	{3.8, 5.1, 1.5, 0.3, CLS_SETOSA, 1},
	{3.3, 5, 1.4, 0.2, CLS_SETOSA, 1},
	{3.7, 5.3, 1.5, 0.2, CLS_SETOSA, 1},
	{3.1, 6.9, 4.9, 1.5, CLS_VERSICOLOR, 1},
	{2.4, 4.9, 3.3, 1, CLS_VERSICOLOR, 1},
	{3.1, 6.4, 5.5, 1.8, CLS_VIRGINICA, 1},
	{3.8, 5.7, 1.7, 0.3, CLS_SETOSA, 1},
	{3, 7.6, 6.6, 2.1, CLS_VIRGINICA, 1},
	{3.1, 6.9, 5.4, 2.1, CLS_VIRGINICA, 1},
	{4.1, 5.2, 1.5, 0.1, CLS_SETOSA, 1},
	{2.8, 6.8, 4.8, 1.4, CLS_VERSICOLOR, 1},
	{3.2, 4.6, 1.4, 0.2, CLS_SETOSA, 1},
	{2.3, 5, 3.3, 1, CLS_VERSICOLOR, 1},
	{3, 7.1, 5.9, 2.1, CLS_VIRGINICA, 1},
	{3.2, 7.2, 6, 1.8, CLS_VIRGINICA, 1},
	{2.8, 5.6, 4.9, 2, CLS_VIRGINICA, 1},
	{3.5, 5, 1.3, 0.3, CLS_SETOSA, 1},
	{3, 4.4, 1.3, 0.2, CLS_SETOSA, 1},
	{4.4, 5.7, 1.5, 0.4, CLS_SETOSA, 1},
	{3, 4.9, 1.4, 0.2, CLS_SETOSA, 1},
	{3, 6.6, 4.4, 1.4, CLS_VERSICOLOR, 1},
	{3, 5.6, 4.1, 1.3, CLS_VERSICOLOR, 1},
	{2.6, 7.7, 6.9, 2.3, CLS_VIRGINICA, 1},
	{3, 5.9, 4.2, 1.5, CLS_VERSICOLOR, 1},
	{2.8, 6.4, 5.6, 2.1, CLS_VIRGINICA, 1},
	{2.8, 7.7, 6.7, 2, CLS_VIRGINICA, 1},
	{2.8, 6.1, 4, 1.3, CLS_VERSICOLOR, 1},
	{3.8, 7.9, 6.4, 2, CLS_VIRGINICA, 1},
	{3.6, 4.9, 1.4, 0.1, CLS_SETOSA, 1},
	{3.2, 4.7, 1.6, 0.2, CLS_SETOSA, 1},
	{3.2, 6.4, 5.3, 2.3, CLS_VIRGINICA, 1},
	{3.4, 6.2, 5.4, 2.3, CLS_VIRGINICA, 1},
	{2.9, 6, 4.5, 1.5, CLS_VERSICOLOR, 1},
	{3.4, 5.4, 1.5, 0.4, CLS_SETOSA, 1},
	{3.5, 5.5, 1.3, 0.2, CLS_SETOSA, 1},
	{3.9, 5.4, 1.3, 0.4, CLS_SETOSA, 1},
	{3, 5.7, 4.2, 1.2, CLS_VERSICOLOR, 1},
	{2.8, 6.4, 5.6, 2.2, CLS_VIRGINICA, 1},
	{3.4, 5, 1.5, 0.2, CLS_SETOSA, 1},
	{2.5, 5.5, 4, 1.3, CLS_VERSICOLOR, 1},
	{3, 6.8, 5.5, 2.1, CLS_VIRGINICA, 1},
	{2.7, 6, 5.1, 1.6, CLS_VERSICOLOR, 1},
	{2.4, 5.5, 3.8, 1.1, CLS_VERSICOLOR, 1},
	{2.8, 6.5, 4.6, 1.5, CLS_VERSICOLOR, 1},
	{3.1, 4.8, 1.6, 0.2, CLS_SETOSA, 1},
	{4, 5.8, 1.2, 0.2, CLS_SETOSA, 1},
	{3, 6.7, 5.2, 2.3, CLS_VIRGINICA, 1},
	{2.5, 5.7, 5, 2, CLS_VIRGINICA, 1},
	{2.9, 6.3, 5.6, 1.8, CLS_VIRGINICA, 1},
	{3.2, 6.5, 5.1, 2, CLS_VIRGINICA, 1},
	{2.7, 5.8, 5.1, 1.9, CLS_VIRGINICA, 1},
	{2.9, 6.1, 4.7, 1.4, CLS_VERSICOLOR, 1},
	{3.1, 6.7, 4.7, 1.5, CLS_VERSICOLOR, 1},
	{2.7, 5.8, 4.1, 1, CLS_VERSICOLOR, 1},
	{3, 6.5, 5.2, 2, CLS_VIRGINICA, 1},
	{3, 6.7, 5, 1.7, CLS_VERSICOLOR, 1},
	{3, 4.8, 1.4, 0.1, CLS_SETOSA, 1},
	{3.2, 6.9, 5.7, 2.3, CLS_VIRGINICA, 1},
	{3.6, 4.6, 1, 0.2, CLS_SETOSA, 1},
	{2.5, 6.7, 5.8, 1.8, CLS_VIRGINICA, 1},
	{3, 5.6, 4.5, 1.5, CLS_VERSICOLOR, 1},
	{3.8, 5.1, 1.9, 0.4, CLS_SETOSA, 1},
	{2.2, 6.2, 4.5, 1.5, CLS_VERSICOLOR, 1},
	{2.3, 6.3, 4.4, 1.3, CLS_VERSICOLOR, 1},
	{3, 6.1, 4.9, 1.8, CLS_VIRGINICA, 1},
	{2.9, 6.4, 4.3, 1.3, CLS_VERSICOLOR, 1},
	{2.7, 5.8, 5.1, 1.9, CLS_VIRGINICA, 1},
	{2.8, 5.8, 5.1, 2.4, CLS_VIRGINICA, 1},
	{2.8, 6.1, 4.7, 1.2, CLS_VERSICOLOR, 1},
	{3.2, 5, 1.2, 0.2, CLS_SETOSA, 1},
	{2.6, 6.1, 5.6, 1.4, CLS_VIRGINICA, 1},
	{3.1, 6.9, 5.1, 2.3, CLS_VIRGINICA, 1},
	{2.9, 7.3, 6.3, 1.8, CLS_VIRGINICA, 1},
	{3.7, 5.1, 1.5, 0.4, CLS_SETOSA, 1},
	{3, 6.5, 5.8, 2.2, CLS_VIRGINICA, 1},
	{2.8, 7.4, 6.1, 1.9, CLS_VIRGINICA, 1},
	{3.2, 6.8, 5.9, 2.3, CLS_VIRGINICA, 1},
	{3, 6, 4.8, 1.8, CLS_VIRGINICA, 1},
	{3.1, 4.9, 1.5, 0.1, CLS_SETOSA, 1},
	{3.8, 7.7, 6.7, 2.2, CLS_VIRGINICA, 1},
	{2.8, 6.3, 5.1, 1.5, CLS_VIRGINICA, 1},
	{3, 4.8, 1.4, 0.3, CLS_SETOSA, 1},
	{2.7, 5.6, 4.2, 1.3, CLS_VERSICOLOR, 1},
	{3.2, 7, 4.7, 1.4, CLS_VERSICOLOR, 1},
	{2.7, 5.8, 3.9, 1.2, CLS_VERSICOLOR, 1},
	{3.6, 7.2, 6.1, 2.5, CLS_VIRGINICA, 1},
	{3.2, 5.9, 4.8, 1.8, CLS_VERSICOLOR, 1},
	{2.5, 6.3, 5, 1.9, CLS_VIRGINICA, 1},
	{2.7, 6.4, 5.3, 1.9, CLS_VIRGINICA, 1},
	{3.2, 4.4, 1.3, 0.2, CLS_SETOSA, 1},
	{3.8, 5.1, 1.6, 0.2, CLS_SETOSA, 1},
	{3, 7.7, 6.1, 2.3, CLS_VIRGINICA, 1},
	{3.4, 5.2, 1.4, 0.2, CLS_SETOSA, 1},
	{2.6, 5.7, 3.5, 1, CLS_VERSICOLOR, 1},
	{2.8, 5.7, 4.1, 1.3, CLS_VERSICOLOR, 1},
	{3.6, 5, 1.4, 0.2, CLS_SETOSA, 1},
	{3.4, 5.4, 1.7, 0.2, CLS_SETOSA, 1},
	{3.5, 5.1, 1.4, 0.2, CLS_SETOSA, 1},
	{3, 6.5, 5.5, 1.8, CLS_VIRGINICA, 1},
	{2.4, 5.5, 3.7, 1, CLS_VERSICOLOR, 1},
	{3.5, 5.1, 1.4, 0.3, CLS_SETOSA, 1},
	{3.4, 6, 4.5, 1.6, CLS_VERSICOLOR, 1},
	{2.7, 6.3, 4.9, 1.8, CLS_VIRGINICA, 1},
	{2.3, 4.5, 1.3, 0.3, CLS_SETOSA, 1},
	{2.3, 5.5, 4, 1.3, CLS_VERSICOLOR, 1},
	{3, 7.2, 5.8, 1.6, CLS_VIRGINICA, 1},
	{2.2, 6, 5, 1.5, CLS_VIRGINICA, 1},
	{3.3, 6.3, 6, 2.5, CLS_VIRGINICA, 1},
	{3, 6.1, 4.6, 1.4, CLS_VERSICOLOR, 1},
	{3, 5, 1.6, 0.2, CLS_SETOSA, 1},
	{3.2, 6.4, 4.5, 1.5, CLS_VERSICOLOR, 1},
	{3.9, 5.4, 1.7, 0.4, CLS_SETOSA, 1},
	{3, 5.4, 4.5, 1.5, CLS_VERSICOLOR, 1},
	{2.9, 4.4, 1.4, 0.2, CLS_SETOSA, 1},
	{2.5, 6.3, 4.9, 1.5, CLS_VERSICOLOR, 1},
	{2.9, 5.7, 4.2, 1.3, CLS_VERSICOLOR, 1},
	{2.9, 6.2, 4.3, 1.3, CLS_VERSICOLOR, 1},
	{4.2, 5.5, 1.4, 0.2, CLS_SETOSA, 1},
	{3.1, 6.7, 4.4, 1.4, CLS_VERSICOLOR, 1},
	{3.3, 6.3, 4.7, 1.6, CLS_VERSICOLOR, 1},
	{3.4, 4.6, 1.4, 0.3, CLS_SETOSA, 1},
	{3.7, 5.4, 1.5, 0.2, CLS_SETOSA, 1},
	{2.5, 5.6, 3.9, 1.1, CLS_VERSICOLOR, 1},
	{3.3, 5.1, 1.7, 0.5, CLS_SETOSA, 1},
	{3, 4.3, 1.1, 0.1, CLS_SETOSA, 1},
	{3.1, 6.7, 5.6, 2.4, CLS_VIRGINICA, 1},
	{3.5, 5.2, 1.5, 0.2, CLS_SETOSA, 1},
	{3.4, 6.3, 5.6, 2.4, CLS_VIRGINICA, 1},
	{3.1, 4.9, 1.5, 0.2, CLS_SETOSA, 1},
	{3.4, 5, 1.6, 0.4, CLS_SETOSA, 1},
	{3.1, 4.6, 1.5, 0.2, CLS_SETOSA, 1},
	{2, 5, 3.5, 1, CLS_VERSICOLOR, 1},
	{2.7, 5.2, 3.9, 1.4, CLS_VERSICOLOR, 1},
	{2.5, 5.1, 3, 1.1, CLS_VERSICOLOR, 1},
	{2.9, 5.6, 3.6, 1.3, CLS_VERSICOLOR, 1},
	{3.4, 4.8, 1.6, 0.2, CLS_SETOSA, 1},
	{2.8, 5.7, 4.5, 1.3, CLS_VERSICOLOR, 1},
	{2.6, 5.8, 4, 1.2, CLS_VERSICOLOR, 1},
	{2.5, 4.9, 4.5, 1.7, CLS_VIRGINICA, 1},
	{2.9, 6.6, 4.6, 1.3, CLS_VERSICOLOR, 1}
};

void normalize() {
	//ofstream of;
	//of.open("normalized.txt");

	//get mins and maxs
	double swn = 100000, swx = 0, sln = 100000, slx = 0, pwn = 100000, pwx = 0, pln = 100000, plx = 0;
	for (int i = 0; i < data.size(); i++) {
		swn = data[i].sepal_w < swn ? data[i].sepal_w : swn;
		swx = data[i].sepal_w > swx ? data[i].sepal_w : swx;

		sln = data[i].sepal_l < sln ? data[i].sepal_l : sln;
		slx = data[i].sepal_l > slx ? data[i].sepal_l : slx;

		pwn = data[i].petal_w < pwn ? data[i].petal_w : pwn;
		pwx = data[i].petal_w > pwx ? data[i].petal_w : pwx; 
		
		pln = data[i].petal_l < pln ? data[i].petal_l : pln;
		plx = data[i].petal_l > plx ? data[i].petal_l : plx;
	}

	for (int i = 0; i < data.size(); i++) {
		data[i].sepal_w = ((data[i].sepal_w - swn) / (swx - swn));
		data[i].sepal_l = ((data[i].sepal_l - sln) / (slx - sln));
		data[i].petal_w = ((data[i].petal_w - pwn) / (pwx - pwn));
		data[i].petal_l = ((data[i].petal_l - pln) / (plx - pln));

	//	if (data[i].cls != CLS_SETOSA)
	//		of << data[i].sepal_w << '\t' << data[i].sepal_l << '\t' << data[i].petal_w << '\t' << data[i].petal_l <<  '\t' << (data[i].cls == CLS_VIRGINICA ? "virginica" : "versicolor") << '\n';
	}

	//of.close();
}

double calc_model(const iris& dat, const iris& weights) {
	return (dat.b * weights.b)
		+ (dat.petal_w * weights.petal_w)
		+ (dat.petal_l * weights.petal_l)
//		+ (dat.sepal_w * weights.sepal_w)
//		+ (dat.sepal_l * weights.sepal_l)
		;
}

int count_errors(int cls, const iris& weights) {
	int count = 0;
	for (int i = 0; i < data.size(); i++) {
		int c = data[i].cls == cls ? -1 : 1;

		double model = calc_model(data[i], weights);

		if ((c < 0 || model < 0) && (c >= 0 || model >= 0)) {
			count++;
		}
	}
	return count;
}

//returns number of misses
//out(result) -> perceptron coefficients
int get_perceptron(int cls, iris* result) {
	double learning_rate = .1;

	iris weights;

	weights.b =		  (double)rand() / RAND_MAX - 1;
	weights.sepal_w = (double)rand() / RAND_MAX - 1;
	weights.sepal_l = (double)rand() / RAND_MAX - 1;
	weights.petal_w = (double)rand() / RAND_MAX - 1;
	weights.petal_l = (double)rand() / RAND_MAX - 1;

	int minerrors = data.size() + 1;

	for (int epoch = 0; epoch < 2000; epoch++) {
		int errors = 0;

		for (int i = 0; i < data.size(); i++) {
			if (cls == CLS_SETOSA)
				continue;

			int c = data[i].cls == cls ? -1 : 1;

			double model = calc_model(data[i], weights);

			if ((c < 0 || model < 0) && (c >= 0 || model >= 0)) {
				weights.b		+= learning_rate * c * data[i].b;
				weights.sepal_w += learning_rate * c * data[i].sepal_w;
				weights.sepal_l += learning_rate * c * data[i].sepal_l;
				weights.petal_w += learning_rate * c * data[i].petal_w;
				weights.petal_l += learning_rate * c * data[i].petal_l;
			}

			errors = count_errors(cls, weights);

			if (errors <= minerrors) {
				minerrors = errors;

				result->b = weights.b;
				result->petal_l = weights.petal_l;
				result->petal_w = weights.petal_w;
				result->sepal_l = weights.sepal_l;
				result->sepal_w = weights.sepal_w;

				if (errors == 0)
					break;
			}

		}	
		
		shuffle(data.begin(), data.end(), std::default_random_engine(chrono::system_clock::now().time_since_epoch().count()));
	}

	return minerrors;
}

int main() {
	srand(clock());

	normalize();

	ofstream out;
	out.open("outfile.txt");

	iris res1 = {0}, res2 = {0}, res3 = {0}, res4 = {0}, res5 = {0};
	iris avg;
	int miss = 0;

	/*miss = get_perceptron(CLS_SETOSA, &res1);				out << miss << '\t' << res1.sepal_w << '\t' << res1.sepal_l << '\t' << res1.petal_w << '\t' << res1.petal_l << '\t' << res1.b << '\n';
	miss = get_perceptron(CLS_SETOSA, &res2);				out << miss << '\t' << res2.sepal_w << '\t' << res2.sepal_l << '\t' << res2.petal_w << '\t' << res2.petal_l << '\t' << res2.b << '\n';
	miss = get_perceptron(CLS_SETOSA, &res3);				out << miss << '\t' << res3.sepal_w << '\t' << res3.sepal_l << '\t' << res3.petal_w << '\t' << res3.petal_l << '\t' << res3.b << '\n';
	miss = get_perceptron(CLS_SETOSA, &res4);				out << miss << '\t' << res4.sepal_w << '\t' << res4.sepal_l << '\t' << res4.petal_w << '\t' << res4.petal_l << '\t' << res4.b << '\n';
	miss = get_perceptron(CLS_SETOSA, &res5);				out << miss << '\t' << res5.sepal_w << '\t' << res5.sepal_l << '\t' << res5.petal_w << '\t' << res5.petal_l << '\t' << res5.b << '\n';

	avg.petal_l = (res1.petal_l + res2.petal_l + res3.petal_l + res4.petal_l + res5.petal_l) / 5;
	avg.petal_w = (res1.petal_w + res2.petal_w + res3.petal_w + res4.petal_w + res5.petal_w) / 5;
	avg.sepal_l = (res1.sepal_l + res2.sepal_l + res3.sepal_l + res4.sepal_l + res5.sepal_l) / 5;
	avg.sepal_w = (res1.sepal_w + res2.sepal_w + res3.sepal_w + res4.sepal_w + res5.sepal_w) / 5;
	avg.b = (res1.b + res2.b + res3.b + res4.b + res5.b) / 5;
	miss = count_errors(CLS_SETOSA, avg);
	out << miss << '\t' << res1.sepal_w << '\t' << avg.sepal_l << '\t' << avg.petal_w << '\t' << avg.petal_l << '\t' << avg.b << '\n';*/

	//cout << "\n";
	/*miss = get_perceptron(CLS_VIRGINICA, &res1);				out << miss << '\t' << res1.sepal_w << '\t' << res1.sepal_l << '\t' << res1.petal_w << '\t' << res1.petal_l << '\t' << res1.b << '\n';
	miss = get_perceptron(CLS_VIRGINICA, &res2);				out << miss << '\t' << res2.sepal_w << '\t' << res2.sepal_l << '\t' << res2.petal_w << '\t' << res2.petal_l << '\t' << res2.b << '\n';
	miss = get_perceptron(CLS_VIRGINICA, &res3);				out << miss << '\t' << res3.sepal_w << '\t' << res3.sepal_l << '\t' << res3.petal_w << '\t' << res3.petal_l << '\t' << res3.b << '\n';
	miss = get_perceptron(CLS_VIRGINICA, &res4);				out << miss << '\t' << res4.sepal_w << '\t' << res4.sepal_l << '\t' << res4.petal_w << '\t' << res4.petal_l << '\t' << res4.b << '\n';
	miss = get_perceptron(CLS_VIRGINICA, &res5);				out << miss << '\t' << res5.sepal_w << '\t' << res5.sepal_l << '\t' << res5.petal_w << '\t' << res5.petal_l << '\t' << res5.b << '\n';

	avg.petal_l = (res1.petal_l + res2.petal_l + res3.petal_l + res4.petal_l + res5.petal_l) / 5;
	avg.petal_w = (res1.petal_w + res2.petal_w + res3.petal_w + res4.petal_w + res5.petal_w) / 5;
	avg.sepal_l = (res1.sepal_l + res2.sepal_l + res3.sepal_l + res4.sepal_l + res5.sepal_l) / 5;
	avg.sepal_w = (res1.sepal_w + res2.sepal_w + res3.sepal_w + res4.sepal_w + res5.sepal_w) / 5;
	avg.b = (res1.b + res2.b + res3.b + res4.b + res5.b) / 5;
	miss = count_errors(CLS_VIRGINICA, avg);
	out << miss << '\t' << res1.sepal_w << '\t' << avg.sepal_l << '\t' << avg.petal_w << '\t' << avg.petal_l << '\t' << avg.b << '\n';*/

	out.close();
	
	return 0;
}

