#include "insertionsort.h"

#include "visualizer.h"

using namespace std;

void insertionsort(std::vector<int>* l) {
	insertionsort(l, 1, l->size() - 1);
}

void insertionsort(std::vector<int>* l, int s, int e) {
	for (int i = s; i <= e; i++) {
		int p = i;
		while (p > 0 && (*l)[p - 1] > (*l)[p]) {
			swap((*l)[p - 1], (*l)[p]);
			p--;
		}
	}
}
