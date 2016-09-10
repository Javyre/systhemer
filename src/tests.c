#ifndef NDEBUG
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include "tests.h"
#include "utils.h"

#define UPDATE_CURR_PROG() do{ if (g_current_prog != NULL) free(g_current_prog); g_current_prog = NULL; g_current_prog = strMkCpy(__func__); g_num_errors_total += g_num_errors; g_num_errors = 0; g_call_num = 0; printf("\n%s%lu total test fails so far...\n" KDEFAULT, (g_num_errors_total > 0 ? BKRED : BKGRN), (unsigned long)g_num_errors_total); printf("=======================\n"); }while(0);

void testTestsMode() { TEST_PRINT_VALUE(%d, testsMode) }

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
  /* static int g_num_errors = 0; */
  /* static char *prev_call_name; */
  bool res;
  g_call_num++;
  if (strcmp(exp, result) == 0) {
    T_PRINT("\nTest #%lu of %s" BKGRN " ran successfully\n", (unsigned long) g_call_num, g_current_prog);
    res = true;
  } else {
    g_num_errors++;
    T_PRINT("\nTest #%lu of %s" BKRED " ran unsuccessfully: \n\texpected: %s\n\treceived: %s\n%s%s%s%lu test fails so far...\n", (unsigned long) g_call_num, g_current_prog, exp, result, original == NULL ? "" : "\toriginal: ", original == NULL ? "" : original, original == NULL ? "" : "\n", (unsigned long) g_num_errors);
    res = false;
  }
  /* free(prev_call_name); */
  /* prev_call_name = strMkCpy(call_name); */

  if (!res && exit_on_failed_test)
    EXIT(1);

  return res;
}

size_t testAll() {
  testStrTrimStrAware();
  testIsInsideOfStr();
  testStrTrimInRange();

  testRegex();

  UPDATE_CURR_PROG();

  free(g_current_prog);
  g_current_prog = NULL;

  return g_num_errors_total;
}

#define TEST_STR_TRIM_STR_AWARE(rgnl, xpct)       \
  do {                                            \
    original = strMkCpy(rgnl);                    \
    str = strMkCpy(original);                     \
    strTrimStrAware(str);                         \
    testStrExpect(original, xpct, str, __func__); \
    free(original);                               \
    free(str);                                    \
  } while (0);

void testStrTrimStrAware() {
  UPDATE_CURR_PROG();
  char *original;
  char *str;

  TEST_STR_TRIM_STR_AWARE("\"foo   bar\"", "\"foo   bar\"");
  TEST_STR_TRIM_STR_AWARE(" \"foo   bar\"", "\"foo   bar\"");
  TEST_STR_TRIM_STR_AWARE("\"foo   bar\"   ", "\"foo   bar\"");
  TEST_STR_TRIM_STR_AWARE(" \"foo   bar\"   ", "\"foo   bar\"");

  TEST_STR_TRIM_STR_AWARE(" \"foo   ba\\\"r\" ", "\"foo   ba\\\"r\"");

  original = NULL;
  str = NULL;
}
#undef TEST_STR_TRIM_STR_AWARE

#define TEST_IS_INSIDE_OF_STR(rgnl, xpct)                       \
  do {                                                          \
    original = strMkCpy(rgnl);                                  \
    str = calloc(strlen(original)+1, sizeof(char));             \
    for (size_t i = 0; i < strlen(original); i++)               \
      str[i] = isInsideOfStr(original, original+i) ? '=': '_';  \
    testStrExpect(original, xpct, str, __func__);               \
    free(str);                                                  \
    free(original);                                             \
  } while (0);

void testIsInsideOfStr() {
  UPDATE_CURR_PROG();
  char *original;
  char *str;

  TEST_IS_INSIDE_OF_STR("foo bar \"foo...\" bar",
                        "________========____");


  TEST_IS_INSIDE_OF_STR("  ",
                        "__");

  TEST_IS_INSIDE_OF_STR(" ",
                        "_");

  TEST_IS_INSIDE_OF_STR("\"\"",
                        "==");

  TEST_IS_INSIDE_OF_STR(" \"\"",
                        "_==");

  TEST_IS_INSIDE_OF_STR(" \"\" ",
                        "_==_");

  TEST_IS_INSIDE_OF_STR(" \" foo\\\"bar \" ",
                        "_============_");

  str = NULL;
  original = NULL;
}
#undef TEST_IS_INSIDE_OF_STR

#define TEST_STR_TRIM_IN_RANGE(rgnl, xpct, frm, to) \
  do {                                              \
    original = strMkCpy(rgnl);                      \
    str = strMkCpy(original);                       \
    strTrimInRange(str+frm, str+to);                \
    testStrExpect(original, xpct, str, __func__);   \
    free(original);                                 \
    free(str);                                      \
  } while (0);
