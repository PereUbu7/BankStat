#ifndef STRINGUTILS
#define STRINGUTILS

#include <stdlib.h>

int ctod(char *value);
int replaceCharacterWithStringEnd(char *string, char character);
char *getFileExtension(char *filename);
int strReplace(char *dest, char *src, char target, char goal);
void insertCharacterAtBeginningOfString(char *dest, char *src, char character);
void getRandomHexColor(char color[]);


#endif