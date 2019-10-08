#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <random>
#include <conio.h>

using namespace std;

void merge(vector<int>& list, int start, int middle, int end) {
	int leftpos = start; //first list pos
	int rightpos = middle + 1; //second list pos
	int curpos = 0; //current position of temp list
	vector<int> c;
	c.resize(end - start + 1);
	//move smallest element from each list into copy until one side is empty
	while (leftpos <= middle && rightpos <= end) {
		if (list[leftpos] <= list[rightpos]) {
			c[curpos++] = list[leftpos++];
		}
		else {
			c[curpos++] = list[rightpos++];
		}
	}
	//copy rest of left side
	while (leftpos <= middle) {
		c[curpos++] = list[leftpos++];
	}
	//copy rest of right side
	while (rightpos <= end) {
		c[curpos++] = list[rightpos++];
	}
	//move copy back
	for (curpos = 0; curpos < c.size(); curpos++) {
		list[curpos + start] = c[curpos];
	}
}

void mergesort(vector<int>& list, int start, int end) {
	if (start >= end) //only one element
		return;
	int middle = (start + end) / 2; //divide roughly in half
	
	mergesort(list, start, middle);
	mergesort(list, middle + 1, end);
	merge(list, start, middle, end);//merge halves together
}

void threaded_mergesort(vector<int>& list, int start, int end, int numThreads = 1) {
	if (start >= end) //only one element
		return;
	int middle = (start + end) / 2; //divide roughly in half

	if (numThreads <= 1) {
		mergesort(ref(list), start, end);
	}
	else {
		thread left(threaded_mergesort, ref(list), start, middle, numThreads / 2);//sort left half
		thread right(threaded_mergesort, ref(list), middle + 1, end, numThreads / 2);//sort right half
		left.join();		
		right.join();
	}	
	merge(list, start, middle, end);//merge halves together
}

bool testsort(vector<int>& list) {
	for (int i = 1; i < list.size(); i++) {
		if (list[i - 1] > list[i]) {
			return false;
		}
	}
	return true;
}

int main() {
	srand(clock());

	int n = 1000000;
	int max_threads = 10;

	vector<vector<int>> nums_lists(max_threads);

	//initialize list with random numbers
	for (int i = 0; i < n; i++) {
		int rand_int = rand();
		for (int j = 0; j < max_threads; j++) {
			nums_lists[j].push_back(rand_int);
		}
	}
	
	for (int num_threads = 1; num_threads <= max_threads; num_threads++) {
		auto start = chrono::steady_clock::now();	

		threaded_mergesort(nums_lists[num_threads - 1], 0, n - 1, num_threads);
		
		auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);
		cout << "Num threads: " << num_threads << ". Elapsed time: " << elapsed.count() << " milliseconds." << endl;

		testsort(nums_lists[num_threads - 1]);
	}
	
	_getch();
	return 0;
}