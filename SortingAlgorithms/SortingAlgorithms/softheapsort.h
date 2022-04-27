#pragma once

#include "insertionsort.h"

/* File: softheap.h
 * Developer: Matt Millican, May 2016
 * ----------------------------------
 * Implementation of a soft heap. Rather than the version using binomial trees
 * introduced in Chazelle's original paper, this soft heap uses binary trees
 * according to the strategy outlined in Kaplan/Zwick, 2009.
 */

#include <stdlib.h>
#include <assert.h> // for assert
#include <math.h> // For log and ceil. Remember to link math library!

 /* An item in a soft heap tree node's list. */
template<class T>
struct cell {
	T elem;
	cell<T> *next;
};

/* A node in a tree in a soft heap. The node has access to its left and right children,
 * but does not need access to its parent. It contains a ckey (its priority), its rank,
 * the number of elements in its list, and its "size": a parameter defined such that
 * its list always contains Theta(size) elements so long as the node is not a leaf.
 * Its list is stored as a doubly linked list. */
template<class T>
struct node {
	node<T> *left, *right;
	cell<T> *first, *last;
	int ckey, rank, size, nelems;
};

/* Structure representing a binary tree in a soft heap's rootlist. The tree stores
 * its rank, which is the maximum possible height of its root (although the
 * root is not guaranteed to have that height at all times). The tree
 * is wired to its predecessor and successor in the rootlist, which have
 * rank less than and greater than this tree's rank, respectively.
 * The tree also has a pointer to its own root.
 *
 * Binary trees in a soft-heap are heap-ordered according to the "ckeys" of the nodes
 * in the trees. Each node stores a list of items under one ckey; the ckey is
 * an upper bound on the original priorities of all items in the node's list.
 * The final element of a softheap tree is a pointer "sufmin" to the tree of minimum
 * root ckey in the segment of the rootlist beginning at this tree.
 */
template<class T>
struct tree {
	tree<T> *prev, *next, *sufmin;
	node<T> *root;
	int rank;
};


/* Structure representing a soft heap. The soft heap object has access
 * to the first tree in its root list, the rank of the highest-order tree
 * in its root list, its error parameter epsilon, and the parameter
 * r(epsilon) that defines the maximum node rank for which a node
 * is guaranteed to contain only uncorrupted elements. */
template<class T>
struct softheap {
	tree<T> *first;
	int rank;
	double epsilon;
	int r;
};

/***************************************** UTILITY FUNCTIONS **************************************/

/* Function: leaf
 * --------------
 * Return true if and only if this soft heap tree node
 * has no children.
 */
template<class T>
static inline bool leaf(node<T> *x) {
	return (x->left == NULL && x->right == NULL);
}

/* Function: get_r
 * ---------------
 * Return the parameter r(epsilon) for this soft heap.
 * r is the largest integer such that a node of that rank
 * contains only uncorrupted elements.
 */
static inline int get_r(double epsilon) {
	return (int)ceil(-log(epsilon) / log(2)) + 5;
}

/* Function: swapLR
 * ----------------
 * Swap the left and right children of this node.
 */
template<class T>
static inline void swapLR(node<T> *x) {
	node<T> *tmp = x->left;
	x->left = x->right;
	x->right = tmp;
}

/* Function: get_next_size
 * -----------------------
 * Get the size of a soft heap node with given rank.
 * Given a parameter r(epsilon) for a soft heap, the size
 * of a node of rank k is 1 (if k <= r) or ceil(3/2 * size(k-1))
 * otherwise.
 */
static inline int get_next_size(int rank, int prevrank_size, int r) {
	if (rank <= r) return 1;
	return (3 * prevrank_size + 1) / 2;
}

/**************************************** HEAP & ITEM CREATION *************************************/

/* Function: addcell
 * -----------------
 * Creates a list cell containing the parameter element
 * and concatenates it to the end of the linked list pointed
 * to by listend.
 */
