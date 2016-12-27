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

#define RE_STRING_LITTERAL "(\"(((\\\\\")|[^\"])+)\"|(\"\"))"
#define RE_REGEX_LITTERAL "(\\/(((\\\\\\/)|[^\\/])+)\\/|(\\/\\/))"
#define RE_ANY_LITTERAL "(" RE_STRING_LITTERAL "|" RE_REGEX_LITTERAL ")"

typedef enum {
  T_NULL = 0,
  T_STRING,
  T_REGEX
} STRING_TYPE;
#define SET_STRING_TYPE(str_tp, dlm_chr, scp_chr)                       \
  SET_DELIM_CHAR(((str_tp) == T_NULL ? NULL_DELIM : (str_tp) == 1 ? STR_DELIM : REGEX_DELIM), dlm_chr); \
  SET_ESCAPE_CHAR(((str_tp) == T_NULL ? NULL_ESCAPE : (str_tp) == 1 ? STR_ESCAPE : REGEX_ESCAPE), scp_chr);

/* NOTE: keep the values of enums: STRING_TYPE,
   DELIM_TYPE and ESCAPE_TYPE the same
   (for casting reasons i.e.: (STRING_TYPE)STR_DELIM -> T_STRING)*/
typedef enum {
  NULL_DELIM = 0,
  STR_DELIM,
  REGEX_DELIM
} DELIM_TYPE;
#define STR_DELIM_CHAR '\"'
#define REGEX_DELIM_CHAR '/'
#define SET_DELIM_CHAR(dlm_tp, dlm_chr) const char dlm_chr = (dlm_tp == NULL_DELIM) ? '\0' : (dlm_tp == 1 ? STR_DELIM_CHAR : REGEX_DELIM_CHAR);

typedef enum {
  NULL_ESCAPE = 0,
  STR_ESCAPE,
  REGEX_ESCAPE,
}ESCAPE_TYPE;
#define STR_ESCAPE_CHAR '\\'
#define REGEX_ESCAPE_CHAR '\\'
/* The following SET_DELIM_CHAR is modified to suppress compile warnings */
/* Refer to the second definition for changes etc. */
/* TODO: find a better way of suppressing this warning */
#define SET_ESCAPE_CHAR(scp_tp, scp_chr) const char scp_chr = (scp_tp == NULL_ESCAPE) ? '\0' : '\\';
/* #define SET_ESCAPE_CHAR(scp_tp, scp_chr) const char scp_chr = (scp_tp == NULL_ESCAPE) ? '\0' : (scp_tp == 1 ? STR_ESCAPE_CHAR : REGEX_ESCAPE_CHAR); */

#define KNRM     "\x1B[0m"
#define BKNRM    "\e[1;0m"
#define KRED     "\x1B[31m"
#define BKRED    "\e[1;31m"
#define KGRN     "\x1B[32m"
#define BKGRN    "\e[1;32m"
#define KYEL     "\x1B[33m"
#define BKYEL    "\e[1;33m"
#define KBLU     "\x1B[34m"
#define BKBLU    "\e[1;34m"
#define KMAG     "\x1B[35m"
#define BKMAG    "\e[1;35m"
#define KCYN     "\x1B[36m"
#define BKCYN    "\e[1;36m"
#define KWHT     "\x1B[37m"
#define KDEFAULT "\x1b[0m"
#define BKWHT    "\e[1;37m"

#define ILISTO   BKRED "<" BKRED
#define ILISTC   BKRED ">" BKRED

#define PRINT_VALUE(type, token, color) printf(color #token " is " #type "\x1b[0m" "\n", token);

/* Note: The following PRINT_VALUES macros are deprecated and
   are present for backwards compatibility purposes */
#define WARNING_PRINT_VALUE(type, token) if (warnings_on) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKYEL);}
#define VERBOSE_PRINT_VALUE(type, token) if (verboseMode) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKBLU);}
#ifndef NDEBUG
#define TEST_PRINT_VALUE(type, token) if (testsMode) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKNRM );}
#endif

/* Old PRINT macros... */
/* #define WARNING_PRINT(message) if(warnings_on) printf(PACKAGE ": " BKYEL"%s" "\x1b[0m" "\n", message); */
/* #define WARNING_PRINT_VALUE(type, token) if (warnings_on) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKYEL);} */
/* #define VERBOSE_PRINT(message) if(verboseMode) printf(PACKAGE ": " BKBLU "%s" "\x1b[0m" "\n", message); */
/* #define VERBOSE_PRINT_VALUE(type, token) if (verboseMode) {printf(PACKAGE ": "); PRINT_VALUE(type, token, BKBLU);} */

/* New PRINT macros... (call the non-HELPER variants)
 * (backwards compatible with old macros but support formatting) */
#define WARNING_PRINT_HELPER(fmt, ...) printf(PACKAGE ": " BKYEL fmt "\x1b[0m" "\n%s", __VA_ARGS__);
#define WARNING_PRINT(...) if(verboseMode) {VERBOSE_PRINT_HELPER(__VA_ARGS__, "");}

#define VERBOSE_PRINT_HELPER(fmt, ...) printf(PACKAGE ": " BKBLU fmt "\x1b[0m" "\n%s", __VA_ARGS__);
#define VERBOSE_PRINT(...) if(verboseMode) {VERBOSE_PRINT_HELPER(__VA_ARGS__, "");}

#ifndef NDEBUG
#define TEST_PRINT(message) if (testsMode) {printf(PACKAGE ": " BKNRM "%s" "\x1b[0m" "\n", message);}
#define T_PRINT(message, ...) if (testsMode) {printf(BKNRM message KDEFAULT, __VA_ARGS__);}
#endif

/* TODO: perfect error msgs or make an error msg util library of sorts */
#define ERROR_PRINT_HELPER(fmt, ...) fprintf(stderr, BKRED PACKAGE " ERROR" ": " fmt "\x1b[0m" "\n%s", __VA_ARGS__);
#define ERROR_PRINT(...) if(1) {ERROR_PRINT_HELPER(__VA_ARGS__, "");}

/* strcatf: example usage:
 *   char *s = strdup("123");
 *   strcatf(s, " 45 %d", 678);
 *   printf("%s", s);
 * output:
 *   > 123 45 678
 */
#define strcatf_HELPER(s, fmt, ...) sprintf(s+strlen(s), fmt "%s" , __VA_ARGS__);
#define strcatf(s, ...) if(1) {strcatf_HELPER(s, __VA_ARGS__, "");}


#define freennull(p) do { free(p); p=NULL; } while (0);

#define EXIT(exval)                                                            \
  do {                                                                         \
    if (exval == 2) {                                                          \
      yyerror("probably runtime error (some function returned exit code 2!)"); \
      break;                                                                   \
    }                                                                          \
    if (exit_on_err) {                                                         \
      exit(exval);                                                             \
    }                                                                          \
  } while (0);


char *g_uth_fpath;
bool verboseMode;
bool exit_on_err;
bool warnings_on;
bool o_illustrate_recursive_list;
bool o_illustrate_print_non_friendlied;

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
