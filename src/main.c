// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local includes
#include "main.h"
#include "tests.h"
#include "utils.h"
#include "unitheme.h"

int main(int argc, char *argv[]) {
  parseArgs(argc, argv);
  loadUniTheme("../files/default.uth");

  if (testsMode) {
    testTestsMode();

    char *string =
        malloc(strlen("    Hello\t\t   world\t              :D    \t") + 1);
    strcpy(string, "    Hello\t\t   world\t              :D    \t");

    // strc
    // testStrTrimPointer(string);
    // testStrTrimPointerNew(string);
    // string should now be "Hello world :D"
    // VERBOSE_PRINT_VALUE(%s, string);

    testFileIO();
  }



  return 0;
}
