#include "bubblesort.h"

using namespace std;

void bubblesort(vector<int>* l) {
	for (int i = 0; i < l->size(); i++) {
		for (int j = 0; j < l->size() - i - 1; j++) {
			if ((*l)[j] > (*l)[j + 1]) swap((*l)[j], (*l)[j + 1]);
		}
	}
}