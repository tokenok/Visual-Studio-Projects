#include "visualizer.h"

#include <iostream>
#include <conio.h>

using namespace std;

void print(const vector<int>* list) {
	//system("cls");
	for (int i = 0; i < list->size(); i++) {
		for (int j = 0; j < list->at(i); j++) {
			cout << "|";
		}
		cout << '\n';
	}
	cout << '\n';
	_getch();
}

void printv(const vector<int>& list) {
	for (int i = 0; i < list.size(); i++) {
		cout << list[i] << ",";
	}
	cout << '\n';
}

void printv(const vector<int>& list, int s, int e) {
	for (int i = s; i <= e; i++) {
		cout << list[i] << ",";
	}
	cout << '\n';
}
