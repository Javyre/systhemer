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
#include "systhemerUnitheme.h"

int main(int argc, char *argv[]) {
  verboseMode = false;
  exit_on_err = true;
  o_illustrate_recursive_list = true;
  o_illustrate_print_non_friendlied = true;

#ifndef NDEBUG
  /* size_t num_errs = 0; */
  exit_on_failed_test_end = true;
  exit_on_failed_test = false;
#endif

  warnings_on = false;
  parseArgs(argc, argv);

#ifndef NDEBUG
  if (testsMode) {
    testTestsMode();
    return testAll();
    /* num_errs = testAll(); */
    /* if (num_errs != 0 && exit_on_failed_test_end) */
    /*   EXIT(num_errs); */
  } else {
#endif
    /* loadUniTheme("../files/default3.uth"); */
    /* sysUniStart("../files/default4.uth"); */
    if (g_uth_fpath != NULL)
      sysUniStart(g_uth_fpath);
    else {
      fprintf(stderr, BKRED "no file name entered!\n" KDEFAULT);
      printHelp(1);
    }
    /* runLex("../files/default4.uth"); */
    runExecUitheme();
#ifndef NDEBUG
  }
#endif

  return 0;
}
