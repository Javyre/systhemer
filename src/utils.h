#ifndef UTILS_H
#define UTILS_H

//C includes (that we use in this header)
#include <stdbool.h>

//Package defines and dev options (constant)
#define PACKAGE "SysThemer"
#define VERSION "0.0.1"
#define EXIT_ON_ERR true


#define KNRM  "\x1B[0m"
#define BKNRM  "\e[1;0m"
#define KRED  "\x1B[31m"
#define BKRED  "\e[1;31m"
#define KGRN  "\x1B[32m"
#define BKGRN  "\e[1;32m"
#define KYEL  "\x1B[33m"
#define BKYEL  "\e[1;33m"
#define KBLU  "\x1B[34m"
#define BKBLU  "\e[1;34m"
#define KMAG  "\x1B[35m"
#define BKMAG  "\e[1;35m"
#define KCYN  "\x1B[36m"
#define BKCYN  "\e[1;36m"
#define KWHT  "\x1B[37m"
#define BKWHT  "\e[1;37m"

#define PRINT_VALUE(token, color) printf(color #token " is %d" "\n", token);
#define VERBOSE_PRINT(message) if(verboseMode) printf(BKBLU PACKAGE ": %s\n", message);
#define VERBOSE_PRINT_VALUE(token) if (verboseMode) {printf(PACKAGE ": "); PRINT_VALUE(token, BKBLU);}
#define TEST_PRINT(message) if (testsMode) {printf(BKYEL PACKAGE ": %s\n", message);}
#define TEST_PRINT_VALUE(token) if (testsMode) {printf(PACKAGE ": "); PRINT_VALUE(token, BKYEL);}

bool testsMode;
bool verboseMode;

//Function declarations
void printHelp(const int);
void verboseMessage(const char*);
void parseArgs(int, char* []);

#endif
