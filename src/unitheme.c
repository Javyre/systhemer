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
      rmEscape(buff);
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

void strOverlap(char *dest, char *from, char *to, char *from2, char *to2) {
  if (to2 == NULL) {
    to2 = from2;
    while (*to2 != '\0')
      to2++;
  }

  char *holder = malloc((to-from)+(to2-from2)+1);
  char *holder2 = malloc((to2-from2)+1);

  strncpy(holder, from, to-from);
  strncpy(holder2, from2, to2-from2);
  strcat(holder, holder2);
  strcpy(dest, holder);


  free(holder);
  free(holder2);
}

void rmComment(char *in) {
  char *src = in;
  while (*src != '\0' && *src != '\n') {
    if (*src == '#' && *(src-1) != '\\')
      *src = '\0';
    src++;
  }
}

void rmEscape(char *currentBuffer) {
  char *src = currentBuffer;
  while (*src != '\0' && *src != '\n')
    src++;
  if (*src == '\n')
    fprintf(stderr, "Warning: found a '\n' (newline) character in current buffer string! This is dangerous!!!");
  src = currentBuffer;
  while (*src != '\0') {
    if (*src == '\\' && (src[1] == '\\' || src[1] == '#')) {
      strOverlap(currentBuffer, currentBuffer, (src-1), (src+1), NULL);
    }
    src++;
  }
}

//check if last char is for a line extension or just part of '//'
bool isLastExtension(char *last, char* first) {
  char* src = last;
  int numOfBSlashes = 0;
  while (*src == '\\' && src != first) {
    numOfBSlashes ++;
    src--;
  }
  if (numOfBSlashes % 2 != 0)
    return true;
  return false;
}

bool hasLineExtension(char *currentBuffer) {
  strTrim(currentBuffer);
  char *lastChar = currentBuffer;
  while (*lastChar != '\0' && *lastChar != '\n')
    lastChar++;

  //if (*(lastChar-1) == '\\' && *lastChar == '\0' && *(lastChar-2) != '\\') { // lastchar-1 should point to the \ and lastchar: \0
  if (isLastExtension((lastChar-1), currentBuffer)) {
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
  rmComment(holder);
  while (hasLineExtension(holder)) {
    //line+=new;
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
