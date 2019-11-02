#ifndef TAGNAV
#define TAGNAV
// Takes a key word string and finds the first occurence of that string in a list of transactions in the  "description" string. Returns pointer to found transaction
struct transLList *findKeyWord(char *keyWord, struct transLList *list, int verbose);

// Returns pointer to keyword with name "keyWordName" or NULL if none
struct keyWord *findKeyWord_v2(char *keyWordName, struct keyWord *list, int verbose);

// Takes a list of tags and a list of transactions. Returns number of mathces found
int findTaggedElements(struct tagLList *tagList, struct transLList *transList, int verbose);

// Takes a list of tags and a list of transactions. Returns number of no-matches found
int findUntaggedElements(struct tagLList *tagList, struct transLList *transList, int verbose);

// Checks whether a transaction is tagged or not. Returns 0 if true or 1 if false
int checkTaggedStatus(struct tagLList *tagList, struct transLList *trans, int verbose);

// Takes a certain month and a tag and calculates total sum of costs in that tag, that month
float calcMonthSumOfTag(char month, int year, struct tagLList *tag, int verbose);

// Takes a certain month and calculated total sum of costs in that month
float calcMonthSumOfTrans(char month, int year, struct transLList *trans, int verbose);

// Returns pointer to tag with name "tagName" or NULL if none
struct tagLList *findTag(char *tagName, struct tagLList *tagList);

// Prints all tags to the screen
void printTags(struct tagLList *tagList, int verbose);

// Prints all keywords in a certain tag to the screen
void printKeyWords(struct keyWord *keyWordList, int verbose);

// Returns pointer to first element in linked list
struct transLList *findFirstElement(struct transLList *inElement, int verbose);

#endif