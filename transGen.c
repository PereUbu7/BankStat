#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tags.h"


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



// -------------------------- Functions ------------------------------

int readLine(FILE *file, unsigned long *transID, int *day1, int *day2, int *month1, int *month2, int *year1, 
             int *year2, char *transText, float *transAmount1, float *transAmount2, int verbose)
{

  char readChar = 1;
  
  char line[256];

  char sDay1[3];
  char sDay2[3];
  char sMonth1[3];
  char sMonth2[3];
  char sYear1[5];
  char sYear2[5];
  char sTransAmount1[256];
  char sTransAmount2[256];

  static char readManual[9] = {'-', '-', '\t', '-', '-', '\t', '\t', '\t', '\n'};
  char *storeVariableManual[9]; 
  int readManualCount = 0;
  int storeVariableManualCount = 0;
  int charCount = 0;
  int lineCount = 0;

  /* Old date format MM-DD-YYYY 
  storeVariableManual[0] = sMonth1;
  storeVariableManual[1] = sDay1;
  storeVariableManual[2] = sYear1;
  storeVariableManual[3] = sMonth2;
  storeVariableManual[4] = sDay2;
  storeVariableManual[5] = sYear2;
  storeVariableManual[6] = transText;
  storeVariableManual[7] = sTransAmount1;
  storeVariableManual[8] = sTransAmount2; */
  
  /* New date format YYYY-MM-DD */
  storeVariableManual[0] = sYear1;
  storeVariableManual[1] = sMonth1;
  storeVariableManual[2] = sDay1;
  storeVariableManual[3] = sYear2;
  storeVariableManual[4] = sMonth2;
  storeVariableManual[5] = sDay2;
  storeVariableManual[6] = transText;
  storeVariableManual[7] = sTransAmount1;
  storeVariableManual[8] = sTransAmount2;

  if ( fgets(line, sizeof(line), file) == NULL ) {
       if(verbose) perror("Error reading line"); 
     return(-1);
     }
  
  while( readChar != '\n' ) {
         readChar = line[lineCount];

         if ( readChar != readManual[readManualCount] ){
            storeVariableManual[storeVariableManualCount][charCount] = readChar;
            charCount++;
            }
         else {
              storeVariableManual[storeVariableManualCount][charCount] = 0;
              readManualCount++;
              storeVariableManualCount++;
              charCount = 0;
              }
         lineCount++;
         }
         
         *day1 = atoi(sDay1);
         *day2 = atoi(sDay2);
         *month1 = atoi(sMonth1);
         *month2 = atoi(sMonth2);
         *year1 = atoi(sYear1);
         *year2 = atoi(sYear2);
         
         ctod(&sTransAmount1);
         ctod(&sTransAmount2);
         
         *transAmount1 = atof(sTransAmount1);
         *transAmount2 = atof(sTransAmount2);
         
         *transID = (long)*transAmount1 + (unsigned long)*year1*10000 + (unsigned long)*month1*100000 + (unsigned long)*day1*1000000;
	
  return 0;
}


// Uses readLine to read bank transaction file and stores info in a linked list.
// *transListStart=NULL if starting new list. To append a list, *transListStart points to last element.
//  Returns pointer to first element in list

struct transLList *readBankFileToLList(FILE *bankFile, struct transLList *transListStart, int verbose)
{
       
  unsigned long transID;
  
  struct tagLList *newList;
  struct transLList *transList;


  int day1;
  int day2;
  int month1;
  int month2;
  int year1;
  int year2;
  char transText[256];
  float transAmount1;
  float transAmount2;
  
  int count = 0;
  
  
  int freeAtEnd = (int)transListStart;
  
  rewind(bankFile);
  
  // If starting a new list
  if( transListStart == NULL )
  {
      transList = malloc(sizeof(struct transLList));
      transList->prev = NULL;
  }
  else // If appending to a list
  {
      transListStart->next = malloc(sizeof(struct transLList));
      transList = transListStart;
  }

