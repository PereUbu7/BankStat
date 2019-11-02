#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tags.h"
#include "tagNav.h"
#include "tagGen.h"

// ------------------------------ Functions --------------------------------


// Takes a list of tags and a list of transactions. Returns number of matches found
int findTaggedElements(struct tagLList *tagList, struct transLList *transList, int verbose)
{
       
       struct keyWord *currentKeyWord = tagList->child;
       struct taggedTransLList *currentTaggedTransList = NULL;
       struct transLList *foundTrans = NULL, *inTransList = transList;
       struct tagLList *inTagList = tagList;
       
       int count = 0;
       
       if(verbose) printf("Entering findTaggedElements\n");
       
       if( transList == NULL || tagList == NULL )
           return(0);
           
               
       while(1)
       {
               //printf("currentKeyWord: %s tag:%s trans: %s\n", currentKeyWord->name, tagList->name, transList->description);        
               foundTrans = findKeyWord(currentKeyWord->name, transList, verbose);
               //if(foundTrans != NULL) printf("Found:%s %f\t", foundTrans->description, foundTrans->value);
               //if(foundTrans == NULL)
               //printf("Found trans = NULL\n\n");
               
               // If found a match, go to next to continue search
               if( foundTrans != NULL )
               {   // Go to next transaction to continue search from here
                   transList = foundTrans->next;
                   
                   // If first tagged transaction
                   if( currentTaggedTransList == NULL )
                   {   
                       //printf("First tagged transaction\n");
                       // Allocate mem and set list to mem
                       currentTaggedTransList = malloc(sizeof(struct taggedTransLList));
                       currentTaggedTransList->prev = NULL;
                       tagList->transList = currentTaggedTransList;
                       
                       // Link tagged trans list element to trans element
                       currentTaggedTransList->trans = foundTrans;
                       count++;
                   }
                   else
                   {   // If not first, allocate memory and go to next
                       //printf("Not first tagged transaction\n");
                       currentTaggedTransList->next = malloc(sizeof(struct taggedTransLList));
                       currentTaggedTransList->next->prev = currentTaggedTransList;
                       currentTaggedTransList = currentTaggedTransList->next;
                       
                       // Link tagged trans list element to trans element
                       currentTaggedTransList->trans = foundTrans;
                       count++;
                   }
                   // Let it point to transaction
                   //currentTaggedTransList->trans = foundTrans;
               }
               else
               {
                   // if no found transactions, go to next keyword and reset transaction list
                   //printf("Go to next key word\n\n");
                   currentKeyWord = currentKeyWord->next;
                   transList = inTransList;
               }
               
               if( transList == NULL )
               {
                   // if last element in transaction list, go to next keyword and reset transaction list
                   //printf("Go to next key word\n\n");
                   currentKeyWord = currentKeyWord->next;
                   transList = inTransList;
               }
               
               // If no more key words in tag, but not last tag, go to next tag and reset keyword and trans list
               if( currentKeyWord == NULL && tagList != NULL )
               {   
                   //printf("Go to next tag:");
                   tagList = tagList->next;
                   // If not last tag
                   if( tagList != NULL )
                   {
                       currentKeyWord = tagList->child;
                       //printf(" %s\n\n", tagList->name);
                       currentTaggedTransList = NULL;
                   }
                   else
                   {   // if end of key words and end of tags: break loop and exit
                       //printf("Last tag and last key word\n");
                       break;
                   }
                   transList = inTransList;
               } 
       }

       findUntaggedElements(inTagList, inTransList, verbose);

       if(verbose) printf("Exiting findTaggedElements... count = %d\n", count);
       
       return(count);
}


// Takes a key word string and finds the first occurence of that string in a list of transactions in the  "description" string. 
// Returns pointer to found transaction
struct transLList *findKeyWord(char *keyWord, struct transLList *list, int verbose)
{
       int found = 1;
       struct transLList *ret = NULL;
       
       
       found = strstr(list->description, keyWord);
       
       //printf("findKeyWord\n");
       
       while(!found || list == NULL)
       {
			if( list->next != NULL )
				list = list->next;
			else
				break;
			
			found = strstr(list->description, keyWord);
			
       }
       
       // If a transaction was found, return it. Otherwise ret = NULL
       if(found)
       {
                ret = list;
                //printf("Found:%s %.2f\n", ret->description, ret->value);
       }
       
       return(ret);
}

// Takes a certain month and a tag and calculates total sum of transactions in that tag, that month
float calcMonthSumOfTag(char month, int year, struct tagLList *tag, int verbose)
{
      struct taggedTransLList *currentTrans = tag->transList;
      
      float sum = 0;

      while (currentTrans != NULL)
      {
          if (currentTrans->trans->month == month && currentTrans->trans->year == year && currentTrans->trans->value < 0)
              sum += currentTrans->trans->value;

          currentTrans = currentTrans->next;
      }

      //if(verbose) printf("calcMonthSumOfTag: %d-%d sum: %.2f\n", month, year, sum);

      return(sum);
}

// Takes a certain month and calculated total sum of costs in that month
float calcMonthSumOfTrans(char month, int year, struct transLList *trans, int verbose)
{
      float sum = 0;
      
      while(trans != NULL)
      {
                  if( trans->month == month && trans->year == year && trans->value < 0 )
                  sum += trans->value;
                  
                  trans = trans->next;
      }
      
      if(verbose) printf("calcMonthSumOfTran: %d-%d sum: %.2f\n", month, year, sum);
      
      return(sum);
}


