 #include "mergesort.h"

#include <iostream>
#include <conio.h>

#include "visualizer.h"

using namespace std;

void merge(vector<int>* list, int start, int middle, int end) {
	int l = start, r = middle + 1, p = 0;
	vector<int> c(end - start + 1);

	while (l <= middle && r <= end) 
		c[p++] = (*list)[l] <= (*list)[r] ? (*list)[l++] : (*list)[r++];	

	while (l <= middle) c[p++] = (*list)[l++];
	while (r <= end) c[p++] = (*list)[r++];

	copy(c.begin(), c.end(), (*list).begin() + start);
}

void mergesort(vector<int>* list, int start, int end) {
	if (start >= end) return;
	int middle = (start + end) / 2;

	mergesort(list, start, middle);
	mergesort(list, middle + 1, end);
	merge(list, start, middle, end);
}

void mergesort(vector<int>* list) {
	mergesort(list, 0, list->size() - 1);
}

