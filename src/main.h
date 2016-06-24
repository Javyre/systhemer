#ifndef MAIN_H
#define MAIN_H

//C includes (that we use in this header)
#include <stdbool.h>

//Package defines and dev options (constant)
#define PACKAGE "SysThemer"
#define VERSION "0.0.1"
#define EXIT_ON_ERR true

//Default options (dynamic)
bool verboseMode = false;

//Function declarations
void printHelp(int);
void parseArgs(int argc, char *argv[]);

#endif
