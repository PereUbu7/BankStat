#ifndef TAGGEN
#define TAGGEN

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tags.h"

// Creates a linked list tree from file. Returns pointer to first tag
struct tagLList *readTagToLList(FILE *fp, int verbose);

// Writes a list tree of tags and keywords to file *fp. Returns true on success and false on failure
int writeTagLListToFile(FILE *fp, struct tagLList *tagList, int verbose);

// Create a new tag next after "tag" with name in *name. Set new relations. Return pointer to new tag
struct tagLList *createNextTag(struct tagLList *tag, char *name, int verbose);

// Create a new key word at the beginning of a keyword list, as child to tag
struct keyWord *createFirstKeyWord(struct tagLList *tag, char *name, int verbose);

// Create a new key word next after "word" with name in *name. Set new relations. Return pointer to new key word
struct keyWord *createNextKeyWord(struct keyWord *word, char *name, int verbose);

// Frees a linked list of keyWord structs and returns the amount freed
int freeKeyWordList(struct keyWord *word, int verbose);

// Frees a linked list of tagLList structs and returns the amount of tags freed
int freeTagList(struct tagLList *tag, int verbose);

// Frees a list of tragged transactions (DOES NOT FREE THE TRANSACTIONS THEMSELVES. ONLY THE TAGGED LIST) Returns # of freed elements
int freeTaggedTransList(struct taggedTransLList *taggedTrans, int verbose);

// Takes a linked list of tags, with child key words and writes it as text into *fp, returns 0 so far.
int writeTagFromLList(FILE *fp, struct tagLList *tags, int verbose);

// Takes a linked list of transactions and sorts it according to the gnome sorting algorithm. Returns pointer to new start element
struct taggedTransLList *gnomeSortTaggedTransLList(struct taggedTransLList *listStart, int verbose);

// Compare dates in two different transaction elements. Returns -1 if equal, 0 if trans2 < trans1 and 1 if trans1 < trans2.
int cmpTaggedTransDates(struct taggedTransLList *trans1, struct taggedTransLList *trans2, int verbose);

// Swaps trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal
int swapAdjacentTaggedTransElements(struct taggedTransLList *trans1, struct taggedTransLList *trans2, int verbose);

// Returns pointer to first element in linked list
struct taggedTransLList *findFirstTaggedElement(struct taggedTransLList *inElement, int verbose);

// Writes a table into file "fileName" or file pointer fp (cannot take both!) 
// -If "fileName": open new file and write a line.
// -If fp: append fp with line of values.
FILE *writeCalcMonthTabSeparatedTable(char *fileName, FILE *fp, int close, int verbose, int month, int year, int num,  float argListOfValues[]);

// Writes a file "fileName" with all monthly cost sums of tag "tagName"
int writeMonthSumOfTag(char *fileName, char *tagName, struct tagLList *tagList, int verbose);

// Writes a file "fileName" with all monthly cost sums of all transactions
int writeMonthSumOfTrans(char *fileName, struct transLList *transList, int verbose);

// Writes a file "fileName" with monthly ratios of all tags to total sum of costs. 
int writeMonthSumOfTags(char *fileName, struct transLList *transList, struct tagLList *tagList, int verbose);

struct tagLList *findFirstTag(struct tagLList *tagList);

struct tagLList *findLastTag(struct tagLList *tagList);

// Returns pointer to last element in keyword list
struct keyWord *findLastKeyword(struct keyWord *inKeyword);

int tagListLen(struct tagLList *tagList, int verbose);

void initHtmlTableFile(FILE *fp);

void endHtmlTableFile(FILE *fp);

void writeLabelToHtmlTableFile(FILE *fp, int month, int year);

void writeValueToHtmlTableFile(FILE *fp, float value);

#endif