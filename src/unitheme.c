#include "unitheme.h"
#include "priv_unitheme.h"
#include "execUnitheme.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

static programDef *currentProg;
static programDefs progDefs;

void loadUniTheme(const char *filename) {
  char *buff = malloc(256);
  char *tmp;
  FILE *UniThemeFile = fopen(filename, "r");
  currentLine = 0;
  initRegExpressions();
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
  destroyRegExpressions();
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

/* This fucntion compiles the regular expressions at the beggining */
/* of the program so that it isnt done multiple times at runtime */
/*
  Mega expression for detecting valid def list:
  https://regex101.com/r/kY5iB8/2
  https://regex101.com/r/kY5iB8/3
  https://regex101.com/r/kY5iB8/4
  https://regex101.com/r/kY5iB8/5

  def\s+(string)\s+\S+(?<s>\s*\n*\s*)\{\g<s>((?<str>("((\\")|[^"(\\")])+"|""))\g<s>,\g<s>)*(\g<str>\g<s>)?\};?
  def\s+(regex)\s+\S+(?<s>\s*\n*\s*)\{\g<s>((?<str>(\/((\\\/)|[^\/(\\\/)])+\/|\/\/))\g<s>,\g<s>)*(\g<str>\g<s>)?\};?

  Universal (detects valid )
  (def\s+(regex)\s+\S+(?<s>\s*\n*\s*)\{\g<s>((?<reg>(\/((\\\/)|[^\/(\\\/)])+\/|\/\/))\g<s>,\g<s>)*(\g<reg>\g<s>)?\};?)|(def\s+(string)\s+\S+\g<s>\{\g<s>((?<str>("((\\")|[^"(\\")])+"|""))\g<s>,\g<s>)*(\g<str>\g<s>)?\};?)
*/
void initRegExpressions() {
  int re_err_code = 0;
  size_t re_err_offset = 0;
  PCRE2_UCHAR *re_err_msg = NULL;

  g_re_exps = calloc(g_re_exps_size, sizeof(char *));

  g_re_exp_is_def_regex_list =
    strMkCpy("def\\s+" DEF_STRING_KEYWORD "\\s+(\\S+)(?<s>[\\s\\n]*)\\{\\g<s>((?<str>" RE_STRING_LITTERAL ")\\g<s>,\\g<s>)*(\\g<str>\\g<s>)?\\};?");
  /* free(g_re_exp_is_def_regex_list); */

  g_re_exp_is_def_string_list =
    strMkCpy("def\\s+" DEF_REGEX_KEYWORD "\\s+(\\S+)(?<s>[\\s\\n]*)\\{\\g<s>((?<reg>" RE_REGEX_LITTERAL ")\\g<s>,\\g<s>)*(\\g<reg>\\g<s>)?\\};?");

  g_re_exp_is_def_string_assig =
    strMkCpy("^\\s*\\n*\\s*(?<s>[\\s\\n]*)string\\g<s>\\S+\\g<s>=\\g<s>" RE_STRING_LITTERAL "\\g<s>;");

  g_re_exp_is_def_regex_assig =
    strMkCpy("^\\s*\\n*\\s*(?<s>[\\s\\n]*)regex\\g<s>\\S+\\g<s>=\\g<s>" RE_REGEX_LITTERAL "\\g<s>;");

  g_re_codes = calloc(g_re_codes_size, sizeof(pcre2_code *));

  for (size_t i=0; i<g_re_exps_size&&i<g_re_codes_size; i++) {
    g_re_codes[i] = pcre2_compile((PCRE2_SPTR)g_re_exps[i], PCRE2_ZERO_TERMINATED, 0, &re_err_code, &re_err_offset, NULL);
    if (g_re_codes[i] == NULL) {
      fprintf(stderr, BKRED "REGEX COMPILATION FAILED!!\n" KDEFAULT);
      re_err_msg = calloc(256, sizeof(PCRE2_UCHAR));
      pcre2_get_error_message(re_err_code, re_err_msg, 256);
      fprintf(stderr, BKRED "%s\n" KDEFAULT, re_err_msg);
      free(re_err_msg);
    }
  }

}

/* This function free the expressions compiled by initRegExpressions() */
void destroyRegExpressions() {
    for (size_t i=0; i<g_re_exps_size; i++)
      free(g_re_exps[i]);
    free(g_re_exps);
    g_re_exps = NULL;

    for (size_t i=0; i<g_re_codes_size; i++)
      pcre2_code_free(g_re_codes[i]);
    free(g_re_codes);
    g_re_codes= NULL;
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

/* Returns the datatype of the def as well as running the compiled rexeprs and returning T_NULL if didnt match anything */
STRING_TYPE getDefType(char *in, pcre2_code *re_code_is_def_string, pcre2_code *re_code_is_def_regex) {
  STRING_TYPE str_type = T_NULL;

  int re_err_code = 0;
  PCRE2_UCHAR *re_err_msg = NULL;

  pcre2_match_data *re_match_data = NULL;

  re_match_data = pcre2_match_data_create_from_pattern(re_code_is_def_string, NULL); /* Create match data */
  re_err_code = pcre2_match(re_code_is_def_string,                                   /* Test for match */
                            (PCRE2_SPTR)in,
                            strlen(in),
                            0,
                            0,
                            re_match_data,
                            NULL);
  pcre2_match_data_free(re_match_data);                                          /* Dispose of the match data right away */
  if (re_err_code <= 0 && re_err_code != -1){                                    /* Error code -1 means just no match */
    VERBOSE_PRINT_VALUE(%d, re_err_code);
    re_err_msg = malloc(256);
    pcre2_get_error_message(re_err_code, re_err_msg, 256);
    fprintf(stderr, BKRED "%s\n" KDEFAULT, re_err_msg);
    free(re_err_msg);
  } else if (re_err_code > 0) {
    WARNING_PRINT("VALID STRING DEF");
    str_type = T_STRING;
  } else if (re_err_code == -1) {
    re_match_data = pcre2_match_data_create_from_pattern(re_code_is_def_string, NULL); /* Create match data */
    re_err_code = pcre2_match(re_code_is_def_regex,                                   /* Test for match */
                              (PCRE2_SPTR)in,
                              strlen(in),
                              0,
                              0,
                              re_match_data,
                              NULL);
    pcre2_match_data_free(re_match_data);                                          /* Dispose of the match data right away */
    if (re_err_code <= 0 && re_err_code != -1){                                    /* Error code -1 means just no match */
      VERBOSE_PRINT_VALUE(%d, re_err_code);
      re_err_msg = malloc(256);
      pcre2_get_error_message(re_err_code, re_err_msg, 256);
      fprintf(stderr, BKRED "%s\n" KDEFAULT, re_err_msg);
      free(re_err_msg);
    } else if (re_err_code > 0) {
      WARNING_PRINT("VALID REGEX DEF");
      str_type = T_REGEX;
    } else if (re_err_code == -1) {
      /* WARNING_PRINT("INVALID DEF"); */
      return T_NULL;
    }
  }

  return str_type;
}

void getListAttr(char *in, char **outListName, list **outListItems, STRING_TYPE str_type) {
  char *begin_item = NULL;
  char *end_item = NULL;
  char *src = NULL;
  char *tmpc = NULL;
  char *tok = NULL;
  char *k_word;

  /* ==== Set type keyword ==== */
  switch (str_type) {
  case T_STRING:
    k_word = strMkCpy(DEF_STRING_KEYWORD);
    break;
  case T_REGEX:
    k_word = strMkCpy(DEF_REGEX_KEYWORD);
    break;
  default:
    k_word = NULL;
    fprintf(stderr, BKRED "Error: Unknown STRING_TYPE: %d passed to getListAttr!", str_type);
    EXIT(1);
    break;
  }

  /* ==== Get list name ==== */
  tmpc = strMkCpy(in);
  tok = strtok(tmpc, " \t\n");
  while (tok) {
    if (strcmp(tok, "def") && strcmp(tok, k_word)) {
      *outListName = strMkCpyInRange(tok, ((tok[strlen(tok)-1] == '{') ? strlen(tok)-1 : 0));
      VERBOSE_PRINT_VALUE(%s, *outListName);
      break;
    }
    tok = strtok(NULL, " \t\n");
  }
  free(tmpc);
  tmpc = NULL;
  tok = NULL;

  /* ==== Get list items ==== */
  /* Init list */
  *outListItems = (list *)calloc(1, sizeof(list));
  defsInitList(*outListItems, 20);

  /* Skip to char after opening { char */
  src = in;
  for (;*src!='{';src++);
  src++;

  /* Find and insert items one by one */
  VERBOSE_PRINT_VALUE(%s, src);
  if (str_type == T_STRING) {
    while (getNextStr(src, &begin_item, &end_item)) {
      defsInsertList(*outListItems, strMkCpyInRange(begin_item, end_item-begin_item+1));
      src = end_item+1;

      VERBOSE_PRINT_VALUE(%s, (*outListItems)->items[(*outListItems)->used-1]);
  }
  } else if (str_type == T_REGEX) {
    while (getNextRegEx(src, &begin_item, &end_item)) {
      defsInsertList(*outListItems, strMkCpyInRange(begin_item, end_item-begin_item+1));
      src = end_item+1;

      VERBOSE_PRINT_VALUE(%s, (*outListItems)->items[(*outListItems)->used-1]);
    }

  }

  free(k_word);
  k_word = NULL;
}

bool isList(char *in, char **outListName, list **outListItems, STRING_TYPE *str_type) {

  *str_type = getDefType(in, g_re_code_is_def_string_list, g_re_code_is_def_regex_list);

  if (*str_type != T_NULL) {
    getListAttr(in, outListName, outListItems, *str_type);
    return true;
  } else {
    return false;
  }
}

void getAssigAttr(char *in, char **outTok, char **outValue, STRING_TYPE str_type) {
  char *tmpc = NULL;
  char *tok = NULL;
  char *k_word;

  char *begin_val = NULL;
  char *end_val = NULL;

  /* ==== Set type keyword ==== */
  switch (str_type) {
  case T_STRING:
    k_word = strMkCpy(DEF_STRING_KEYWORD);
    break;
  case T_REGEX:
    k_word = strMkCpy(DEF_REGEX_KEYWORD);
    break;
  default:
    k_word = NULL;
    fprintf(stderr, BKRED "Error: Unknown STRING_TYPE: %d passed to getListAttr!", str_type);
    EXIT(1);
    break;
  }

  /* ==== Get assig name ==== */
  tmpc = strMkCpy(in);
  tok = strtok(tmpc, " \t\n");
  while (tok) {
    /* if (strcmp(tok, "def") && strcmp(tok, k_word)) { */
    if (strcmp(tok, k_word)) {
      *outTok = strMkCpyInRange(tok, ((tok[strlen(tok)-1] == '{') ? strlen(tok)-1 : 0));
      VERBOSE_PRINT_VALUE(%s, *outTok);
      break;
    }
    tok = strtok(NULL, " =\t\n");
  }
  free(tmpc);
  tmpc = NULL;
  tok = NULL;

  /* ==== Get assig value ==== */
  if (str_type == T_STRING)
    getNextStr(in, &begin_val, &end_val);
  else if (str_type == T_REGEX)
    getNextStr(in, &begin_val, &end_val);
  *outValue = strMkCpyInRange(begin_val, end_val-begin_val+1);

  free(k_word);
  k_word = NULL;
}

bool isAssignation(char *in, char **outTok, char **outValue, STRING_TYPE* str_type) {
  *str_type = getDefType(in, g_re_code_is_def_string_assig, g_re_code_is_def_regex_assig);

  if (*str_type != T_NULL) {
    getAssigAttr(in, outTok, outValue, *str_type);
    return true;
  } else {
    return false;
  }
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
  STRING_TYPE assig_type = T_NULL;
  char *listName = NULL;
  list *listItems = NULL;
  STRING_TYPE list_item_type = T_NULL;
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
  } else if (isAssignation(currentBuffer, &assigTok, &assigValue, &assig_type)) {
    printf("ITS AN ASSIGNATION!!!\n");
    /* evalAssignation; */
    evalAssig(currentBuffer, assigTok, assigValue, assig_type);
    assigTok = NULL;
    assigValue = NULL;
  } else if (isList(currentBuffer, &listName, &listItems, &list_item_type)) {
    printf("ITS A LIST!!! ###%s###%d###\n", listName, (int)listItems->used);
    /*evalList*/;
    evalList(currentBuffer, listName, listItems, list_item_type);
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

void evalAssig(char *currentBuffer, char *tok, char *value, STRING_TYPE str_type) {
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

void evalList(char *currentBuffer, char *listName, list *content, STRING_TYPE str_type) {
  if (currentProg->name != NULL) {
    if (str_type == T_REGEX) { /* Automaticaly treat as regular expressions and not strings (delimiters: // and different escape chars) */
      for (size_t i = 0; i < content->used; i++) {
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
        strTrimStrAware(content->items[i]); //TODO: Replace this line with appropriate regex equivalent
        regexUnregex(&content->items[i]); //TODO: Replace this line with appropriate regex equivalent
        strRealloc(&content->items[i]);
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
      }
      currentProg->tokens = content;
    } else if (str_type == T_STRING) { /* Automatically treat as strings (delimiters: "" and different escape chars) */
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