 while(0 == readLine(bankFile, &transID, &day1, &day2, &month1, &month2, &year1, &year2, transText, &transAmount1, &transAmount2, verbose)){  

	count++;
	
    if( transList->next == NULL )
	transList->next = malloc(sizeof(struct transLList));
	
	if( transListStart == NULL ){
	    transListStart = transList->next;
     }
	
	transList->next->ID = transID;
	transList->next->day = day1;
	transList->next->month = month1;
	transList->next->year = year1;
	transList->next->value = transAmount1;
	transList->next->description = malloc(strlen(transText)+1);
	strcpy(transList->next->description, transText);
	transList->next->prev = transList;
	transList->next->next = NULL;
	
	transList = transList->next;
 }
 
 if( !freeAtEnd )
 {
     free(transListStart->prev);
     transListStart->prev = NULL;
 }
 
 printf("readBankFileToLList: read %d transactions.\n", count);
 
 return(transListStart);
}

// Returns pointer to first element in linked list
struct transLList *findFirstElement(struct transLList *inElement, int verbose)
{
       while(inElement->prev != NULL)
       inElement = inElement->prev;
       
       return(inElement);
}

// Returns pointer to last element in linked list
struct transLList *findLastElement(struct transLList *inElement, int verbose)
{
       while(inElement->next != NULL)
       inElement = inElement->next;
       
       return(inElement);
}


int writeTransFromLList(FILE *fp, struct transLList *transaction, int verbose)
{
    
    if(verbose) printf("Entering writeTransFromLList\n");
    
    if( transaction == NULL )
    {
        if(verbose) printf("No transaction found - Exiting without making any changes.\n");
        return(-1);
    }
    
    //Make sure fp starts at beginning of file
    rewind(fp);
    
    while(transaction != NULL)
    {
    
     // Writes [transID]\n to file
     fprintf(fp, "[%u]\n", transaction->ID);
    
     // Write all transaction data to file
     fprintf(fp, "year=%d\n", transaction->year);
     fprintf(fp, "month=%d\n", transaction->month);
     fprintf(fp, "day=%d\n", transaction->day);
     fprintf(fp, "description=%s\n", transaction->description);
     fprintf(fp, "value=%.2f\n", transaction->value);
     fprintf(fp, "\n");
    
     // Write same thing to stdout as verification
     //if(verbose) printf("[%u]\nyear=&d\nmonth=%d\nday=%d\ndescription=%s\nvalue=%.2f\n\n", transaction->ID, transaction->year, transaction->month, transaction->day, transaction->description, transaction->value);
    
     // Jump to next transaction in linked list
     transaction = transaction->next;
    }
    
    if(verbose) printf("Exiting writeTransFromLList...\n");
    
    return(0);
}





// Compare dates in two different transaction elements. Returns -1 if equal, 0 if trans2 < trans1 and 1 if trans1 < trans2.
// On returning 1, comute time is minimized. 0 returned and compute time is maximized
int cmpTransDates(struct transLList *trans1, struct transLList *trans2, int verbose)
{
    
    if( trans1->year < trans2->year )
        return(1);
    else if( trans1->year == trans2->year )
    {
         if( trans1->month < trans2->month )
             return(1);
         else if( trans1->month == trans2->month )
              {
                  if( trans1->day < trans2->day )
                      return(1);
                  else if( trans1->day == trans2->day )
                       return(-1);
                  else if( trans1->day > trans2->day )
                       return(0);
              }
         else if( trans1->month > trans2->month )
              return(0);
    }
    else if( trans1->year > trans2->year )
         return(0);    
    
}


// Swaps trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal

