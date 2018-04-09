#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <conio.h>

using namespace std;

#define E (2.7182818284590452353602874713526624977572470937L)
#define PI (3.1415926535897932384626433832795028841971693994L)

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MAX3(a,b,c) MAX(MAX(a,b),c)

enum {
	SETOSA, VERSICOLOR, VIRGINICA
};

struct iris {
	double sepal_width, sepal_length, petal_length, petal_width;
	int cls;
};

vector<iris> iris_data = {
	{2.3, 4.5, 1.3, 0.3, SETOSA},
	{2.9, 4.4, 1.4, 0.2, SETOSA},
	{3, 4.8, 1.4, 0.1, SETOSA},
	{3, 5, 1.6, 0.2, SETOSA},
	{3, 4.9, 1.4, 0.2, SETOSA},
	{3, 4.4, 1.3, 0.2, SETOSA},
	{3.1, 4.8, 1.6, 0.2, SETOSA},
	{3.1, 4.9, 1.5, 0.1, SETOSA},
	{3.1, 4.9, 1.5, 0.2, SETOSA},
	{3.2, 4.6, 1.4, 0.2, SETOSA},
	{3.2, 4.4, 1.3, 0.2, SETOSA},
	{3.2, 4.7, 1.3, 0.2, SETOSA},
	{3.2, 5, 1.2, 0.2, SETOSA},
	{3.2, 4.7, 1.6, 0.2, SETOSA},
	{3.3, 5, 1.4, 0.2, SETOSA},
	{3.4, 5.4, 1.5, 0.4, SETOSA},
	{3.4, 4.8, 1.9, 0.2, SETOSA},
	{3.4, 4.8, 1.6, 0.2, SETOSA},
	{3.4, 4.6, 1.4, 0.3, SETOSA},
	{3.4, 5.4, 1.7, 0.2, SETOSA},
	{3.4, 5, 1.6, 0.4, SETOSA},
	{3.4, 5, 1.5, 0.2, SETOSA},
	{3.5, 5, 1.6, 0.6, SETOSA},
	{3.5, 5.2, 1.5, 0.2, SETOSA},
	{3.5, 5.1, 1.4, 0.2, SETOSA},
	{3.5, 5.1, 1.4, 0.3, SETOSA},
	{3.6, 5, 1.4, 0.2, SETOSA},
	{3.6, 4.6, 1, 0.2, SETOSA},
	{3.7, 5.3, 1.5, 0.2, SETOSA},
	{3.7, 5.1, 1.5, 0.4, SETOSA},
	{3.8, 5.1, 1.9, 0.4, SETOSA},
	{3.8, 5.1, 1.5, 0.3, SETOSA},
	{3.8, 5.1, 1.6, 0.2, SETOSA},
	{3.8, 5.7, 1.7, 0.3, SETOSA},
	{3.9, 5.4, 1.7, 0.4, SETOSA},
	{4, 5.8, 1.2, 0.2, SETOSA},
	{4.1, 5.2, 1.5, 0.1, SETOSA},
	{4.2, 5.5, 1.4, 0.2, SETOSA},
	{2, 5, 3.5, 1, VERSICOLOR},
	{2.2, 6.2, 4.5, 1.5, VERSICOLOR},
	{2.2, 6, 4, 1, VERSICOLOR},
	{2.3, 5, 3.3, 1, VERSICOLOR},
	{2.3, 5.5, 4, 1.3, VERSICOLOR},
	{2.3, 6.3, 4.4, 1.3, VERSICOLOR},
	{2.4, 5.5, 3.8, 1.1, VERSICOLOR},
	{2.4, 4.9, 3.3, 1, VERSICOLOR},
	{2.4, 5.5, 3.7, 1, VERSICOLOR},
	{2.5, 5.1, 3, 1.1, VERSICOLOR},
	{2.5, 6.3, 4.9, 1.5, VERSICOLOR},
	{2.6, 5.8, 4, 1.2, VERSICOLOR},
	{2.7, 6, 5.1, 1.6, VERSICOLOR},
	{2.7, 5.2, 3.9, 1.4, VERSICOLOR},
	{2.7, 5.6, 4.2, 1.3, VERSICOLOR},
	{2.8, 6.1, 4, 1.3, VERSICOLOR},
	{2.8, 6.8, 4.8, 1.4, VERSICOLOR},
	{2.8, 5.7, 4.1, 1.3, VERSICOLOR},
	{2.8, 5.7, 4.5, 1.3, VERSICOLOR},
	{2.8, 6.1, 4.7, 1.2, VERSICOLOR},
	{2.8, 6.5, 4.6, 1.5, VERSICOLOR},
	{2.9, 6.2, 4.3, 1.3, VERSICOLOR},
	{2.9, 5.6, 3.6, 1.3, VERSICOLOR},
	{2.9, 5.7, 4.2, 1.3, VERSICOLOR},
	{2.9, 6, 4.5, 1.5, VERSICOLOR},
	{2.9, 6.6, 4.6, 1.3, VERSICOLOR},
	{2.9, 6.1, 4.7, 1.4, VERSICOLOR},
	{3, 5.4, 4.5, 1.5, VERSICOLOR},
	{3, 5.7, 4.2, 1.2, VERSICOLOR},
	{3, 5.6, 4.5, 1.5, VERSICOLOR},
	{3, 6.1, 4.6, 1.4, VERSICOLOR},
	{3, 6.6, 4.4, 1.4, VERSICOLOR},
	{3, 6.7, 5, 1.7, VERSICOLOR},
	{3, 5.6, 4.1, 1.3, VERSICOLOR},
	{3.1, 6.7, 4.7, 1.5, VERSICOLOR},
	{3.1, 6.9, 4.9, 1.5, VERSICOLOR},
	{3.2, 5.9, 4.8, 1.8, VERSICOLOR},
	{3.2, 7, 4.7, 1.4, VERSICOLOR},
	{3.2, 6.4, 4.5, 1.5, VERSICOLOR},
	{3.3, 6.3, 4.7, 1.6, VERSICOLOR},
	{3.4, 6, 4.5, 1.6, VERSICOLOR},
	{2.2, 6, 5, 1.5, VIRGINICA},
	{2.5, 4.9, 4.5, 1.7, VIRGINICA},
	{2.5, 6.7, 5.8, 1.8, VIRGINICA},
	{2.6, 7.7, 6.9, 2.3, VIRGINICA},
	{2.6, 6.1, 5.6, 1.4, VIRGINICA},
	{2.7, 5.8, 5.1, 1.9, VIRGINICA},
	{2.7, 6.3, 4.9, 1.8, VIRGINICA},
	{2.7, 5.8, 5.1, 1.9, VIRGINICA},
	{2.7, 6.4, 5.3, 1.9, VIRGINICA},
	{2.8, 7.4, 6.1, 1.9, VIRGINICA},
	{2.8, 5.6, 4.9, 2, VIRGINICA},
	{2.8, 6.3, 5.1, 1.5, VIRGINICA},
	{2.8, 6.2, 4.8, 1.8, VIRGINICA},
	{2.8, 5.8, 5.1, 2.4, VIRGINICA},
	{2.8, 7.7, 6.7, 2, VIRGINICA},
	{2.9, 7.3, 6.3, 1.8, VIRGINICA},
	{2.9, 6.3, 5.6, 1.8, VIRGINICA},
	{3, 6.7, 5.2, 2.3, VIRGINICA},
	{3, 6.8, 5.5, 2.1, VIRGINICA},
	{3, 6.1, 4.9, 1.8, VIRGINICA},
	{3, 6.5, 5.2, 2, VIRGINICA},
	{3, 6, 4.8, 1.8, VIRGINICA},
	{3, 6.5, 5.8, 2.2, VIRGINICA},
	{3, 6.5, 5.5, 1.8, VIRGINICA},
	{3, 7.7, 6.1, 2.3, VIRGINICA},
	{3, 7.6, 6.6, 2.1, VIRGINICA},
	{3.1, 6.7, 5.6, 2.4, VIRGINICA},
	{3.1, 6.4, 5.5, 1.8, VIRGINICA},
	{3.1, 6.9, 5.1, 2.3, VIRGINICA},
	{3.1, 6.9, 5.4, 2.1, VIRGINICA},
	{3.2, 6.9, 5.7, 2.3, VIRGINICA},
	{3.2, 7.2, 6, 1.8, VIRGINICA},
	{3.2, 6.5, 5.1, 2, VIRGINICA},
	{3.2, 6.4, 5.3, 2.3, VIRGINICA},
	{3.2, 6.8, 5.9, 2.3, VIRGINICA},
	{3.3, 6.7, 5.7, 2.5, VIRGINICA},
	{3.3, 6.7, 5.7, 2.1, VIRGINICA},
	{3.4, 6.2, 5.4, 2.3, VIRGINICA},
	{3.4, 6.3, 5.6, 2.4, VIRGINICA},
	{3.8, 7.7, 6.7, 2.2, VIRGINICA},
	{3.8, 7.9, 6.4, 2, VIRGINICA},
	{3, 4.8, 1.4, 0.3, SETOSA},
	{3, 4.3, 1.1, 0.1, SETOSA},
	{3.1, 4.6, 1.5, 0.2, SETOSA},
	{3.3, 5.1, 1.7, 0.5, SETOSA},
	{3.4, 5.1, 1.5, 0.2, SETOSA},
	{3.4, 5.2, 1.4, 0.2, SETOSA},
	{3.5, 5, 1.3, 0.3, SETOSA},
	{3.5, 5.5, 1.3, 0.2, SETOSA},
	{3.6, 4.9, 1.4, 0.1, SETOSA},
	{3.7, 5.4, 1.5, 0.2, SETOSA},
	{3.9, 5.4, 1.3, 0.4, SETOSA},
	{4.4, 5.7, 1.5, 0.4, SETOSA},
	{2.5, 5.6, 3.9, 1.1, VERSICOLOR},
	{2.5, 5.5, 4, 1.3, VERSICOLOR},
	{2.6, 5.5, 4.4, 1.2, VERSICOLOR},
	{2.6, 5.7, 3.5, 1, VERSICOLOR},
	{2.7, 5.8, 3.9, 1.2, VERSICOLOR},
	{2.7, 5.8, 4.1, 1, VERSICOLOR},
	{2.9, 6.4, 4.3, 1.3, VERSICOLOR},
	{3, 5.9, 4.2, 1.5, VERSICOLOR},
	{3.1, 6.7, 4.4, 1.4, VERSICOLOR},
	{2.5, 6.3, 5, 1.9, VIRGINICA},
	{2.5, 5.7, 5, 2, VIRGINICA},
	{2.8, 6.4, 5.6, 2.1, VIRGINICA},
	{2.8, 6.4, 5.6, 2.2, VIRGINICA},
	{3, 7.1, 5.9, 2.1, VIRGINICA},
	{3, 5.9, 5.1, 1.8, VIRGINICA},
	{3, 7.2, 5.8, 1.6, VIRGINICA},
	{3.3, 6.3, 6, 2.5, VIRGINICA},
	{3.6, 7.2, 6.1, 2.5, VIRGINICA}
};

