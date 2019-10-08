#include "heapsort.h"

#include "visualizer.h"

using namespace std;

int getparenti(int n) {
	return (n - 1) / 2;
}

void getchildreni(int n, int* a, int* b) {
	*a = 2 * n + 1;
	*b = 2 * n + 2;
}

void heapify(vector<int>* l) {
	for (int i = 1; i < l->size(); i++) {
		int ci = i;
		while (ci > 0) {
			int pi = getparenti(ci);
			//check if child is greater than parent
			if ((*l)[ci] > (*l)[pi]) {
				//swap child and parent
				int t = (*l)[ci];
				(*l)[ci] = (*l)[pi];
				(*l)[pi] = t;
				//swap the pointers too
				ci = pi;
			}
			else break;
		}
	}
}

void deheapify(vector<int>* l) {
	int li = l->size() - 1;
	for (int i = 0; i < l->size(); i++) {
		//move first element to start of the sorted end
		int t = (*l)[0];
		(*l)[0] = (*l)[li];
		(*l)[li--] = t;

		int pi = 0, c1i, c2i;
		//swap new root down to make heap valid
		while (1) {
			getchildreni(pi, &c1i, &c2i);

			//check bounds
			if (c1i >= l->size() - i - 1) break;
			if (c2i >= l->size() - i - 1) c2i = c1i;

			//get index of greater child
			int gci = (*l)[c1i] > (*l)[c2i] ? c1i : c2i;

			//check if greatest child is bigger than parent
			if ((*l)[gci] > (*l)[pi]) {
				//swap them if child is bigger
				int t = (*l)[gci];
				(*l)[gci] = (*l)[pi];
				(*l)[pi] = t;

				//swap the pointers of child and parent too
				pi = gci;
			}
			else break;
		}
	}

	
}

void heapsort(vector<int>* list) {
	heapify(list);
	deheapify(list);
}

