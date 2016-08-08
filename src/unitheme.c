#include "utils.h"
#include "unitheme.h"

#include <stdlib.h>
#include <string.h>



void loadUniTheme(const char *filename) {
  char *buff = malloc(256);
  FILE *UniThemeFile = fopen(filename, "r");
  currentLine = 0;
  defsInitArr(&progDefs, 5);
  currentProg = calloc(1, sizeof(programDef));

  if (UniThemeFile == NULL) {
    fprintf(stderr, "Error: file %s not found!", filename);
    return;
  }

  while (fgets(buff, 256, UniThemeFile) != NULL) {
    currentLine++;
    if (buff[0] == '\n' || buff[0] == '#') {
      continue;
    } else {
      getFullLine(&buff, UniThemeFile);
      //rmEscape(&buff);
      strTrimStrAware(buff);
      rmEscape(&buff);
      evalLine(buff);
    }
    buff = realloc(buff, 256);
  }
  free(buff);
  fclose(UniThemeFile);
  defsFree(&progDefs);
  free(currentProg);
}

//--------------------------------------------------------------------

void defsInitArr(programDefs *a, size_t initialSize) {
    // Allocate initial space
    a->progs= (programDef **)malloc(initialSize * sizeof(programDef *));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    // Initialize all values of the array to 0
    for(size_t i = 0; i<initialSize; i++)
    {
        memset(&a->progs[i],0,sizeof(programDef *));
    }
}

// Add element to array
void defsInsert(programDefs *a, programDef *element) {
    if (a->used == a->size)
    {
        a->size *= 2;
        a->progs = (programDef **)realloc(a->progs, a->size * sizeof(programDef *));
        // Initialize the last/new elements of the reallocated progs
        for(size_t i = a->used; i < a->size; i++)
          {
            memset(&a->progs[i],0,sizeof(programDef *));
          }
    }

    a->progs[a->used] = element;



    a->used++;
}

void defsFree(programDefs *a) {
    // Free all name variables of each array element first
    for(size_t i=0; i < a->used; i++)
    {
        free(a->progs[i]->name);
        free(a->progs[i]->tokens);
        free(a->progs[i]->path);
        free(a->progs[i]->execBefore);
        free(a->progs[i]->execAfter);
        a->progs[i]->name=NULL;
        a->progs[i]->tokens=NULL;
        a->progs[i]->path=NULL;
        a->progs[i]->execBefore=NULL;
        a->progs[i]->execAfter=NULL;

        free(a->progs[i]->beginDef);
        free(a->progs[i]->endDef);
        a->progs[i]->beginDef=NULL;
        a->progs[i]->endDef=NULL;
        /* a->progs[1]->beginDef=0; */
        /* a->progs[1]->endDef=0; */
    }

    // Now free the array 
    free(a->progs);
    a->progs= NULL;

    a->used = 0;
    a->size = 0;
}
//--------------------------------------------------------------------

void rmComment(char *in) {
  char *src = in;
  while (*src != '\0' && *src != '\n') {
    if (*src == '#' && *(src-1) != '\\')
      *src = '\0';
    src++;
  }
}

void rmEscape(char **currentBuffer) {
  char *original = malloc(strlen(*currentBuffer)+1);
  strcpy(original, *currentBuffer);
  char *src = original;
  /*
  while (*src != '\0' && *src != '\n')
    src++;
  if (*src == '\n')
    fprintf(stderr, "Warning: found a '\\n' (newline) character in current buffer string! This is dangerous!!!\n");
  src = original;
  */
  while (*src != '\0') {
    if (!isInsideOfStr(original, src)) {
      if (*src == '\\' && (src[1] == '\\' || src[1] == '#')) {
        strOverlap(original, original, (src-1), (src+1), NULL);
        src++;
      } else if (*src == '\\' && (src[1] != '\\' && src[1] != '#')) {
        fprintf(stderr, BKRED "Error: Found escape character outside of string with invalid successor \"%c\": \n\t%s\n\t%s\n", src[1], original, genWrongUnderline(original, src, src+1));
        exit(1);
      }
    } else {
      if (*src == '\\' && (src[1] == '\"')) {
        //strOverlap(original, original, (src-1), (src+1), NULL);
        //src++;
        ;
      } /*else if (*src == '\\' && (src[1] != '\"')) {
        fprintf(stderr, BKRED "Error: Found escape character inside of string with invalid successor \"%c\": \n\t%s\n\t%s\n", src[1], original, genWrongUnderline(original, src, src+1));
        exit(1);
        }*/

    }
    src++;
  }

  *currentBuffer = realloc(*currentBuffer, strlen(original)+1);
  strcpy(*currentBuffer, original);
  free(original);
}

