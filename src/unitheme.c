#include "unitheme.h"
#include "priv_unitheme.h"
#include "execUnitheme.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

static programDef *currentProg;
static programDefs progDefs;

void loadUniTheme(const char *filename) {
  char *buff = malloc(256);
  char *tmp;
  FILE *UniThemeFile = fopen(filename, "r");
  currentLine = 0;
  defsInitArr(&progDefs, 5);
  currentProg = calloc(1, sizeof(programDef));
  defsInitDef(currentProg, 20);

  if (UniThemeFile == NULL) {
    fprintf(stderr, "Error: file %s not found!", filename);

    free(buff);
    defsFree(&progDefs);
    defsFreeDef(currentProg);
    free(currentProg);
    return;
  }

  while (fgets(buff, 256, UniThemeFile) != NULL) {
    currentLine++;
    if (buff[0] == '\n' || buff[0] == '#') {
      continue;
    } else {
      getFullLine(&buff, UniThemeFile);
      // rmEscape(&buff);
      strTrimStrAware(buff);
      rmEscape(&buff);
      evalLine(buff);
    }
    tmp = realloc(buff, 256);
    if (tmp == NULL) {
      fprintf(stderr, BKRED "Error: Failed to realocate memory for the buff variable in the unitheme reader!" KDEFAULT);
      EXIT(1);

    } else {
      buff = tmp;
      tmp = NULL;
    }
  }
  free(buff);
  fclose(UniThemeFile);
  /* defsFree(&progDefs); */

  /* No need to free currentProg because it will be freed by defsFree */
  /* defsFreeDef(currentProg); */
  /* free(currentProg); */
}

void runExecUitheme() {
  execUnitheme(&progDefs);
  freeProgs();
}

void freeProgs() {
  defsFree(&progDefs);
  free(currentProg);
}

//--------------------------------------------------------------------
void defsInitDef(const programDef *def, const size_t initial_tok_size) {
  /* def->tokens = (list *)calloc(1, sizeof(list *)); */
  /* def->tokens.items = (char **)calloc(initial_tok_size, sizeof(char *)); */
  /* def->tokens.used = 0; */
  /* def->tokens.size = initial_tok_size; */
  /* memset(&def->tokens.items[0], 0, sizeof(char *) * initial_tok_size); */

  /* /\* def->snippets = (list *)calloc(1, sizeof(list *)); *\/ */
  /* def->snippets.items = (char **)calloc(initial_tok_size, sizeof(char *)); */
  /* def->snippets.used = 0; */
  /* def->snippets.size = initial_tok_size; */
  /* memset(&def->snippets.items[0], 0, sizeof(char *) * initial_tok_size); */


}


void defsInitList(list *list, size_t inital_list_size) {
  list->items = (char **)calloc(inital_list_size, sizeof(char *));
  list->used = 0;
  list->size = inital_list_size;
  memset(&list->items[0], 0, sizeof(char *) * inital_list_size);

}

void defsInsertList(list *list, char *element) {
  if (list->used == list->size) {
    list->size *= 2;
    list->items = (char **)realloc(list->items, list->size * sizeof(char *));
    // Initialize the last/new elements of the reallocated progs
    for (size_t i = list->used; i < list->size; i++) {
      memset(&list->items[i], 0, sizeof(char *));
    }
  }
  list->items[list->used] = element;
  list->used++;
}

void defsFreeList(list *list) {
  for (size_t i = 0; i < list->used; i++) {
    free(list->items[i]);
    list->items[i] = NULL;
  }
  free(list->items);
  list->items = NULL;
  list->used = 0;
  list->size = 0;
}

void defsInitArr(programDefs *a, size_t initialSize) {
  // Allocate initial space
  a->progs = (programDef **)calloc(initialSize, sizeof(programDef *));

  a->used = 0;           // no elements used
  a->size = initialSize; // available nr of elements

  // Initialize all values of the array to 0
  for (size_t i = 0; i < initialSize; i++) {
    memset(&a->progs[i], 0, sizeof(programDef *));
  }


}

// Add element to array
void defsInsert(programDefs *a, programDef *element) {
  if (a->used == a->size) {
    a->size *= 2;
    a->progs = (programDef **)realloc(a->progs, a->size * sizeof(programDef *));
    // Initialize the last/new elements of the reallocated progs
    for (size_t i = a->used; i < a->size; i++) {
      memset(&a->progs[i], 0, sizeof(programDef *));
    }
  }

  a->progs[a->used] = element;
  /* a->progs[a->used] = calloc(1, sizeof(programDef)); */

  /* a->progs[a->used]->name = element->name; */
  /* a->progs[a->used]->tokens = element->tokens; */
  /* a->progs[a->used]->path = element->path; */
  /* a->progs[a->used]->execBefore = element->execBefore; */
  /* a->progs[a->used]->execAfter = element->execAfter; */

  a->used++;
}

