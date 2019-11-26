#include "tagGen.h"

//--------------------------- Functions -----------------------------

// Creates a linked list tree from file. Returns pointer to first tag
struct tagLList *readTagToLList(FILE *fp, int verbose)
{
    struct keyWord *childPosition = NULL;

    struct tagLList *tags, *startTag = NULL;

    int tmpLineLength = 50, tmpWordLength = 50;
    int createRelation = 0;

    char tmpLine[tmpLineLength];
    char tmpWord[tmpWordLength];
    char *endBracketPos, *startBracketPos;

    if (verbose)
        printf("readTagToLList: Entering\n");

    while (1)
    {

        // Read a line. Hopefully a [tag] line
        if (!createRelation && fgets(tmpLine, tmpLineLength - 1, fp) == NULL)
        {
            if (verbose)
                printf("readTagToLList: Exiting\n\n");

            return (startTag);
        }

        // if line starts with '[' and has a ']' somewhere afterwards, then...
        if (tmpLine[0] == '[' && (endBracketPos = strchr((char *)tmpLine + 1, ']')) != NULL)
        {
            // Copy string between brackets to tmpWord
            memcpy(tmpWord, (char *)tmpLine + 1, endBracketPos - tmpLine - 1);

            // Set terminating NULL-character after tmpWord
            tmpWord[endBracketPos - tmpLine - 1] = '\0';

            // Create struct relation with new sibbling
            if (createRelation)
            {
                tags = createNextTag(tags, tmpWord, verbose);
                createRelation = 0;
            }
            else
            {
                // Allocate mem for first tag struct
                tags = malloc(sizeof(struct tagLList));
                tags->name = malloc(strlen(tmpWord) + 1);
                tags->prev = NULL;
                tags->next = NULL;
                tags->transList = NULL;
                startTag = tags;
            }

            // Copy string to new tag struct
            strcpy(tags->name, tmpWord);
            if (verbose)
                printf("tag:%s\n", tags->name);

            //---------------------------------------------------
            // Now we should start adding keyWords

            while (1)
            {
                // Read next line
                if (fgets(tmpLine, tmpLineLength - 1, fp) == NULL)
                {
                    if (verbose)
                        printf("Read error or EOF\n");
                    if (verbose)
                        printf("readTagToLList: Exiting\n\n");

                    return (startTag);
                }

                // If new [tag] found; break little loop and go into big again
                if (tmpLine[0] == '[' && (endBracketPos = strchr((char *)tmpLine + 1, ']')) != NULL)
                {
                    if (verbose)
                        printf("Found unexcpected tag line - set relations\n");
                    // Make sure struct relations are set
                    createRelation = 1;
                    childPosition = NULL;
                    break;
                }
                // else, if not a comment or newline; suspect keyWord
                else if (tmpLine[0] != '%' && tmpLine[0] != '\n' && tmpLine[0] != '\r')
                {
                    // Remove the newline caracter at the end of tmpLine
                    replaceCharacterWithStringEnd(tmpLine, '\n');
                    replaceCharacterWithStringEnd(tmpLine, '\r');

                    // If first keyWord in tag
                    if (childPosition == NULL)
                    {
                        childPosition = malloc(sizeof(struct keyWord));
                        childPosition->prev = NULL;
                        childPosition->next = NULL;
                        childPosition->parent = tags;
                        tags->child = childPosition;
                        childPosition->name = malloc(strlen(tmpLine) + 1);
                        strcpy(childPosition->name, tmpLine);
                    }
                    else
                    {
                        childPosition = createNextKeyWord(childPosition, tmpLine, verbose);
                    }

                    if (verbose)
                        printf("\tkeyWord: %s\n", childPosition->name);
                }
                else if (tmpLine[0] == '%')
                {
                    if (verbose)
                        printf("Comment\n");
                }
                else if (tmpLine[0] == '\n')
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

    if (verbose)
        printf("readTagToLList: Exiting\n\n");

    return (startTag);
}

// Writes a list tree of tags and keywords to file *fp. Returns true on success and false on failure
int writeTagLListToFile(FILE *fp, struct tagLList *tagList, int verbose)
{
    struct keyWord *currentKeyWord = NULL;
    struct tagLList *currentTag = tagList;

    printf("writeTagLListToFile: Entering\n");

    while (currentTag != NULL)
    {
        fprintf(fp, "[%s]\n", currentTag->name);

        currentKeyWord = currentTag->child;

        while (currentKeyWord != NULL)
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
    tag->name = malloc(strlen(name) + 1);
    strcpy(tag->name, name);

    return (tag);
}

struct keyWord *createNextKeyWord(struct keyWord *word, char *name, int verbose)
{

    //if(verbose) printf("createNextKeyWord: Entering\n");
    if (word == NULL)
    {
        printf("createNextKeyWord:ERROR! given keyword does not exist.\n");
        return (NULL);
    }
    word->next = malloc(sizeof(struct keyWord));

    word->next->prev = word;

    word->next->parent = word->parent;

    word = word->next;

    word->next = NULL;
    word->name = malloc(strlen(name) + 1);
    strcpy(word->name, name);

    //if(verbose) printf("createNextKeyWord: Exiting\n");

    return (word);
}

struct keyWord *createFirstKeyWord(struct tagLList *tag, char *name, int verbose)
{
    struct keyWord *word;

    //if(verbose) printf("createFirstKeyWord: Entering\n");

    word = malloc(sizeof(struct keyWord));

    word->prev = NULL;
    word->parent = tag;
    word->next = NULL;
    word->name = malloc(strlen(name) + 1);
    strcpy(word->name, name);

    tag->child = word;

    //if(verbose) printf("createFirstKeyWord: Exiting\n");

    return (word);
}

// Frees a linked list of keyWord structs and returns the amount freed
int freeKeyWordList(struct keyWord *word, int verbose)
{
    int numberOfFreedKeyWords = 0;

    //if(verbose) printf("freeKeyWordList: Entering\n");

    if (word == NULL)
        return (0);

    while (1)
    {
        if (word->name != NULL)
            free(word->name);
        //if(verbose) printf("\tFreed keyWord name #%d. ", numberOfFreedKeyWords+1);

        // If word-next points to SOMETHING; suspect keyWord struct
        if (word->next != NULL)
        {
            word = word->next;
            free(word->prev);
            //if(verbose) printf("\tFreed keyWord #%d\n", numberOfFreedKeyWords+1);
            numberOfFreedKeyWords++;
        }
        else
        {
            break;
        }
    }
    if (word != NULL)
        free(word);
    //if(verbose) printf("\tFreed keyWord #%d\n", numberOfFreedKeyWords+1);

    //if(verbose) printf("freeKeyWordList: Exiting\n");

    return (numberOfFreedKeyWords + 1);
}

// Frees a linked list of tagLList structs and returns the amount of tags freed
int freeTagList(struct tagLList *tag, int verbose)
{

    int numberOfFreedTags = 0;
    int numberOfFreedKeyWords = 0;
    int numberOfFreedTaggedTrans = 0;

    //if(verbose) printf("freeTagList: Entering\n");

    if (tag == NULL)
        return (0);

    while (1)
    {
        if (tag->name != NULL)
        {
            free(tag->name);
            //if(verbose) printf("Freed tag name #%d\n",numberOfFreedTags+1);
        }

        if (tag->child != NULL)
        {
            numberOfFreedKeyWords += freeKeyWordList(tag->child, verbose);
        }

        if (tag->transList != NULL)
        {
            numberOfFreedTaggedTrans += freeTaggedTransList(tag->transList, verbose);
        }

        // If tag-next points to SOMETHING; suspect tagLList struct
        if (tag->next != NULL)
        {
            tag = tag->next;
            free(tag->prev);
            //if(verbose) printf("Freed tag #%d\n",numberOfFreedTags+1);
            numberOfFreedTags++;
        }
        else
            break;
    }
    if (tag != NULL)
        free(tag);
    //if(verbose) printf("Freed tag #%d\nExiting freeTagList\n\n",numberOfFreedTags+1);
    //if(verbose) printf("freeTagList: Exiting\n");

    return (numberOfFreedTags + 1);
}

int writeTagFromLList(FILE *fp, struct tagLList *tags, int verbose)
{
    if (verbose)
        printf("writeTagFromLList: Entering\n");
    // se till att fp är på rätt ställe i filen
    rewind(fp);

    while (tags != NULL)
    {

        // Writes "[name]\n" to file
        fprintf(fp, "[%s]\n", tags->name);

        //print to stdout as test
        if (verbose)
            printf("[%s]\n", tags->name);

        while (tags->child != NULL)
        {

            // Write keyWord name to file
            fprintf(fp, "%s\n", tags->child->name);

            // printf to stdout as test
            if (verbose)
                printf("%s\n", tags->child->name);

            // Jump to next in keyWord list
            tags->child = tags->child->next;
        }
        // Write newline before next [tag} to file
        fprintf(fp, "\n");

        // Printf to stdout for test
        if (verbose)
            printf("\n");

        // Jump to next in list
        tags = tags->next;
    }

    if (verbose)
        printf("writeTagFromLList: Exiting\n");

    return (0);
}

// Takes a linked list of transactions and sorts it according to the gnome sorting algorithm. Returns pointer to new start element
struct taggedTransLList *gnomeSortTaggedTransLList(struct taggedTransLList *listStart, int verbose)
{
    struct taggedTransLList *currentElement = NULL, *upperBound = listStart->next;

    if (verbose)
        printf("gnomeSortTaggedTransLList: Entering\n");

    while (upperBound != NULL)
    {
        currentElement = upperBound;

        while (currentElement->prev != NULL && !cmpTransDates(currentElement->prev->trans, currentElement->trans, verbose))
        {

            if (currentElement->prev->prev == NULL)
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

    if (verbose)
        printf("gnomeSortTaggedTransList: Exiting\n");

    return (findFirstTaggedElement(currentElement, verbose));
}

// Swaps trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal
int swapTaggedTransElements(struct taggedTransLList *trans1, struct taggedTransLList *trans2, int verbose)
{
    struct taggedTransLList *innerPrev, *innerNext;

    if (trans1 == NULL || trans2 == NULL || trans1 == trans2)
        return (0);

    //printf("Swapping tagged %s and %s\n", trans1->trans->description, trans2->trans->description);

    // Setting outer relations
    if (trans1->prev != NULL)
        trans1->prev->next = trans2;
    if (trans1->next != NULL)
        trans1->next->prev = trans2;

    if (trans2->prev != NULL)
        trans2->prev->next = trans1;
    if (trans2->next != NULL)
        trans2->next->prev = trans1;

    innerPrev = trans1->prev;
    innerNext = trans1->next;

    trans1->next = trans2->next;
    trans1->prev = trans2->prev;

    trans2->next = innerNext;
    trans2->prev = innerPrev;

    return (1);
}

// Swaps adjacent trans1 and trans2. Returns 1 on succses and 0 if elements are NULL or equal or not adjacent
int swapAdjacentTaggedTransElements(struct taggedTransLList *trans1, struct taggedTransLList *trans2, int verbose)
{
    struct taggedTransLList *outerPrev, *outerNext, *tmp;

    // Return zero if error
    if (trans1 == NULL || trans2 == NULL || trans1 == trans2 || !(trans1->next == trans2 || trans1->prev == trans2))
        return (0);

    //printf("Swapping adjacent tagged %s and %s\n", trans1->trans->description, trans2->trans->description);

    if (trans1->next == trans2)
    {
        // Swapping external relations
        if (trans1->prev != NULL)
            trans1->prev->next = trans2;
        if (trans2->next != NULL)
            trans2->next->prev = trans1;

        // Swapping internal relations
        tmp = trans1->prev;
        trans1->prev = trans2;
        trans2->prev = tmp;

        tmp = trans2->next;
        trans2->next = trans1;
        trans1->next = tmp;
    }
    else if (trans2->next == trans1)
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

    return (1);
}

// Returns pointer to first element in linked list
struct taggedTransLList *findFirstTaggedElement(struct taggedTransLList *inElement, int verbose)
{
    while (inElement->prev != NULL)
        inElement = inElement->prev;

    return (inElement);
}

struct taggedTransLList *findLastTaggedElement(struct taggedTransLList *inElement, int verbose)
{
    while (inElement->next != NULL)
        inElement = inElement->next;

    return (inElement);
}

// Writes a table into file "fileName" or file pointer fp (cannot take both!), returns fp or -1 on error or NULL on closed file
// -If "fileName": open new file and write a line.
// -If fp: append fp with line of values.
// -If close, close fp
FILE *writeCalcMonthTabSeparatedTable(char *fileName, FILE *fp, int close, int verbose, int month, int year, int num, float argListOfValues[])
{

    int x;
    float tmpValue = 0;

    // The six ifs below are to figure out whether the first to arguments (*fileName, *fp) are correctly done and what do to in the different settings
    if (fileName != NULL && fp != NULL)
    {
        if (verbose)
            printf("writeCalcMonthTabSeparatedTable: Cannot take both fileName and fp!\n");
        return (-1);
    }
    else if (fileName == NULL && fp == NULL)
    {
        if (verbose)
            printf("writeCalcMonthTabSeparatedTable: Need either fileName or fp!\n");
        printf("month %d year %d\n", month, year);
        return (-1);
    }
    else if (fileName != NULL)
    {
        if ((fp = fopen(fileName, "wt")) == NULL)
        {
            perror("Error opening file");
            return (-1);
        }
        if (verbose)
            printf("writeCalcMonthTabSeparatedTable: Creating file %s\n", fileName);
    }
    else if (fp != NULL && close)
    {
        if (verbose)
            printf("writeCalcMonthTabSeparatedTable: Closing file\n");
        fclose(fp);
        return (NULL);
    }
    else if (fp != NULL && !close)
    {
        if (verbose)
            printf("writeCalcMonthTabSeparatedTable: Appending already open file\n");
        fseek(fp, 0, SEEK_END); // Go to end of file
    }
    else
        return (-1);

    // Write date
    fprintf(fp, "%d-%d", month, year);
    if (verbose)
        printf("writeCalcMonthTabSeparatedTable: %d-%d", month, year);

    for (x = 0; x < num; x++)
    { // float calcMonthSumOfTag(char month, int year, struct tagLList *tag, int verbose);

        tmpValue = argListOfValues[x]; // Retrieve next argument

        fprintf(fp, "\t%.2f", tmpValue);
        if (verbose)
            printf("\t%.2f", tmpValue);
    }

    // Put newline in file
    fprintf(fp, "\n");
    if (verbose)
        printf("\n");

    return (fp);
}

// Writes a table into html file "fileName" or file pointer fp (cannot take both!), returns fp or -1 on error or NULL on closed file
// -If "fileName": open new file and write a line.
// -If fp: append fp with line of values.
// -If close, close fp
FILE *writeCalcMonthHtmlCssTable(char *fileName, FILE *fp, int close, int verbose, int month, int year, int num, float argListOfValues[], char *argListOfLabels[])
{

    int x;
    float tmpValue = 0;

    // The six ifs below are to figure out whether the first to arguments (*fileName, *fp) are correctly done and what do to in the different settings
    if (fileName != NULL && fp != NULL)
    {
        if (verbose)
            printf("writeCalcMonthHtmlCssTable: Cannot take both fileName and fp!\n");
        return (-1);
    }
    else if (fileName == NULL && fp == NULL)
    {
        if (verbose)
            printf("writeCalcMonthHtmlCssTable: Need either fileName or fp!\n");
        printf("month %d year %d\n", month, year);
        return (-1);
    }
    else if (fileName != NULL)
    {
        if ((fp = fopen(fileName, "wt")) == NULL)
        {
            perror("Error opening file");
            return (-1);
        }

        //initHtmlTableFile(fp);
        //writeLegendPositionToHtmlTableFile(fp, "Test", 1.4);
        //writeCategoryPositionToHtmlTableFile(fp, "char *categoryId", 200);
        //writeLabelPositionToHtmlTableFile(fp, "Tjobla", 600);
        //writeEndStylingStartDataToHtmlTableFile(fp);
        //writeLegendNameToHtmlTableFile(fp, "Tjobla2");
        //writeEndLegendNamesToHtmlTableFile(fp);

        if (verbose)
            printf("writeCalcMonthHtmlCssTable: Creating file %s\n", fileName);
    }
    else if (fp != NULL && close)
    {
        //endHtmlTableFile(fp);
        
        if (verbose)
            printf("writeCalcMonthHtmlCssTable: Closing file\n");
        fclose(fp);
        return (NULL);
    }
    else if (fp != NULL && !close)
    {
        if (verbose)
            printf("writeCalcMonthHtmlCssTable: Appending already open file\n");
        fseek(fp, 0, SEEK_END); // Go to end of file
    }
    else
        return (-1);

    // Write date
    //writeDateToHtmlTableFile(fp, month, year);

    if (verbose)
        printf("writeCalcMonthHtmlCssTable: %d-%d", month, year);
    
    //writeLabelsToHtmlTableFile(fp, num, argListOfLabels);

    for (x = 0; x < num; x++)
    {

        tmpValue = argListOfValues[x]; // Retrieve next argument

        //writeValueToHtmlTableFile(fp, tmpValue);
        //writeCategoryDataToHtmlTableFile(fp, argListOfLabels[x], argListOfValues, (int)(argListOfValues[x]/100), NULL);

        if (verbose)
            printf("\t%.2f", tmpValue);
    }

    //writeEndCategoryDataToHtmlTableFile(fp);

    //writeTicksToHtmlTableFile(fp, 20, 462.3);
    //endHtmlTableFile(fp);

    if (verbose)
        printf("\n");

    return (fp);
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
    startYear = taggedList->trans->year;

    // Extract last month and year
    taggedList = findLastTaggedElement(taggedList, verbose);

    endMonth = taggedList->trans->month;
    endYear = taggedList->trans->year;

    // Prepare loop
    currentYear = startYear;
    currentMonth = startMonth;

    while ((currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear))
    {
        tmpValue[0] = calcMonthSumOfTag(currentMonth, currentYear, tagList, verbose);

        // If first call; open new file
        if (currentMonth == startMonth && currentYear == startYear)
            fp = writeCalcMonthTabSeparatedTable(fileName, NULL, 0, verbose, currentMonth, currentYear, 1, tmpValue);
        // Otherwise, append fp
        else
            fp = writeCalcMonthTabSeparatedTable(NULL, fp, 0, verbose, currentMonth, currentYear, 1, tmpValue);

        // Date counter
        currentMonth++;
        if (currentMonth > 12)
        {
            currentMonth = 1;
            currentYear++;
        }
    }

    // Close file
    writeCalcMonthTabSeparatedTable(NULL, fp, 1, verbose, currentMonth, currentYear, 0, NULL);

    return (0);
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
    endYear = transList->year;

    // Extract first month and year
    transList = findFirstElement(transList, verbose);

    startMonth = transList->month;
    startYear = transList->year;

    // Prepare loop
    currentYear = startYear;
    currentMonth = startMonth;

    while ((currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear))
    {
        tmpValue[0] = calcMonthSumOfTrans(currentMonth, currentYear, transList, verbose);

        // If first call; open new file
        if (currentMonth == startMonth && currentYear == startYear)
            fp = writeCalcMonthTabSeparatedTable(fileName, NULL, 0, verbose, currentMonth, currentYear, 1, tmpValue);
        // Otherwise, append fp
        else
            fp = writeCalcMonthTabSeparatedTable(NULL, fp, 0, verbose, currentMonth, currentYear, 1, tmpValue);

        // Date counter
        currentMonth++;
        if (currentMonth > 12)
        {
            currentMonth = 1;
            currentYear++;
        }
    }

    // Close file
    writeCalcMonthTabSeparatedTable(NULL, fp, 1, verbose, currentMonth, currentYear, 0, NULL);

    return (0);
}

// Writes a file "fileName" with monthly ratios of all tags to total sum of costs.
int writeMonthSumOfTags(char *fileName, struct transLList *transList, struct tagLList *tagList, int verbose)
{
    struct taggedTransLList *taggedList;
    struct tagLList *tagListStart;
    char startMonth, endMonth, currentMonth;
    int startYear, endYear, currentYear, n = 0, num = tagListLen(tagList, verbose);
    FILE *fp;
    float tmpTagValue[num];

    /* tagLables must be freed! */
    char **tagLabels = getTagLabels(tagList);

    if (verbose)
        printf("writeMonthSumOfTags: Entering\n");

    // Go to first tag
    tagList = findFirstTag(tagList);
    tagListStart = tagList;

    taggedList = tagList->transList;

    // Extract last month and year
    transList = findLastElement(transList, verbose);

    endMonth = transList->month;
    endYear = transList->year;

    // Extract first month and year
    transList = findFirstElement(transList, verbose);

    startMonth = transList->month;
    startYear = transList->year;

    // Prepare loop
    currentYear = startYear;
    currentMonth = startMonth;

    if(strcmp(getFileExtension(fileName), "html") == 0)
    {
        writeCalcMonthHtmlFile(fileName, startYear, endYear, startMonth, endMonth, tagList, verbose);
        return 0;
    }

    while ((currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear))
    {
        // Reset counters
        tagList = tagListStart;
        n = 0;

        // Calculate cost ratios for every tag
        while (tagList != NULL)
        {
            tmpTagValue[n] = calcMonthSumOfTag(currentMonth, currentYear, tagList, verbose);
            n++;
            tagList = tagList->next;
        }

        // If first call; open new file
        if (currentMonth == startMonth && currentYear == startYear)
            fp = writeCalcMonthTabSeparatedTable(fileName, NULL, 0, verbose, currentMonth, currentYear, num, tmpTagValue);
        // Otherwise, append fp
        else
            fp = writeCalcMonthTabSeparatedTable(NULL, fp, 0, verbose, currentMonth, currentYear, num, tmpTagValue);

        // Date counter
        currentMonth++;
        if (currentMonth > 12)
        {
            currentMonth = 1;
            currentYear++;
        }
    }

    // Close file
    if (writeCalcMonthTabSeparatedTable(NULL, fp, 1, verbose, currentMonth, currentYear, 0, NULL) == NULL)
        printf("writeMonthSumOfTags: Successfully wrote %s\n", fileName);

    if (verbose)
        printf("writeMonthSumOfTags: Exiting\n");

    /* Freeing tag labels list */
    for(int i = 0; i < num; i++)
        free(tagLabels[i]);
    free(tagLabels);

    return 1;
}

struct tagLList *findFirstTag(struct tagLList *tagList)
{
    while (tagList->prev != NULL)
        tagList = tagList->prev;

    return (tagList);
}

int tagListLen(struct tagLList *tagList, int verbose)
{
    int c = 0;

    tagList = findFirstTag(tagList);

    while (tagList != NULL)
    {
        tagList = tagList->next;
        c++;
    }

    return (c);
}

struct tagLList *findLastTag(struct tagLList *tagList)
{
    while (tagList->next != NULL)
        tagList = tagList->next;

    return (tagList);
}

// Returns pointer to last element in keyword list
struct keyWord *findLastKeyword(struct keyWord *inKeyword)
{
    if (inKeyword == NULL)
        return (NULL);

    while (inKeyword->next != NULL)
        inKeyword = inKeyword->next;

    return (inKeyword);
}

// Frees a list of tragged transactions (DOES NOT FREE THE TRANSACTIONS THEMSELVES. ONLY THE TAGGED LIST) Returns # of freed elements
int freeTaggedTransList(struct taggedTransLList *taggedTrans, int verbose)
{
    struct taggedTransLList *current;
    int count = 0;

    if (verbose)
        printf("freeTaggedTransList: Entering\n");

    while (taggedTrans != NULL)
    {
        current = taggedTrans;
        taggedTrans = taggedTrans->next;
        free(current);
        count++;
    }

    if (verbose)
        printf("freeTaggedTransList: Freed %d tagged elements. Exiting\n", count);

    return (count);
}

void writeCalcMonthHtmlFile(char *fileName, int startYear, int endYear, int startMonth, int endMonth, struct tagLList *tagList, int verbose)
{
    static int categoryWidth = 300;
    static int chartHeight = 500;
    static int barMargin = 5;
    static int categoryMargin = 30;

    int tagIndex = 0, monthIndex = 0;
    int currentYear = startYear;
    int currentMonth = startMonth;
    int numberOfTags = getTagCount(tagList, verbose);
    int numberOfMonths = (endYear-startYear)*12+(endMonth-startMonth);
    float **tagValues = malloc( sizeof(float*)*numberOfMonths );
    float maxTagValue;
    int *tagPixelsValues = malloc( sizeof(int)*numberOfTags );

    int chartWidth = (int)categoryWidth * numberOfMonths;
    int barWidth = (int)(categoryWidth - categoryMargin) / numberOfTags - barMargin;
    int ticksWidth = (int)categoryWidth * numberOfMonths;
    int gridSegmentWidth = categoryWidth;
    float legendWidth = 7.5;

    int categoryPosition = 0, labelPosition;
    float legendPosition, legendDistance = 2.5;

    char tempTagName[50];

    struct tagLList *tag, *tagListStart = findFirstTag(tagList);
    FILE *fp;

    /* Open file */
    if ((fp = fopen(fileName, "wt")) == NULL)
    {
        perror("Error opening file");
        return;
    }

    initHtmlTableFile(fp, chartWidth, chartHeight, barWidth, ticksWidth, gridSegmentWidth, legendWidth);

    tag = tagListStart;
    legendPosition = 0;
    labelPosition = barMargin;
    while(tag != NULL)
    {
        char labelColor[7];

        rgb color = getRandomRgbColor(1, 1);

        sprintf(labelColor, "%02X%02X%02X", (int)(255*color.r), (int)(255*color.g), (int)(255*color.g));

        strReplace(tempTagName, tag->name, ' ', '_');
        writeLegendPositionToHtmlTableFile(fp, tempTagName, legendPosition);
        writeLabelPositionToHtmlTableFile(fp, tempTagName, labelPosition, labelColor);
        tag = tag->next;
        legendPosition += legendDistance;
        labelPosition += barWidth + barMargin;
    }

    maxTagValue = 0;

    /* Iterate through all months */
    while ((currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear))
    {
        char categoryId[50];
        sprintf(categoryId, "%d_-_%d", currentMonth, currentYear);
        insertCharacterAtBeginningOfString(categoryId, categoryId, 's');
        writeCategoryPositionToHtmlTableFile(fp, categoryId, categoryPosition);
        categoryPosition += categoryWidth;

        // Reset counters
        tag = tagListStart;
        tagIndex = 0;
        tagValues[monthIndex] = malloc(sizeof(float)*numberOfTags);

        // Calculate cost ratios for every tag
        while (tag != NULL)
        {
            tagValues[monthIndex][tagIndex] = -calcMonthSumOfTag(currentMonth, currentYear, tag, verbose);

            /* Find biggest value */
            if(tagValues[monthIndex][tagIndex] > maxTagValue)
                maxTagValue = tagValues[monthIndex][tagIndex];

            tagIndex++;
            tag = tag->next;
        }

        // Date counter
        currentMonth++;
        if (currentMonth > 12)
        {
            currentMonth = 1;
            currentYear++;
        }

        monthIndex++;
    }

    writeEndStylingStartDataToHtmlTableFile(fp);

    tag = tagListStart;
    while (tag != NULL)
    {
        strReplace(tempTagName, tag->name, ' ', '_');
        writeLegendNameToHtmlTableFile(fp, tempTagName, tag->name);
        tag = tag->next;
    }

    writeEndLegendNamesToHtmlTableFile(fp);

    monthIndex = 0;
    currentYear = startYear;
    currentMonth = startMonth;
    while ((currentYear == endYear && currentMonth <= endMonth) || (currentYear < endYear))
    {
        char categoryName[50];
        sprintf(categoryName, "%d - %d", currentMonth, currentYear);
        strReplace(tempTagName, categoryName, ' ', '_');
        insertCharacterAtBeginningOfString(tempTagName, tempTagName, 's');

        /* Scale tag values into pixels */
        for(int i = 0; i< numberOfTags; i++)
            tagPixelsValues[i] = (int)chartHeight*tagValues[monthIndex][i]/maxTagValue;

        writeCategoryDataToHtmlTableFile(fp, currentMonth, currentYear, tempTagName, categoryName, tagValues[monthIndex], tagPixelsValues, tagListStart);

        /* Free month */
        free(tagValues[monthIndex]);

        // Date counter
        currentMonth++;
        if (currentMonth > 12)
        {
            currentMonth = 1;
            currentYear++;
        }

        monthIndex++;
    }

    free(tagValues);
    free(tagPixelsValues);

    writeEndCategoryDataToHtmlTableFile(fp);

    // for(int i = 1; i < 5; i++)
    // {
    //     writeTicksToHtmlTableFile(fp, (int)chartHeight/4, i*maxTagValue/4);
    // }

    endHtmlTableFile(fp);
}

void initHtmlTableFile(FILE *fp, int chartWidth, int chartHeight, int barWidth, int ticksWidth, int gridSegmentWidth, float legendWidth)
{
    fprintf(fp, "<!DOCTYPE HTML><html><head><meta charset=\"UTF-8\"><title>My transactions by category</title><style>body,html {height: 100%%;}");
    fprintf(fp, "body {display: inline-block;justify-content: center;align-items: center;font-family: \"fira-sans-2\", Verdana, sans-serif;}");
    fprintf(fp, "#q-graph {display: block;position: absolute;width: %dpx;height: %dpx;margin: 1.1em 0 0;padding: 0;background: transparent;font-size: 11px;}", chartWidth, chartHeight);
    fprintf(fp, "#q-graph caption {caption-side: top;width: 100px; text-transform: uppercase;letter-spacing: .5px;position: fixed;z-index: 10;font-weight: bold;left: 50%%;}");
    fprintf(fp, "#q-graph .qtr,#q-graph th,#q-graph .bar {position: absolute;bottom: 0;width: %dpx;", gridSegmentWidth);
    fprintf(fp, "z-index: 2;margin: 0;padding: 0;text-align: center;}");
    fprintf(fp, "#q-graph td {transition: all .3s ease;}");
    fprintf(fp, "#q-graph .bar:hover {background-color: desaturate(#85144b, 100);	opacity: .9;color: white;}");
    fprintf(fp, "#q-graph thead tr {position: fixed;right: 130px;top: 100px;bottom: auto;margin: -2.5em 0 0 5em;}");
    fprintf(fp, "#q-graph thead th {position: aboslute; width: %fem;height: auto;padding: 0.5em 1em;}", legendWidth);
    fprintf(fp, " #q-graph tbody .qtr {height: %dpx;	padding-top: 2px;border-right: 1px dotted #C4C4C4;color: #AAA;}", chartHeight);
    fprintf(fp, "#q-graph tbody th {bottom: -1.75em;vertical-align: top;font-weight: normal;color: #333;}");
    fprintf(fp, "#q-graph .bar {width: %dpx;border: 1px solid;border-bottom: none;color: #000;}", barWidth);
    fprintf(fp, "#q-graph .bar p {position: relative; transform: rotate(-90deg); bottom: 25px; margin: 5px 0 0;padding: 0;opacity: .4;}");
    fprintf(fp, "#q-graph .bar .transBox {text-align: left;color: black;display: inherit;position: absolute;left: 100%%;width: auto;z-index: 998;color: black;background-color: lightyellow;border-radius: 10px;padding: 1em;}");
    fprintf(fp, "#q-graph .bar .transLine {font: 10px/1.0 'Lucida Grande', sans-serif;color: black;z-index: 999;}");
    fprintf(fp, "#q-graph .bar .transBox, #q-graph .bar .transBox .transLine {transition-duration: 0.5s;transition-delay: 0.5s;visibility: hidden;opacity: 0;}");
    fprintf(fp, "#q-graph .bar:hover .transBox, #q-graph .bar:hover .transBox .transLine {visibility: visible;opacity: 1;}");
    fprintf(fp, "#ticks {position: fixed;left: 2px;width: %dpx;height: %dpx;z-index: 1;	margin: 1.1em 1.1em 0;font-size: 10px;font-family: \"fira-sans-2\", Verdana, sans-serif;}", ticksWidth, chartHeight);
    fprintf(fp, "#ticks .tick {	position: relative;	border-bottom: 1px dotted #C4C4C4;width: %dpx;}", chartWidth);
    fprintf(fp, "#ticks .tick p {position: absolute;left: -5em;	top: -0.8em;margin: 0 0 0 4em;}");
}

void writeLegendPositionToHtmlTableFile(FILE *fp, char *legendName, float legendPosition)
{
    fprintf(fp, " #q-graph thead th.%s {top: %fem;left: 0;line-height: 2;}", legendName, legendPosition);
}

void writeCategoryPositionToHtmlTableFile(FILE *fp, char *categoryId, int categoryPosition)
{
    fprintf(fp, "#q-graph #%s {left: %dpx;}", categoryId, categoryPosition);
}

void writeLabelPositionToHtmlTableFile(FILE *fp, char *labelClass, int labelPosition, char *labelColor)
{
    fprintf(fp, "#q-graph .%s {left: %dpx;background-color: #%s;border-color: transparent;}", labelClass, labelPosition, labelColor);
}

void writeEndStylingStartDataToHtmlTableFile(FILE *fp)
{
    fprintf(fp, "</style></head><body><table id=\"q-graph\"><caption>Title</caption><thead><tr>");
}

void writeLegendNameToHtmlTableFile(FILE *fp, char *labelClass, char *labelName)
{
    fprintf(fp, "<th class=\"%s\">%s</th>", labelClass, labelName);
}

void writeEndLegendNamesToHtmlTableFile(FILE *fp)
{
    fprintf(fp, "</tr></thead><tbody>");
}

void writeCategoryDataToHtmlTableFile(FILE *fp, int month, int year, char *categoryId, char *categoryName, float dataList[], int *dataPixelList, struct tagLList *tag)
{
    int i = 0;
    char tempTagName[50], date[50];
    struct taggedTransLList *currentTransaction = tag->transList;

    fprintf(fp, "<tr class=\"qtr\" id=\"%s\"><th scope=\"row\">%s</th>", categoryId, categoryName);

    while(tag != NULL)
    {
        strReplace(tempTagName, tag->name, ' ', '_');
        fprintf(fp, "<td class=\"%s bar\"style=\"height: %dpx;\">", tempTagName, dataPixelList[i]);
        fprintf(fp, "<p>%0.2f</p>", dataList[i]);
        fprintf(fp, "<table class=\"transBox\"><tbody>");

        currentTransaction = tag->transList;

        while (currentTransaction != NULL)
        {
            if (currentTransaction->trans->month == month && currentTransaction->trans->year == year)
            {
                sprintf(date, "%d/%d %d", currentTransaction->trans->day, currentTransaction->trans->month, currentTransaction->trans->year);

                fprintf(fp, "<tr class=\"transLine\">");
                fprintf(fp, "<td class=\"date\">%s</td>", date);
                fprintf(fp, "<td class=\"description\">%s</td>", currentTransaction->trans->description);
                fprintf(fp, "<td class=\"value\">%.2f</td></tr>", currentTransaction->trans->value);
            }

            currentTransaction = currentTransaction->next;
        }

        fprintf(fp, "</tbody></table></td>");

        i++;
        tag = tag->next;
    }

    fprintf(fp, "</tr>");
}

void writeEndCategoryDataToHtmlTableFile(FILE *fp)
{
    fprintf(fp, "</tbody></table><div id=\"ticks\">");
}

void writeTicksToHtmlTableFile(FILE *fp, int tickHeight, float tickValue)
{
    fprintf(fp, "<div class=\"tick\" style=\"height: %dpx;\">", tickHeight);
    fprintf(fp, "<p>%f.0</p></div>", tickValue);
}

void endHtmlTableFile(FILE *fp)
{
    fprintf(fp, "</div></body></html>");
}

void writeDateToHtmlTableFile(FILE *fp, int month, int year)
{
    fprintf(fp, "<h1>%d - %d</h1>\n", year, month);
}

void writeLabelsToHtmlTableFile(FILE *fp, int num, char *label[])
{
    fprintf(fp, "<li>\n");

    for(int i = 0; i < num; i++)
        fprintf(fp, "\t<div class=\"label\">%s</div>\n", label[i]);

    fprintf(fp, "</li>\n");
}

void writeValueToHtmlTableFile(FILE *fp, float value)
{
    fprintf(fp, "<li class=\"teal\" style=\"height:95%%;\">\n<div class=\"percent\">%f.2</li>\n", -value);
}

char **getTagLabels(struct tagLList *tagList)
{
    struct tagLList *currentTag = tagList;
    char **listOfTagLabels;

    /* get longest tag string 
       and count number of tags */
    int longest = 0, numberOfTags = 0;

    while(currentTag != NULL)
    {
        numberOfTags++;
        int currentLength = strlen(currentTag->name);

        if(currentLength > longest)
            longest = currentLength;

        currentTag = currentTag->next;
    }

    /* Allocate memory */
    listOfTagLabels = malloc(numberOfTags*sizeof(char*));

    /* Copy all tag labels into list */
    currentTag = tagList;
    
    for(int i = 0; i < numberOfTags; i++)
    {
        listOfTagLabels[i] = malloc((longest+1)*sizeof(char*));
        strcpy(listOfTagLabels[i], currentTag->name);
        currentTag = currentTag->next;
    }

    return listOfTagLabels;
}