template<class T>
static cell<T> *addcell(T elem, cell<T> *listend) {
	cell<T>* c = (cell<T>*)malloc(sizeof(cell<T>));
	c->elem = elem;
	if (listend != NULL) listend->next = c;
	c->next = NULL;
	return c;
}

/* Function: makenode
 * ------------------
 * Constructs a rank-0 soft heap binary tree node containing just the parameter
 * element. Its ckey matches the element, since that element is the only
 * object in its list.
 */
template<class T>
static node<T> *makenode(T elem) {
	node<T> *x = (node<T>*)malloc(sizeof(node<T>));
	x->first = x->last = addcell<T>(elem, NULL);
	x->ckey = elem;
	x->rank = 0;
	x->size = x->nelems = 1;
	x->left = x->right = NULL;
	return x;
}

/* Function: maketree
 * ------------------
 * Constructs a soft heap binary tree consisting of exactly one node
 * housing the parameter element.
 */
template<class T>
static tree<T> *maketree(T elem) {
	tree<T> *Tree = (tree<T>*)malloc(sizeof(tree<T>));
	Tree->root = makenode(elem);
	Tree->prev = Tree->next = NULL;
	Tree->rank = 0;
	Tree->sufmin = Tree;
	return Tree;
}

/* Function: makeheap_empty
 * ------------------------
 * Constructs an empty soft heap with the provided error parameter.
 */
template<class T>
softheap<T>* makeheap_empty(double epsilon) {
	// Ensure error parameter is valid
	if (epsilon <= 0 || epsilon >= 1) return NULL;// error(1, 0, "Soft heap error parameter must fall in (0,1)");

	softheap<T>* s = (softheap<T>*)malloc(sizeof(softheap<T>));
	s->first = NULL;
	s->rank = -1; // Ensures that any insertion will just return the SH containing the inserted elem
	s->epsilon = epsilon;
	s->r = get_r(epsilon);
	return s;
}

/* Function: makeheap
 * ------------------
 * Construct a soft heap with error parameter epsilon containing element elem.
 * This is done by constructing a tree of rank 0 containing a single rank-0
 * node. The node has one item in its item list, which is the item inserted.
 */

template<class T>
softheap<T> *makeheap(T elem, double epsilon) {
	softheap<T> *s = makeheap_empty<T>(epsilon);
	s->first = maketree(elem);
	s->rank = 0;
	return s;
}

/* Function: destroy_node
 * ----------------------
 * Deallocates all the cells in this node's item list,
 * recursively destroys its left and right children,
 * then deallocates its memory. For use in destroy_heap.
 */
template<class T>
static void destroy_node(node<T> *treenode) {
	if (treenode == NULL) return;

	cell<T> *curr = treenode->first, *next;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}

	destroy_node(treenode->left);
	destroy_node(treenode->right);
	free(treenode);
}

/* Function: destroy_heap
 * ----------------------
 * Destroys this soft heap and deallocates all its associated memory
 * by iterating over its list of trees, destroying them all, and then
 * destroying the heap struct.
 */
template<class T>
void destroy_heap(softheap<T> *P) {
	if (P == NULL) return;

	tree<T> *curr = P->first, *next;
	while (curr != NULL) {
		next = curr->next;
		destroy_node(curr->root);
		free(curr);
		curr = next;
	}

	free(P);
}


/************************************ HEAP STRUCTURE MANIPULATION *********************************/

/* Function: moveList
 * ------------------
 * Remove the item list of src and append it to the end
 * of the item list of dst.
 */
template<class T>
static void moveList(node<T> *src, node<T> *dst) {
	assert(src->first != NULL);
	if (dst->last != NULL) dst->last->next = src->first;
	else dst->first = src->first;
	dst->last = src->last;

	dst->nelems += src->nelems;
	src->nelems = 0;
	src->first = src->last = NULL;
}

