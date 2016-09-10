// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local includes
#include "main.h"

#ifndef NDEBUG
#include "tests.h"
#endif

#include "utils.h"
#include "unitheme.h"

int main(int argc, char *argv[]) {
  verboseMode = false;
  exit_on_err = true;

#ifndef NDEBUG
  exit_on_failed_test_end = true;
  exit_on_failed_test = false;
#endif

  warnings_on = false;
  parseArgs(argc, argv);

#ifndef NDEBUG
  if (testsMode) {
    testTestsMode();
    testAll();
    if (g_num_errors != 0 && exit_on_failed_test_end)
      EXIT(1);
  } else {
#endif
    loadUniTheme("../files/default2.uth");
    runExecUitheme();
#ifndef NDEBUG
  }
#endif

  return 0;
}
