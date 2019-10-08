#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <conio.h>

#include "C:\CPPlibs\common\f\common.h"

using namespace std;

void Arrays_LeftRotation() {
	int n, r, s;
	cin >> n >> r;
	vector<int> l(n);
	s = (n - r) % n;
	for (int i = s; i < n + s; i++)
		cin >> l[i % n];

	for (int i = 0; i < l.size(); i++)
		cout << l[i] << " ";
	cout << '\n';
}

void Strings_MakingAnagrams() {
	string a, b;
	cin >> a >> b;
	
	map<char, pair<int, int>> x;
	for (int i = 0; i < a.size(); i++) 
		x[a[i]].first++;

	for (int i = 0; i < b.size(); i++) 
		x[b[i]].second++;

	int res = 0;
	for (auto a : x) 
		res += abs(a.second.first - a.second.second);

	cout << res << '\n';
}

void HashTables_RansomNote() {
	bool res = true;
	int ml, nl;
	cin >> ml >> nl;
	vector<string> m(ml);
	vector<string> n(nl);
	for (int i = 0; i < ml; i++) cin >> m[i];
	for (int i = 0; i < nl; i++) cin >> n[i];

	map<string, int> x;
	for (int i = 0; i < m.size(); i++) 	x[m[i]]++;
	for (int i = 0; i < n.size(); i++) {
		if (x[n[i]]-- == 0) {
			res = false;
			break;
		}
	}

	cout << (res ? "Yes" : "No") << '\n';
}

struct Node {
	int data;
	struct Node* next;
};
bool has_cycle(Node* head) {
	set<Node*> v;

	while (head) {
		head = head->next;
		if (v.find(head) != v.end()) return true;
		v.insert(head);
	}
	return false;
}

void Stacks_BalancedBracket() {
	auto is_balanced = [](string expression) {
		vector<char> l;
		for (auto c : expression) {
			if (c == '{' || c == '(' || c == '[')
				l.push_back(c);
			else if (l.size()) {
				char lc = l[l.size() - 1];
				if (!(lc == '{' && c == '}' || lc == '(' && c == ')' || lc == '[' && c == ']'))
					return false;				
				l.pop_back();
			}
			else return false;
		}
		return !l.size();
	};

	int t;
	cin >> t;
	for (int a0 = 0; a0 < t; a0++) {
		string expression;
		cin >> expression;
		bool answer = is_balanced(expression);
		cout << (answer ? "YES" : "NO") << '\n';
	}
}

int main() {
	Stacks_BalancedBracket();

	_getch();
	return 0;
}