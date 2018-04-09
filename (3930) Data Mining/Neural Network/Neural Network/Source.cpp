#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <time.h>
#include <conio.h>
#include <iomanip>

#define E (2.7182818284590452353602874713526624977572470937L)
#define PI (3.1415926535897932384626433832795028841971693994L)

using namespace std;

double learning_rate = .001;

double calc_sigmoid(double x) {
	return (1.0 / exp(-x));
}

double calc_sigmoid_diriv(double x) {
	double t = calc_sigmoid(x);
	return t * (1 - t);
}

double get_rand() {
	return (double)rand() / RAND_MAX - 1;
}

class hidden_node {
	public:

	double output = 0;
	double error = 0;

	vector<double> weights;

	vector<hidden_node*> layer_nodes;

	double activation_func() {
		output = calc_sigmoid(output);
		return output;
	}
};

class input_node {
	public:
	vector<hidden_node*> layer_nodes;
	vector<double> weights;
};

void normalize(vector<double>& data, double* min, double* max) {
	//get mins and maxs
	*min = 100000; *max = -100000;
	for (int i = 0; i < data.size(); i++) {
		if (data[i] < *min)
			*min = data[i];
		if (data[i] > *max)
			*max = data[i];
	}

	for (int i = 0; i < data.size(); i++) {
		data[i] = (data[i] - *min) / (*max - *min);
//		data[i] = data[i] * 2 - 1;//[-1,1]
	}
}

double denormalize_val(double val, double min, double max) {
//	val = (val + 1) / 2; //[-1,1]
	return val * (max - min) + min;
}

double f1(double x) {
	return 2.0 * x;
}
double f2(double x) {
	return sin(x);
}

double neural_network(vector<double> training_set, vector<double> testing_set, double(*func)(double)) {
	double training_min_val, training_max_val,
		testing_min_val, testing_max_val,
		training_output_min_val, training_output_max_val, 
		testing_output_min_val, testing_output_max_val;

	vector<double> training_set_output, testing_set_output;
	for (int i = 0; i < training_set.size(); i++) {
		training_set_output.push_back((*func)(training_set[i]));
	}
	for (int i = 0; i < testing_set.size(); i++) {
		testing_set_output.push_back((*func)(testing_set[i]));
	}

	normalize(training_set, &training_min_val, &training_max_val);
	normalize(testing_set, &testing_min_val, &testing_max_val);
	normalize(training_set_output, &training_output_min_val, &training_output_max_val);
	normalize(testing_set_output, &testing_output_min_val, &testing_output_max_val);

	//start initializing neural network nodes
	input_node input1;
	input_node input_bias;

	hidden_node n1, n2, n3, n4, n5, n_bias;
	n_bias.output = 1;

	hidden_node output1;

	input1.layer_nodes = {&n1, &n2, &n3, &n4, &n5};
	input_bias.layer_nodes = input1.layer_nodes;
	output1.layer_nodes = {&n1, &n2, &n3, &n4, &n5, &n_bias};

	//create random weights
	for (int i = 0; i < input1.layer_nodes.size(); i++) {
		input1.weights.push_back(get_rand());
		input_bias.weights.push_back(get_rand());
	}
	for (int i = 0; i < output1.layer_nodes.size(); i++)
		output1.weights.push_back(get_rand());

	double RMSE = 0;

	for (int n = 0; n < 10; n++) {
		double total_error = 0;

		for (int i = 0; i < training_set.size(); i++) {
			double in = training_set[i];
		
			//forward propagate
			for (int j = 0; j < input1.weights.size(); j++) {//loop through hidden nodes				
				input1.layer_nodes[j]->output = (input1.weights[j] * in);
				input_bias.layer_nodes[j]->output += input_bias.weights[j];
				input1.layer_nodes[j]->activation_func();
			}

			n_bias.output = 1;
			output1.output = 0;
			for (int j = 0; j < output1.layer_nodes.size(); j++) {
				output1.output += (output1.layer_nodes[j]->output * output1.weights[j]);
			}
			output1.activation_func();

			//----------------------------------------------
			//back propagate		
			double o = output1.output;
			double t = training_set_output[i];

		//	cout << in << " " << t << '\n';

			output1.error = -(t - o) * calc_sigmoid_diriv(o);
		
			//get hidden node errors
			for (int j = 0; j < output1.layer_nodes.size(); j++) {
				output1.layer_nodes[j]->error = (calc_sigmoid_diriv(output1.layer_nodes[j]->output) * output1.weights[j] * output1.error);;
			}

			//get new output weights (hidden nodes to output)
			for (int j = 0; j < output1.weights.size(); j++) {
				output1.weights[j] += ((-learning_rate) * output1.error * output1.layer_nodes[j]->output);
			}

			//get new hidden layer weights (output to hidden nodes)
			for (int j = 0; j < input1.layer_nodes.size(); j++) {
				input1.weights[j] += ((-learning_rate) * input1.layer_nodes[j]->error * in);
			}
			//dont forget the input bias node
			for (int j = 0; j < input_bias.layer_nodes.size(); j++) {
				input_bias.weights[j] += ((-learning_rate) * input_bias.layer_nodes[j]->error);
			}

			total_error += pow(output1.error, 2);
		}

		RMSE = sqrt(total_error / training_set.size());

		if (n % 1000 == 0)
			cout << RMSE << '\n';
	}
	cout << RMSE << '\n';

	for (int i = 0; i < testing_set.size(); i++) {
		double in = training_set[i];

		//forward propagate
		for (int j = 0; j < input1.weights.size(); j++) {//loop through hidden nodes				
			input1.layer_nodes[j]->output = (input1.weights[j] * in);
			input_bias.layer_nodes[j]->output += input_bias.weights[j];
			input1.layer_nodes[j]->activation_func();
		}

		n_bias.output = 1;
		output1.output = 0;
		for (int j = 0; j < output1.layer_nodes.size(); j++) {
			output1.output += (output1.layer_nodes[j]->output * output1.weights[j]);
		}
		output1.activation_func();

		double o = output1.output;
		double t = training_set_output[i];

		output1.error = -(t - o) * calc_sigmoid_diriv(o);

		double din = denormalize_val(in, testing_min_val, testing_max_val);
		double doo = denormalize_val(o, testing_output_min_val, testing_output_max_val);
		double dt = denormalize_val(t, testing_output_min_val, testing_output_max_val);

		cout << output1.error << " " << din << " " << dt  << " " << doo << " " << o << '\n';
	}


	return 0;
}

int main() {
	srand(clock());

	//intialize data sets
	vector<double> two_x_testing_set;
	vector<double> two_x_training_set;
	for (double i = -9.98; i <= 10.00; i += .04) {
		two_x_testing_set.push_back(i - .02);
		two_x_training_set.push_back(i);
	}

	vector<double> sin_x_testing_set;
	vector<double> sin_x_training_set;
	for (double i = 0.02513274122871834590770114706624 / 2; i < 4 * PI; i += 0.02513274122871834590770114706624) {
		sin_x_testing_set.push_back(i - (0.02513274122871834590770114706624 / 2));
		sin_x_training_set.push_back(i);
	}

	neural_network(two_x_training_set, two_x_testing_set, f1);

	//neural_network(sin_x_training_set, sin_x_testing_set, f2);

	_getch();

	return 0;
}
