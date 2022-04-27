#include <iostream>
#include <conio.h>
#include <thread>
#include <vector>
#include <vector>
#include <Windows.h>
#include <numeric>
#include <string>

using namespace std;

#define NUM_HOLES 100

int jump_hole(int pos) {
	if (pos == 0) return 1;	
	else if (pos == NUM_HOLES - 1) 	return NUM_HOLES - 2;	
	else return pos + (rand() % 2 ? 1 : -1);	
}

void print(std::vector<bool>& holes, int pos, int rpos) {
	for (int i = 0; i < holes.size(); i++) {
		if (i == pos) cout << "@";
		else cout << (holes[i] ? "%" : ".");
	}
	cout << '\n';
}

bool check_hole(std::vector<bool>& holes, int pos, int* rpos) {
	print(holes, pos, *rpos);

	if (holes[pos]) 
		return true;

	holes[*rpos] = false;
	*rpos = jump_hole(*rpos);
	holes[*rpos] = true;

	return false;
}

void rabbit() {
	srand(clock());

	for (int runs = 0;; runs++) {
		if (runs % 1000 == 0)
			cout << runs << '\n';

		bool rabbit_found = false;
		std::vector<bool> holes(NUM_HOLES, false);

		int rpos = rand() % NUM_HOLES;

		holes[rpos] = true;

		for (int i = 1; i < holes.size(); i++) {
			if (rabbit_found = check_hole(holes, i, &rpos)) {
				std::cout << "rabbit found " << i << "\n";
				break;
			}
		}

		if (!rabbit_found) {
			cout << "run2\n";
			for (int i = 1; i < holes.size(); i++) {
				if (rabbit_found = check_hole(holes, i, &rpos)) {
					std::cout << "rabbit found " << i << "\n";
					break;
				}
			}
		}

		if (!rabbit_found) {
			std::cout << "rabbit not found\n";
			break;
		}
	}
}

struct node {
	node* next;
	int val;

	node() {
		this->val = 0;
		this->next = 0;
	}

	node(int v) {
		this->val = v;
		this->next = 0;
	}
};

class linked_list {
	node* root;

public:
	linked_list() {
		root = NULL;
	}

	void append_val(int v) {
		node* n = new node(v);
		if (!root) {
			root = n;
			return;
		}
		node* t = root;
		while (t->next != NULL)
			t = t->next;
		t->next = n;
	}

	node* reverse() {
		node* c = root;
		node* p = 0;
		while (c) {
			node* n = c->next;
			c->next = p;
			p = c;
			c = n;
		}
		root = p;
		return root;
	}

	void print() {
		node* t = root;
		while (t->next != NULL) {
			cout << t->val << ",";
			t = t->next;
		}
		if (t) 
			cout << t->val;
		cout << '\n';
	}
};

//b/w image pixels matrix 2d array
//0 1
//remove all black pixels not connected to border of the image
//2 or more pixels orthogonaly connected to border

typedef std::vector<std::vector<int>> matrix;

void removeIslands(matrix& m) {
	matrix mm = {
		{1,0,0,0,0,0},
		{0,1,0,1,1,1},
		{0,0,1,0,1,0},
		{1,1,0,0,1,0},
		{1,0,1,1,0,0},
		{1,0,0,0,0,1}
	};

	removeIslands(m);
}

/*
100000
010111
001010
110010
101100
100001

--->

100000
000111
000010
110010
100000
100001


*/

int get_second_lowest_highest_sum(const std::vector<int>& list) {
	if (!list.size()) return false;
	int smallest, smallest2, largest, largest2;
    //assume first element is smallest and biggest 
	smallest = largest = list[0];

	//first pass to find smallest and largest
	for (int i = 0; i < list.size(); i++) {
		if (list[i] > largest) {
			largest = list[i];
		}
		else if (list[i] < smallest) {
			smallest = list[i];
		}
	}

	smallest2 = largest;
	largest2 = smallest;

	//second pass to find 2nd smallest and 2nd largest
	for (int i = 0; i < list.size(); i++) {
		if (list[i] != largest && list[i] > largest2) {
			largest2 = list[i];
		}
		if (list[i] != smallest && list[i] < smallest2) {
			smallest2 = list[i];
		}
	}

	return smallest2 + largest2;
}

std::string parse_string(const std::string& input) {
	const std::string prefix = "Organization";
	std::string new_s = "";

	if (input.find(prefix, 0) == 0) 
		new_s = input.substr(prefix.length());

	return new_s;
}

int main() {
	for (int i = 1; i <= 100; i++) 
		std::cout << (i % 15 == 0 ? "FizzBuzz" : i % 3 == 0 ? "Fizz" : i % 5 == 0 ? "Buzz" : std::to_string(i)) << '\n';

	//_getch();
	return 0;
}