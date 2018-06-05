#include <stdio.h>
#include <stdarg.h>
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
FILE *writeCalcMonthTable(char *fileName, FILE *fp, int close, int verbose, int month, int year, int num,  float argListOfValues[]);

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

// ----------- Prototypes from other file --------------------------------------

// Takes a certain month and a tag and calculates total sum of costs in that tag, that month
float calcMonthSumOfTag(char month, int year, struct tagLList *tag, int verbose);

// Takes a certain month and calculated total sum of costs in that month
float calcMonthSumOfTrans(char month, int year, struct transLList *trans, int verbose);

// Returns pointer to tag with name "tagName" or NULL if none
struct tagLList *findTag(char *tagName, struct tagLList *tagList);

// Returns pointer to last element in linked list
struct transLList *findLastElement(struct transLList *inElement, int verbose);

// Returns pointer to first element in linked list
struct transLList *findFirstElement(struct transLList *inElement, int verbose);

//--------------------------- Functions -----------------------------

// Creates a linked list tree from file. Returns pointer to first tag
struct tagLList *readTagToLList(FILE *fp, int verbose)
{
 struct keyWord *childPosition = NULL; 
 
 struct tagLList *tags, *startTag=NULL; 
 
 int tmpLineLength = 50, tmpWordLength = 50;
 int createRelation = 0;

 char tmpLine[ tmpLineLength ];
 char tmpWord[ tmpWordLength ];
 char *endBracketPos, *startBracketPos;
 
 if(verbose) printf("readTagToLList: Entering\n");
 
 while(1)
 {

          // Read a line. Hopefully a [tag] line
          if( fgets( tmpLine, tmpLineLength-1, fp ) == NULL )
           {
                 if(verbose) printf("readTagToLList: Exiting\n\n");

                 return(startTag);
           }

 
 // if line starts with '[' and has a ']' somewhere afterwards, then...
 if( tmpLine[0] == '[' && (endBracketPos = strchr((char*)tmpLine+1, ']')) != NULL )
 {
     // Copy string between brackets to tmpWord
     memcpy(tmpWord, (char*)tmpLine+1, endBracketPos-tmpLine-1);
     
     // Set terminating NULL-character after tmpWord
     tmpWord[endBracketPos-tmpLine-1] = '\0';
     
     // Create struct relation with new sibbling
     if(createRelation)
     {
                       tags = createNextTag(tags, tmpWord, verbose);
                       createRelation = 0;
     }
     else
     {
                       // Allocate mem for first tag struct
                       tags = malloc(sizeof(struct tagLList));
                       tags->name = malloc(strlen(tmpWord)+1);
                       tags->prev = NULL;
                       tags->next = NULL;
					   tags->transList = NULL;
                       startTag = tags;
     }
     
     // Copy string to new tag struct
     strcpy(tags->name, tmpWord);
     if(verbose) printf("tag:%s\n", tags->name);
     
     //---------------------------------------------------
     // Now we should start adding keyWords
     
     while(1)
     {
             // Read next line
             if( fgets( tmpLine, tmpLineLength-1, fp ) == NULL )
                 {
                        if(verbose) printf("Read error or EOF\n");
                         if(verbose) printf("readTagToLList: Exiting\n\n");

                         return(startTag);
                 } 
   
             // If new [tag] found; break little loop and go into big again
             if( tmpLine[0] == '[' && (endBracketPos = strchr((char*)tmpLine+1, ']')) != NULL )    
                 {
                            if(verbose) printf("Found unexcpected tag line - set relations\n");
                        // Make sure struct relations are set
                        createRelation = 1;   
                        childPosition = NULL; 
                        break;    
                 }           
             // else, if not a comment or newline; suspect keyWord
             else if( tmpLine[0] != '%' && tmpLine[0] != '\n' )
             {
                  // Remove the newline caracter at the end of tmpLine
                  tmpLine[strlen(tmpLine)-1] = '\0';
                  
                              // If first keyWord in tag
                              if( childPosition == NULL )
                              {
                               childPosition = malloc(sizeof(struct keyWord));
                               childPosition->prev = NULL;
                               childPosition->next = NULL;
                               childPosition->parent = tags;
                               tags->child = childPosition;
                               childPosition->name = malloc(strlen(tmpLine)+1);
                               strcpy(childPosition->name, tmpLine);
                              }
                              else
                              {
                               childPosition = createNextKeyWord(childPosition, tmpLine, verbose);      
                              }

                  if(verbose) printf("\tkeyWord: %s\n", childPosition->name);
             }
             else if(tmpLine[0] == '%' )
             {
                  if(verbose) printf("Comment\n");
             }  
             else if (tmpLine[0] == '\n' )
             {
                  childPosition = NULL;
                  createRelation = 1;
                  break; 
             }
     }
 }
 else
 createRelation = 0;

}

