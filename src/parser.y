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

int yyerror(const char *p);
size_t yyerror_count = 0;


%}

%union {
  int  intgr;
  char operator;
  char *str;
  list *lst;
  t_ptr_list *ptr_lst;
  memory_address addr;
  memory_item *mem_item;
};

%token KSTRING KREGEX KSEMI KCOMMA KEQUALS KPLUS KMINUS KTIMES KDIVIDED LBRACE RBRACE LSQUBRACE RSQUBRACE KBEGINDEF KENDDEF KDEF KPERIOD LPAREN RPAREN KCOLON
%token <str> TSTR TRGXP TIDENTIFIER
%token <intgr> TINTEGER
/* %type <addr> first_string_list_item last_string_list_item first_string_list_itemp last_string_list_itemp
 * %type <addr> first_regex_list_item last_regex_list_item first_regex_list_itemp last_regex_list_itemp */
/* %type <addr> first_list_item last_list_item first_list_itemp last_list_itemp */
%type <addr> int_p string_p regex_p list_content_p str_p pointer
/* %type <ptr_lst> string_list_content
 * %type <ptr_lst> regex_list_content */
%type <ptr_lst> list_content
/* %type <operator> operator */

%destructor { VERBOSE_PRINT("FREEING STRING %s", $$); free($$); } <str>
/* %destructor { defsFreeList($$); free($$); } <lst> */
%destructor { VERBOSE_PRINT("FREEING PTR LIST %p", $$); ptrListFree($$); free($$); } <ptr_lst>

%left KPLUS KMINUS
%left KTIMES KDIVIDED
%left LSQUBRACE RSQUBRACE

%%

line
 /* : %empty */
 : line definition KSEMI
 | definition KSEMI
 | line function_call KSEMI
 | function_call KSEMI
 | line error KSEMI

definition
 : variable_def
 /* | list_def */

/* function_call
 *  : TIDENTIFIER LPAREN string_p RPAREN { handleFuncCall($1, $3, T_STRING); } */

pointer
: pointer LSQUBRACE pointer RSQUBRACE                   { $$=handleListIndex($1, $3);      }
| pointer LSQUBRACE pointer KCOLON pointer RSQUBRACE    { $$=handleListSublist($1, $3, $5);      }

| LPAREN pointer RPAREN                  { $$=$2; }
/* | LPAREN pointer operator pointer RPAREN { $$=handleOperation($2, $3, $4); } */
/* | pointer operator pointer               { $$=handleOperation($1, $2, $3); } */
| KMINUS pointer                         { $$=handleOperation($2, 'n', $2); }
| pointer KPLUS    pointer               { $$=handleOperation($1, '+', $3); }
| pointer KMINUS   pointer               { $$=handleOperation($1, '-', $3); }
| pointer KTIMES   pointer               { $$=handleOperation($1, '*', $3); }
| pointer KDIVIDED pointer               { $$=handleOperation($1, '/', $3); }
| list_content_p
| str_p
| int_p
| TIDENTIFIER { $$ = memoryGetAddresByFriendly(g_friendlies, $1); free($1); }

/* operator */
/* : KPLUS       { $$='+'; } */
/* | KMINUS      { $$='-'; } */
/* | KTIMES      { $$='*'; } */
/* | KDIVIDED    { $$='/'; } */

function_call
 /* : TIDENTIFIER list_content_p { handleFuncCall($1, $2); } */
 : TIDENTIFIER pointer {
   if(memoryGetRootType(g_memory, $2) == t_list) {
     handleFuncCall($1, $2);
   } else {
     yyerror("semantic error (pointer root does not represent a list)");
     YYERROR;
   }
 }
/* | TIDENTIFIER TIDENTIFIER    {
 *   handleFuncCall($1, memoryGetAddresByFriendly(g_friendlies, $2));
 *   free($2);
 * } */

variable_def
: KDEF TIDENTIFIER KEQUALS pointer         { handleAssigDef($2, $4); }
/* | KDEF TIDENTIFIER KEQUALS TIDENTIFIER     { handlePointerAssig($2, $4); } */
 /* : KDEF TIDENTIFIER KEQUALS string_p        { handleAssigDef($2, $4); }
  * | KDEF TIDENTIFIER KEQUALS regex_p         { handleAssigDef($2, $4); }
  * | KDEF TIDENTIFIER KEQUALS TIDENTIFIER     { handlePointerAssig($2, $4); }
  * | KDEF TIDENTIFIER KEQUALS list_content_p  { handleAssigDef($2, $4); } */
/* | KDEF TIDENTIFIER KEQUALS list_content_p { handleAssigDef($3, $5, T_REGEX); } */


/* list_def
 * : KDEF KSTRING TIDENTIFIER KEQUALS string_list_content { handleListDef($3, $5, T_STRING); }
 * | KDEF KREGEX TIDENTIFIER KEQUALS regex_list_content { handleListDef($3, $5, T_REGEX); } */
/* list_def
 *  : KDEF KSTRING TIDENTIFIER KEQUALS list_content { handleListDef($3, $5, T_STRING); }
 *  | KDEF KREGEX  TIDENTIFIER KEQUALS list_content { handleListDef($3, $5, T_REGEX); } */

list_content_p
 : LBRACE list_content RBRACE {
   /* create mem_item for insertion into memory */
   memory_item *item = malloc(sizeof(memory_item));
   item->list = $2;

   /* insert item and keep address */
   $$ = memoryInsert(g_memory, item, t_list);
 }

/* NOTE: currently printing pointer values to suppress some bison gen warnings */
/* ==== list content ==== */

list_content
/* last item */
 : list_content pointer{
   if ($$->yy_has_last_item) {
     yyerror("semantic error: list content already contains last item");
     YYERROR;
     /* EXIT(1); */
   }
   $$->yy_has_last_item = true;
   ptrListInsert($$, $2);
   VERBOSE_PRINT("-last list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }
/* nth item */
 | list_content pointer KCOMMA {
   if ($$->yy_has_last_item) {
     yyerror("semantic error: list content already contains last item");
     YYERROR;
     /* EXIT(1); */
   }
   ptrListInsert($$, $2);
   VERBOSE_PRINT("-list item: \t%lu into (%p)", (unsigned long)$2, $$);
 }
/* first item */
 | pointer KCOMMA {
   $$ = (t_ptr_list *)malloc(sizeof(t_ptr_list));
   ptrListInit($$, 20);
   $$->yy_has_last_item = false;

   ptrListInsert($$, $1);
   VERBOSE_PRINT("-first list item: \t%lu into (%p)", (unsigned long)$1, $$);
 }
/* only item */
 | pointer {
   $$ = (t_ptr_list *)malloc(sizeof(t_ptr_list));
   ptrListInit($$, 20);
   $$->yy_has_last_item = true;

   ptrListInsert($$, $1);
   VERBOSE_PRINT("-only list item: \t%lu into (%p)", (unsigned long)$1, $$);
 }


/* --------------------------------------------------------------------------- */
int_p
: TINTEGER   {
  memory_item *item = malloc(sizeof(memory_item));
  item->integer = $1;
  $$ = memoryInsert(g_memory, item, t_int);
  VERBOSE_PRINT("integer %d in %lu"  , g_memory->content[memoryGetRootAddress(g_memory, $$)]->integer, (unsigned long)$$);
 }

str_p
: string_p
| regex_p

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


int yyerror(const char *p) {
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
  fprintf(stderr, BKRED "Error: %s : line %d:%d:%d: " BKYEL "%s\n" KDEFAULT,
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