void defsFree(programDefs *a) {
  // Free all name variables of each array element first
  for (size_t i = 0; i < a->used; i++) {
    VERBOSE_PRINT_VALUE(%s, a->progs[i]->name);
    VERBOSE_PRINT_VALUE(%p, a->progs[i]->name);

    if (a->progs[i]->tokens != NULL) {
      VERBOSE_PRINT_VALUE(%d, (int)a->progs[i]->tokens->used);
      for (size_t ii = 0; ii < a->progs[i]->tokens->used; ii++)
        VERBOSE_PRINT_VALUE(%s, a->progs[i]->tokens->items[ii]);
    } else {
      VERBOSE_PRINT_VALUE(%p, a->progs[i]->tokens);
    }

    if (a->progs[i]->snippets != NULL) {
      VERBOSE_PRINT_VALUE(%d, (int)a->progs[i]->snippets->used);
      for (size_t ii = 0; ii < a->progs[i]->snippets->used; ii++)
        VERBOSE_PRINT_VALUE(%s, a->progs[i]->snippets->items[ii]);
    } else {
      VERBOSE_PRINT_VALUE(%p, a->progs[i]->snippets);
    }

    VERBOSE_PRINT_VALUE(%s, a->progs[i]->path);
    VERBOSE_PRINT_VALUE(%s, a->progs[i]->execBefore);
    VERBOSE_PRINT_VALUE(%s, a->progs[i]->execAfter);
    if (verboseMode)
      printf("=================================\n");

    free(a->progs[i]->name);
    if (a->progs[i]->tokens != NULL) {
      defsFreeList(a->progs[i]->tokens);
      free(a->progs[i]->tokens);
    }
    if (a->progs[i]->snippets != NULL) {
      defsFreeList(a->progs[i]->snippets);
      free(a->progs[i]->snippets);
    }
    free(a->progs[i]->path);
    free(a->progs[i]->execBefore);
    free(a->progs[i]->execAfter);
    a->progs[i]->name = NULL;
    a->progs[i]->tokens = NULL;
    a->progs[i]->snippets = NULL;
    a->progs[i]->path = NULL;
    a->progs[i]->execBefore = NULL;
    a->progs[i]->execAfter = NULL;

    free(a->progs[i]->beginDef);
    free(a->progs[i]->endDef);
    a->progs[i]->beginDef = NULL;
    a->progs[i]->endDef = NULL;
    /* a->progs[1]->beginDef=0; */
    /* a->progs[1]->endDef=0; */
    /* free(a->progs[i]->toklen); */
    /* a->progs[i]->toklen= NULL; */

    free(a->progs[i]);
  }

  // Now free the array
  free(a->progs);
  a->progs = NULL;

  a->used = 0;
  a->size = 0;
}

void defsFreeDef(programDef* prg) {
  free(prg->name);
  /* free(prg->tokens); */
  /* free(prg->snippets); */
  free(prg->path);
  free(prg->execBefore);
  free(prg->execAfter);
  prg->name = NULL;
  /* prg->tokens = NULL; */
  /* prg->snippets = NULL; */
  prg->path = NULL;
  prg->execBefore = NULL;
  prg->execAfter = NULL;

  free(prg->beginDef);
  free(prg->endDef);
  prg->beginDef = NULL;
  prg->endDef = NULL;
}
//--------------------------------------------------------------------

void rmComment(char *in) {
  char *src = in;
  while (*src != '\0' && *src != '\n') {
    if (*src == '#' && *(src - 1) != '\\')
      *src = '\0';
    src++;
  }
}

void rmEscape(char **currentBuffer) { /* Apply's only to everything outside of strings and regex's */
  char *original = malloc(strlen(*currentBuffer) + 1);
  strcpy(original, *currentBuffer);
  char *src = original;
  while (*src != '\0') {
    if (!isInsideOfStr(original, src) && !isInsideOfRegEx(original, src)) {
      if (*src == '\\' && (src[1] == '\\' || src[1] == '#')) {
        strOverlap(original, original, (src - 1), (src + 1), NULL);
        src++;
      } else if (*src == '\\' && (src[1] != '\\' && src[1] != '#')) {
        fprintf(stderr, BKRED "Error: Found escape character outside of string "
                              "with invalid successor \"%c\": \n\t%s\n\t%s\n",
                src[1], original, genWrongUnderline(original, src, src + 1));
        EXIT(1);
      }
    }
    src++;
  }

  *currentBuffer = realloc(*currentBuffer, strlen(original) + 1);
  strcpy(*currentBuffer, original);
  free(original);
}

