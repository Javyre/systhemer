%locations

%{
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "unitheme.h"
#include "memory.h"
#include "utils.h"
extern int yylex();
extern int yylineno;
extern char *yytext;
extern int yycolumn;
extern char linebuf[];


int yyerror(char *p);
size_t yyerror_count = 0;


%}

%union {
  char *str;
  list *lst;
  t_ptr_list *ptr_lst;
  memory_address addr;
  memory_item *mem_item;
};

%token KSTRING KREGEX KSEMI KCOMMA KEQUALS LBRACE RBRACE KBEGINDEF KENDDEF KDEF KPERIOD LPAREN RPAREN
%token <str> TSTR TRGXP TIDENTIFIER
%type <addr> first_string_list_item last_string_list_item first_string_list_itemp last_string_list_itemp
%type <addr> first_regex_list_item last_regex_list_item first_regex_list_itemp last_regex_list_itemp
%type <addr> string_p regex_p
%type <ptr_lst> string_list_content
%type <ptr_lst> regex_list_content

%destructor { free($$); } <str>
%destructor { defsFreeList($$); free($$); } <lst>
%destructor { ptrListFree($$); free($$); } <ptr_lst>

%%

line
 : line definition KSEMI
 | definition KSEMI
 | line function_call KSEMI
 | function_call KSEMI
 | line error KSEMI

definition
 : variable_def
 | list_def

function_call
 : TIDENTIFIER LPAREN string_p RPAREN { handleFuncCall($1, $3, T_STRING); }

variable_def
: KDEF KSTRING TIDENTIFIER KEQUALS string_p { handleAssigDef($3, $5, T_STRING); }
| KDEF KREGEX TIDENTIFIER KEQUALS regex_p { handleAssigDef($3, $5, T_REGEX); }
| KDEF TIDENTIFIER KEQUALS TIDENTIFIER { handlePointerAssig($2, $4); }


list_def
: KDEF KSTRING TIDENTIFIER KEQUALS string_list_content { handleListDef($3, $5, T_STRING); }
| KDEF KREGEX TIDENTIFIER KEQUALS regex_list_content { handleListDef($3, $5, T_REGEX); }

/* NOTE: currently printing pointer values to suppress some bison gen warnings */
/* ==== string list content ==== */

