#ifndef UNITHEME_H
#define UNITHEME_H

#include <stdbool.h>
#include <stdio.h>

typedef struct
{
  char *name;
  char *tokens;
  char *path;
  char *execBefore;
  unsigned int *beginDef;
  unsigned int *endDef;
  char *execAfter;
} programDef;
programDef *currentProg;

typedef struct
{
  programDef **progs;
  size_t used;
  size_t size;
} programDefs;
programDefs progDefs;

unsigned int currentLine;

void loadUniTheme(const char *filename);

void defsInitArr(programDefs *a, size_t initialSize);

void defsInsert(programDefs *a, programDef *element);

void defsFree(programDefs *a);

void rmComment(char *in);

void rmEscape(char **currentBuffer);

bool hasLineExtension(char **currentBuffer);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

bool isList (char *in, char **outListName, char ***outListItems, int *outNumItems);

bool isAssignation (char *in, char **outTok, char **outValue);

bool isStatement (char* in, char** outCall, char** outArg);

void evalLine(char *currentBuffer);

void evalStatement(char *currentBuffer, char *statCall, char *statArg);

void evalAssig(char* currentBuffer, char* tok, char* value);

void evalList(char* currentBuffer, char* listName, char** listItems, int numItems);


#endif