// Returns pointer to tag with name "tagName" or NULL if none
struct tagLList *findTag(char *tagName, struct tagLList *tagList)
{

    while(1)
    {
            if( !strcmp(tagName, tagList->name) )
            break;
            
            tagList = tagList->next;
            
            if( tagList == NULL )
            return(NULL);
    }
    return(tagList);
}


// Returns pointer to keyword with name "keyWordName" or NULL if none
struct keyWord *findKeyWord_v2(char *keyWordName, struct keyWord *list, int verbose)
{
	while(1)
	{
		if( !strcmp(keyWordName, list->name) )
			break;
		
		list = list->next;
		
		if( list == NULL )
			return(NULL);
	}
	return(list);
}


// Takes a list of tags and a list of transactions. Returns number of no-matches found
int findUntaggedElements(struct tagLList *tagList, struct transLList *transList, int verbose)
{
       
       //struct keyWord *currentKeyWord = tagList->child;
       struct taggedTransLList *currentTaggedTransList = NULL;
       //struct transLList *foundTrans = NULL, *inTransList = transList;
       struct tagLList *untaggedTag;
       
       int count = 0;
         
       if(verbose) printf("Entering findUntaggedElements\n");
       
       if( transList == NULL || tagList == NULL )
           {
                     if(verbose) printf("findUntaggedElements: no tagList or transList. Exiting...\n");
                     return(0);
           }
   
       untaggedTag = findLastTag(tagList);
       
       tagList = findFirstTag(tagList);    
       transList = findFirstElement(transList, verbose);

       while(transList != NULL)
       {
               // If transaction is tagged, go to next
               if( checkTaggedStatus(tagList, transList, verbose) )
               {  
				   //printf("%s is tagged\n", transList->description);
                   //transList = transList->next;
				   ;
				   //printf("Adress:%d|%d|%s\n", transList, transList->ID, transList->description);
               }
               else
               { // Else, add transaction to "no tag" tag
				//printf("%s is not tagged\n", transList->description);
                 count++;
              
                 // If "no tag" tag don't already exist, create it
                 if( strcmp(untaggedTag->name, "no tag") )
                 {
					 //printf("findUntaggedElements: Creating \"no tag\"-tag\n");
                     untaggedTag = createNextTag(untaggedTag, "no tag", verbose);
                 }
                 
                 // If no tagged list in "no tag" tag, create first element
                 if( untaggedTag->transList == NULL )
                 {
					 //printf("findUntaggedElements: Creating first element in \"no tag\" list\n");
                     untaggedTag->transList = malloc(sizeof(struct taggedTransLList));
                     currentTaggedTransList = untaggedTag->transList;
                     currentTaggedTransList->prev = NULL;
                     currentTaggedTransList->next = NULL;
                     currentTaggedTransList->trans = transList;
                 }
                 else
                 { // Create new element
					//printf("findUntaggedElements: Creating new element\n");
                     currentTaggedTransList->next = malloc(sizeof(struct taggedTransLList));
                     currentTaggedTransList->next->prev = currentTaggedTransList;
                     currentTaggedTransList = currentTaggedTransList->next;
                     currentTaggedTransList->next = NULL;
                     currentTaggedTransList->trans = transList;
                 }
				 
               }
			   
			   transList = transList->next;
				//if(transList != NULL) printf("Adress:%d|%d|%s\n", transList, transList->ID, transList->description);
			   //else printf("Translist = NULL\n");

               
       }
       
       if(verbose) printf("Exiting findUntaggedElements... count = %d\n", count);
       
       return(count);
}

// Checks whether a transaction is tagged or not. Returns 1 if true or 0 if false
int checkTaggedStatus(struct tagLList *tagList, struct transLList *trans, int verbose)
{
    struct keyWord *currentKeyWord;
    
    currentKeyWord = tagList->child;
    
    //if(verbose) printf("checkTaggedStatus: Entering\n");
    
    while(tagList != NULL)
    {
                  while(currentKeyWord != NULL)
                  {
                                       // If found a match, return 0 (TRUE)
                                       if( strstr(trans->description, currentKeyWord->name) != NULL )
                                       {
                                        //if(verbose) printf("checkTaggedStatus: Transaction %s is tagged\n", trans->description);
                                        return(1);
                                       }
                                       
                                       currentKeyWord = currentKeyWord->next;
                  }
                  
                  // Go to next tag
                  tagList = tagList->next;
                  // Set key word to new tag
                  if(tagList != NULL)
                             currentKeyWord = tagList->child;
                  
    }
    
    //if(verbose) printf("checkTaggedStatus: Transaction %d/%d %.2f %s is NOT tagged\n", trans->day, trans->month, trans->value, trans->description);
    // Return 1 (FALSE) (We didn't find any match
    return(0);
}

// Prints all tags to the screen
void printTags(struct tagLList *tagList, int verbose)
{
	while( tagList != NULL )
	{
		printf("%s\n", tagList->name);
		tagList = tagList->next;
	}
}

// Prints all keywords in a certain tag to the screen
void printKeyWords(struct keyWord *keyWordList, int verbose)
{
	while( keyWordList != NULL )
	{
		printf("%s\n", keyWordList->name);
		keyWordList = keyWordList->next;
	}
}