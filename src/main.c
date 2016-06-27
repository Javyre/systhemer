// C includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <getopt.h>

//Local includes
#include "main.h"
#include "utils.h"
#include "tests.h"

int main(int argc, char *argv[]) {
  parseArgs(argc, argv);

  if (testsMode) {
    testTestsMode();
    char *string = malloc(strlen("    Hello\t\t   world\t              :D    ")+1);
    strcpy(string, "    Hello\t\t   world\t              :D    ");
    //string = testStrTrim(string);
    testStrTrimPointer(string);
    VERBOSE_PRINT_VALUE(%s, string);
    //testFileIO();
  }

  
  return 0;
}