bool hasLineExtension(char **currentBuffer) {
  char *src = *currentBuffer;
  while (*src != '\0')
    src++;

  if (*(src - 1) == ';' || (*(src - 2) == ';' && *(src - 1) == '\n')) {
    src = *currentBuffer;
    while (*src != ';')
      src++;
    *(src + 1) = '\0';
    *currentBuffer = realloc(*currentBuffer, strlen(*currentBuffer) + 1);
    return false;
  }

  return true;
}

void getFullLine(char **currentBuffer, FILE *UniThemeFile) {
  // char *holder = malloc(strlen(currentBuffer) + 1);
  char *holder = calloc(256, sizeof(char));
  char *tmp = NULL;
  // char *nextLine = malloc(256);
  strcpy(holder, *currentBuffer);
  rmComment(holder);
  while (hasLineExtension(&holder)) {
    currentLine++;
    // line+=new;
    fgets(*currentBuffer, 256, UniThemeFile);
    tmp = realloc(holder, strlen(holder) + 1 + strlen(*currentBuffer) + 1);
    if (tmp == NULL) {
      fprintf(stderr, "Error: reallocation failed in %s at line %d", __FILE__, __LINE__);
    } else {
      holder = tmp;
      tmp = NULL;
    }
    strcat(holder, " ");
    strcat(holder, *currentBuffer);
    rmComment(holder);
  }
  *currentBuffer = realloc(*currentBuffer, strlen(holder) + 1);
  strcpy(*currentBuffer, holder);

  // free(nextLine);
  if (tmp != NULL)
    free(tmp);
  free(holder);
}

bool isList(char *in, char **outListName, list **outListItems) {
  char *src = in;
  /* char **listItems; */
  /* char **listItemsSrc = *outListItems; */
  /* char **lastItem; */
  bool isList = false;
  bool open = false;
  char *whereOpened = NULL;
  char *tmpc;
  char *tok;
  char *contents;

  size_t numItems = 0;
  int tmp;

  while (*src != '\0') {
    if (*src == '\n') {
      src++;
      continue;
    }
    if (*src == '{' && !isInsideOfStr(in, src) && !isInsideOfRegEx(in, src)) {
      if (open) {
        fprintf(stderr,
                BKRED "Error: List opened more than once: \n\t%s\n\t%s\n", in,
                genWrongUnderline(in, src, src));
        exit(1);
      }
      whereOpened = src;
      open = true;
      isList = true;
    } else if (*src == '}' && !isInsideOfStr(in, src) && !isInsideOfRegEx(in, src)) {
      if (!open) {
        fprintf(stderr,
                BKRED "Error: List closed more than once: \n\t%s\n\t%s\n", in,
                genWrongUnderline(in, src, src));
        exit(1);
      }
      open = false;

      tmp = ((src - 1) - (whereOpened + 1)) + 1;
      contents = malloc(tmp + 1);
      memset(contents, '\0', tmp + 1);
      strncpy(contents, whereOpened + 1, tmp);
      VERBOSE_PRINT_VALUE(%s, contents);
      tmpc = contents;
      while (*tmpc != '\0') {
        *tmpc = (*tmpc == '\n') ? ' ' : *tmpc;
        tmpc++;
      }
      VERBOSE_PRINT_VALUE(%s, contents);

      tmpc = src;
      tmpc--;
      while (*tmpc == ' ' || *tmpc == '\n')
        tmpc--;
      if (*tmpc == '\"') {
        numItems++;
      } else if (*tmpc != '\n' && *tmpc != ',') {
        fprintf(stderr, BKRED "Error: Invalid character \"%c\" after end of "
                              "last item: \n\t%s\n\t%s\n",
                *tmpc, in, genWrongUnderline(in, tmpc, tmpc));
        exit(1);
      }

    } else if (*src == ',' && open && !isInsideOfStr(in, src) && !isInsideOfRegEx(in, src)) {
      numItems++;
    }
    src++;
  }
  if (isList && open) {
    fprintf(stderr, BKRED "Error: Found unclosed list: \n\t%s\n\t%s\n", in,
            genWrongUnderline(in, whereOpened, src));
    exit(1);
  }
  if (isList) {

    strTrimStrAware(in);
    src = in;
    while (*src != ' ')
      src++;
    tmp = ((whereOpened - 1) - src) + ((*(whereOpened - 1) == ' ') ? 0 : 1);
    *outListName = malloc(tmp);
    memset(*outListName, '\0', tmp);
    strncpy(*outListName, src + 1, tmp - 1);

    /* listItems = malloc(sizeof(char *) * numItems); */
    /* lastItem = (listItemsSrc + (numItems - 1)); */

    /* tok = strtok(contents, ","); */
    /* tmp = 0; */
    /* for (int i = 0; i < numItems; i++) { */
    /*   listItems[i] = malloc(sizeof(char) * (strlen(tok) + 1)); */
    /*   strcpy(listItems[i], tok); */
    /*   VERBOSE_PRINT_VALUE(%s, listItems[i]); */
    /*   tok = strtok(NULL, ","); */
    /* } */

    /* *outListItems = listItems; */

    *outListItems = (list *)calloc(1, sizeof(list));
    defsInitList(*outListItems, 20);
    tok = strtok(contents, ",");
    for (size_t i = 0; i < numItems; i++) {
      defsInsertList(*outListItems, strMkCpy(tok));
      VERBOSE_PRINT_VALUE(%s, (*outListItems)->items[(*outListItems)->used-1]);
      tok = strtok(NULL, ",");
    }
    free(contents);
  }
  return isList;
}

