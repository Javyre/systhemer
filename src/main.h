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
void printHelp(const int);
void parseArgs(int, char* []);
void verboseMessage(const char*);

#endif
