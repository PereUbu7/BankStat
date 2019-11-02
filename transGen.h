#ifndef TRANSGEN
#define TRANSGEN

#include <stdio.h>

// Reads a line in a bank file and extracts all information. Returns 0 when done or -1 when a line coudn't be read.
int readLine(FILE *file, unsigned long *transID, int *day1, int *day2, int *month1, int *month2, int *year1, int *year2, char *transText, float *transAmount1, float *transAmount2, int verbose);

// Uses readLine to read bank transaction file and stores info in a linked list.
// *transListStart=NULL if starting new list. To append a list, *transListStart points to last element.
//  Returns pointer to first element in list
struct transLList *readBankFileToLList(FILE *bankFile, struct transLList *transListStart, int verbose);

// Returns pointer to first element in linked list
struct transLList *findFirstElement(struct transLList *inElement, int verbose);

// Returns pointer to last element in linked list
struct transLList *findLastElement(struct transLList *inElement, int verbose);

// Takes a linked list of transactions and writes it as text into *fp. Returns 0 on succsess and -1 if list pointer transaction == NULL
int writeTransFromLList(FILE *fp, struct transLList *transaction, int verbose);

// Compare dates in two different transaction elements. Returns -1 if equal, 0 if trans2 < trans1 and 1 if trans1 < trans2.
int cmpTransDates(struct transLList *trans1, struct transLList *trans2, int verbose);

// Reads a list of transactions and stores it into a linked list. Returns pointer to first element
struct transLList *readTransToLList(FILE *fp, int verbose);

// Returns number of elements in list
int transListLen(struct transLList *list, int verbose);

// Appends *listToAppend with data in bank file fileName. Returns pointer to first element in new list
struct transLList *appendTransLList(char *fileName, struct transLList *listToAppend, int verbose);

// Frees all elements in a list of transactions. Return number of elements freed or -1 if unsuccessfull
int freeTransList(struct transLList *trans, int verbose);


#endif