bool hasLineExtension(char **currentBuffer) {
  char *src = *currentBuffer;
  while (*src != '\0')
    src++;

  if (*(src-1) == ';' || (*(src-2) == ';' && *(src-1) == '\n')){
    src = *currentBuffer;
    while (*src != ';')
      src++;
    *(src+1) = '\0';
    *currentBuffer = realloc(*currentBuffer, strlen(*currentBuffer)+1);
    return false;
  }

  return true;
}

void getFullLine(char **currentBuffer, FILE *UniThemeFile) {
  //char *holder = malloc(strlen(currentBuffer) + 1);
  char *holder = calloc(256, sizeof(char));
  //char *nextLine = malloc(256);
  strcpy(holder, *currentBuffer);
  rmComment(holder);
  while (hasLineExtension(&holder)) {
    currentLine++;
    //line+=new;
    fgets(*currentBuffer, 256, UniThemeFile);
    holder = realloc(holder, strlen(holder) + 1 + strlen(*currentBuffer) + 1);
    strcat(holder, " ");
    strcat(holder, *currentBuffer);
    rmComment(holder);
  }
  *currentBuffer = realloc(*currentBuffer, strlen(holder)+1);
  strcpy(*currentBuffer, holder);

  //free(nextLine);
  free(holder);
}

bool isList (char *in, char **outListName, char ***outListItems, int *outNumItems) {
  char *src = in;
  char **listItems;
  char **listItemsSrc = *outListItems;
  char **lastItem;
  bool isList = false;
  bool open = false;
  char *whereOpened;
  char *tmpc;
  char *tok;
  char *contents;

  int numItems = 0;
  int tmp;

  while (*src != '\0') {
    if (*src == '\n') {
      src++;
      continue;
    }
    if (*src == '{') {
      if (open) {
        fprintf(stderr, BKRED "Error: List opened more than once: \n\t%s\n\t%s\n", in, genWrongUnderline(in, src, src));
        exit(1);
      }
      whereOpened = src;
      open = true;
      isList = true;
    } else if (*src == '}') {
      if (!open) {
        fprintf(stderr, BKRED "Error: List closed more than once: \n\t%s\n\t%s\n", in, genWrongUnderline(in, src, src));
        exit(1);
      }
      open = false;

      tmp = ((src-1)-(whereOpened+1))+1;
      contents = malloc(tmp+1);
      memset(contents, '\0', tmp+1);
      strncpy(contents, whereOpened+1, tmp);
      VERBOSE_PRINT_VALUE(%s, contents);
      tmpc = contents;
      while(*tmpc != '\0') {
        *tmpc = (*tmpc == '\n') ? ' ' : *tmpc;
        tmpc++;
      }
      strTrimStrAware(contents);
      VERBOSE_PRINT_VALUE(%s, contents);

      tmpc = src;
      tmpc--;
      while (*tmpc == ' '|| *tmpc == '\n') tmpc--;
      if (*tmpc == '\"') {
        numItems++;
      } else if (*tmpc != '\n' && *tmpc != ',') {
        fprintf(stderr, BKRED "Error: Invalid character \"%c\" after end of last item: \n\t%s\n\t%s\n", *tmpc, in, genWrongUnderline(in, tmpc, tmpc));
        exit(1);
      }

    } else if (*src == ',' && open && !isInsideOfStr(in, src)) {
      numItems++;
    }
    src++;
  }
  if (isList && open) {
    fprintf(stderr, BKRED "Error: Found unclosed list: \n\t%s\n\t%s\n", in, genWrongUnderline(in, whereOpened, src));
    exit(1);
  }
  if (isList) {
    *outNumItems = numItems;

    strTrimStrAware(in);
    src = in;
    while (*src != ' ') src++;
    tmp = ((whereOpened-1)-src) + ((*(whereOpened-1) == ' ') ? 0 : 1);
    *outListName = malloc(tmp);
    memset(*outListName, '\0', tmp);
    strncpy(*outListName, src+1, tmp-1);


    listItems = malloc(sizeof(char *) * numItems);
    lastItem = (listItemsSrc+(numItems-1));

    tok = strtok(contents, ",");
    tmp = 0;
    for (int i=0; i<numItems; i++) {
      listItems[i] = malloc(sizeof(char) * (strlen(tok) +1));
      strcpy(listItems[i], tok);
      VERBOSE_PRINT_VALUE(%s, listItems[i]);
      tok = strtok(NULL, ",");
    }

    *outListItems = listItems;
    free(contents);

  }
  return isList;
}