 if(verbose) printf("readTagToLList: Exiting\n\n");

 return(startTag);
}

// Writes a list tree of tags and keywords to file *fp. Returns true on success and false on failure
int writeTagLListToFile(FILE *fp, struct tagLList *tagList, int verbose)
{
	struct keyWord *currentKeyWord = NULL;
	struct tagLList *currentTag = tagList;
	
	printf("writeTagLListToFile: Entering\n");
	
	while( currentTag != NULL )
	{
		fprintf(fp, "[%s]\n", currentTag->name);
		
		currentKeyWord = currentTag->child;
		
		while( currentKeyWord != NULL )
		{
			fprintf(fp, "%s\n", currentKeyWord->name);
			
			currentKeyWord = currentKeyWord->next;
		}
		
		fprintf(fp, "\n");
		
		currentTag = currentTag->next;
	}
	
	printf("writeTagLListToFile: Exiting\n");
	return 1;
}

// Create a new tag next after "tag" with name in *name. Set new relations. Return pointer to new tag
struct tagLList *createNextTag(struct tagLList *tag, char *name, int verbose)
{
    tag->next = malloc(sizeof(struct tagLList));

    tag->next->prev = tag;
    
    tag = tag->next;
    
    tag->child = NULL;
    tag->next = NULL;
    tag->transList = NULL;
    tag->name = malloc(strlen(name)+1);
    strcpy(tag->name, name);
    
    return(tag);
}

struct keyWord *createNextKeyWord(struct keyWord *word, char *name, int verbose)
{
	
	//if(verbose) printf("createNextKeyWord: Entering\n");
	if( word == NULL )
	{
		printf("createNextKeyWord:ERROR! given keyword does not exist.\n");
		return(NULL);
	}
		word->next = malloc(sizeof(struct keyWord));
       
		word->next->prev = word;
		
		word->next->parent = word->parent;
       
		word = word->next;

		word->next = NULL;
		word->name = malloc(strlen(name)+1);
		strcpy(word->name, name);
	
	//if(verbose) printf("createNextKeyWord: Exiting\n");
       
       return(word);
}

struct keyWord *createFirstKeyWord(struct tagLList *tag, char *name, int verbose)
{
	struct keyWord *word;
	
	//if(verbose) printf("createFirstKeyWord: Entering\n");
	
		word = malloc(sizeof(struct keyWord));
       
		word->prev = NULL;
		word->parent = tag;
		word->next = NULL;
		word->name = malloc(strlen(name)+1);
		strcpy(word->name, name);
		
		tag->child = word;
	
	//if(verbose) printf("createFirstKeyWord: Exiting\n");
       
       return(word);
}

// Frees a linked list of keyWord structs and returns the amount freed
int freeKeyWordList(struct keyWord *word, int verbose)
{
       int numberOfFreedKeyWords = 0;
	   
       //if(verbose) printf("freeKeyWordList: Entering\n");
       
       if( word == NULL )
       return(0);
       
       while(1)
       {
               if( word->name != NULL )
               free(word->name);
               //if(verbose) printf("\tFreed keyWord name #%d. ", numberOfFreedKeyWords+1);
               
               // If word-next points to SOMETHING; suspect keyWord struct
               if( word->next != NULL )
               {
                word = word->next;
                free(word->prev);
                //if(verbose) printf("\tFreed keyWord #%d\n", numberOfFreedKeyWords+1);
                numberOfFreedKeyWords++;
               }
               else{
               break;}
       }
       if( word != NULL )
       free(word);
       //if(verbose) printf("\tFreed keyWord #%d\n", numberOfFreedKeyWords+1);
	   
	   //if(verbose) printf("freeKeyWordList: Exiting\n");
       
       return(numberOfFreedKeyWords+1);
}