vector<iris> means = {
	{3.410526316, 4.981578947, 1.478947368, 0.242105263},
	{2.77804878, 5.948780488, 4.3, 1.348780488},
	{2.980487805, 6.607317073, 5.548780488, 2.014634146}
};
vector<iris> stds = {
	{0.376893041, 0.339200382, 0.178821812, 0.100354989},
	{0.332800387, 0.539963865, 0.494469413, 0.197638497},
	{0.319546172, 0.657415487, 0.579276252, 0.271625639}
};

double density(double mean, double std, double x) {
	return (1.0 / (sqrt(2 * PI) * std)) * pow(E, -((pow(x - mean, 2)) / (2 * std * std)));
}

string get_class(int id) {
	switch (id) {
		case VERSICOLOR:
			return "versicolor";
		case SETOSA:
			return "setosa";
		case VIRGINICA:
			return "virginica";
		default:
			return "";
	}
	return "";
}

int naive_bayes(const vector<iris>& data, const string& outfile) {
	int virginica_count = 0, versicolor_count = 0, setosa_count = 0;

	int error_count = 0;

	for (unsigned int i = 0; i < data.size(); i++) {
		switch (data[i].cls) {
			case SETOSA:
				setosa_count++;
				break;
			case VIRGINICA:
				virginica_count++;
				break;
			case VERSICOLOR:
				versicolor_count++;
				break;
		}
	}

	ofstream out;
	out.open(outfile);

	for (unsigned int i = 0; i < data.size(); i++) {
		double setosa_PE =
			(density(means[SETOSA].sepal_width, stds[SETOSA].sepal_width, data[i].sepal_width) *
			density(means[SETOSA].sepal_length, stds[SETOSA].sepal_length, data[i].sepal_length) *
			density(means[SETOSA].petal_length, stds[SETOSA].petal_length, data[i].petal_length)) *
			density(means[SETOSA].petal_width, stds[SETOSA].petal_width, data[i].petal_width);

		double versicolor_PE =
			(density(means[VERSICOLOR].sepal_width, stds[VERSICOLOR].sepal_width, data[i].sepal_width) *
			density(means[VERSICOLOR].sepal_length, stds[VERSICOLOR].sepal_length, data[i].sepal_length) *
			density(means[VERSICOLOR].petal_length, stds[VERSICOLOR].petal_length, data[i].petal_length)) *
			density(means[VERSICOLOR].petal_width, stds[VERSICOLOR].petal_width, data[i].petal_width);

		double viginica_PE =
			(density(means[VIRGINICA].sepal_width, stds[VIRGINICA].sepal_width, data[i].sepal_width) *
			density(means[VIRGINICA].sepal_length, stds[VIRGINICA].sepal_length, data[i].sepal_length) *
			density(means[VIRGINICA].petal_length, stds[VIRGINICA].petal_length, data[i].petal_length)) *
			density(means[VIRGINICA].petal_width, stds[VIRGINICA].petal_width, data[i].petal_width);

		double sum = data[i].sepal_width + data[i].sepal_length + data[i].petal_length + data[i].petal_width;

		double setosa_chance = (setosa_PE * ((double)setosa_count / (double)data.size())) / sum;
		double versicolor_chance = (versicolor_PE * ((double)versicolor_count / (double)data.size())) / sum;
		double virginica_chance = (viginica_PE * ((double)virginica_count / (double)data.size())) / sum;

		double largest = MAX3(virginica_chance, setosa_chance, versicolor_chance);
		int predict = largest == virginica_chance ? VIRGINICA : largest == setosa_chance ? SETOSA : VERSICOLOR;

		if (predict != data[i].cls) {
			error_count++;

			double den = 1;// setosa_chance + versicolor_chance + virginica_chance;//???

			out << "NBC predict: " << get_class(predict)
				<< ". actual: " << get_class(data[i].cls) << ". "
				<< "setosa chance: " << setosa_chance / den << ". "
				<< "versicolor chance:" << versicolor_chance / den << ". "
				<< "virginica chance: " << virginica_chance / den << ".\n";
		}
	}

	out.close();

	return error_count;
}

int main() {
	vector<iris> training_set(iris_data.begin(), iris_data.end() - 30);
	vector<iris> testing_set(iris_data.begin() + 120, iris_data.end());

	int training_error = naive_bayes(training_set, "training_errors.txt");
	int testing_error = naive_bayes(testing_set, "testing_errors.txt");
	int full_error = naive_bayes(iris_data, "full_errors.txt");

	cout << "training errors: " << training_error << " " << (double)(training_set.size() - training_error) / training_set.size() * 100 << "%\n";
	cout << "testing errors: " << testing_error << " " << (double)(testing_set.size() - testing_error) / testing_set.size() * 100 << "%\n";
	cout << "full errors: " << full_error << " " << (double)(iris_data.size() - full_error) / iris_data.size() * 100 << "%\n";

	_getch();
	return 0;
}