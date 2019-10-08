#include "quicksort.h"

#include "visualizer.h"

#include <iostream>

#include "insertionsort.h"

using namespace std;

int MedianOfThree(vector<int>* l, int s, int e) {
	int mid = (s + e) / 2;
	if ((*l)[e] < (*l)[s])
		swap((*l)[s], (*l)[e]);
	if ((*l)[mid] < (*l)[s])
		swap((*l)[mid], (*l)[s]);
	if ((*l)[e] < (*l)[mid])
		swap((*l)[e], (*l)[mid]);
	return (*l)[mid];
}

int partition(vector<int>* l, int s, int e) {
	int p = MedianOfThree(l, s++, e--);	
	while (true) {
		while ((*l)[s] < p) s++;
		while ((*l)[e] > p) e--;

		if (s >= e) 
			return e;

		swap((*l)[s++], (*l)[e--]);
	}
	return e;
}

void quicksort(vector<int>* l, int s, int e) {
	if (s < e) {
		int p = partition(l, s, e);
		quicksort(l, s, p);
		quicksort(l, p + 1, e);
	}
}

void quicksort(vector<int>* l) {
	quicksort(l, 0, l->size() - 1);
}

void quicksortinsertion(vector<int>* l, int s, int e) {
	if (s < e && e - s > 10) {
		int p = partition(l, s, e);
		quicksort(l, s, p);
		quicksort(l, p + 1, e);
	}
}

void quicksortinsertion(vector<int>* l) {
	quicksortinsertion(l, 0, l->size() - 1);
	insertionsort(l);
}