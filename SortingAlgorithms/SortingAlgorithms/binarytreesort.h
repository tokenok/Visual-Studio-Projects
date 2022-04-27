#pragma once

#include <set>       // for multiset
#include <algorithm> // for copy

template<class T, class C>
void binarytreesort(T _First, T _Last, C compare) {
	std::multiset<typename std::iterator_traits<T>::value_type, C> tree(_First, _Last, compare);
	std::copy(tree.begin(), tree.end(), _First);
}

template<class T>
void binarytreesort(T _First, T _Last) {
	binarytreesort(_First, _Last, std::less<>());
}

/* AVL node */
template <class T>
class AVLnode {
public:
	T key;
	int balance;
	AVLnode *left, *right, *parent;

	AVLnode(T k, AVLnode *p) : key(k), balance(0), parent(p),
		left(NULL), right(NULL) {}

	~AVLnode() {
		delete left;
		delete right;
	}
};

/* AVL tree */
template <class T>
class AVLtree {
public:
	AVLtree(void);
	~AVLtree(void);

	bool insert(T key);
	void deleteKey(const T key);
	void printBalance();
	void inOrder();
	void preOrder();
	void postOrder();
	AVLnode<T>* getRoot();

private:
	AVLnode<T> *root;

	AVLnode<T>* rotateLeft(AVLnode<T> *a);
	AVLnode<T>* rotateRight(AVLnode<T> *a);
	AVLnode<T>* rotateLeftThenRight(AVLnode<T> *n);
	AVLnode<T>* rotateRightThenLeft(AVLnode<T> *n);
	void rebalance(AVLnode<T> *n);
	int height(AVLnode<T> *n);
	void setBalance(AVLnode<T> *n);
	void printBalance(AVLnode<T> *n);
	void inOrder(AVLnode<T> *t);
	void preOrder(AVLnode<T> *t);
	void postOrder(AVLnode<T> *t);
//	void clearNode(AVLnode<T> *n);
};

/* AVL class definition */
template<class T>
AVLnode<T>* AVLtree<T>::getRoot() {
	return root;
}

template <class T>
void AVLtree<T>::rebalance(AVLnode<T> *n) {
	setBalance(n);

	if (n->balance == -2) {
		if (height(n->left->left) >= height(n->left->right))
			n = rotateRight(n);
		else
			n = rotateLeftThenRight(n);
	}
	else if (n->balance == 2) {
		if (height(n->right->right) >= height(n->right->left))
			n = rotateLeft(n);
		else
			n = rotateRightThenLeft(n);
	}

	if (n->parent != NULL) {
		rebalance(n->parent);
	}
	else {
		root = n;
	}
}

template <class T>
AVLnode<T>* AVLtree<T>::rotateLeft(AVLnode<T> *a) {
	AVLnode<T> *b = a->right;
	b->parent = a->parent;
	a->right = b->left;

	if (a->right != NULL)
		a->right->parent = a;

	b->left = a;
	a->parent = b;

	if (b->parent != NULL) {
		if (b->parent->right == a) {
			b->parent->right = b;
		}
		else {
			b->parent->left = b;
		}
	}

	setBalance(a);
	setBalance(b);
	return b;
}

template <class T>
AVLnode<T>* AVLtree<T>::rotateRight(AVLnode<T> *a) {
	AVLnode<T> *b = a->left;
	b->parent = a->parent;
	a->left = b->right;

	if (a->left != NULL)
		a->left->parent = a;

	b->right = a;
	a->parent = b;

	if (b->parent != NULL) {
		if (b->parent->right == a) {
			b->parent->right = b;
		}
		else {
			b->parent->left = b;
		}
	}

	setBalance(a);
	setBalance(b);
	return b;
}

template <class T>
AVLnode<T>* AVLtree<T>::rotateLeftThenRight(AVLnode<T> *n) {
	n->left = rotateLeft(n->left);
	return rotateRight(n);
}

