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
  int start;
  int end;
  char *execAfter;
};
struct programDef currentProg;

void loadUniTheme(const char *filename);

void rmComment(char *in);

void rmEscape(char **currentBuffer);

bool hasLineExtension(char **currentBuffer);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

bool isList (char *in);

bool isAssignation (char *in, char **outTok, char **outValue);

void evalLine(char *currentBuffer);

void evalAssig(char* currentBuffer, char* tok, char* value);

#endif