// Frees a linked list of tagLList structs and returns the amount of tags freed
int freeTagList(struct tagLList *tag, int verbose)
{
       
       int numberOfFreedTags = 0;
       int numberOfFreedKeyWords = 0;
       int numberOfFreedTaggedTrans = 0;
       
       //if(verbose) printf("freeTagList: Entering\n");
       
       if( tag == NULL )
       return(0);
       
       while(1)
       {
               if( tag->name != NULL ){
               free(tag->name);
               //if(verbose) printf("Freed tag name #%d\n",numberOfFreedTags+1);
               }
               
               if( tag->child != NULL ){
               numberOfFreedKeyWords += freeKeyWordList(tag->child, verbose);
               }
               
               if( tag->transList != NULL){
               numberOfFreedTaggedTrans += freeTaggedTransList(tag->transList, verbose);
			   }
               
       
               // If tag-next points to SOMETHING; suspect tagLList struct
               if( tag->next != NULL )
               {
                tag = tag->next;
                free(tag->prev);
                //if(verbose) printf("Freed tag #%d\n",numberOfFreedTags+1);
                numberOfFreedTags++;
               }
               else
               break;
       }
       if( tag != NULL )
       free(tag);
       //if(verbose) printf("Freed tag #%d\nExiting freeTagList\n\n",numberOfFreedTags+1);
	   //if(verbose) printf("freeTagList: Exiting\n");
       
       return(numberOfFreedTags+1);
}



int writeTagFromLList(FILE *fp, struct tagLList *tags, int verbose) 
{    
    if(verbose)  printf("writeTagFromLList: Entering\n");    
    // se till att fp är på rätt ställe i filen
    rewind(fp);

    while(tags != NULL){
               
    // Writes "[name]\n" to file
    fprintf(fp, "[%s]\n", tags->name);
       
    //print to stdout as test
    if(verbose) printf("[%s]\n", tags->name);
    
    while(tags->child != NULL){
                            
                            // Write keyWord name to file
                            fprintf(fp, "%s\n", tags->child->name);
                            
                            // printf to stdout as test
                            if(verbose) printf("%s\n", tags->child->name);
                            
                            // Jump to next in keyWord list
                            tags->child = tags->child->next;
                            }
    // Write newline before next [tag} to file                            
    fprintf(fp, "\n");
    
    // Printf to stdout for test
    if(verbose) printf("\n");
    
    // Jump to next in list
    tags = tags->next;
    
    }
	
	if(verbose)  printf("writeTagFromLList: Exiting\n");  
    
    
   return(0);     
}

// Takes a linked list of transactions and sorts it according to the gnome sorting algorithm. Returns pointer to new start element
struct taggedTransLList *gnomeSortTaggedTransLList(struct taggedTransLList *listStart, int verbose)
{
       struct taggedTransLList *currentElement = NULL, *upperBound = listStart->next;
       
       if(verbose) printf("gnomeSortTaggedTransLList: Entering\n");
       
       while( upperBound != NULL )
       {
              currentElement = upperBound;

              while( currentElement->prev != NULL &&  !cmpTransDates(currentElement->prev->trans, currentElement->trans) )
              {

                     if( currentElement->prev->prev == NULL )
                     {
                         listStart = currentElement;
                         swapAdjacentTaggedTransElements(currentElement->prev, currentElement, verbose);
                         currentElement = listStart;
                     }
                     else
                     swapAdjacentTaggedTransElements(currentElement->prev, currentElement, verbose);
              }
              
              upperBound = upperBound->next;
       }
       
       if(verbose) printf("gnomeSortTaggedTransList: Exiting\n");
   
   return(findFirstTaggedElement(currentElement, verbose));    
       
}



