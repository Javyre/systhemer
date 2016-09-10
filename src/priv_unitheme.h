#ifndef PRIV_UNITHEME_H
#define PRIV_UNITHEME_H

#include <stdbool.h>
#include <stdio.h>

size_t currentLine;

void rmComment(char *in);

void rmEscape(char **currentBuffer);

bool hasLineExtension(char **currentBuffer);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

bool isList (char *in, char **outListName, list **outListItems);

bool isAssignation (char *in, char **outTok, char **outValue);

bool isStatement (char* in, char** outCall, char** outArg);

void evalLine(char *currentBuffer);

void evalStatement(char *currentBuffer, char *statCall, char *statArg);

void evalAssig(char* currentBuffer, char* tok, char* value);

void evalList(char* currentBuffer, char* listName, list *content);

#endif
