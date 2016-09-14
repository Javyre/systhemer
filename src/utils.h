#ifndef UTILS_H
#define UTILS_H

//C includes (that we use in this header)
#include <stdbool.h>
#include <stdlib.h>

//Package defines and dev options (constant)
#define PACKAGE "SysThemer"
#define VERSION "0.0.1"

#define DEF_STRING_KEYWORD "string"
#define DEF_REGEX_KEYWORD "regex"

/* typedef unsigned char STRING_TYPE; */
typedef enum {
  T_NULL = 0,
  T_STRING,
  T_REGEX
} STRING_TYPE;
#define SET_STRING_TYPE(str_tp, dlm_chr, scp_chr)                       \
  SET_DELIM_CHAR(((str_tp) == T_NULL ? NULL_DELIM : (str_tp) == 1 ? STR_DELIM : REGEX_DELIM), dlm_chr); \
  SET_ESCAPE_CHAR(((str_tp) == T_NULL ? NULL_ESCAPE : (str_tp) == 1 ? STR_ESCAPE : REGEX_ESCAPE), scp_chr);

typedef unsigned char DELIM_TYPE;
#define NULL_DELIM 0
#define STR_DELIM 1
#define REGEX_DELIM 2
#define STR_DELIM_CHAR '\"'
#define REGEX_DELIM_CHAR '/'
#define SET_DELIM_CHAR(dlm_tp, dlm_chr) const char dlm_chr = (dlm_tp == NULL_DELIM) ? '\0' : (dlm_tp == 1 ? STR_DELIM_CHAR : REGEX_DELIM_CHAR);

typedef unsigned char ESCAPE_TYPE;
#define NULL_ESCAPE 0
#define STR_ESCAPE 1
#define REGEX_ESCAPE 2
#define STR_ESCAPE_CHAR '\\'
#define REGEX_ESCAPE_CHAR '\\'
/* The following SET_DELIM_CHAR is modified to suppress compile warnings */
/* Refer to the second definition for changes etc. */
/* TODO: find a better way of suppressing this warning */
#define SET_ESCAPE_CHAR(scp_tp, scp_chr) const char scp_chr = (scp_tp == NULL_ESCAPE) ? '\0' : '\\';
/* #define SET_ESCAPE_CHAR(scp_tp, scp_chr) const char scp_chr = (scp_tp == NULL_ESCAPE) ? '\0' : (scp_tp == 1 ? STR_ESCAPE_CHAR : REGEX_ESCAPE_CHAR); */

#define KNRM  "\x1B[0m"
#define BKNRM  "\e[1;0m"
#define KRED  "\x1B[31m"
#define BKRED  "\e[1;31m"
#define KGRN  "\x1B[32m"
#define BKGRN  "\e[1;32m"
#define KYEL  "\x1B[33m"
#define BKYEL  "\e[1;33m"
#define KBLU  "\x1B[34m"
#define BKBLU  "\e[1;34m"
#define KMAG  "\x1B[35m"
#define BKMAG  "\e[1;35m"
#define KCYN  "\x1B[36m"
#define BKCYN  "\e[1;36m"
#define KWHT  "\x1B[37m"
#define KDEFAULT "\x1b[0m"
#define BKWHT  "\e[1;37m"

#define PRINT_VALUE(type, token, color) printf(color #token " is " #type "\x1b[0m" "\n", token);
#define WARNING_PRINT(message) if(warnings_on) printf(PACKAGE ": " BKYEL"%s" "\x1b[0m" "\n", message);
#define WARNING_PRINT_VALUE(type, token) if (warnings_on) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKYEL);}
#define VERBOSE_PRINT(message) if(verboseMode) printf(PACKAGE ": " BKBLU "%s" "\x1b[0m" "\n", message);
#define VERBOSE_PRINT_VALUE(type, token) if (verboseMode) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKBLU);}
#ifndef NDEBUG
#define TEST_PRINT(message) if (testsMode) {printf(PACKAGE ": " BKNRM "%s" "\x1b[0m" "\n", message);}
#define T_PRINT(message, ...) if (testsMode) {printf(BKNRM message KDEFAULT, __VA_ARGS__);}
#define TEST_PRINT_VALUE(type, token) if (testsMode) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKNRM );}
#endif

#define EXIT(exval) if (exit_on_err) {exit(exval);}

bool verboseMode;
bool exit_on_err;
bool warnings_on;

#ifndef NDEBUG
bool testsMode;
bool exit_on_failed_test_end;
bool exit_on_failed_test;
#endif

//Function declarations
bool utilIsInsideOf(char *str, char *pos, const DELIM_TYPE delim, const ESCAPE_TYPE escape);
bool isInsideOfStr(char *str, char *pos);
bool isInsideOfRegEx(char *str, char *pos);

void printHelp(const int);
void verboseMessage(const char*);
void parseArgs(int, char* []);
char *genWrongUnderline(char *line, char *from, char *to);
char *strMkCpyInRange(const char *from, size_t numchars);
char *strMkCpy(const char *in);
void strTrim(char *in);
void strTrimInRange(char *from, char *to);
bool isEmptyStr (char *str);
bool isEmptyStrInRange (char *from, char *to);
void strTrimStrAware(char *in);
void strRealloc(char **str);

void utilRmEscape(char *str, DELIM_TYPE delim, ESCAPE_TYPE escape);
void strRmEscape(char *str);
void regexRmEscape(char *str);

void strOverlap(char *dest, char *from, char *to, char *from2, char *to2);

void utilUnstring(char **str, DELIM_TYPE delim);
void strUnstring(char **str);
void regexUnregex(char **str);

int getNextStr(char *in, char **out_begin, char** out_end);
int getNextRegEx(char *in, char **out_begin, char** out_end);

#endif
