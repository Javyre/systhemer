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
      //rmEscape(&buff);
      strTrimStrAware(buff);
      rmEscape(&buff);
      evalLine(buff);
    }
    buff = realloc(buff, 256);
  }
  free(buff);
  fclose(UniThemeFile);
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
    if (!isInsideOfStr(original, src)) {
      if (*src == '\\' && (src[1] == '\\' || src[1] == '#')) {
        strOverlap(original, original, (src-1), (src+1), NULL);
        src++;
      } else if (*src == '\\' && (src[1] != '\\' && src[1] != '#')) {
        fprintf(stderr, BKRED "Error: Found escape character outside of string with invalid successor \"%c\": \n\t%s\n\t%s\n", src[1], original, genWrongUnderline(original, src, src+1));
        exit(1);
      }
    } else {
      if (*src == '\\' && (src[1] == '\"')) {
        strOverlap(original, original, (src-1), (src+1), NULL);
        src++;
      } /*else if (*src == '\\' && (src[1] != '\"')) {
        fprintf(stderr, BKRED "Error: Found escape character inside of string with invalid successor \"%c\": \n\t%s\n\t%s\n", src[1], original, genWrongUnderline(original, src, src+1));
        exit(1);
        }*/

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

bool isList (char *in) {
  int lengthOfIn = strlen(in)+1;
  char *copy = malloc(lengthOfIn);
  memset(copy, '\0', lengthOfIn);

  bool isList = false;
  bool open = false;
  char *whereOpened;
  char *tok = strtok(copy, " ");

  while (tok != NULL) {
    if (*tok == '{') {
      if (open) {
        fprintf(stderr, BKRED "Error: List opened more than once: \n\t%s\n\t%s\n", copy, genWrongUnderline(copy, tok, tok));
        exit(1);
      }
      whereOpened = tok;
      open = true;
      isList = true;
    } else if (*tok == '}') {
      if (!open) {
        fprintf(stderr, BKRED "Error: List closed more than once: \n\t%s\n\t%s\n", copy, genWrongUnderline(copy, tok, tok));
        exit(1);
      }
      open = false;
    }
    tok = strtok(NULL, " ");
  }
  if (isList && open) {
    fprintf(stderr, BKRED "Error: Found unclosed list: \n\t%s\n\t%s\n", copy, genWrongUnderline(copy, whereOpened, tok));
    exit(1);
  }
  free(copy);
  copy=NULL;

  return isList;
}

bool isAssignation (char *in, char **outTok, char **outValue) {
  char *src = in;
  char *end = in;
  while (*end != '\0') end++;
  char *subStr;
  int temp;


  while (*src != '\0') {
    if (*src == '=' && !isEmptyStrInRange(in, src-1) && !isInsideOfStr(in, src)) {
      if (isEmptyStrInRange(src+1, end-2)) {

        subStr = malloc(((src-1)-in)+2);
        memset(subStr, '\0', ((src-1)-in)+2);
        strncpy(subStr, in, (src-1)-in);

        fprintf(stderr, BKRED "Error: empty value assigned to %s : \n\t%s\n\t%s\n" KDEFAULT, subStr, in, genWrongUnderline(in, src+1, end-2));

        free(subStr);
        subStr=NULL;
        exit(1);
      }
      temp = ((src-1)-in)+1;
      *outTok = malloc(temp +1);
      memset(*outTok, '\0', temp+1);
      strncpy(*outTok, in, temp);

      temp = ((end-2)-(src+1))+1;
      *outValue = malloc(temp +1);
      memset(*outValue, '\0', temp+1);
      strncpy(*outValue, src+1, temp);

      return true;
    }
    src++;
  }
  return false;
}

void evalLine (char* currentBuffer) {
  char *src        = currentBuffer;
  char *assigTok   = NULL;
  char *assigValue = NULL;
  while (*src == ' ' || *src == '\t')
    src++;
  VERBOSE_PRINT("Evaluating line bufer...");
  VERBOSE_PRINT_VALUE(%s, src);

  if (isList(currentBuffer)) {
    printf("ITS A LIST!!!\n");
    /*evalList*/;
  }

  if (isAssignation(currentBuffer, &assigTok, &assigValue)){
   printf("ITS AN ASSIGNATION!!!\n");
    /* evalAssignation; */
   evalAssig(currentBuffer, assigTok, assigValue);
  }

}

void evalAssig(char* currentBuffer, char* tok, char* value) {
      if (strcmp(tok, "path") == 0) {
        printf("FOUND PATH ASSIGNATION!!!\n");
      }
}