bool isAssignation (char *in, char **outTok, char **outValue) {
  char *src = in;
  char *end = in;
  while (*end != '\0') end++;
  char *subStr;
  int temp;


  while (*src != '\0') {
    if (*src == '=' && !isEmptyStrInRange(in, src-1) && !isInsideOfStr(in, src)) {
      if (isEmptyStrInRange(src+1, end-2)) {

        subStr = malloc(((src-1)-in)+2);
        memset(subStr, '\0', ((src-1)-in)+2);
        strncpy(subStr, in, (src-1)-in);

        fprintf(stderr, BKRED "Error: empty value assigned to %s : \n\t%s\n\t%s\n" KDEFAULT, subStr, in, genWrongUnderline(in, src+1, end-2));

        free(subStr);
        subStr=NULL;
        exit(1);
      }
      temp = ((src-1)-in)+1;
      *outTok = malloc(temp +1);
      memset(*outTok, '\0', temp+1);
      strncpy(*outTok, in, temp);

      temp = ((end-2)-(src+1))+1;
      *outValue = malloc(temp +1);
      memset(*outValue, '\0', temp+1);
      strncpy(*outValue, src+1, temp);

      return true;
    }
    src++;
  }
  return false;
}

bool isStatement (char* in, char** outCall, char** outArg) {
  char *copyIn = strMkCpyInRange(in, strlen(in)-1);
  int words = 0;
  for (char *tok = strtok(copyIn, " "); tok != NULL; tok = strtok(NULL, " ")) {
    if (words == 0)
      *outCall = strMkCpy(tok);
    else if (words == 1)
      *outArg = strMkCpy(tok);
    words++;
  }
  free(copyIn);
  copyIn = NULL;
  if (words != 2)
    return false;
  return true;
}

void evalLine (char* currentBuffer) {
  char *src        = currentBuffer;
  char *statCall   = NULL;
  char *statArg    = NULL;
  char *assigTok   = NULL;
  char *assigValue = NULL;
  char *listName   = NULL;
  char **listItems = NULL;
  int  numItems;
  while (*src == ' ' || *src == '\t')
    src++;
  if (verboseMode) printf("=====================================\n");
  VERBOSE_PRINT("Evaluating line bufer...");
  VERBOSE_PRINT_VALUE(%s, src);

  if (isStatement(currentBuffer, &statCall, &statArg)) {
    printf("ITS A STATEMENT!!!\n");
    /*evalStatement*/;
    evalStatement(currentBuffer, statCall, statArg);
    statCall = NULL;
    statArg = NULL;
  } else if (isAssignation(currentBuffer, &assigTok, &assigValue)){
    printf("ITS AN ASSIGNATION!!!\n");
    /* evalAssignation; */
    evalAssig(currentBuffer, assigTok, assigValue);
    assigTok = NULL;
    assigValue = NULL;
  } else if (isList(currentBuffer, &listName, &listItems, &numItems)) {
    printf("ITS A LIST!!! ###%s###%d###\n", listName, numItems);
    /*evalList*/;
    evalList(currentBuffer, listName, listItems, numItems);
    listName = NULL;
    listItems = NULL;
    numItems = 0;
  } else {
    fprintf(stderr, BKRED "Error: Could not recognize (line #%d): %s", currentLine, currentBuffer);
    exit(1);
  }


}

void evalStatement(char *currentBuffer, char *statCall, char *statArg) {
  strUnstring(&statArg);
  if (strcmp(statCall, "begindef") == 0) {
    if (currentProg->name == NULL) {
      currentProg->beginDef = malloc(sizeof(unsigned int) * 1);
      *currentProg->beginDef = currentLine;
      currentProg->name = statArg;
    } else {
      fprintf(stderr, BKRED "Error: Called begindef twice! : check line #%d\n" KDEFAULT, currentLine);
      exit(1);
    }
  } else if (strcmp(statCall, "enddef") == 0) {
    VERBOSE_PRINT("ENDING DEF SECTION");
    /* store program detail of struct somehow */
    currentProg->endDef = malloc(sizeof(unsigned int) * 1);
    *currentProg->endDef = currentLine;
    defsInsert(&progDefs, currentProg);

    currentProg = calloc(1, sizeof(programDef));
    currentProg->name = NULL;

  }
  printf("%s\n", statCall);
  printf("%s\n", statArg);
  free(statCall);
  statCall = NULL;
  //free(statArg);
}

void evalAssig(char* currentBuffer, char* tok, char* value) {
  if (strcmp(tok, "path") == 0) {
    printf("FOUND PATH ASSIGNATION!!!\n");
  }
  free(tok);
  free(value);
}

void evalList(char* currentBuffer, char* listName, char** listItems, int numItems) {



  for(int i = 0; i < numItems; i++)
		free(listItems[i]);
	free(listItems);
  free(listName);
}

