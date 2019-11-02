#ifndef MAIN
#define MAIN

// Routine of function calls to add keyword to a tag
int addKeywordRoutine(char *tagKeywordString, int verbose);

// Routine of function calls to print first and last dates in transaction list
int printTransInterval(int verbose);

// Routine of function call to add tag
int addTagRoutine(char *tagStr, int verbose);

// Routine of function call to print out all tags
int printTagsRoutine(int verbose);

// Routine of function call to print out all keywords in a tag
int printKeyWordsRoutine(char *tagStr, int verbose);

// Routine of function calls to append bankfile to existing transaction file
int appendRoutine(char *fileName, int verbose);

// Routine of function call to write a tab file "fileName" of all tag costs
int writeTagCostTabRoutine(char *fileName, int verbose);

#endif