string_list_content
/* last item */
 : string_list_content last_string_list_item {
   ptrListInsert($$, $2);
   VERBOSE_PRINT("-last list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }
/* nth item */
 | string_list_content string_p KCOMMA {
   /* memory_item *item = malloc(sizeof(memory_item)); */
   /* item->str = $2; */
   /* memory_address address = memoryInsert(g_memory, item, t_str); */

   ptrListInsert($$, $2);
   VERBOSE_PRINT("-list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }
/* first item */
 | first_string_list_item {
   $$ = (t_ptr_list *)malloc(sizeof(t_ptr_list));
   ptrListInit($$, 20);

   ptrListInsert($$, $1);
   VERBOSE_PRINT("-first list item: \t%lu into (%p)", (unsigned long)$1, $$);
 }
/* only item */
 | LBRACE last_string_list_item {
   $$ = (t_ptr_list *)malloc(sizeof(t_ptr_list));
   ptrListInit($$, 20);

   ptrListInsert($$, $2);
   VERBOSE_PRINT("-only list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }

last_string_list_item
: last_string_list_itemp              { $$ = $1; if (g_memory->content_type[(memoryGetRootAddress(g_memory, $$))] != t_str) yyerror("type missmatch"); }

last_string_list_itemp
: string_p RBRACE                    { $$ = $1; }
| string_p KCOMMA RBRACE             { $$ = $1; }
| TIDENTIFIER RBRACE                 { $$ = memoryGetAddresByFriendly(g_friendlies , $1); free($1); }
| TIDENTIFIER KCOMMA RBRACE          { $$ = memoryGetAddresByFriendly(g_friendlies , $1); free($1); }

first_string_list_item
: LBRACE string_p KCOMMA { $$ = $2; }
| LBRACE TIDENTIFIER KCOMMA { $$ = memoryGetAddresByFriendly(g_friendlies , $2); free($2); if (g_memory->content_type[(memoryGetRootAddress(g_memory, $$))] != t_str) yyerror("type missmatch"); }

/* ==== regex list content ==== */

regex_list_content
/* last item */
: regex_list_content last_regex_list_item {
  ptrListInsert($$, $2);
  VERBOSE_PRINT("-last list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }
/* nth item */
| regex_list_content regex_p KCOMMA {
  /* memory_item *item = malloc(sizeof(memory_item)); */
  /* item->str = $2; */
  /* memory_address address = memoryInsert(g_memory, item, t_rgx); */

  ptrListInsert($$, $2);
  VERBOSE_PRINT("-list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }
| regex_list_content TIDENTIFIER KCOMMA {
  /* memory_item *item = malloc(sizeof(memory_item)); */
  /* item->str = $2; */
  /* memory_address address = memoryInsert(g_memory, item, t_rgx); */

  memory_address address = memoryGetAddresByFriendly(g_friendlies , $2);
  free($2);
  if (g_memory->content_type[(memoryGetRootAddress(g_memory, address))] != t_rgx) yyerror("type missmatch");

  ptrListInsert($$, address);
  VERBOSE_PRINT("-list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }
/* first item */
| first_regex_list_item {
  $$ = (t_ptr_list *)malloc(sizeof(t_ptr_list));
  ptrListInit($$, 20);

  ptrListInsert($$, $1);
  VERBOSE_PRINT("-first list item: \t%lu into (%p)", (unsigned long)$1, $$);
  }
/* only item */
| LBRACE last_regex_list_item {
  $$ = (t_ptr_list *)malloc(sizeof(t_ptr_list));
  ptrListInit($$, 20);

  ptrListInsert($$, $2);
  VERBOSE_PRINT("-only list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }

last_regex_list_item
: last_regex_list_itemp              { $$ = $1; if (g_memory->content_type[(memoryGetRootAddress(g_memory, $$))] != t_rgx) yyerror("type missmatch"); }

last_regex_list_itemp
: regex_p RBRACE                     { $$ = $1; }
| regex_p KCOMMA RBRACE              { $$ = $1; }
| TIDENTIFIER RBRACE                 { $$ = memoryGetAddresByFriendly(g_friendlies , $1); free($1);}
| TIDENTIFIER KCOMMA RBRACE          { $$ = memoryGetAddresByFriendly(g_friendlies , $1); free($1);}

first_regex_list_item
: LBRACE regex_p KCOMMA              { $$ = $2; }
| LBRACE TIDENTIFIER KCOMMA          { $$ = memoryGetAddresByFriendly(g_friendlies , $2); free($2); if (g_memory->content_type[(memoryGetRootAddress(g_memory, $$))] != t_rgx) yyerror("type missmatch"); }

/* --------------------------------------------------------------------------- */

string_p
: TSTR {
  memory_item *item = malloc(sizeof(memory_item));
  item->str = $1;
  $$ = memoryInsert(g_memory, item, t_str);
  strUnstring(&g_memory->content[memoryGetRootAddress(g_memory, $$)]->str);
  VERBOSE_PRINT("string litteral %s in %lu"  , g_memory->content[memoryGetRootAddress(g_memory, $$)]->str , (unsigned long)$$);
 }

regex_p
: TRGXP {
  memory_item *item = malloc(sizeof(memory_item));
  item->str = $1;
  $$ = memoryInsert(g_memory, item, t_rgx);
  regexUnregex(&g_memory->content[memoryGetRootAddress(g_memory, $$)]->str);
  VERBOSE_PRINT("regex litteral %s in %lu"  , g_memory->content[memoryGetRootAddress(g_memory, $$)]->str, (unsigned long)$$);
 }
%%


int yyerror(char *p) {
  char *message = p;
  char *underline, *snippet;
  yylloc.first_column++;
  yylloc.last_column++;

  if (strcmp(p, "syntax error") == 0) {
    message = calloc(strlen(p) + 120 + 1, sizeof(char));
    strcpy(message, p);
    strcat(message, " (token or keyword did not match any semantic rule)");
  }

  /* print main message */
  fprintf(stderr, BKRED "Error: %s : line %d:%d:%d: " BKMAG "%s\n" KDEFAULT,
          message, yylineno, yylloc.first_column, yylloc.last_column, yytext);

  /* print out snippet of code and highlight the bad part */
  snippet = (char *)calloc(strlen(linebuf) + (256 * 8) + 1, sizeof(char));
  for (size_t i = 0; i < strlen(linebuf); i++) {
    sprintf(
        snippet + strlen(snippet), "%s%c" KDEFAULT,
        ((i < yylloc.first_column || i > yylloc.last_column) ? KMAG : BKRED),
        linebuf[i]);
  }
  fprintf(stderr, BKRED "%s\n" KDEFAULT, snippet);

  /* underline the bad part under the snippet */
  underline = (char *)calloc(strlen(linebuf)+1, sizeof(char));
  for (size_t i=0; i<=yylloc.first_column; i++) {
    underline[i] = ' ';
  }
  for (size_t i=yylloc.first_column; i<=yylloc.last_column; i++) {
    underline[i] = '^';
  }
  fprintf(stderr, BKRED "%s\n" KDEFAULT, underline);

  /* free buffers */
  if (message != p)
    free(message);
  free(underline);
  free(snippet);

  yyerror_count++;
  return 1;
}