bool isAssignation(char *in, char **outTok, char **outValue) {
  char *src = in;
  char *end = in;
  while (*end != '\0')
    end++;
  char *subStr;
  int temp;

  while (*src != '\0') {
    if (*src == '=' && !isEmptyStrInRange(in, src - 1) &&
        !isInsideOfStr(in, src)) {
      if (isEmptyStrInRange(src + 1, end - 2)) {

        subStr = malloc(((src - 1) - in) + 2);
        memset(subStr, '\0', ((src - 1) - in) + 2);
        strncpy(subStr, in, (src - 1) - in);

        fprintf(stderr, BKRED
                "Error: empty value assigned to %s : \n\t%s\n\t%s\n" KDEFAULT,
                subStr, in, genWrongUnderline(in, src + 1, end - 2));

        free(subStr);
        subStr = NULL;
        exit(1);
      }
      temp = ((src - 1) - in) + 1;
      *outTok = malloc(temp + 1);
      memset(*outTok, '\0', temp + 1);
      strncpy(*outTok, in, temp);

      temp = ((end - 2) - (src + 1)) + 1;
      *outValue = malloc(temp + 1);
      memset(*outValue, '\0', temp + 1);
      strncpy(*outValue, src + 1, temp);

      return true;
    }
    src++;
  }
  return false;
}

bool isStatement(char *in, char **outCall, char **outArg) {
  char *copyIn = strMkCpyInRange(in, strlen(in) - 1);
  int words = 0;
  for (char *tok = strtok(copyIn, " "); tok != NULL; tok = strtok(NULL, " ")) {
    if (words == 0)
      *outCall = strMkCpy(tok);
    else if (words == 1)
      *outArg = strMkCpy(tok);
    words++;
  }
  free(copyIn);
  copyIn = NULL;
  if (words != 2) {
    free(*outCall);
    *outCall = NULL;
    free(*outArg);
    *outArg = NULL;
    return false;
  }
  for (size_t i = 0; i < strlen(in); i++) {
    if (in[i] == '=' && !isInsideOfStr(in, in+i)) {
      free(*outCall);
      *outCall = NULL;
      free(*outArg);
      *outArg = NULL;
      return false;
    }
  }

  strTrimStrAware(*outCall);
  /* strTrimStrAware(*outArg); */

  return true;
}

void evalLine(char *currentBuffer) {
  char *src = currentBuffer;
  char *statCall = NULL;
  char *statArg = NULL;
  char *assigTok = NULL;
  char *assigValue = NULL;
  char *listName = NULL;
  list *listItems = NULL;
  while (*src == ' ' || *src == '\t')
    src++;
  VERBOSE_PRINT("Evaluating line bufer...");
  VERBOSE_PRINT_VALUE(%s, src);

  if (isStatement(currentBuffer, &statCall, &statArg)) {
    printf("ITS A STATEMENT!!!\n");
    /*evalStatement*/;
    evalStatement(currentBuffer, statCall, statArg);
    statCall = NULL;
    statArg = NULL;
  } else if (isAssignation(currentBuffer, &assigTok, &assigValue)) {
    printf("ITS AN ASSIGNATION!!!\n");
    /* evalAssignation; */
    evalAssig(currentBuffer, assigTok, assigValue);
    assigTok = NULL;
    assigValue = NULL;
  } else if (isList(currentBuffer, &listName, &listItems)) {
    printf("ITS A LIST!!! ###%s###%d###\n", listName, (int)listItems->used);
    /*evalList*/;
    evalList(currentBuffer, listName, listItems);
    listName = NULL;
    listItems = NULL;
  } else {
    fprintf(stderr, BKRED "Error: Could not recognize (line #%d): %s",
            (int)currentLine, currentBuffer);
    exit(1);
  }

  if (verboseMode)
    printf("=====================================\n");
}

