#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

// Default options (dynamic)
bool verboseMode = true;
bool testsMode = false;

void printHelp(const int exval) {
  printf("%s,%s show working getopt example\n", PACKAGE, VERSION);
  printf("%s [-h] [-V]\n\n", PACKAGE);

#ifndef NDEBUG
  printf("  -t              set test flag (verbose + run tests)\n");
#endif
  printf("  -h              print this help and exit\n");
  printf("  -V              print version and exit\n");
  printf("  -v              set verbose flag\n");
  printf("  -e              disable exit on errors (Does not supress error messages!)\n");
  printf("  -w              enable warning messages\n");

  if (exval == 0 || exit_on_err)
    EXIT(exval);
}

void verboseMessage(const char *message) { //\e[1;34m and \e[m make text blue
  VERBOSE_PRINT("%s", message);
}

void testMessage(const char *message) {
  printf("\e[1;34m%s: %s\e[m\n", PACKAGE, message);
}

void parseArgs(int argc, char *argv[]) {
  static struct option const long_options[] = {
#ifndef NDEBUG
    {"tests"    , no_argument, NULL, 't'},
#endif
    {"help"     , no_argument, NULL, 'h'},
    {"version"  , no_argument, NULL, 'V'},
    {"verbose"  , no_argument, NULL, 'v'},
    {"warnings" , no_argument, NULL, 'w'},
    {"exitonerr", no_argument, NULL, 'e'}};
  int opt;

  // Checks for commands
  /* if(argc == 1) {
    fprintf(stderr, "This program needs arguments....\n\n");
    printHelp(1);
    }*/

  // Main loop that checks each toggle and argument
  while ((opt = getopt_long(argc, argv,
#ifndef NDEBUG
                            "t"
#endif
                            "hVvew", long_options, NULL)) != -1) {
    switch (opt) {
#ifndef NDEBUG
    case 't':
      verboseMode = true;
      testsMode = true;
      break;
#endif
    case 'h':
      printHelp(0);
      break;
    case 'V':
      printf(PACKAGE " " VERSION "\n");
      exit(0);
      break;
    case 'v':
      verboseMode = true;
      printf("%s: Verbose option is set '%c'\n", PACKAGE, optopt);
      verboseMessage("Testing...");
      break;
    case 'e':
      exit_on_err = false;
      printf("%s: Exit on error option is disabled '%c'\n", PACKAGE, optopt);
      break;
    case 'w':
      warnings_on = true;
      printf("%s: Warnings enabled '%c'\n", PACKAGE, optopt);
      break;
    case '?':
      fprintf(stderr, "%s: Error - No such option: `%c'\n\n", PACKAGE, optopt);
      printHelp(1);
      break;
    }
  }
}

