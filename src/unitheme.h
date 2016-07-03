#ifndef UNITHEME_H
#define UNITHEME_H

#include <stdbool.h>
#include <stdio.h>

void loadUniTheme(const char *filename);

void strTrim(char *in);

bool hasLineExtension(char *currentBuffer);

void getFullLine(char *currentBuffer, FILE *UniThemeFile);

void evalLine(char *currentBuffer);

#endif