// Swaps trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal
int swapTaggedTransElements(struct taggedTransLList *trans1, struct taggedTransLList *trans2, int verbose)
{
    struct taggedTransLList *innerPrev, *innerNext;
    
    if( trans1 == NULL || trans2 == NULL || trans1 == trans2 )
        return(0);
        
    //printf("Swapping tagged %s and %s\n", trans1->trans->description, trans2->trans->description);
    
    // Setting outer relations
    if( trans1->prev != NULL )
        trans1->prev->next = trans2;
    if( trans1->next != NULL )
        trans1->next->prev = trans2;
        
    if( trans2->prev != NULL )
        trans2->prev->next = trans1;
    if( trans2->next != NULL )
        trans2->next->prev = trans1;
        
    innerPrev = trans1->prev;
    innerNext = trans1->next;
    
    trans1->next = trans2->next;
    trans1->prev = trans2->prev;
    
    trans2->next = innerNext;
    trans2->prev = innerPrev;
    
    return(1);
        
    
}

// Swaps adjacent trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal or not adjacent
int swapAdjacentTaggedTransElements(struct taggedTransLList *trans1, struct taggedTransLList *trans2, int verbose)
{
    struct taggedTransLList *outerPrev, *outerNext, *tmp;
    
    // Return zero if error
    if( trans1 == NULL || trans2 == NULL || trans1 == trans2 || !( trans1->next == trans2 || trans1->prev == trans2 ) )
        return(0);
        
        //printf("Swapping adjacent tagged %s and %s\n", trans1->trans->description, trans2->trans->description);
    
    if( trans1->next == trans2 )
    {        
        // Swapping external relations        
        if( trans1->prev != NULL )
         trans1->prev->next = trans2;
        if( trans2->next != NULL )
         trans2->next->prev = trans1;
                
        // Swapping internal relations
        tmp = trans1->prev;
        trans1->prev = trans2;
        trans2->prev = tmp;
        
        tmp = trans2->next;
        trans2->next = trans1;
        trans1->next = tmp;
        
    }
    else if( trans2->next == trans1 )
    {
         
         //Swapping extrenal relations
         trans2->prev->next = trans1;
         trans1->next->prev = trans2;
                  
         // Swapping internal relations
         tmp = trans2->prev;
         trans2->prev = trans1;
         trans1->prev = tmp;
         
         tmp = trans1->next;
         trans1->next = trans2;
         trans2->next = tmp;

    }
    
    return(1);
}

// Returns pointer to first element in linked list
struct taggedTransLList *findFirstTaggedElement(struct taggedTransLList *inElement, int verbose)
{
       while(inElement->prev != NULL)
       inElement = inElement->prev;
       
       return(inElement);
}

struct taggedTransLList *findLastTaggedElement(struct taggedTransLList *inElement, int verbose)
{
       while(inElement->next != NULL)
       inElement = inElement->next;
       
       return(inElement);
}

// Writes a table into file "fileName" or file pointer fp (cannot take both!), returns fp or -1 on error or NULL on closed file
// -If "fileName": open new file and write a line.
// -If fp: append fp with line of values.
// -If close, close fp
FILE *writeCalcMonthTable(char *fileName, FILE *fp, int close, int verbose, int month, int year, int num, float argListOfValues[] )
{
    
    int x;
    float tmpValue=0;
    
    // The six ifs below are to figure out whether the first to arguments (*fileName, *fp) are correctly done and what do to in the different settings
    if( fileName != NULL && fp != NULL )
    {
        if(verbose) printf("writeCalcMonthTable: Cannot take both fileName and fp!\n");
        return(-1);
    }
    else if( fileName == NULL && fp == NULL )
    {
         if(verbose) printf("writeCalcMonthTable: Need either fileName or fp!\n");
         printf("month %d year %d\n", month, year);
         return(-1);
    }
    else if( fileName != NULL )
    {
        if( (fp=fopen(fileName, "wt")) == NULL )
        {
            perror("Error opening file");
            return(-1);
        }
        if(verbose) printf("writeCalcMonthTable: Creating file %s\n", fileName);
    }
    else if( fp != NULL && close )
    {
         if(verbose) printf("writeCalcMonthTable: Closing file\n");
         fclose(fp);
         return(NULL);
    }
    else if( fp != NULL && !close )
    {
         if(verbose) printf("writeCalcMonthTable: Appending already open file\n");
         fseek(fp, 0, SEEK_END); // Go to end of file
    }
    else
    return(-1);
    
    
    // Write date
    fprintf(fp, "%d-%d", month, year);
    if(verbose) printf("writeCalcMonthTable: %d-%d", month, year);
    
    for( x = 0; x<num; x++ )
    {// float calcMonthSumOfTag(char month, int year, struct tagLList *tag, int verbose);

         tmpValue = argListOfValues[x]; // Retrieve next argument 

         fprintf(fp, "\t%.2f", tmpValue);
         if(verbose) printf("\t%.2f", tmpValue);
    }

    // Put newline in file    
    fprintf(fp, "\n");
    if(verbose) printf("\n");
    
    

return(fp);
}


