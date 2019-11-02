#include "stringUtils.h"

// Comma to dot
//--------------
// Removes all commas (,) in a string an replaces them with dots (.)
// Also removes all blanks
int ctod(char *value)
{
    int count = 0;
    int blanks = 0;
    
    while(value[count+blanks] != 0)
    {
                if(value[count+blanks] == ',') 
                {           
                value[count] = '.';          
                count++;
                }
                else if(value[count+blanks] == ' ' || value[count+blanks] == '\t' || value[count+blanks] == '\n')
                blanks++;
                else
                {
                value[count] = value[count+blanks];
                count++;
                }
    }
    return(0);
}

int replaceCharacterWithStringEnd(char *string, char character)
{
    int i = 0;

    while(string[i] != '\0')
    {
        if(string[i] == character)
        {
            string[i] = '\0';
            return 1;
        }
        i++;
    }
    return 0;
}