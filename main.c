#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tags.h"
#include "main.h"
#include "tagNav.h"
#include "tagGen.h"
#include "transMod.h"
#include "transGen.h"

//--------------------------------------------------------------------------
//-------------------- Functions -------------------------------------------
//--------------------------------------------------------------------------


// Routine of function calls to add keyword to a tag
int addKeywordRoutine(char *tagKeywordString, int verbose)
{
    FILE *tagFP;
    struct tagLList *tagList = NULL;
	struct tagLList *chosenTag;
	struct keyWord *newKeyword;
	char *tagStr,
		*keywordStr;
	
	// Parse arguent
	if( (tagStr = strtok(tagKeywordString, "|")) != NULL )
	{
		printf("Parsing: Found tag:%s\n", tagStr);
		
		if( (keywordStr = strtok(NULL, "|")) != NULL )
		{
			printf("Parsing: Found keyword:%s\n", keywordStr);
		}
		else
		{
			printf("No keyword - Quitting...\n");
			return -1;
		}
	}
	else
	{
		printf("No tag - Quitting...\n");
		return -1;
	}
	
	
	// Open tag file to read
	if( (tagFP = fopen("tags.txt", "rt")) == NULL)
	{
		if(verbose) perror("Error open tag file");
		return(-1);
	}
	
	// Open tag list
	tagList = readTagToLList(tagFP, verbose);
	
	// close tag file
	fclose(tagFP);
	
	
	if( (chosenTag = findTag(tagStr, tagList)) != NULL )
	{
		if( chosenTag->child == NULL)
		{
			newKeyword = createFirstKeyWord(chosenTag, keywordStr, verbose);
		}
		else
		{
			// Kolla så att keywordStr inte redan finns som keyword
			if( findKeyWord_v2(keywordStr, chosenTag->child, verbose) == NULL )
			{
				newKeyword = createNextKeyWord(findLastKeyword(chosenTag->child), keywordStr, verbose);
			}
			else
			{
				fprintf(stderr, "ERROR: Keyword \'%s\' already exists.\n", keywordStr);
				return(-1);
			}
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Tag \'%s\' doesn't exist.\n", tagStr);
		return(-1);
	}
	
	if( newKeyword != NULL )
	{
		printf("Added keyword:%s to tag:%s\n", newKeyword->name, chosenTag->name);
	}
	
	// Open tag file to write
	if( (tagFP = fopen("tags.txt", "wt")) == NULL)
	{
		if(verbose) perror("Error open tag file");
		return(-1);
	}
	
	// Writes a list tree of tags and keywords to file *fp. Returns true on success and false on failure
	if( writeTagLListToFile(tagFP, tagList, verbose) )
	{
		if(verbose) printf("wrote tag list to file:tags.txt\n");
	}
	else
	{
		fprintf(stderr, "ERROR: Could not write tag list to file:tags.txt\n");
	}
	
	fclose(tagFP);
	
	
	// Free list of tags
	freeTagList(tagList, verbose);
	
    
    return(0);  
}

// Routine of function calls to print first and last dates in transaction list
int printTransInterval(int verbose)
{
    FILE *transFP;
    struct transLList *transList = NULL;

    // Open tag file to read
    if( (transFP = fopen("transactions.txt", "rt")) == NULL)
	{
        if(verbose) perror("Error open transaction file");
        return(-1);
    }
        
    // Open transaction list
    transList = readTransToLList(transFP, verbose);
	
    // close tag file
    fclose(transFP);
	
	// Sort transaction list
    transList = gnomeSortTransLList(findFirstElement(transList, verbose), verbose);
	
	// Print first transaction date
	transList = findFirstElement(transList, verbose);
	if(verbose) printf("Start:");
	printf("%4d-%02d-%02d\n", transList->year, transList->month, transList->day);
	
	// Print last transaction date
	transList = findLastElement(transList, verbose);
	if(verbose) printf("End:");
	printf("%4d-%02d-%02d\n", transList->year, transList->month, transList->day);
	
	
    // Free list of transactions
    if( freeTransList(transList, verbose) == -1 )
	{
		fprintf(stderr, "printTransInterval: Error while freeing linked transaction list.\n");
	}
    
    return(0);  
}

// Routine of function call to add tag
int addTagRoutine(char *tagStr, int verbose)
{
    FILE *tagFP;
    struct tagLList *tagList = NULL;
	struct tagLList *newTag;

    // Open tag file to read
    if( (tagFP = fopen("tags.txt", "rt")) == NULL)
	{
        if(verbose) perror("Error open tag file");
        return(-1);
    }
        
    // Open tag list
    tagList = readTagToLList(tagFP, verbose);
	
    // close tag file
    fclose(tagFP);
	
	// Go to last tag
	if( findTag(tagStr, tagList) == NULL )
	{
		// Add new tag to end of list
		newTag = createNextTag(findLastTag(tagList), tagStr, verbose);
		if( newTag->name != NULL )
		{
			printf("%s is added to tag list\n", newTag->name);
			
			// Open file for writing
			if( (tagFP = fopen("tags.txt", "wt")) == NULL)
			{
				if(verbose) perror("Error open tag file");
				return(-1);
			}
			
			// Write list tree to file
			if( writeTagLListToFile(tagFP, tagList, verbose) )
			{
				printf("Wrote successfully to file tags.txt\n");
			}
			else
			{
				fprintf(stderr, "Failed in writing to file tags.txt\n");
			}
			
			// Close file for writing
			fclose(tagFP);
		}
		else
		{
			fprintf(stderr, "ERROR while adding tag \'%s\'\n", tagStr);
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Tag \'%s\' already exists\n", tagStr);
	}
	
	
	
	
    // Free list of tags
    freeTagList(tagList, verbose);
    
    return(0);  
}

// Routine of function call to print out all tags
int printTagsRoutine(int verbose)
{
    FILE *tagFP;
    struct tagLList *tagList = NULL;

    // Open tag file to read
    if( (tagFP = fopen("tags.txt", "rt")) == NULL)
	{
        if(verbose) perror("Error open tag file");
        return(-1);
    }
        
    // Open tag list
    tagList = readTagToLList(tagFP, verbose);
	
    // close tag file
    fclose(tagFP);
	
	// Print all tags to screen
	printTags(tagList, verbose);
	
    // Free list of tags
    freeTagList(tagList, verbose);
    
    return(0);  
}

// Routine of function call to print out all keywords in a tag
int printKeyWordsRoutine(char *tagStr, int verbose)
{
    FILE *tagFP;
    struct tagLList *tagList = NULL;
	struct tagLList *chosenTag;

    // Open tag file to read
    if( (tagFP = fopen("tags.txt", "rt")) == NULL)
	{
        if(verbose) perror("Error open tag file");
        return(-1);
    }
        
    // Open tag list
    tagList = readTagToLList(tagFP, verbose);
	
    // close tag file
    fclose(tagFP);
	
	
	if( (chosenTag = findTag(tagStr, tagList)) != NULL )
	{
		printKeyWords(chosenTag->child, verbose);
	}
	else
	{
		printf("ERROR: Could not find tag: %s\n", tagStr);
	}
	
	
    // Free list of tags
    freeTagList(tagList, verbose);
    
    return(0);  
}

// Routine of function call to write a tab file "fileName" of all tag costs
int writeTagCostTabRoutine(char *fileName, int verbose)
{
    FILE *transFP, *tagFP, *costFP;
    struct transLList *transList = NULL;
    struct tagLList *tagList = NULL;
    
    // Open transaction file to read
    if( (transFP = fopen("transactions.txt", "rt")) == NULL ){
        if(verbose) perror("Error opening transaction file");
        return(-1);
        }
        
    // Open transaction list
    transList = readTransToLList(transFP, verbose);
    
    // close transaction file
    fclose(transFP);
    
    // Open tag file to read
    if( (tagFP = fopen("tags.txt", "rt")) == NULL){
        if(verbose) perror("Error open tag file");
        return(-1);
        }
        
    // Open tag list
    tagList = readTagToLList(tagFP, verbose);
    // close tag file
    fclose(tagFP);

    // Make list with tagged elements
    findTaggedElements(tagList, findFirstElement(transList, verbose), verbose);

    // Write file "fileName" with monthly cost sums of all tags in tabs
    writeMonthSumOfTags(fileName , transList, tagList, verbose);
    
    // Free list of transactions
    freeTransList(transList, verbose);
    
    // Free list of tags
    freeTagList(tagList, verbose);
    
    return(0);  
}

int appendRoutine(char *fileName, int verbose)
{
    FILE *transFP;
    struct transLList *transList = NULL;

    // Open transaction file to append 
    if( (transFP = fopen("transactions.txt", "rt")) == NULL ){
        if(verbose) perror("Error opening transaction file");
        return(-1);
        }
    
    // Open original transaction list
    transList = readTransToLList(transFP, verbose);
    
    // Close transaction file
    fclose(transFP);
    
    // Append bank list to transaction list
    if ( (transList = appendTransLList(fileName, transList, verbose)) == -1 )
    return(-1);
    
    // Remove any doubles
    printf("Removed %d elements.\n", removeTwinTransElements(transList, verbose));
    
    // Sort new list
    transList = gnomeSortTransLList(findFirstElement(transList, verbose), verbose);
    // Remove twin elements
    //
    
    // Open transaction file to be saved 
    if( (transFP = fopen("transactions.txt", "wt")) == NULL ){
        if(verbose) perror("Error opening transaction file");
        return(-1);
        }    
        
    // Save new list to file
    if( writeTransFromLList(transFP, transList, verbose) == 0 )
    printf("writeTransFromLList: Successfully wrote transactions.txt\n");
    
    // Free list of transactions
    freeTransList(transList, verbose);
    
    return(0);
}

int test(int verbose)
{

  struct tagLList *tagList;
  struct transLList *transList = NULL, *tempList = NULL, *tmp;
  struct transLList first;
  struct taggedTransLList *taggedTrans;
  
  char m =7;

  float summa;
  
    
  FILE *file;
  FILE *newFile;
  
  first.ID = 0;
  first.description = malloc(30);
  strcpy(first.description, "FORSTA");
  first.value = 1;
  first.year = 2016;
  first.month = 7;
  first.day = 2;
  first.prev = NULL;
  first.next = NULL;

  printf("------------STARTAR TEST()------------\n");

  // Open input file
  if ( (file = fopen("jul16.txt", "rt")) == NULL ){
     perror("Error opening file"); 
     return(-1);
     }

  transList = readBankFileToLList(file, NULL, verbose);

  fclose(file);
  
  //transList = appendTransLList("aug16.txt", transList, verbose);
  //transList = appendTransLList("sep16.txt", transList, verbose);
  //transList = appendTransLList("okt16.txt", transList, verbose);
  //transList = appendTransLList("nov16.txt", transList, verbose);
  //transList = appendTransLList("dec16.txt", transList, verbose);
  //transList = appendTransLList("jan17.txt", transList, verbose);

  //transList = gnomeSortTransLList(transList, verbose);
  
  //removeTwinTransElements(transList, verbose);
  

// Create file from linked list
//writeTagFromLList(newFile, &list1); 

//fclose(newFile);
  // Open output file
  if ( (newFile = fopen("out.txt", "r")) == NULL ){
     perror("Error opening file"); 
     return(-1);
     }  

// Create linked list from file
//tagList = readTagToLList(newFile, verbose);

fclose(file);
fclose(newFile);

  if ( (newFile = fopen("transactions.txt", "w")) == NULL ){
     perror("Error opening file"); 
     return(-1);
     }  
    
     
     //tempList = findFirstElement(transList, verbose);
   
//findTaggedElements(tagList, tempList, verbose);

//taggedTrans = gnomeSortTaggedTransLList(taggedTrans, verbose);

//writeMonthSumOfTag("MatTab.txt", "Mat", tagList, verbose);

//writeMonthSumOfTrans("transTab.txt", transList, verbose);

//writeMonthSumOfTags("SumsTab.txt" , transList, tagList, verbose);

writeTransFromLList(newFile, transList, verbose);
 
fclose(newFile);

freeTransList(transList, verbose);


     
//tempList = readTransToLList(file);

//swapAdjacentTransElements(tempList->next, tempList->next->next);
 
     
//writeTransFromLList(file, tempList);


// Free linked list
freeTagList(tagList, verbose);

system("PAUSE");

  return 0;
}