/* str = strMkCpyInRange(original+frm, (to-frm)+1);   \ */
/* printf("%s\n", str);                               \ */
/* free(str);                                         \ */
void testStrTrimInRange() {
  UPDATE_CURR_PROG();
  char *original;
  char *str;

  TEST_STR_TRIM_IN_RANGE("foo   bar   \"   foo   ...\"   bar",
                         "foo bar"  " \"   foo   ...\"   bar", 0, 12);
  TEST_STR_TRIM_IN_RANGE("foo   bar   \"   foo   ...\"   bar",
                         "foo   bar   \"   foo   ...\""" bar", 25, 31);

  TEST_STR_TRIM_IN_RANGE("foo   bar   \"   foo   ...\"   bar",
                         "foo bar"   "\"   foo   ...\"   bar", 0, 11);
  TEST_STR_TRIM_IN_RANGE("foo   bar   \"   foo   ...\"   bar",
                         "foo   bar   \"   foo   ...\"" "bar", 26, 31);

  TEST_STR_TRIM_IN_RANGE(" \"foo\"",
                         "\"foo\"", 0, 1);
  TEST_STR_TRIM_IN_RANGE("\"foo\" ",
                         "\"foo\"", 4, 5);

  /* TEST_STR_TRIM_IN_RANGE("foo   bar   \"   foo   ...\"   bar", "foo bar \"   foo   ...\" bar"); */

  str = NULL;
  original = NULL;
}
#undef TEST_STR_TRIM_IN_RANGE

#define TEST_REGEX(ntr, rg_xpr, xpct)                                   \
  do {                                                                  \
    entry = strMkCpy(ntr);                                              \
    reg_expr = strMkCpy(rg_xpr);                                        \
    str = NULL;                                                         \
    reg_comped = pcre2_compile((PCRE2_SPTR)reg_expr, PCRE2_ZERO_TERMINATED, 0, &error_code, &error_offset, NULL); \
    if (reg_comped == NULL) {                                           \
      fprintf(stderr, BKRED "REGEX COMPILATION FAILED!!\n" KDEFAULT);   \
      err_msg = calloc(256, sizeof(PCRE2_UCHAR));                       \
      pcre2_get_error_message(error_code, err_msg, &err_msg_len);       \
      fprintf(stderr, BKRED "%s\n" KDEFAULT, err_msg);                  \
      free(err_msg);                                                    \
    } else {                                                            \
      match_data = pcre2_match_data_create_from_pattern(reg_comped, NULL); \
      error_code = pcre2_match(reg_comped, (PCRE2_SPTR)entry, strlen(entry), 0, 0, match_data, NULL); \
      pcre2_match_data_free(match_data);                                \
      if (error_code >= 0) {                                            \
        str = strMkCpy("match");                                        \
      } else {                                                          \
        str = strMkCpy("no match");                                     \
                                                                        \
        VERBOSE_PRINT_VALUE(%d, error_code);                            \
        err_msg = calloc(256, sizeof(PCRE2_UCHAR));                     \
        pcre2_get_error_message(error_code, err_msg, &err_msg_len);     \
        fprintf(stderr, BKRED "%s\n" KDEFAULT, err_msg);                \
        free(err_msg);                                                  \
      }                                                                 \
    }                                                                   \
                                                                        \
    original = malloc(256);                                             \
    sprintf(original, "%s and %s", entry, reg_expr);                    \
    strRealloc(&original);                                              \
    testStrExpect(original, xpct, str == NULL ? "ERROR" : str, __func__); \
                                                                        \
    pcre2_code_free(reg_comped);                                        \
    free(entry);                                                        \
    free(original);                                                     \
    free(str);                                                          \
    free(reg_expr);                                                     \
    str = original = entry = reg_expr = err_msg = match_data = reg_comped = NULL; \
    error_code = error_offset = err_msg_len = 0;                        \
  } while (0);
void testRegex() {
  UPDATE_CURR_PROG();
  char *str;
  char *original;
  char *entry;
  char *reg_expr;
  int error_code = 0;
  size_t error_offset = 0;
  pcre2_match_data *match_data;
  PCRE2_UCHAR *err_msg = NULL;
  PCRE2_SIZE err_msg_len = 0;

  pcre2_code *reg_comped;

  TEST_REGEX("foo bar",
             "^\\S+\\s\\S+\\n?$",
             "match");
  TEST_REGEX("client.focused          #4c7899 #285577 #ffffff #2e9ef4   #285577",
             "^\\n?client\\.focused\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\n?$",
             "match");
  TEST_REGEX("client.unfocused        $inactive-bg-color  $inactive-bg-color $inactive-text-color #B2FFA9",
             "^\\n?client\\.unfocused\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\n?$",
             "match");



}

#endif