/* Function: sift
 * --------------
 * The primary reorganizational strategy of the soft heap, called whenever
 * a non-leaf soft heap tree node has fewer items in its list than it should
 * according to its rank. The parameter node x steals the item list and ckey
 * of whichever child has lower ckey, which pushes the length of its list above
 * its size paremeter while maintaining the heap property with respect to ckeys.
 * Then, to repair the child (which is now deficient as x once was), we recursively
 * call sift on the child (unless it was a leaf, in which case it cannot be repaired).
 * Once x's child has been repaired or destroyed, x itself may or may not still be
 * deficient; if it is still deficient and has not become a leaf, we repeat the process
 * of stealing from children and recursively repairing children until x is repaired or a leaf.
 */
template<class T>
static void sift(node<T> *x) {
	while (x->nelems < x->size && !leaf(x)) {
		// For simplicity, switch left and right children so that left child exists & has smaller ckey
		if (x->left == NULL || (x->right != NULL && x->left->ckey > x->right->ckey)) swapLR(x);
		moveList(x->left, x); // concat left's list to x's to replenish x
		x->ckey = x->left->ckey;

		// if left was a leaf, it can't be repaired, so destroy it
		if (leaf(x->left)) {
			free(x->left);
			x->left = NULL;
		}
		else {
			sift(x->left);
		}
	} // Repeat as necessary until x is repaired or until x is a leaf and no more repairs are possible
}

/* Function: combine
 * -----------------
 * Another important restructuring operation, used whenever we merge two trees of equal rank.
 * Creates a new node z with children x and y and rank 1 + rank(x), sets its size parameter,
 * and then fills its list by sifting through its children.
 */
template<class T>
static node<T> *combine(node<T> *x, node<T> *y, int r) {
	node<T> *z = (node<T>*)malloc(sizeof(node<T>));
	z->left = x;
	z->right = y;
	z->rank = x->rank + 1;
	z->nelems = 0;
	z->first = z->last = NULL;

	z->size = get_next_size(z->rank, x->size, r);
	sift(z);
	return z;
}

/* Function: insert_tree
 * ---------------------
 * Inserts a tree from the rootlist of some external heap into the rootlist of
 * into_heap, immediately before the tree pointed to by successor.
 * Wires it into the pointer structure of the heap as necessary, including
 * making it the first tree of into_heap if the tree pointed to by successor
 * has no predecessors.
 */
template<class T>
static void insert_tree(softheap<T> *into_heap, tree<T> *inserted, tree<T> *successor) {
	inserted->next = successor;

	if (successor->prev == NULL) into_heap->first = inserted;
	else successor->prev->next = inserted;
	inserted->prev = successor->prev;
	successor->prev = inserted;
}

/* Function: remove_tree
 * ---------------------
 * Removes the soft heap tree pointed to by removed from the parameter heap.
 * This entails wiring its predecessor and successor in the heap's rootlist
 * to each other (if they exist) and setting the heap's first tree to be
 * the removed tree's successor if the removed tree was the first in the rootlist.
 */
template<class T>
static void remove_tree(softheap<T> *outof_heap, tree<T> *removed) {
	if (removed->prev == NULL) outof_heap->first = removed->next;
	else removed->prev->next = removed->next;
	if (removed->next != NULL) removed->next->prev = removed->prev;
}

/* Function: update_suffix_min
 * ---------------------------
 * Updates the sufmin pointers of T and all trees preceding T in T's rootlist.
 * This should be done whenever heap restructuring affects a segment of the
 * rootlist ending at T, i.e. if an element is extracted from T, if T is the
 * final tree created by a soft heap meld, or if T's successor is removed.
 * Whenever any of these occur, the segment of the heap ending at T may have
 * a new root of minimum ckey, meaning every sufmin pointer until T must be edited.
 * Given the recursive definition of a sufmin pointer this is easy to revise by
 * moving backwards from T.
 */
template<class T>
static void update_suffix_min(tree<T> *Tree) {
	while (Tree != NULL) {
		if (Tree->next == NULL || Tree->root->ckey <= Tree->next->sufmin->root->ckey) Tree->sufmin = Tree;
		else Tree->sufmin = Tree->next->sufmin;
		Tree = Tree->prev;
	}
}

/* Function: merge_into
 * --------------------
 * The first step of soft heap melding. Given a soft heap P whose rank is no more
 * than that of heap Q, walk through the root lists of both heaps, placing each tree
 * from P immediately before the first tree of Q with equal or greater rank.
 */