// Writes a file "fileName" with all monthly sums of tag "tagName"
int writeMonthSumOfTag(char *fileName, char *tagName, struct tagLList *tagList, int verbose)
{
    
    char startMonth, endMonth, currentMonth;
    int startYear, endYear, currentYear;
    FILE *fp;
    float tmpValue[1];
    
    struct taggedTransLList *taggedList;
    

    // Find correct tag
    tagList = findTag(tagName, tagList);
    
    taggedList = tagList->transList;
    
    // Extract first month and year
    taggedList = findFirstTaggedElement(taggedList, verbose);
    
    startMonth = taggedList->trans->month;
    startYear  = taggedList->trans->year;
    
    // Extract last month and year
    taggedList = findLastTaggedElement(taggedList, verbose);
    
    endMonth = taggedList->trans->month;
    endYear  = taggedList->trans->year;
    
    // Prepare loop
    currentYear = startYear;
    currentMonth = startMonth;
    
    
    while(  (currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear) )
    {
            tmpValue[0] = calcMonthSumOfTag(currentMonth, currentYear, tagList, verbose);
            
           // If first call; open new file
           if(currentMonth == startMonth && currentYear == startYear)
           fp = writeCalcMonthTable(fileName, NULL, 0, verbose, currentMonth, currentYear, 1, tmpValue);
           // Otherwise, append fp
           else
           fp = writeCalcMonthTable(NULL, fp, 0, verbose, currentMonth, currentYear, 1, tmpValue);
           
           // Date counter
           currentMonth++;
           if( currentMonth > 12 )
           {
               currentMonth = 1;
               currentYear++;
           }
    }
    
    // Close file
    writeCalcMonthTable(NULL, fp, 1, verbose, currentMonth, currentYear, 0, NULL);
    
    
    return(0);
}

// Writes a file "fileName" with all monthly cost sums of all transactions
int writeMonthSumOfTrans(char *fileName, struct transLList *transList, int verbose)
{
    char startMonth, endMonth, currentMonth;
    int startYear, endYear, currentYear;
    FILE *fp;
    float tmpValue[1];
    

    // Extract last month and year
    transList = findLastElement(transList, verbose);
    
    endMonth = transList->month;
    endYear  = transList->year;
        
    // Extract first month and year
    transList = findFirstElement(transList, verbose);
    
    startMonth = transList->month;
    startYear  = transList->year;
    
    // Prepare loop
    currentYear = startYear;
    currentMonth = startMonth;
    

    while(  (currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear) )
    {
            tmpValue[0] = calcMonthSumOfTrans(currentMonth, currentYear, transList, verbose);
            
           // If first call; open new file
           if(currentMonth == startMonth && currentYear == startYear)
           fp = writeCalcMonthTable(fileName, NULL, 0, verbose, currentMonth, currentYear, 1, tmpValue);
           // Otherwise, append fp
           else
           fp = writeCalcMonthTable(NULL, fp, 0, verbose, currentMonth, currentYear, 1, tmpValue);
           
           // Date counter
           currentMonth++;
           if( currentMonth > 12 )
           {
               currentMonth = 1;
               currentYear++;
           }
    }   
    
    // Close file
    writeCalcMonthTable(NULL, fp, 1, verbose, currentMonth, currentYear, 0, NULL);
    
    
    return(0);    
}

