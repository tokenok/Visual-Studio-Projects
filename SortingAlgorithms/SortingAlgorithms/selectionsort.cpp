#include "selectionsort.h"

using namespace std;

void selectionsort(vector<int>* l) {
	for (int i = 0; i < l->size(); i++) {
		int li = 0;
		for (int j = 1; j < l->size() - i; j++) 
			li = (*l)[j] > (*l)[li] ? j : li;
		swap((*l)[li], (*l)[l->size() - i - 1]);
	}
}