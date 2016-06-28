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

//Converts "   This\t    \t    text     " into "This text"
void testStrTrimPointer (char *in) {
  char *firstin = in; // Hold position of first char in char* in
  char *source = in;  // Used to rewrite char* in
  
  TEST_PRINT_VALUE(%s, in);
  while (*in!='\0') { // While it hasnt reached the last char
    TEST_PRINT_VALUE(%c, *in);
    if(*in!=' ' && *in!='\t'){
      *source = *in;
      if (*(in+1)==' ' || *(in+1)=='\t') {
	source++;
	in++;
	*source = ' ';
      }
      source++;
      in++;
    } else {
      in++;
    }
  }
  if (*(source-1)==' ')
    *(source-1)='\0';
  else
    *source='\0';
  
  in = firstin;
  TEST_PRINT_VALUE(%s, in);
}

void testStrTrimPointerNew (char *in) {
  char *out = malloc(strlen(in)+1);
  char *token;

  token = strtok(in, " \t");
  while (token!=NULL) {
    TEST_PRINT_VALUE(%s, token);
    strcat(out, token);
    strcat(out, " ");
    token = strtok(NULL, " \t");
  }
  
  char *outlast = out;
  while (*outlast++);
  outlast-=2;
  if (*outlast==' ') *outlast = '\0';
  TEST_PRINT_VALUE(%s, out);
  strcpy(in, out);
  free(out);
}


void testFileIO () {
  /* 
     client.focused something something something
     client.unfocused ... ... ...
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