// KOLLA ATT DEN FUNKAR!!!
int swapTransElements(struct transLList *trans1, struct transLList *trans2, int verbose)
{
    struct transLList *innerPrev, *innerNext;
    
    if( trans1 == NULL || trans2 == NULL || trans1 == trans2 )
        return(0);
        
    if(verbose) printf("Swapping %s and %s\n", trans1->description, trans2->description);
    
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



// Reads a list of transactions and stores it into a linked list. Returns pointer to first element
struct transLList *readTransToLList(FILE *fp, int verbose)
{
       struct transLList *currentElement=NULL, *startElement=NULL, *prevElement=NULL;
       
       int tmpLineLength = 50, tmpWordLength = 50;
       int newElement = 0;

       char tmpLine[ tmpLineLength ];
       char tmpWord[ tmpWordLength ];
       char *tmpToken;
       char *endBracketPos, *startBracketPos;
       char delimiter[2] = "=";
 
       if(verbose) printf("Entering readTransToLList\n");
       
       rewind(fp);
       
       startElement = malloc(sizeof(struct transLList));
       startElement->prev = NULL;
       
       currentElement = startElement;
       
       while(1)
       {
               // Read a line.
               if( fgets( tmpLine, tmpLineLength-1, fp ) == NULL )
               {
                   if(verbose) printf("Exiting readTransToLList\n\n");

                   return(startElement);
               }
               
               // if line starts with '[' and has a ']' somewhere afterwards, then...
               if( tmpLine[0] == '[' && (endBracketPos = strchr((char*)tmpLine+1, ']')) != NULL )
               {
                   // Copy string between brackets to tmpWord
                   memcpy(tmpWord, (char*)tmpLine+1, endBracketPos-tmpLine-1);
                   
                   // Set terminating NULL-character after tmpWord
                   tmpWord[endBracketPos-tmpLine-1] = (int)NULL;
                   
                   // If not first element, create a new element
                   if( newElement )
                   {
                       prevElement = currentElement;
                       currentElement->next = malloc(sizeof(struct transLList));
                       
                       currentElement = currentElement->next;
                       currentElement->prev = prevElement;
                       
                       newElement = 0;
                   }
                   
                   currentElement->ID = atol(tmpWord);

               }
               else
               {
                   // Remove the newline caracter at the end of tmpLine
                   if( strlen(tmpLine) > 1 )
                   {
                       tmpLine[strlen(tmpLine)-1] = '\0';
                   }
                   else
                   {
                       newElement = 1;
                   }

                   // Get first token
                   tmpToken = (char*)strtok(tmpLine, delimiter);
               
               
                   if( !strcmp(tmpToken, "year") )
                   {
                       tmpToken = (char*)strtok(NULL, delimiter);
                       currentElement->year = atoi( tmpToken );
                   }
                   else if( !strcmp(tmpToken, "month") )
                   {
                        tmpToken = (char*)strtok(NULL, delimiter);
                        currentElement->month = atoi( tmpToken );
                   }
                   else if( !strcmp(tmpToken, "day") )
                   {
                        tmpToken = (char*)strtok(NULL, delimiter);
                        currentElement->day = atoi( tmpToken );
                   }
                   else if( !strcmp(tmpToken, "value") )
                   {
                        tmpToken = (char*)strtok(NULL, delimiter);
                        currentElement->value = atof( tmpToken );
                   }
                   else if( !strcmp(tmpToken, "description") )
                   {
                        tmpToken = (char*)strtok(NULL, delimiter);
                        currentElement->description = malloc(strlen(tmpToken)+1);
                        strcpy(currentElement->description, tmpToken);
                   } 
             }
                   
       }
       
       return(startElement);
}

// Returns number of element in list
int transListLen(struct transLList *list, int verbose)
{
    int c = 0;
    
    list = findFirstElement(list, verbose);
    
    while(list != NULL)
    {
          list = list->next;
          c++;     
    }
    
    return(c);
}

// Appends *listToAppend with data in bank file fileName. Returns pointer to first element in new list
struct transLList *appendTransLList(char *fileName, struct transLList *listToAppend, int verbose)
{
       FILE *file;
       
         // Open input file
  if ( (file = fopen(fileName, "rt")) == NULL ){
     perror("Error opening bank file"); 
     return(-1);
     }
     
  // Append data in file to listToAppend   
  listToAppend = readBankFileToLList(file, findLastElement(listToAppend, verbose), verbose);
  
  // Close file
  fclose(file);
  
  // Go to first element
  listToAppend = findFirstElement(listToAppend, verbose);
  
  return(listToAppend);
}

// Frees all elements in a list of transactions. Return number of elements freed or -1 if unsuccessfull
int freeTransList(struct transLList *trans, int verbose)
{
    struct transLList *current;
    int count = 0;
    
    if(verbose) printf("freeTransList: Entering\n");
    
    trans = findFirstElement(trans, verbose);
    
    while(trans != NULL)
    {                
                // If memory has been allocated to hold a description, free it
                if(trans->description != NULL)
                free(trans->description);
                
                // Free current transaction and go to next
                current = trans;
                trans = trans->next;
                free(current);
                count++;
    }
    
    if(verbose) printf("freeTransList: Freed %d transactions. Exiting\n", count);
    
    return(count);
}
