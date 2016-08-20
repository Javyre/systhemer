#ifndef UNITHEME_H
#define UNITHEME_H

#include <stdbool.h>
#include <stdio.h>

typedef struct
{
  char **items;
  size_t used;
  size_t size;
} list;

typedef struct
{
  char *name;
  list *tokens;
  list *snippets;
  char *path;
  char *execBefore;
  size_t *beginDef;
  size_t *endDef;
  char *execAfter;
} programDef;
/* programDef *currentProg; */

typedef struct
{
  programDef **progs;
  size_t used;
  size_t size;
} programDefs;
/* programDefs progDefs; */

size_t currentLine;

void loadUniTheme(const char *filename);

void defsInitDef(const programDef *def, const size_t initial_tok_size);

void defsInitArr(programDefs *a, size_t initialSize);

void defsInsert(programDefs *a, programDef *element);

void defsFree(programDefs *a);

void defsFreeDef(programDef* prg);

void rmComment(char *in);

void rmEscape(char **currentBuffer);

bool hasLineExtension(char **currentBuffer);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

bool isList (char *in, char **outListName, list **outListItems);

bool isAssignation (char *in, char **outTok, char **outValue);

bool isStatement (char* in, char** outCall, char** outArg);

void evalLine(char *currentBuffer);

void evalStatement(char *currentBuffer, char *statCall, char *statArg);

void evalAssig(char* currentBuffer, char* tok, char* value);

void evalList(char* currentBuffer, char* listName, list *content);


#endif
