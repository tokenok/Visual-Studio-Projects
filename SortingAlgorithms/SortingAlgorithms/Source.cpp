#include <iostream>
#include <vector>
#include <conio.h>
#include <thread>

#include "visualizer.h"
#include "Benchmark.h"

#include "mergesort.h"
#include "heapsort.h"
#include "quicksort.h"
#include "insertionsort.h"
#include "selectionsort.h"
#include "bubblesort.h"
#include "timsort.h"

using namespace std;

enum {
	RANDOM, NEAR_SORTED, REVERSED, SORTED, SORTED2
};

void init(vector<int>* l, int mix) {
	switch (mix) {
		case RANDOM: {
			for (int i = 0; i < l->size(); i++) {
				(*l)[i] = rand();
			}
			break;
		}
		case NEAR_SORTED: {
			for (int i = 0; i < l->size(); i++) {
				(*l)[i] = i + 1;
			}
			for (int i = 0; i < l->size() / 100; i++) {
				swap((*l)[rand() % l->size()], (*l)[rand() % l->size()]);
			}
			break;
		}
		case REVERSED: {
			for (int i = 0; i < l->size(); i++) {
				(*l)[i] = l->size() - i;
			}
			break;
		}
		case SORTED: {
			for (int i = 0; i < l->size(); i++) {
				(*l)[i] = i + 1;
			}
			break;
		}
		case SORTED2: {
			int mid = l->size() / 2;
			for (int i = 0, v = 1; i < l->size(); i++, v++) {
				(*l)[i] = v;
				if (v % (l->size() / 2) == 0)
					v = 0;
			}
		}
	}	
}

#define TEST(size,mix,alg) test(size,#alg,mix,#mix,alg)

#define TESTSUITE(size, mix)	TEST(size, quicksort, mix);\
								TEST(size, mergesort, mix);\
								TEST(size, heapsort, mix);\
								TEST(size, quicksortinsertion, mix);\
								TEST(size, selectionsort, mix);\
								TEST(size, insertionsort, mix);\
								TEST(size, bubblesort, mix);\
								TEST(size, gfx::timsort, mix);\
								TEST(size, std::sort, mix);
	
bool testsort(const TraceVector<int>& a) {
	for (int i = 0; i < a.size() - 1; i++) {
		if (a[i] > a[i + 1])
			return false;
	}
	return true;
}

#define TESTIMP(x)	TraceVector<int> list;\
					list.resize(size);\
					init(&list, mix);\
					Benchmark timer;\
					x;\
					long long time = timer.end();\
					if (!testsort(list)) cout << "Failed!!!!";\
					cout << name << " (" << size << " elements)[" << mix_name << "]: " << (double)time / 1000.0 << "ms" << '\n';

void test(int size, const char* name, int mix, const char* mix_name, void(*func)(TraceVector<int>*)) {
	TESTIMP(func(&list))
}

void test(int size, const char* name, int mix, const char* mix_name, void(*func)(TraceVector<int>::iterator, TraceVector<int>::iterator)) {
	TESTIMP(func(list.begin(), list.end()))	
}

void test(int size, const char* name, int mix, const char* mix_name, int(*func)(TraceVector<int>*, int)) {
	TESTIMP(func(&list, list.size()))
}

int main() {
	srand(clock());

	TraceVector<int> t;
	t.push_back(1);
	for (int i = 0; i < t.size(); i++) {
		cout << t[i] << '\n';
	}


	//TEST(100000, REVERSED, gfx::timsort);
	//TEST(100000, REVERSED, quicksort);
	//TEST(100000, REVERSED, quicksortinsertion);
	//TEST(100000, REVERSED, mergesort);
	//TEST(100000, REVERSED, std::sort);	
	//printf("\n");
	//TEST(100000, SORTED, gfx::timsort);
	//TEST(100000, SORTED, quicksort);
	//TEST(100000, SORTED, quicksortinsertion);
	//TEST(100000, SORTED, mergesort);
	//TEST(100000, SORTED, std::sort);
	//printf("\n");
	//TEST(100000, RANDOM, gfx::timsort);
	//TEST(100000, RANDOM, quicksort);
	//TEST(100000, RANDOM, quicksortinsertion);
	//TEST(100000, RANDOM, mergesort);
	//TEST(100000, RANDOM, std::sort);
	//printf("\n");
	//TEST(100000, NEAR_SORTED, gfx::timsort);
	//TEST(100000, NEAR_SORTED, quicksort);
	//TEST(100000, NEAR_SORTED, quicksortinsertion);
	//TEST(100000, NEAR_SORTED, mergesort);
	//TEST(100000, NEAR_SORTED, std::sort);
	//printf("\n");
	//TEST(100000, SORTED2, gfx::timsort);
	//TEST(100000, SORTED2, quicksort);
	//TEST(100000, SORTED2, quicksortinsertion);
	//TEST(100000, RANDOM, mergesort);	
	//TEST(100000, SORTED2, std::sort);
	//printf("\n");

	cout << "Done\n";
	_getch();
	return 0;
}