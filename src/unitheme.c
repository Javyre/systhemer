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

  while (fgets(buff, sizeof(buff), UniThemeFile) != NULL) {
    if (buff[0] == '\n' || buff[0] == '#') {
      continue;
    } else {
      getFullLine(buff, UniThemeFile);
      evalLine(buff);
    }
  }
}

bool hasLineExtension(char *currentBuffer) {
  char *s = currentBuffer;
  while (*s != '\0') {
    if (*s == '\\') {
      s++;
      while (*s == ' ' || *s == '\t' || *s == '\0' || *s == '\n') {
        if (*s == '\0' || *s == '\n') {
          return true;
        }
        s++;
      }
      continue;
    }

    s++;
  }
  return false;
}

void getFullLine(char* currentBuffer, FILE *UniThemeFile) {
  while (hasLineExtension(currentBuffer)) {
    //line+=new;
    strcat(currentBuffer, fgets(currentBuffer, sizeof(currentBuffer), UniThemeFile));
  }
}

void evalLine(char* currentBuffer) {
  return;
}
