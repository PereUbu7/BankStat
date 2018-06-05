#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "tags.h"

// Searches for elements with equal ID:s and removes one of them
int removeTwinTransElements(struct transLList *trans, int verbose);

// Removes an element from the list and returns pointer to precedent element
struct transLList *removeTransElement(struct transLList *trans, int verbose);

// Takes a linked list of transactions and sorts it according to the gnome sorting algorithm. Returns pointer to new start element
struct transLList *gnomeSortTransLList(struct transLList *listStart, int verbose);

// Swaps trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal
int swapAdjacentTransElements(struct transLList *trans1, struct transLList *trans2, int verbose);


//--------------------- Functions ------------------------------


int removeTwinTransElements(struct transLList *trans, int verbose)
{
    struct transLList *storedTrans, *tmpTrans;
	
	int count = 0;
    
    // Go to first element in list
    storedTrans = findFirstElement(trans);
    
    while( storedTrans != NULL )
    {
           trans = storedTrans->next;
           
           while( trans != NULL )
           {

                  if( trans->ID == storedTrans->ID )
                  {
                      if(verbose) printf("Found twin element. ID: %lu. %d/%d-%d\n", trans->ID, trans->day, trans->month, trans->year);
                      tmpTrans = trans->next;
                      removeTransElement(trans, verbose);
                      trans = tmpTrans;
					  
					  count++;
                  }
                  else
                      trans = trans->next;
           }
           
           storedTrans = storedTrans->next;
    }
	
	if(verbose) printf("removeTwinTransElements: removed %d elements.\n", count);
	
	return(count);
}


// Takes a linked list of transactions and sorts it according to the gnome sorting algorithm. Returns pointer to new start element
struct transLList *gnomeSortTransLList(struct transLList *listStart, int verbose)
{
       struct transLList *currentElement = NULL, *upperBound = listStart->next;
       clock_t startTime, endTime;
       
       startTime = clock();
       
       if(verbose) printf("Entering gnomeSortTransLList\n");
       
       while( upperBound != NULL )
       {
              currentElement = upperBound;

              while( currentElement->prev != NULL &&  !cmpTransDates(currentElement->prev, currentElement) )
              {

                     if( currentElement->prev->prev == NULL )
                     {
                         listStart = currentElement;
                         swapAdjacentTransElements(currentElement->prev, currentElement, verbose);
                         currentElement = listStart;
                     }
                     else
                     swapAdjacentTransElements(currentElement->prev, currentElement, verbose);
              }
              
              upperBound = upperBound->next;
       }
       
       endTime = clock();
       
       if(verbose) printf("Sorting took %ld ms\nms per sec: %ld\n", endTime - startTime, CLOCKS_PER_SEC);
       
       if(verbose) printf("Exiting gnomeSortTransList\n");
   
   return(findFirstElement(currentElement));    
       
}

// Swaps adjacent trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal or not adjacent
int swapAdjacentTransElements(struct transLList *trans1, struct transLList *trans2, int verbose)
{
    struct transLList *outerPrev, *outerNext, *tmp;
    
    // Return zero if error
    if( trans1 == NULL || trans2 == NULL || trans1 == trans2 || !( trans1->next == trans2 || trans1->prev == trans2 ) )
        return(0);
        
        if(verbose) printf("Swapping adjacent %s and %s\n", trans1->description, trans2->description);
    
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


// Removes an element from the list and returns pointer to precedent element
struct transLList *removeTransElement(struct transLList *trans, int verbose)
{
       
       struct transLList *returnElement = NULL;
       
       if( trans->next != NULL && trans->prev != NULL )
       {
           trans->prev->next = trans->next;
           trans->next->prev = trans->prev;
           returnElement = trans->prev;
           if(verbose) printf("Removing element: %lu - ", trans->ID);
           free(trans);
       } // if last element
       else if( trans->next == NULL )
       {
            trans->prev->next = NULL;
            returnElement = trans->prev;
            if(verbose) printf("Removing last element: %lu - ", trans->ID);
            free(trans);
       }
       else if( trans->prev == NULL )
       {
            trans->next->prev = NULL;
            returnElement = trans->next;
            if(verbose) printf("Removing first element: %lu - ", trans->ID);
            free(trans);
       }
       else
            return(NULL);
            
       if(verbose) printf("Removed element\n");
       
       return(returnElement);
}
