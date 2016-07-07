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
      getFullLine(&buff, UniThemeFile);
      rmEscape(&buff);
      strTrimStrAware(buff);
      evalLine(buff);
    }
    buff = realloc(buff, 256);
  }
  free(buff);
  fclose(UniThemeFile);
}

char *genWrongUnderline(char *line, char *from, char *to) {
  char *underline = calloc(strlen(line)+1, sizeof(char));
  char *srcUnderline = underline;
  char *srcLine = line;
  while (*srcLine != '\0') {
    if (srcLine >= from && srcLine <= to)
      *srcUnderline = '^';
    else
      *srcUnderline = ' ';
    srcLine++;
    srcUnderline++;
  }
  return underline;
}

void rmComment(char *in) {
  char *src = in;
  while (*src != '\0' && *src != '\n') {
    if (*src == '#' && *(src-1) != '\\')
      *src = '\0';
    src++;
  }
}

void rmEscape(char **currentBuffer) {
  char *original = malloc(strlen(*currentBuffer)+1);
  strcpy(original, *currentBuffer);
  char *src = original;
  /*
  while (*src != '\0' && *src != '\n')
    src++;
  if (*src == '\n')
    fprintf(stderr, "Warning: found a '\\n' (newline) character in current buffer string! This is dangerous!!!\n");
  src = original;
  */
  while (*src != '\0') {
    if (*src == '\\' && (src[1] == '\\' || src[1] == '#')) {
      strOverlap(original, original, (src-1), (src+1), NULL);
      src++;
    } else if (*src == '\\' && (src[1] != '\\' && src[1] != '#')) {
      fprintf(stderr, BKRED "Error: Found escape character with invalid successor \"%c\": \n\t%s\n\t%s\n", src[1], original, genWrongUnderline(original, src, src+1));
      exit(1);
    }
    src++;
  }

  *currentBuffer = realloc(*currentBuffer, strlen(original)+1);
  strcpy(*currentBuffer, original);
  free(original);
}

bool hasLineExtension(char **currentBuffer) {
  char *src = *currentBuffer;
  while (*src != '\0')
    src++;

  if (*(src-1) == ';' || (*(src-2) == ';' && *(src-1) == '\n')){
    src = *currentBuffer;
    while (*src != ';')
      src++;
    *(src+1) = '\0';
    *currentBuffer = realloc(*currentBuffer, strlen(*currentBuffer)+1);
    return false;
  }

  return true;
}

void getFullLine(char **currentBuffer, FILE *UniThemeFile) {
  //char *holder = malloc(strlen(currentBuffer) + 1);
  char *holder = calloc(256, sizeof(char));
  //char *nextLine = malloc(256);
  strcpy(holder, *currentBuffer);
  rmComment(holder);
  while (hasLineExtension(&holder)) {
    //line+=new;
    fgets(*currentBuffer, 256, UniThemeFile);
    holder = realloc(holder, strlen(holder) + 1 + strlen(*currentBuffer) + 1);
    strcat(holder, " ");
    strcat(holder, *currentBuffer);
    rmComment(holder);
  }
  *currentBuffer = realloc(*currentBuffer, strlen(holder)+1);
  strcpy(*currentBuffer, holder);

  //free(nextLine);
  free(holder);
}

void evalLine(char* currentBuffer) {
  char *src = currentBuffer;
  while (*src == ' ' || *src == '\t')
    src++;
  VERBOSE_PRINT("Evaluating line bufer...");
  VERBOSE_PRINT_VALUE(%s, src);


}
