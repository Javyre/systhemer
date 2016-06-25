#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "utils.h"


//Default options (dynamic)
bool verboseMode = true;
bool testsMode = true;

void printHelp(const int exval) {
  printf("%s,%s show working getopt example\n", PACKAGE, VERSION); 
  printf("%s [-h] [-V]\n\n", PACKAGE);

  printf("  -t              set test flag (verbose + run tests)\n");
  printf("  -h              print this help and exit\n");
  printf("  -V              print version and exit\n");
  printf("  -v              set verbose flag\n");

  if (exval==0 || EXIT_ON_ERR)
    exit(exval);
}

void verboseMessage(const char* message){ //\e[1;34m and \e[m make text blue
  if (verboseMode) printf("\e[1;34m%s: %s\e[m\n", PACKAGE, message);
}

void parseArgs(int argc, char *argv[]) {
  static struct option const long_options[] =
    {
      {"tests",   no_argument, NULL, 't'}, 
      {"help",    no_argument, NULL, 'h'},
      {"version", no_argument, NULL, 'V'},
      {"verbose", no_argument, NULL, 'v'}
    };
  int opt;
  
  //Checks for commands
  if(argc == 1) {
    fprintf(stderr, "This program needs arguments....\n\n");
    printHelp(1);
  }

  //Main loop that checks each toggle and argument
  while ((opt = getopt_long(argc, argv, "thVv", long_options, NULL)) != -1) {
    switch(opt) {
    case 't':
      verboseMode = true;
      testsMode = true;
      break;
    case 'h':
      printHelp(0);
      break;
    case 'V':
      printf("%s %s\n\n", PACKAGE, VERSION); 
      exit(0);
      break;
    case 'v':
      verboseMode = true;
      printf("%s: Verbose option is set '%c'\n", PACKAGE, optopt);
      verboseMessage("Testing...");
      break;
    default:
    case '?':
      fprintf(stderr, "%s: Error - No such option: `%c'\n\n", PACKAGE, optopt);
      printHelp(1);
    }
  }
}
