#ifndef TRANSMOD
#define TRANSMOD

// Searches for elements with equal ID:s and removes one of them
int removeTwinTransElements(struct transLList *trans, int verbose);

// Removes an element from the list and returns pointer to precedent element
struct transLList *removeTransElement(struct transLList *trans, int verbose);

// Takes a linked list of transactions and sorts it according to the gnome sorting algorithm. Returns pointer to new start element
struct transLList *gnomeSortTransLList(struct transLList *listStart, int verbose);

// Swaps trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal
int swapAdjacentTransElements(struct transLList *trans1, struct transLList *trans2, int verbose);

#endif