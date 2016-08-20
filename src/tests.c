#ifndef NDEBUG
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "tests.h"
#include "utils.h"


void testTestsMode() { TEST_PRINT_VALUE(% d, testsMode) }

char *testStrTrim(char *in) {
  TEST_PRINT_VALUE(%s, in);

  char *firstin = in;
  char *out = malloc(strlen(in));
  char *firstout = out;

  // if (*in!='\0' && *in!= ' ') *out = *in;
  while (*in != '\0') {
    if (*in != '\0' && *in != ' ') {
      *out = *in;
      if (*(in + 1) == ' ') {
        out++;
        in++;
        *out = *in;
      }
      out++;
      in++;
    } else {
      in++;
    }
  }
  *out = '\0';
  out = firstout;
  in = firstin;
  TEST_PRINT_VALUE(%s, in);
  TEST_PRINT_VALUE(%s, out);
  return out;
}

// Converts "   This\t    \t    text     " into "This text"
void testStrTrimPointer(char *in) {
  char *firstin = in; // Hold position of first char in char* in
  char *source = in;  // Used to rewrite char* in

  TEST_PRINT_VALUE(%s, in);
  while (*in != '\0') { // While it hasnt reached the last char
    TEST_PRINT_VALUE(%c, *in);
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

  in = firstin;
  TEST_PRINT_VALUE(%s, in);
}

void testStrTrimPointerNew(char *in) {
  char *out = malloc(strlen(in) + 1);
  memset(out, '\0', strlen(in) + 1);
  char *token;

  token = strtok(in, " \t");
  while (token != NULL) {
    TEST_PRINT_VALUE(%s, token);
    strcat(out, token);
    strcat(out, " ");
    token = strtok(NULL, " \t");
  }

  char *outlast = out;
  while (*outlast++)
    ;
  outlast -= 2;
  if (*outlast == ' ')
    *outlast = '\0';
  TEST_PRINT_VALUE(%s, out);
  strcpy(in, out);
  free(out);
}


bool testStrExpect(const char* original, const char *exp, const char *result, const char *call_name) {
  static size_t call_num = 0;
  /* static int g_num_errors = 0; */
  /* static char *prev_call_name; */
  bool res;
  call_num++;
  if (strcmp(exp, result) == 0) {
    T_PRINT("\nTest #%lu of %s" BKGRN " ran successfully\n", (unsigned long) call_num, g_current_prog);
    res = true;
  } else {
    g_num_errors++;
    T_PRINT("\nTest #%lu of %s" BKRED " ran unsuccessfully: \n\texpected: %s\n\treceived: %s\n%s%s%s%lu test fails so far...\n", (unsigned long) call_num, g_current_prog, exp, result, original == NULL ? "" : "\toriginal: ", original == NULL ? "" : original, original == NULL ? "" : "\n", (unsigned long) g_num_errors);
    res = false;
  }
  /* free(prev_call_name); */
  /* prev_call_name = strMkCpy(call_name); */

  if (!res && exit_on_failed_test)
    EXIT(1);

  return res;
}

void testAll() {
  testStrTrimStrAware();
  testIsInsideOfStr();
}

void testStrTrimStrAware() {
  g_current_prog = __func__;
  char *str;
  char *original;
  str = strMkCpy("\"client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577\"");
  original = strMkCpy(str);
  strTrimStrAware(str);
  testStrExpect(original, "\"client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577\"", str, __func__);
  free(str);
  free(original);

  str = strMkCpy(" \"client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577\"");
  original = strMkCpy(str);
  strTrimStrAware(str);
  testStrExpect(original, "\"client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577\"", str, __func__);
  free(str);
  free(original);

  str = strMkCpy(" \"client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577\" ");
  original = strMkCpy(str);
  strTrimStrAware(str);
  testStrExpect(original, "\"client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577\"", str, __func__);
  free(str);
  free(original);


  str = NULL;
  original = NULL;
}

void testIsInsideOfStr() {
  g_current_prog = __func__;
  char *original;
  char *str;

  original = strMkCpy("Some text \"something...\" abcdefg");
  str = calloc(strlen(original)+1, sizeof(char));
  for (size_t i = 0; i < strlen(original); i++)
    str[i] = isInsideOfStr(original, original+i) ? '1': '0';
  testStrExpect(original, "00000000001111111111111100000000", str, __func__);
  free(original);
  original = NULL;
  free(str);
  str = NULL;
}
#endif