template <class T>
AVLnode<T>* AVLtree<T>::rotateRightThenLeft(AVLnode<T> *n) {
	n->right = rotateRight(n->right);
	return rotateLeft(n);
}

template <class T>
int AVLtree<T>::height(AVLnode<T> *n) {
	if (n == NULL)
		return -1;
	return 1 + std::max(height(n->left), height(n->right));
}

template <class T>
void AVLtree<T>::setBalance(AVLnode<T> *n) {
	n->balance = height(n->right) - height(n->left);
}

template <class T>
void AVLtree<T>::printBalance(AVLnode<T> *n) {
	if (n != NULL) {
		printBalance(n->left);
		std::cout << n->balance << " ";
		printBalance(n->right);
	}
}

template <class T>
AVLtree<T>::AVLtree(void) : root(NULL) {}

template <class T>
AVLtree<T>::~AVLtree(void) {
	delete root;
}

template <class T>
bool AVLtree<T>::insert(T key) {
	if (root == NULL) {
		root = new AVLnode<T>(key, NULL);
	}
	else {
		AVLnode<T>
			*n = root,
			*parent;

		while (true) {
			/*if (n->key == key)
				return false;*/

			parent = n;

			bool goLeft = key < n->key;
			n = goLeft ? n->left : n->right;

			if (n == NULL) {
				if (goLeft) {
					parent->left = new AVLnode<T>(key, parent);
				}
				else {
					parent->right = new AVLnode<T>(key, parent);
				}

				rebalance(parent);
				break;
			}
		}
	}

	return true;
}

template <class T>
void AVLtree<T>::deleteKey(const T delKey) {
	if (root == NULL)
		return;

	AVLnode<T>
		*n = root,
		*parent = root,
		*delNode = NULL,
		*child = root;

	while (child != NULL) {
		parent = n;
		n = child;
		child = delKey >= n->key ? n->right : n->left;
		if (delKey == n->key)
			delNode = n;
	}

	if (delNode != NULL) {
		delNode->key = n->key;

		child = n->left != NULL ? n->left : n->right;

		if (root->key == delKey) {
			root = child;
		}
		else {
			if (parent->left == n) {
				parent->left = child;
			}
			else {
				parent->right = child;
			}

			rebalance(parent);
		}
	}
}

template <class T>
void AVLtree<T>::printBalance() {
	printBalance(root);
	std::cout << std::endl;
}

template <class T>
void AVLtree<T>::inOrder(AVLnode<T> *t) {
	if (t == NULL)
		return;
	inOrder(t->left);
	std::cout << t->key << " ";
	inOrder(t->right);
}

template <class T>
void AVLtree<T>::preOrder(AVLnode<T> *t) {
	if (t == NULL)
		return;
	std::cout << t->key << " ";
	preOrder(t->left);
	preOrder(t->right);
}

template <class T>
void AVLtree<T>::postOrder(AVLnode<T> *t) {
	if (t == NULL)
		return;
	postOrder(t->left);
	postOrder(t->right);
	std::cout << t->key << " ";
}

template <class T>
void AVLtree<T>::inOrder() {
	inOrder(root);
}

template <class T>
void AVLtree<T>::preOrder() {
	preOrder(root);
}

template <class T>
void AVLtree<T>::postOrder() {
	postOrder(root);
}

template<class It, class T>
void AVLTreeSortHelper(It _First, AVLnode<T> *n, unsigned& count) {
	if (n == NULL)
		return;
	AVLTreeSortHelper(_First, n->left, count);
	*(_First + count) = n->key;
	printf("%d ", n->key);
	++count;
	AVLTreeSortHelper(_First, n->right, count);
}

template<class T, class C>
void AVLTreeSort(T _First, T _Last, C compare) {
	AVLtree<typename std::iterator_traits<T>::value_type> tree;

	auto _Pos = _First;
	while (_Pos != _Last) {
		tree.insert(*_Pos);
		++_Pos;
	}

	unsigned count = 0;
	AVLTreeSortHelper(_First, tree.getRoot(), count);
}



