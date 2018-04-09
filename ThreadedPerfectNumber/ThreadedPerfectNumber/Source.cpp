#include <iostream>
#include <vector>
#include <thread>

using namespace std;

void getFactors(long long start, long long end, long long N, vector<long long> &f) {	
	for (long long i = start; i <= end; i++) {
		if (i <= 1)
			continue;
		if (N % i == 0) {
			f.push_back(i); //add first factor	
			f.push_back(N / i);//add second factor
		}
	}
}

bool checkPerfect(vector<long long> &f, long long N) {
	long long s = 1;
	for (long long i = 0; i < f.size(); i++) //sum the list
		s += f[i];
	return s == N;
}

int main() {
	long long N, P;
	while (true) {
		cout << "Enter a number, and a number of threads:\n";
		cin >> N >> P;

		if (N <= 1 || P < 1)//invalid input
			continue;

		vector<long long> buf_factors;
		vector<thread> threads;

		long long upper_bound = (long long)sqrt(N) + 1;
		for (long long i = 0; i < P; i++) {
			long long start = i * upper_bound / P;
			long long end = (i + 1) * upper_bound / P - 1;

			thread a(getFactors, start, end, N, ref(buf_factors));
			threads.emplace_back(move(a));
		}

		for (int i = 0; i < threads.size(); i++) {
			threads[i].join();
		}

		bool res = checkPerfect(buf_factors, N);
		if (res) 
			cout << N << " is a perfect number\n";
		else 
			cout << N << " is not a perfect number\n";
	}
	return 0;
}

