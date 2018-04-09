#ifndef AvlTree_H
#define AvlTree_H

#include <iostream>
#include <string>
#include <sstream>

#include "common/common.h"

class AvlTree {
	private:
	struct treeNode {
		treeNode* left;
		treeNode* right;
		int height;
		std::string data;
		treeNode() { left = NULL; right = NULL; };
		treeNode(const std::string &v, treeNode* l, treeNode* r, int h) { data = v; left = l;  right = r; height = h; };
	};

	treeNode* root;
	void push(const std::string & n, treeNode * & v);
	bool search(const std::string& s, treeNode * & tree);
	public:
	AvlTree();
	~AvlTree();
	void push(const std::string & n);
	void printPreOrder() const;
	void preOrder(treeNode* pre) const;
	void clear(treeNode* & tree);
	void singleRightRotate(treeNode * & n);
	void doubleRightRotate(treeNode * & n);
	void singleLeftRotate(treeNode * & n);
	void doubleLeftRotate(treeNode * & n);
	bool search(const std::string & s);
	int avlHeight(treeNode * h);

	void preOrder(std::ostream &out, treeNode* pre);
	std::string dot();
	void display();
};
AvlTree::AvlTree() {
	root = NULL;

}

AvlTree::~AvlTree() {
	clear(root);
}

void AvlTree::push(const std::string & n) {
	push(n, root);
}

void AvlTree::singleRightRotate(treeNode * & n) {
	treeNode * temp;
	temp = n->right;
	n->right = temp->left;
	temp->left = n;
	n->height = max(avlHeight(n->left), avlHeight(n->right)) + 1;
	n = temp;
	temp->height = max(n->height, avlHeight(temp->right)) + 1;
}

void AvlTree::singleLeftRotate(treeNode * & n) {
	treeNode * temp;
	temp = n->left;
	n->left = temp->right;
	temp->right = n;
	n->height = max(avlHeight(n->left), avlHeight(n->right)) + 1;
	n = temp;
	temp->height = max(avlHeight(temp->left), n->height) + 1;
}

void AvlTree::doubleRightRotate(treeNode * & n) {
	singleLeftRotate(n->right);
	singleRightRotate(n);
}

void AvlTree::doubleLeftRotate(treeNode * & n) {
	singleRightRotate(n->left);
	singleLeftRotate(n);
}

int AvlTree::avlHeight(treeNode * h) {
	int n;
	if (h == NULL) {
		return -1;
	}
	else {
		n = h->height;
		return n;
	}

}


bool AvlTree::search(const std::string& s, treeNode *& tree) {
	if (tree == NULL) {
		return false;
	}
	else if (s < tree->data) {
		return search(s, tree->left);
	}
	else if (tree->data < s) {
		return search(s, tree->right);
	}
	else {
		return true;//here
	}
}

bool AvlTree::search(const std::string &x) {
	if (search(x, root)) 
		return true;
	return false;
}

void AvlTree::clear(treeNode* & tree) {
	if (tree != NULL) {
		clear(tree->left);
		clear(tree->right);
		delete tree;

	}

	tree = NULL;
}

void AvlTree::push(const std::string & n, treeNode* & v) {
	if (v == NULL) {
		v = new treeNode(n, NULL, NULL, 0);
	}
	else {
		if (n < v->data) {
			push(n, v->left);   // goes to left node

			if ((avlHeight(v->left) - avlHeight(v->right)) == 2) {
				if (n < v->left->data) {
					singleLeftRotate(v);
				}
				else {
					doubleLeftRotate(v);
				}
			}
		}
		else if (v->data < n) {
			push(n, v->right);  // goes to right node
			if ((avlHeight(v->right) - avlHeight(v->left)) == 2) {
				if (n > v->right->data) {
					singleRightRotate(v);
				}
				else {
					doubleRightRotate(v);
				}
			}
		}
		else {
			; // duplicate; do nothing.
		}
	}
	int a, b, c;
	a = avlHeight(v->left);
	b = avlHeight(v->right);
	c = max(a, b);
	v->height = c + 1;

}

void AvlTree::printPreOrder() const {
	preOrder(root);
}


void AvlTree::preOrder(treeNode* pre) const {
	if (pre != NULL) {
		std::cout << " " << pre->data << " ";
		preOrder(pre->left);
		preOrder(pre->right);
	}
}

void AvlTree::preOrder(std::ostream &out, treeNode* pre) {
	if (pre != NULL) {
		out << pre->data << "[label=\"" << pre->data
			<< "\\n" << avlHeight(pre) << "\"];\n";
		if (pre->left)
			out << pre->data << " -> "
			<< pre->left->data << ";\n";
		if (pre->right)
			out << pre->data << " -> "
			<< pre->right->data << ";\n";
		preOrder(out, pre->left);
		preOrder(out, pre->right);
	}
}

std::string AvlTree::dot() {
	std::stringstream stm;
	stm << "digraph {\n";
	preOrder(stm, root);
	stm << "}\n";
	return stm.str();
}

void AvlTree::display() {
	std::ofstream out("out.gv");
	out << dot() << std::endl;
	out.close();
	ssystem("dot -Tpng out.gv -o out.png");
	ssystem("eog out.png");
}


#endif

