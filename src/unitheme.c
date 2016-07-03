#include "utils.h"
#include "unitheme.h"

#include <stdlib.h>
#include <string.h>

void loadUniTheme(const char *filename) {
  char *buff = malloc(256);
  FILE *UniThemeFile = fopen(filename, "r");

  if (UniThemeFile == NULL) {
    fprintf(stderr, "Error: file %s not found!", filename);
    return;
  }

  while (fgets(buff, 256, UniThemeFile) != NULL) {
    if (buff[0] == '\n' || buff[0] == '#') {
      continue;
    } else {
      getFullLine(buff, UniThemeFile);
      evalLine(buff);
    }
  }
}

void strTrim(char *in) {
  char *firstin = in; // Hold position of first char in char* in
  char *source = in;  // Used to rewrite char* in

  while (*in != '\0' && *in != '\n') { // While it hasnt reached the last char 
    if (*in != ' ' && *in != '\t') {
      *source = *in;
      if (*(in + 1) == ' ' || *(in + 1) == '\t') {
        source++;
        in++;
        *source = ' ';
      }
      source++;
      in++;
    } else {
      in++;
    }
  }
  if (*(source - 1) == ' ')
    *(source - 1) = '\0';
  else
    *source = '\0';

  VERBOSE_PRINT_VALUE(%s, firstin);
  in = firstin;
}

void rmComment(char *in) {
  char *src = in;
  while (*src != '\0' && *src != '\n') {
    if (*src == '#')
      *src = '\0';
    src++;
  }
}

bool hasLineExtension(char *currentBuffer) {
  strTrim(currentBuffer);
  char *lastChar = currentBuffer;
  while (*lastChar != '\0' && *lastChar != '\n')
    lastChar++;
  if (*(lastChar-1) == '\\' && *lastChar == '\0') { // lastchar-1 should point to the \ and lastchar: \0
    *(lastChar-1) = *lastChar;
    return true;
  } else if (*(lastChar-1) == '\\' && *lastChar == '\n') {
    fprintf(stderr, "Error in hasLineExtension: last char of string is '\\n' in");
    exit(1);
  }

  return false;
}

void getFullLine(char* currentBuffer, FILE *UniThemeFile) {
  char *holder = malloc(strlen(currentBuffer) + 256 + 1);
  strcpy(holder, currentBuffer);
  while (hasLineExtension(holder)) {
    //line+=new;
    rmComment(holder);
    strcat(holder, " ");
    strcat(holder, fgets(currentBuffer, 256, UniThemeFile));
    rmComment(holder);
  }
  rmComment(holder);
  strcpy(currentBuffer, holder);
}

void evalLine(char* currentBuffer) {
  char *src = currentBuffer;
  while (*src == ' ' || *src == '\t')
    src++;



  VERBOSE_PRINT_VALUE(%s, currentBuffer);
}
