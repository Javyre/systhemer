// C includes
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <getopt.h>

//Local includes
#include "main.h"
#include "utils.h"
#include "tests.h"

int main(int argc, char *argv[]) {
  parseArgs(argc, argv);

  if (testsMode)
    testTestsMode();
  
  return 0;
}
