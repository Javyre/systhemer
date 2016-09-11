#ifndef NDEBUG
#ifndef TESTS_H
#define TESTS_H
#include <stdbool.h>

void testTestsMode ();
char* testStrTrim (char *);
void testStrTrimPointer (char *);
void testStrTrimPointerNew (char *);
void testFileIO ();

size_t testAll();
bool testStrExpect(const char *original, const char *exp, const char *result, const char *call_name);
void testStrTrimStrAware();
void testIsInsideOfStr();
void testIsInsideOfRegEx();
void testStrTrimInRange();

void testRegex();
void testRegexRmEscape();
void testRegexUnregex();

#endif
#endif
