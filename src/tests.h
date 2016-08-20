#ifndef NDEBUG
#ifndef TESTS_H
#define TESTS_H
#include <stdbool.h>

size_t g_num_errors = 0;
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

#endif
#endif
