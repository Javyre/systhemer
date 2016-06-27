#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "tests.h"
#include "utils.h"

void testTestsMode () {
  TEST_PRINT_VALUE(%d, testsMode)
}

char* testStrTrim (char *in) {
  TEST_PRINT_VALUE(%s, in);

  char *firstin = in;
  char *out = malloc(strlen(in));
  char *firstout = out;

  //if (*in!='\0' && *in!= ' ') *out = *in;
  while ( *in!='\0'){
    if (*in!='\0' && *in!= ' ') {
      *out = *in;
      if (*(in+1)==' ') {
	out++;
	in++;
	*out = *in;
      }
      out++;
      in++;
    }
    else {
      in++;
    }
  }
  *out = '\0';
  out = firstout;
  in = firstin;
  TEST_PRINT_VALUE(%s, in);
  TEST_PRINT_VALUE(%s, out);
  return out;
}

void testStrTrimPointer (char *in) {
  char *firstin = in;
  char *source = in;

  TEST_PRINT_VALUE(%s, in);
  while (*in!='\0') {
    TEST_PRINT_VALUE(%c, *in);
    if(*in!=' '){
      *source = *in;
      if (*(in+1)==' ') {
	source++;
	in++;
	*source = *in;
      }
      source++;
      in++;
    } else {
      in++;
    }
  }
  *source = '\0';
  in = firstin;
  TEST_PRINT_VALUE(%s, in);
}


void testFileIO () {
  /* 
     client.focused 
     client.unfocused
     client.focused_inactive
     client.urgent

     focused_workspace
     inactive_workspace
     urgent_workspace
   */
  
  FILE *fp = fopen("../files/test/testcfg", "r");
  if (fp==NULL) {
    fprintf(stderr, "Can't open file ../files/test/testcfg !!!\n");
    exit(1);
  }

  char* s, buff[256];
  while ((s = fgets(buff, sizeof(buff), fp)) != NULL){
    if (buff[0] == '\n' || buff[0] == '#')
      continue;

    
  }
  
}


