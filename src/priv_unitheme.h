#ifndef PRIV_UNITHEME_H
#define PRIV_UNITHEME_H

#include <stdbool.h>
#include <stdio.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include "utils.h"
#include "unitheme.h"

typedef struct
{
  pcre2_code *items;
  size_t used;
  size_t size;
} re_code_list;

typedef struct
{
  char **items;
  size_t used;
  size_t size;
} re_exp_list;

/* re_code_list re_codes; */
/* re_exp_list re_exps; */
/* const size_t g_re_exps_size = 4; */
/* const size_t g_re_codes_size = 4; */
#define g_re_exps_size 4
#define g_re_codes_size 4

extern char **g_re_exps;
extern pcre2_code **g_re_codes;
#define g_re_exp_is_def_string_list g_re_exps[0]
#define g_re_exp_is_def_regex_list g_re_exps[1]
#define g_re_exp_is_def_string_assig g_re_exps[2]
#define g_re_exp_is_def_regex_assig g_re_exps[3]

#define g_re_code_is_def_string_list g_re_codes[0]
#define g_re_code_is_def_regex_list g_re_codes[1]
#define g_re_code_is_def_string_assig g_re_codes[2]
#define g_re_code_is_def_regex_assig g_re_codes[3]

/* pcre2_code re_exp_is_def_string_list; */
/* pcre2_code re_exp_is_def_regex_list; */
/* pcre2_code re_exp_is_def_string_assig; */
/* pcre2_code re_exp_is_def_regex_assig; */

/* pcre2_code re_code_is_def_string_list; */
/* pcre2_code re_code_is_def_regex_list; */
/* pcre2_code re_code_is_def_string_assig; */
/* pcre2_code re_code_is_def_regex_assig; */

size_t currentLine;

void initRegExpressions();

void destroyRegExpressions();

void rmComment(char *in);

void rmEscape(char **currentBuffer);

bool hasLineExtension(char **currentBuffer);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

void getFullLine(char **currentBuffer, FILE *UniThemeFile);

STRING_TYPE getDefType(char *in, pcre2_code *re_code_is_def_string, pcre2_code *re_code_is_def_regex);

void getListAttr(char *in, char **outListName, list **outListItems, STRING_TYPE str_type);

void getListAttr(char *in, char **outListName, list **outListItems, STRING_TYPE str_type);

bool isList (char *in, char **outListName, list **outListItems, STRING_TYPE *str_type);

void getAssigAttr(char *in, char **outTok, char **outValue, STRING_TYPE str_type);

bool isAssignation (char *in, char **outTok, char **outValue, STRING_TYPE *str_type);

bool isStatement (char* in, char** outCall, char** outArg);

void evalLine(char *currentBuffer);

void evalStatement(char *currentBuffer, char *statCall, char *statArg);

void evalAssig(char* currentBuffer, char* tok, char* value, STRING_TYPE str_type);

void evalList(char* currentBuffer, char* listName, list *content, STRING_TYPE str_type);

#endif
