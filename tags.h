#ifndef TAGLLIST
#define TAGLLIST

// Linked list pointing to transactions matching a specific tag
struct taggedTransLList
{
       struct transLList *trans;
       struct taggedTransLList *next;
       struct taggedTransLList *prev;
};

// Linked list of transactions
struct transLList
{
       unsigned long ID;
       int year;
       char month;
       char day;
       char *description;
       float value;
       struct transLList *next;
       struct transLList *prev;
};

struct keyWord
{
       char *name;
       struct keyWord *next;
       struct keyWord *prev;
       struct tagLList *parent;
};

// Linked list of tags
struct tagLList
{
       char *name;
       struct keyWord *child;
       struct tagLList *next;
       struct tagLList *prev;
       struct taggedTransLList *transList;
       
};

#endif