template<class T>
static void merge_into(softheap<T> *P, softheap<T> *Q) {
	tree<T> *currP = P->first, *currQ = Q->first;

	while (currP != NULL) {
		while (currQ->rank < currP->rank) currQ = currQ->next;
		// currQ is now the first tree in Q with rank >= currP. Insert currP before it.
		tree<T> *next = currP->next;
		insert_tree(Q, currP, currQ);
		currP = next;
	}
}

/* Function: repeated_combine
 * --------------------------
 * The second step of soft heap melding. Now that all trees of equal rank from the
 * original two heaps are adjacent in the larger heap, this process simulates
 * binary addition using a binomial heap-like strategy in which trees of equal rank
 * are merged and the results are "carried" until a vacancy is found for the rank
 * of the resulting combined tree. We only operate on the heap until we find
 * a tree of rank greater than the smaller (original) heap's rank that doesn't need
 * to be merged with its successor, at which point no successor trees can possibly
 * have partners and merging is no longer necessary.
 */
template<class T>
static void repeated_combine(softheap<T> *Q, int smaller_rank, int r) {
	tree<T> *curr = Q->first;

	while (curr->next != NULL) {
		bool two = (curr->rank == curr->next->rank);
		bool three = (two && curr->next->next != NULL && curr->rank == curr->next->next->rank);

		if (!two) { // only one tree of this rank
			if (curr->rank > smaller_rank) break; // no more combines to do and no carries
			else curr = curr->next;
		}
		else if (!three) { // exactly two trees of this rank
	   // combine them to make a carry, then delete curr->next. 
	   // carry may need to be merged with its next tree, so do not advance curr.
			curr->root = combine(curr->root, curr->next->root, r);
			curr->rank = curr->root->rank;
			tree<T> *tofree = curr->next;
			remove_tree(Q, curr->next); // will change what curr->next points to
			free(tofree);
		}
		else { // exactly three trees of this rank
	   // skip the first so that we can combine the second and third to form a carry
			curr = curr->next;
		}
	}

	if (curr->rank > Q->rank) Q->rank = curr->rank; // Q might have a new highest-rank tree
	update_suffix_min(curr); // this is final tree affected by merge, so update sufmin backwds from here
}

/* Function: extract_elem
 * ----------------------
 * Remove the first element from the item list of node x and return it.
 * To reflect this change, decrement x's nelems counter, change the
 * element it points to as the first item, rewire the prev pointer
 * of the new first element to NULL (if it exists), and reset the
 * last pointer of x if the new list has one or no items.
 */
template<class T>
static int extract_elem(node<T> *x) {
	assert(x->first != NULL);
	cell<T> *todelete = x->first;
	T result = todelete->elem;

	x->first = todelete->next;
	if (x->first == NULL) x->last = NULL;
	else if (x->first->next == NULL) x->last = x->first;

	free(todelete);
	x->nelems--;
	return result;
}

/*************************************** CLIENT-SIDE OPERATIONS ************************************/

/* Function: empty
 * ---------------
 * Returns true if and only if P contains no trees,
 * i.e. it contains no elements.
 */
template<class T>
bool empty(softheap<T> *P) {
	return P->first == NULL;
}

/* Function: insert
 * ----------------
 * Put a new element into soft heap P. If P is nonempty, this can be accomplished
 * by creating a new soft heap for the parameter and melding it into P. However,
 * if P is empty, this strategy will destroy P and leave the client with a freed
 * pointer, so instead we directly insert a new tree containing elem into P's rootlist
 * and set its rank to 0.
 */
template<class T>
void insert(softheap<T> *P, T elem) {
	if (empty(P)) {
		P->first = maketree(elem);
		P->rank = 0;
	}
	else meld(P, makeheap(elem, P->epsilon));
}

