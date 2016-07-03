#ifndef UNITHEME_H
#define UNITHEME_H

#include <stdbool.h>
#include <stdio.h>

struct programDef
{
  
};

void loadUniTheme(const char *filename);

void strTrim(char *in);

void rmComment(char *in);

bool hasLineExtension(char *currentBuffer);

void getFullLine(char *currentBuffer, FILE *UniThemeFile);

void evalLine(char *currentBuffer);

#endif
