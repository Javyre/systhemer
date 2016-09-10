#ifndef NDEBUG
#ifndef TESTS_H
#define TESTS_H
#include <stdbool.h>

static size_t g_num_errors = 0;
static size_t g_call_num = 0;
static char * g_current_prog = NULL;

void testTestsMode ();
char* testStrTrim (char *);
void testStrTrimPointer (char *);
void testStrTrimPointerNew (char *);
void testFileIO ();

void testAll();
bool testStrExpect(const char *original, const char *exp, const char *result, const char *call_name);
void testStrTrimStrAware();
void testIsInsideOfStr();
void testStrTrimInRange();

void testRegex();

#endif
#endif