/* Function: meld
 * --------------
 * Combine all elements of soft heaps P and Q into a new conglomerate heap,
 * destructively modifying P and Q. Return the result. This is implemented
 * by executing a merge_into to push all elements from the lower-rank heap
 * into the higher-rank heap, then calling repeated-combine to combine
 * all trees of duplicate rank.
 */
template<class T>
softheap<T> *meld(softheap<T> *P, softheap<T> *Q) {
	// Do not allow melding if the soft heaps don't seem to have the same error parameter.
	double max_eps = std::max(P->epsilon, Q->epsilon), min_eps = std::min(P->epsilon, Q->epsilon);
	double eps_off = 1 - min_eps / max_eps;
	if (eps_off > 0.001) return NULL;//error(1, 0, "Tried to combine soft heaps with different epsilons");

	// If both softheaps empty, just destroy one and return the other
	if (empty(P) && empty(Q)) {
		free(P);
		return Q;
	}

	softheap<T> *result;
	if (P->rank >= Q->rank) { // meld Q into P
		merge_into(Q, P);
		repeated_combine(P, Q->rank, P->r);
		free(Q);
		result = P;
	}
	else { // meld P into Q
		merge_into(P, Q);
		repeated_combine(Q, P->rank, Q->r);
		free(P);
		result = Q;
	}

	return result;
}

/* Function: extract_min
 * ----------------------
 * Extract and return an element from the node of minimum ckey
 * in the soft heap.
 */
template<class T>
int extract_min(softheap<T> *P) {
	int filler; // I'm just here to prevent code duplication
	return extract_min_with_ckey(P, &filler);
}

/* Function: extract_min_with_ckey
 * -------------------------------
 * Extract and return an element from the node of minimum ckey
 * in the soft heap, and store that ckey in the space pointed to
 * by ckey_into. The node of minimum ckey is the root of some
 * tree in the heap, by the heap property invariant. This tree
 * is pointed to by the sufmin pointer of the first tree in the rootlist.
 * After removing that element from the root, we check whether it is now
 * size-deficient. If so, we sift it (if it has children), ignore it
 * (if it has no children but is not empty), or destroy the tree
 * it roots (if it has no children and is empty). Once this is done, we
 * update the sufmin pointers of T and all its predecessors
 * (or just T's predecessors if T was removed).
 */
template<class T>
T extract_min_with_ckey(softheap<T> *P, int *ckey_into) {
	if (empty(P)) return -1;//error(1, 0, "Tried to extract an element from an empty soft heap");

	tree<T> *Tree = P->first->sufmin; // tree with lowest root ckey
	node<T> *x = Tree->root;
	T e = extract_elem(x);
	*ckey_into = x->ckey;

	if (x->nelems <= x->size / 2) { // x is deficient; rescue it if possible
		if (!leaf(x)) {
			sift(x);
			update_suffix_min(Tree);
		}
		else if (x->nelems == 0) { // x is a leaf and empty; it must be destroyed
			free(x);
			remove_tree(P, Tree);

			if (Tree->next == NULL) { // we removed the highest-ranked tree; reset heap rank and clean up
				if (Tree->prev == NULL) P->rank = -1; // Heap now empty. Rank -1 is sentinel for future melds
				else P->rank = Tree->prev->rank;
			}

			if (Tree->prev != NULL) update_suffix_min(Tree->prev);
			free(Tree);
		}
	}

	return e;
}

template<class T, class C>
inline void softheapsort(T _First, T _Last, C compare) {
	auto length = _Last - _First;
	if (length == 1) return;
	double epsilon = 1.0 / (double)length;
	epsilon = .99;
	if (epsilon <= 0) epsilon = DBL_MIN;

	softheap<T::value_type> *sh = makeheap_empty<T::value_type>(epsilon);
	for (T _Pos = _First; _Pos != _Last; ++_Pos)
		insert(sh, *_Pos);
	for (T _Pos = _First; _Pos != _Last; ++_Pos) 
		*_Pos = extract_min(sh);
	destroy_heap(sh);
	//insertionsort(_First, _Last, compare);
}

template<class T>
inline void softheapsort(T _First, T _Last) {
	softheapsort(_First, _Last, std::less<>());
}


























