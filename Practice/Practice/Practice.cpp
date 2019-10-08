#include <iostream>
#include <map>
#include <vector>

using namespace std;

int find_missing(vector<int> &a, vector<int> &b) {
	int s = a[a.size() - 1];
	int i = 0, j = 0;

	while (i < a.size() && j < b.size()) {
		if (s >= 0) s -= b[j++];
		else s += a[i++]; 
	}

	return s;
}

int main() {
	vector<int> a = { 4, 8, 12, 9, 3 };
	vector<int> b = { 4, 8, 9, 3 };

	cout << find_missing(a, b) << '\n';

	return 0;
}