char *genWrongUnderline(char *line, char *from, char *to) {
  char *underline = calloc(strlen(line) + 1, sizeof(char));
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
char *strMkCpyInRange(const char *from, size_t numchars) {
  if (numchars == 0) {
    numchars = strlen(from);
  }
  char *out = calloc(sizeof(char), numchars + 1);
  strncpy(out, from, numchars);
  return out;
}
char *strMkCpy(const char *in) { return strMkCpyInRange(in, 0); }

void strTrim(char *in) {
  char *firstin = in; // Hold position of first char in char* in
  char *source = in;  // Used to rewrite char* in

  while (*in != '\0' && *in != '\n') { // While it hasnt reached the last char
    if (*in != ' ' && *in != '\t') {
      *source = *in;
      if (*(in + 1) == ' ' || *(in + 1) == '\t') {
        source++;
        in++;
        *source = ' ';
      }
      source++;
      in++;
    } else {
      in++;
    }
  }
  if (*(source - 1) == ' ')
    *(source - 1) = '\0';
  else
    *source = '\0';

  VERBOSE_PRINT_VALUE(%s, firstin);
  in = firstin;
}

void strTrimInRange(char *from, char *to) {
  char *src = from;    // Hold position of first char in char* in
  char *source = from; // Used to rewrite char* in
  if (to == NULL) {
    to = from;
    while (*to != '\0')
      to++;
  }
  if (to > from+strlen(from) || to < from) {
    fprintf(
            stderr, BKRED
            "Error: Passed an out of bounds argument (char *to) to function strTrimInRange() : %s\n"
            "to\t\t\t\t: %p :\t%s\n"
            "from (first char of from)\t: %p :\t%s\n"
            "last char of from\t\t: %p :\t%s\n" KDEFAULT,
            (to < from)
            ? "to pointer is smaller than pointer to first char of from"
            : (to > from+strlen(from))
            ? "to pointer is greater than pointer to last char of from"
            : "If you are reading this something is very very wrong",
            to, "[Unsafe to read]", from, from, from+strlen(from), from+strlen(from));
    EXIT(1);
  }

  /* VERBOSE_PRINT("CALL TO STRTRIMINRANGE"); */
  /* VERBOSE_PRINT_VALUE(%p , from); */
  /* VERBOSE_PRINT_VALUE(%s , from); */
  /* VERBOSE_PRINT_VALUE(%p , to); */
  /* VERBOSE_PRINT_VALUE(%s , to); */

  if (from == to) {
    if (*src == ' ' || *src == '\t') {
      src++;
    } else {
      src++;
      source++;
    }
  }
  while (src < to) { // While it hasnt reached the last char
    if (*src != ' ' && *src != '\t') {
      *source = *src;
      if (*(src + 1) == ' ' || *(src + 1) == '\t') {
        source++;
        src++;
        *source = ' ';
        if (src == to)
          break;
      }
      source++;
      src++;
    } else {
      src++;
    }
  }
  /*if (*src != '\t' && *src != ' ') {
    *source = *src;
    source++;
    }*/
  if (*src == ' ' && src == to) {
    src++;
    if (source-1 >= from && *(source-1) == ' ')
      source--;
  }

  while (*(src - 1) != '\0') {
    *source = *src;
    source++;
    src++;
  }
  /*
  if (*(source - 1) == ' ')
    *(source - 1) = '\0';
  else
    *source = '\0';
    */

  /*if (*to != '\0') {

    strcpy(src, to);
    *(src + strlen(to)) = '\0';
    strOverlap(src, from, to, to, NULL);
    }*/
  // VERBOSE_PRINT_VALUE(%s, src);
}

bool isEmptyStrInRange(char *from, char *to) {
  bool isEmpty = true;
  char *copy;
  char *copySrc;
  if (to == NULL) {
    to = from;
    while (*to != '\0')
      to++;
    to--;
  }
  if (from > to) {
    fprintf(stderr,
            BKRED "Error: isEmptyStrInRange received invalid parameters: from "
                  "address is greater than to address!\nfrom :\t%p :\t%s\nto   "
                  ":\t%p :\t%s\n" KDEFAULT,
            from, from, to, to);
    EXIT(1);
  }

  copy = calloc((to - from) + 4, sizeof(char));
  strncpy(copy, from, to - from);
  copySrc = copy;

  while (*copySrc != '\0') {
    if (*copySrc != ' ' && *copySrc != '\n' && *copySrc != '\t') {
      isEmpty = false;
      break;
    }
    copySrc++;
  }

  free(copy);
  copy = NULL;
  return isEmpty;
}

bool isEmptyStr(char *str) { return isEmptyStrInRange(str, NULL); }

bool utilIsInsideOf(char *str, char *pos, const DELIM_TYPE delim, const ESCAPE_TYPE escape) {
  SET_DELIM_CHAR(delim, delim_char);
  SET_ESCAPE_CHAR(escape, escape_char)

  char *src = str;
  bool is_in_str = false;
  bool set_on_next = false;
  while (*src != '\0') {
    if (set_on_next) {
      is_in_str = false;
      set_on_next = false;
    }

    if (*src == delim_char && src == str) {
      is_in_str = (is_in_str) ? false : true;
    } else if (*src == delim_char && *(src - 1) != escape_char && src != str) {
      if (is_in_str) {
        set_on_next = true;
      } else {
        is_in_str = true;
      }
    }

    if (src == pos) {
      return is_in_str;
    }
    src++;
  }
  if (pos < str || pos > src) {
    fprintf(
        stderr, BKRED
        "Error: Passed an out of bounds pos to function isInsideOfStr() : %s\n"
        "pos\t\t\t: %p :\t%s\n"
        "str (first char of str)\t: %p :\t%s\n"
        "src (last char of str)\t: %p :\t%s\n" KDEFAULT,
        (pos < str)
            ? "pos pointer is smaller than pointer to first char of str"
            : (pos > src)
                  ? "pos pointer is greater than pointer to last char of str"
                  : "If you are reading this something is very very wrong",
        pos, pos, str, str, src, src);
    EXIT(1);
  } else if (*pos == escape_char && *(pos-1) != escape_char) {
    WARNING_PRINT("Warning: Passed an escape character to isInsideOfStr() this isn't supposed to happen!\n")
  }
  fprintf(stderr, BKRED "Error: something went wrong in function "
                        "utilIsInsideOf()... src never matched pos\n" KDEFAULT);
  return false;
}

bool isInsideOfStr(char *str, char *pos) {
  return utilIsInsideOf(str, pos, STR_DELIM, STR_ESCAPE);
}

bool isInsideOfRegEx(char *str, char *pos) {
  return utilIsInsideOf(str, pos, REGEX_DELIM, REGEX_ESCAPE);
}

void strTrimStrAware(char *in) {
  /*
    What we want:
    =: is in string
    _: isnt in string

    __________===========_____
    something "something" else

    =============
    "some string"

     Trim from the first to the last '_'

     Dependencies :
       - strTrimInRange
       - isInsideOfStr
  */

  char *src = in;
  char *begin;
  char *end;
  char *tmp;
  bool isInString = false;
  bool flag = false;
  size_t s1, s2;

  /* Do not process empty string */
  if (strcmp(in, " ") == 0 || strcmp(in, "") == 0) {
    printf("in is \"%s\"\n", in);
    return;
  }

  /* Erase all spaces at the end */
  while (in[strlen(in) -1] == ' ')
    in[strlen(in) -1] = '\0';

  /* Erase all beginning spaces */
  tmp = malloc(strlen(in) + 1);
  while (*src == ' ') {
    strcpy(tmp , in+1);
    strcpy(in, tmp);
    /* VERBOSE_PRINT_VALUE(%s, in); */
  }
  free(tmp);
  tmp = NULL;
  begin = in;
  src = in;

  /* Do not process string where the whole content is a string */
  for (size_t i=0; i<strlen(in); i++) {
    if (!isInsideOfStr(in, &in[i]))
      flag = true;
  }
  if (!flag)
    return;

  /* Main loop */
  while (*src != '\0') {
    /* what happens if last char is " */
    if (*(src + 1) == '\0' &&
        ((!isInsideOfStr(in, src) /*&& (isInsideOfStr(in, src-1) && *(src-1) == '\"' && isInsideOfStr(in, src-2))*/) ||
         (isInsideOfStr(in, src) && *src == '\"' && isInsideOfStr(in, src-1)))) {

      if (isInsideOfStr(in, src) && *src == '\"' && isInsideOfStr(in, src-1)) {
        begin = src;
      }

      if (begin == NULL){
        fprintf(stderr, BKRED
                "Error: About to pass an undefined begin value to strTrimInRange\n"
                "function : %s \nline : %d\nfile : %s\n"
                KDEFAULT, __func__, __LINE__ + 6, "utils.c"); /* Not using __FILE__ because it is determined at compile time */

        EXIT(1);
      }

      /* Erases any left over spaces at the end */
      strTrimInRange(begin, NULL);
      while (in[strlen(in) -1] == ' ')
        in[strlen(in) -1] = '\0';

      return;

    } else if (*(src + 1) == '\0' && isInsideOfStr(in, src)) {
      fprintf(stderr, BKRED
              "Error: an unclosed string has been passed to function "
              "strTrimStrAware in utils.c!: \n\t%s\n\t%s\n" KDEFAULT,
              in, genWrongUnderline(in, src, src + 1));

      EXIT(1);
    }

    /* What happens if first char is " */
    if (*src == '\"' && src == in) {
      isInString = true;
      begin = NULL;
      src++;
      continue;
    }

    /* What happens if char is escape character */
    if (*src == '\\' && *(src-1) != '\\' && src != in) {
      WARNING_PRINT("\\\\\\\\\\\\\\");
      src+=2;
      continue;
    }


    if (isInsideOfStr(in, src) ==
        true) { // If the current char is inside of a string
      if (isInString ==
          false) { // If the previous char wasnt inside of a string
        /* end = (src - 1); */
        end = src;

        s1 = strlen(in);
        strTrimInRange(begin, end);
        s2 = strlen(in);
        if (s1 > s2) {
          src-= (s1-s2);
          /* end = (src - 1); */
          end = src;
        }

      }

      isInString = true;
    } else {                  // If the current char isnt inside of a string
      if (isInString == true) // If the previous char was inside of a string
        begin = (src - 1);

      isInString = false;
    }

    src++;
  }
  /*
    if (isInsideOfStr(in, src-1) == false) {
    end = (src-1);
    strTrimInRange(begin, end);
    return;
    }
  */

    /* Erases any left over spaces at the end */
    while (in[strlen(in) -1] == ' ')
      in[strlen(in) -1] = '\0';
}

void strOverlap(char *dest, char *from, char *to, char *from2, char *to2) {
  if (to2 == NULL) {
    to2 = from2;
    while (*to2 != '\0')
      to2++;
  }
  char *holder = calloc((to - from) + 1 + (to2 - from2) + 2, sizeof(char));
  char *holder2 = calloc((to2 - from2) + 2, sizeof(char));

  strncpy(holder, from, (to - from) + 1);
  strncpy(holder2, from2, (to2 - from2) + 1);
  strcat(holder, holder2);

  strcpy(dest, holder);

  free(holder);
  free(holder2);
}

void strRealloc(char **str) {
  *str = realloc(*str, strlen(*str) + 1);
  if (*str == NULL) {
    fprintf(stderr, "Error: failed to reallocate variable in heap");
    EXIT(1);
  }
}

void utilRmEscape(char *str, DELIM_TYPE delim, ESCAPE_TYPE escape) {
  SET_DELIM_CHAR(delim, delim_char);
  SET_ESCAPE_CHAR(escape , escape_char);

  bool override_inside = false;

  char *src = str;
  while (*src != '\0') {
    if (utilIsInsideOf(str, src, delim, escape) || override_inside) {
      if ( delim == STR_DELIM && src[0] == escape_char &&
           (src[1] == delim_char  ||
            src[1] == escape_char ||
            src[1] == 'n'         ||
            src[1] == 't')) {


        /* foo\"bar -> foo"bar
         *    ^gets popped out
         * src should be pointing to '\'
         * str should be pointing to first char in string */
        strOverlap(str, str, (src - 1), (src + 1), NULL);

        if (*src == 'n')
          *src = '\n';
        else if (*src == 't')
          *src = '\t';
        else if (*src == delim_char)
          override_inside = !override_inside;

        /* foo"bar
         *    ^src  */
        src++;
        /* foo"bar
         *     ^src
         * since src is already pointing to the
         * next character to be analysed we can
         * continue to the next loop direclty */
        continue;
      } else if ( delim == REGEX_DELIM && src[0] == escape_char && (src[1] == delim_char)) {
        strOverlap(str, str, (src - 1), (src + 1), NULL);
        src++;
        continue;

      /* If it's a string that we're processing */
      } else if (delim == STR_DELIM) {
        if (*src == '\\' && (src[1] != '\"')) {
          fprintf(stderr,
                  BKRED "Error: Found escape character inside of string with "
                        "invalid successor \"%c\": \n\t%s\n\t%s\n",
                  src[1], str, genWrongUnderline(str, src, src + 1));
          EXIT(2);
        }
      }
    }
    src++;
  }
}

void strRmEscape(char *str) {
  utilRmEscape(str, STR_DELIM, STR_ESCAPE);
}

void regexRmEscape(char *str) {
  utilRmEscape(str, REGEX_DELIM, REGEX_ESCAPE);
}

void utilUnstring(char **str, DELIM_TYPE delim) {
  SET_DELIM_CHAR(delim, delim_char);

  VERBOSE_PRINT_VALUE(%s, *str);
  strRmEscape(*str);
  VERBOSE_PRINT_VALUE(%s, *str);
  /* get rid of opening delim char */
  for (size_t i = 0; i < strlen(*str); i++) {
    if ((*str)[i] == delim_char) {
      (*str)[i] = ' ';
      break;
    }
    if (i == strlen(*str)-1) {
      fprintf(stderr,
              BKRED "Error: Invalid string %s! Missing delimiter character (%c)\n" KDEFAULT,
              *str, delim_char);
      EXIT(1);
      return;
    }
  }
  /* get rid of closing delim char */
  for (size_t i = strlen(*str);; i--) {
    if ((*str)[i] == delim_char) {
      (*str)[i] = ' ';
      break;
    }
  }

  /* get rid of the space character added at the beginning */
  VERBOSE_PRINT_VALUE(%s, *str);
  if ((*str)[0] == ' ') {
    for (size_t i = 0; i < strlen(*str); i++) {
      (*str)[i] = (*str)[i + 1];
    }
  }

  /* get rid of the space character added at the end */
  VERBOSE_PRINT_VALUE(%s, *str);
  (*str)[strlen(*str) - 1] =
      ((*str)[strlen(*str) - 1] == ' ') ? '\0' : (*str)[strlen(*str) - 1];

  /* reallocate and return */
  strRealloc(str);
  VERBOSE_PRINT_VALUE(%s, *str);
  return;
}

void strUnstring(char **str) {
  utilUnstring(str, STR_DELIM);
}

void regexUnregex(char **str) {
  utilUnstring(str, REGEX_DELIM);
}

/* Returns the first and last char of the first Str sees */
/* Returns 1 if found and 0 for not found */
int getNextStr(char *in, char **out_begin, char** out_end) {
  *out_begin = NULL;
  *out_end = NULL;
  for (size_t i=0;i<strlen(in);i++) {
    if (*out_end != NULL)
      return 1;
    if (isInsideOfStr(in, &in[i]) && *out_begin == NULL)
      *out_begin = &in[i];
    else if (!isInsideOfStr(in, &in[i]) && *out_begin != NULL)
      *out_end = &in[i-1];
  }
  return 0;
}

/* Returns the first and last char of the first RegeEx it sees */
/* Returns 1 if found and 0 for not found */
int getNextRegEx(char *in, char **out_begin, char** out_end) {
  *out_begin = NULL;
  *out_end = NULL;
  for (size_t i=0;i<strlen(in);i++) {
    if (*out_end != NULL)
      return 1;
    if (isInsideOfRegEx(in, &in[i]) && *out_begin == NULL)
      *out_begin = &in[i];
    else if (!isInsideOfRegEx(in, &in[i]) && *out_begin != NULL)
      *out_end = &in[i-1];
  }
  return 0;
}
