#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "utils.h"


//Default options (dynamic)
bool verboseMode = true;
bool testsMode = false;

void printHelp(const int exval) {
  printf("%s,%s show working getopt example\n", PACKAGE, VERSION); 
  printf("%s [-h] [-V]\n\n", PACKAGE);

  printf("  -t              set test flag (verbose + run tests)\n");
  printf("  -h              print this help and exit\n");
  printf("  -V              print version and exit\n");
  printf("  -v              set verbose flag\n");

  if (exval==0 || EXIT_ON_ERR)
    exit(exval);
}

void verboseMessage(const char* message){ //\e[1;34m and \e[m make text blue
  VERBOSE_PRINT(message)
}

void testMessage(const char* message){
  printf("\e[1;34m%s: %s\e[m\n", PACKAGE, message);
}

void parseArgs(int argc, char *argv[]) {
  static struct option const long_options[] =
    {
      {"tests",   no_argument, NULL, 't'},
      {"help",    no_argument, NULL, 'h'},
      {"version", no_argument, NULL, 'V'},
      {"verbose", no_argument, NULL, 'v'}
    };
  int opt;

  //Checks for commands
  /* if(argc == 1) {
    fprintf(stderr, "This program needs arguments....\n\n");
    printHelp(1);
    }*/

  //Main loop that checks each toggle and argument
  while ((opt = getopt_long(argc, argv, "thVv", long_options, NULL)) != -1) {
    switch(opt) {
    case 't':
      verboseMode = true;
      testsMode = true;
      break;
    case 'h':
      printHelp(0);
      break;
    case 'V':
      printf("%s %s\n\n", PACKAGE, VERSION);
      exit(0);
      break;
    case 'v':
      verboseMode = true;
      printf("%s: Verbose option is set '%c'\n", PACKAGE, optopt);
      verboseMessage("Testing...");
      break;
    case '?':
      fprintf(stderr, "%s: Error - No such option: `%c'\n\n", PACKAGE, optopt);
      printHelp(1);
      break;
    }
  }
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
char *strMkCpyInRange(char *from, int numchars) {
  if (numchars == 0) {
    numchars = strlen(from);
  }
  char *out = calloc(sizeof(char), numchars+1);
  strncpy(out, from, numchars);
  return out;
}
char *strMkCpy(char *in) {
  return strMkCpyInRange(in, 0);
}

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

void strTrimInRange (char *from, char *to) {
  char *src = from; // Hold position of first char in char* in
  char *source = from;  // Used to rewrite char* in
  if (to == NULL) {
    to = from;
    while (*to != '\0')
      to++;
  }


  while (src != to) { // While it hasnt reached the last char 
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
  while (*(src-1) != '\0') {
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
  //VERBOSE_PRINT_VALUE(%s, src);


}

bool isEmptyStrInRange (char *from, char *to) {
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
    fprintf(stderr, BKRED "Error: isEmptyStrInRange received invalid parameters: from address is greater than to address!\nfrom :\t%p :\t%s\nto   :\t%p :\t%s\n" KDEFAULT, from, from, to, to);
    exit(1);
  }


  copy = malloc((to-from)+4);
  memset(copy, '\0', (to-from)+4);
  strncpy(copy, from, to-from);
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


bool isEmptyStr (char *str) {
  return isEmptyStrInRange (str, NULL);
}

bool isInsideOfStr (char *str, char *pos) {
  char *src = str;
  bool isInString = false;
  while (*src != '\0') {
    if (*src == '\"' && src == str) {
      isInString = (isInString) ? false : true;
    } else if (*src == '\"' && *(src-1) != '\\' && src != str) {
      isInString = (isInString) ? false : true;
    }

    if (src == pos) {
      return isInString;
      break;
    }
    src++;
  }
  if (pos < str || pos > src) {
    fprintf(stderr, BKRED "Error: Passed an out of bounds pos to function isInsideOfStr() : %s\n"
            "pos\t\t\t: %p :\t%s\n"
            "str (first char of str)\t: %p :\t%s\n"
            "src (last char of src)\t: %p :\t%s\n" KDEFAULT, (pos < str) ?
            "pos pointer is smaller than pointer to first char of str" : (pos > src) ?
            "pos pointer is greater than pointer to last char of str" :
            "If you are reading this something is very very wrong", pos, pos, str, str, src, src);
    exit(1);
  }
  fprintf(stderr, BKRED "Error: something went wrong in function isInsideOfStr()... src never matched pos\n" KDEFAULT);
  return false;
}

void strTrimStrAware (char *in) {
  char *src = in;
  char *begin = in;
  char *end;
  bool isInString = false;

  if (strcmp(in, " ") == 0 || strcmp(in, "") == 0) {
    printf("in is \"%s\"\n", in);
    return;
  }

  while (*src != '\0') {
    if (*(src+1) == '\0' && !isInsideOfStr(in, src)) {
      strTrimInRange(begin, NULL);
      return;
      break;
    } else if (*(src+1) == '\0') {
      fprintf(stderr, BKRED "Error: an unclosed string has been passed to function strTrimStrAware in utils.c!: \n\t%s\n\t%s\n" KDEFAULT, in, genWrongUnderline(in , src, src+1));

      exit(1);
    }
    if (isInsideOfStr(in, src) == true) { // If the current char is inside of a string
      if (isInString == false) {          // If the previous char wasnt inside of a string
        end = (src-1);
        strTrimInRange(begin, end);
      }

      isInString = true;
    } else {                              // If the current char isnt inside of a string
      if (isInString == true)             // If the previous char was inside of a string
        begin = (src-1);

      isInString = false;
    }

    src++;
  }
  /*
  if (isInsideOfStr(in, src-1) == false) {
    end = (src-1);
    strTrimInRange(begin, end);
    return;
    }*/
  return;


}

void strOverlap(char *dest, char *from, char *to, char *from2, char *to2) {
  if (to2 == NULL) {
    to2 = from2;
    while (*to2 != '\0')
      to2++;
  }

  char *holder = malloc((to-from) + 1 + (to2-from2) + 2);
  char *holder2 = malloc((to2-from2) + 2);
  memset(holder, '\0', (to-from)+1+(to2-from2)+2);
  memset(holder2, '\0', (to2-from2)+2);


  strncpy(holder, from, (to-from)+1);
  strncpy(holder2, from2, (to2-from2)+1);
  strcat(holder, holder2);

  strcpy(dest, holder);


  free(holder);
  free(holder2);
}

void strRealloc(char **str) {
  *str = realloc(*str, strlen(*str)+1);
  if (*str == NULL) {
    fprintf(stderr, "Error: failed to reallocate variable in heap");
    exit(1);
  }
}

void strRmEscape(char *str) {
  char *src = str;
  /*
  while (*src != '\0' && *src != '\n')
    src++;
  if (*src == '\n')
    fprintf(stderr, "Warning: found a '\\n' (newline) character in current buffer string! This is dangerous!!!\n");
  src = str;
  */
  while (*src != '\0') {
    if (isInsideOfStr(str, src)) {
      if (*src == '\\' && (src[1] == '\"')) {
        strOverlap(str, str, (src-1), (src+1), NULL);
        src++;
      } else if (*src == '\\' && (src[1] != '\"')) {
        fprintf(stderr, BKRED "Error: Found escape character inside of string with invalid successor \"%c\": \n\t%s\n\t%s\n", src[1], str, genWrongUnderline(str, src, src+1));
        exit(1);
        }

    }
    src++;
  }

}

void strUnstring(char **str) {
  VERBOSE_PRINT_VALUE(%s, *str);
  strRmEscape(*str);
  VERBOSE_PRINT_VALUE(%s, *str);
  for (size_t i = 0; i < strlen(*str); i++) {
    (*str)[i] = ((*str)[i] == '\"') ? ' ' : (*str)[i];
 }
  VERBOSE_PRINT_VALUE(%s, *str);
  if ((*str)[0] == ' ') {
    for (size_t i=0; i < strlen(*str); i++) {
      (*str)[i] =  (*str)[i+1];
    }
  }
  VERBOSE_PRINT_VALUE(%s, *str);
  (*str)[strlen(*str)-1] = ((*str)[strlen(*str)-1] == ' ') ? '\0' : (*str)[strlen(*str)-1];
  strRealloc(str);
  VERBOSE_PRINT_VALUE(%s, *str);
}