// Writes a file "fileName" with monthly ratios of all tags to total sum of costs. 
int writeMonthSumOfTags(char *fileName, struct transLList *transList, struct tagLList *tagList, int verbose)
{
    struct taggedTransLList *taggedList;
    struct tagLList *tagListStart;
    char startMonth, endMonth, currentMonth;
    int startYear, endYear, currentYear, n=0, num = tagListLen(tagList,verbose);
    FILE *fp;
    float tmpTagValue[num];
    
    if(verbose) printf("writeMonthSumOfTags: Entering\n");

    // Go to first tag
    tagList = findFirstTag(tagList);
    tagListStart = tagList;
    
    taggedList = tagList->transList;

    // Extract last month and year
    transList = findLastElement(transList, verbose);
    
    endMonth = transList->month;
    endYear  = transList->year;
        
    // Extract first month and year
    transList = findFirstElement(transList, verbose);
    
    startMonth = transList->month;
    startYear  = transList->year;
    
    // Prepare loop
    currentYear = startYear;
    currentMonth = startMonth;    

    while(  (currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear) )
    {
            // Reset counters
            tagList = tagListStart;
            n=0;
            
            // Calculate cost ratios for every tag
            while(tagList != NULL)
            {
                     tmpTagValue[n] = calcMonthSumOfTag(currentMonth, currentYear, tagList, verbose);
                     n++;
                     tagList = tagList->next;
            }
                     
                     
                     
           // If first call; open new file
           if(currentMonth == startMonth && currentYear == startYear)
           fp = writeCalcMonthTable(fileName, NULL, 0, verbose, currentMonth, currentYear, num, tmpTagValue);
           // Otherwise, append fp
           else
           fp = writeCalcMonthTable(NULL, fp, 0, verbose, currentMonth, currentYear, num, tmpTagValue);
           
           // Date counter
           currentMonth++;
           if( currentMonth > 12 )
           {
               currentMonth = 1;
               currentYear++;
           }
    }   
    
    // Close file
    if( writeCalcMonthTable(NULL, fp, 1, verbose, currentMonth, currentYear, 0, NULL) == NULL )
        printf("writeMonthSumOfTags: Successfully wrote %s\n", fileName);
    
    if(verbose) printf("writeMonthSumOfTags: Exiting\n");
}


struct tagLList *findFirstTag(struct tagLList *tagList)
{
       while(tagList->prev != NULL)
       tagList = tagList->prev;
       
       return(tagList);
}

int tagListLen(struct tagLList *tagList, int verbose)
{
    int c = 0;

    tagList = findFirstTag(tagList);
    
    while(tagList != NULL)
    {
          tagList = tagList->next;
          c++;     
    }

    
    return(c);
}

struct tagLList *findLastTag(struct tagLList *tagList)
{
       while(tagList->next != NULL)
       tagList = tagList->next;

       
       return(tagList);
}

// Returns pointer to last element in keyword list
struct keyWord *findLastKeyword(struct keyWord *inKeyword)
{
	if(inKeyword == NULL)
		return(NULL);
		
	while(inKeyword->next != NULL)
		inKeyword = inKeyword->next;
		
		return(inKeyword);
}

// Frees a list of tragged transactions (DOES NOT FREE THE TRANSACTIONS THEMSELVES. ONLY THE TAGGED LIST) Returns # of freed elements
int freeTaggedTransList(struct taggedTransLList *taggedTrans, int verbose)
{
    struct taggedTransLList *current;
    int count = 0;
    
    if(verbose) printf("freeTaggedTransList: Entering\n");
    
    while(taggedTrans != NULL)
    {
                      current = taggedTrans;
                      taggedTrans = taggedTrans->next;
                      free(current);
                      count++;
    }
    
    if(verbose) printf("freeTaggedTransList: Freed %d tagged elements. Exiting\n", count);
    
    return(count);
}
