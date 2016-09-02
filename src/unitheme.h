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

void loadUniTheme(const char *filename);

void runExecUitheme();

void freeProgs();

void defsInitDef(const programDef *def, const size_t initial_tok_size);

void defsInitArr(programDefs *a, size_t initialSize);

void defsInsert(programDefs *a, programDef *element);

void defsFree(programDefs *a);

void defsFreeDef(programDef* prg);

#endif
