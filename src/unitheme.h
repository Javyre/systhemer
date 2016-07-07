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

void loadUniTheme(const char *filename);

void strTrim(char *in);

void rmComment(char *in);

void rmEscape(char **currentBuffer);

bool isLastExtension(char *last, char* first); 

bool hasLineExtension(char **currentBuffer);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

void evalLine(char *currentBuffer);

#endif
