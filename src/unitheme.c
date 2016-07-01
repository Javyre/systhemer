#include "utils.h"
#include "unitheme.h"

#include <stdio.h>
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
      getFullLine(buff);
      evalLine(buff);
    }
  }
}

void getFullLine(char* currentBuffer) {
  return;
}

void evalLine(char* currentBuffer) {
  return;
}
