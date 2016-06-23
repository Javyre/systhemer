// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>

//Local includes
#include "main.h"

//Package defines and dev options
#define PACKAGE "SysThemer"
#define VERSION "0.0.1"
#define EXIT_ON_ERR false

int main(int argc, char *argv[]) {
  parseArgs(argc, argv);
  
  
  return 0;
}

void print_help(int exval) {
  printf("%s,%s show working getopt example\n", PACKAGE, VERSION); 
  printf("%s [-h] [-V]\n\n", PACKAGE);

  printf("  -h              print this help and exit\n");
  printf("  -V              print version and exit\n");
  printf("  -v              set verbose flag\n");

  if (!exval || EXIT_ON_ERR)
    exit(exval);
}

void parseArgs(int argc, char *argv[]) {
  static struct option const long_options[] =
    {
      {"help",    no_argument, NULL, 'h'},
      {"version", no_argument, NULL, 'V'},
      {"verbose", no_argument, NULL, 'v'}
    };
  int opt;
  
  //If arg count is 1 because the first argument in argv is the command itself
  if(argc == 1) {
    fprintf(stderr, "This program needs arguments....\n\n");
    print_help(1);
  }

  //Main loop that checks each toggle and argument
  while ((opt = getopt_long(argc, argv, "hVv", long_options, NULL)) != -1) {
    switch(opt) {
    case 'h':
      print_help(0);
      break;
    case 'V':
      printf("%s %s\n\n", PACKAGE, VERSION); 
      exit(0);
      break;
    case 'v':
      printf("%s: Verbose option is set `%c'\n", PACKAGE, optopt);
      break;
    default:
    case '?':
      fprintf(stderr, "%s: Error - No such option: `%c'\n\n", PACKAGE, optopt);
      print_help(1);
    }
  }
}