void evalStatement(char *currentBuffer, char *statCall, char *statArg) {
  bool freeArg = false;
  strUnstring(&statArg);
  if (strcmp(statCall, "begindef") == 0) {
    if (currentProg->name == NULL) {
      currentProg->beginDef = calloc(1, sizeof(size_t));
      *currentProg->beginDef = currentLine;
      currentProg->name = statArg;
    } else {
      fprintf(stderr,
              BKRED "Error: Called begindef twice! : check line #%d\n" KDEFAULT,
              (int)currentLine);
      exit(1);
    }
  } else if (strcmp(statCall, "enddef") == 0) {
    VERBOSE_PRINT("ENDING DEF SECTION");
    /* store program detail of struct somehow */
    currentProg->endDef = calloc(1, sizeof(size_t));
    *currentProg->endDef = currentLine;
    defsInsert(&progDefs, currentProg);
    /* free(currentProg); */
    currentProg = NULL;

    currentProg = calloc(1, sizeof(programDef));
    defsInitDef(currentProg, 20);
    currentProg->name = NULL;
    freeArg = true;
  } else {
    fprintf(stderr,
            BKRED "Error: Called undefined function %s! : check line #%d\n" KDEFAULT,
            statCall, (int)currentLine);
    exit(1);

    /* nothing matched */
    freeArg  = true;
  }
  printf("%s\n", statCall);
  printf("%s\n", statArg);
  free(statCall);
  statCall = NULL;
  if (freeArg)
    free(statArg);
  VERBOSE_PRINT_VALUE(%p, statArg);
  statArg = NULL;
}

void evalAssig(char *currentBuffer, char *tok, char *value) {
  bool freeVal = false;
  strTrim(tok);
  strTrimStrAware(value);
  strUnstring(&value);
  if (currentProg->name != NULL) {
    if (strcmp(tok, "path") == 0) {
      printf("FOUND PATH ASSIGNATION!!!\n");
      currentProg->path = value;
    } else if (strcmp(tok, "exec-before") == 0) {
      currentProg->execBefore = value;
    } else if (strcmp(tok, "exec-after") == 0) {
      currentProg->execAfter = value;
    } else {
    /* TODO: Add extra else if for user declared variables */
      fprintf(stderr,
              BKRED "Error: Assigned value to undefined variable %s! : check line #%d\n" KDEFAULT,
              tok, (int)currentLine);
      exit(1);

      freeVal = true;
    }
  } else {
    fprintf(stderr,
            BKRED "Error: Detected assignation outside of programdef scope! : check line #%d\n" KDEFAULT,
            (int)currentLine);
    exit(1);
  }

  free(tok);
  if (freeVal)
    free(value);
}

void evalList(char *currentBuffer, char *listName, list *content) {
  if (currentProg->name != NULL) {
    if (strcmp(listName, "tokens") == 0) { /* Automaticaly treat as regular expressions and not strings (delimiters: // and different escape chars) */
      for (size_t i = 0; i < content->used; i++) {
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
        strTrimStrAware(content->items[i]); //TODO: Replace this line with appropriate regex equivalent
        strUnstring(&content->items[i]); //TODO: Replace this line with appropriate regex equivalent
        strRealloc(&content->items[i]);
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
      }
      currentProg->tokens = content;
    } else if (strcmp(listName, "snippets") == 0) { /* Automatically treat as strings (delimiters: "" and different escape chars) */
      for (size_t i = 0; i < content->used; i++) {
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
        strTrimStrAware(content->items[i]);
        strUnstring(&content->items[i]);
        strRealloc(&content->items[i]);
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
      }
      currentProg->snippets = content;
    } else {
      fprintf(stderr,
              BKRED "Error: Assigned value to undefined list %s! : check line #%d\n" KDEFAULT,
              listName, (int)currentLine);
      exit(1);
    }
  } else {
    fprintf(stderr,
            BKRED "Error: Detected list definition outside of programdef scope! : check line #%d\n" KDEFAULT,
            (int)currentLine);
    exit(1);
  }

  free(listName);
  //defsFreeList(content);
}
