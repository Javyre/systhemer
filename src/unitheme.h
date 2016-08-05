#ifndef UNITHEME_H
#define UNITHEME_H

#include <stdbool.h>
#include <stdio.h>

struct programDef
{
  char *name;
  char *tokens;
  char *path;
  char *execBefore;
  int beginDef;
  int endDef;
  char *execAfter;
};
struct programDef currentProg;

void loadUniTheme(const char *filename);

void rmComment(char *in);

void rmEscape(char **currentBuffer);

bool hasLineExtension(char **currentBuffer);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

bool isList (char *in, char **outListName, char ***outListItems, int *outNumItems);

bool isAssignation (char *in, char **outTok, char **outValue);

bool isStatement (char* in, char** outCall, char** outArg);

void evalLine(char *currentBuffer);

void evalList(char* currentBuffer, char* listName, char** listItems, int numItems);

void evalAssig(char* currentBuffer, char* tok, char* value);

#endif
