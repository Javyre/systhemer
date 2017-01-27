#include "unitheme.h"
#include "memory.h"
#include "priv_unitheme.h"
#include "execUnitheme.h"
#include "utils.h"
#include "builtin_funcs.h"

/* #include "lex.yy.c" */
extern void lexRunLex(const char *in);
extern FILE *yyin;
extern int yylex();
extern int yyparse();
extern int yyerror(const char *p);
extern int yylex_destroy(void);
extern size_t yyerror_count;

#include <stdlib.h>
#include <string.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

char **g_re_exps;
pcre2_code **g_re_codes;

static programDef *currentProg;
static programDefs progDefs;
/* variableList *g_variables; */
memory_holder *g_memory;
friendly_names *g_friendlies;

size_t g_num_defs;
size_t g_size_defs;
size_t g_used_defs;

char **g_def_names;
t_type *g_def_types;

char **contexts;
memory_holder **g_defs;

memory_holder *g_input;
output_holder *g_out_blocks;

void uniInit(size_t min_num_blocks, size_t max_num_blocks, size_t num_fields,
             char **field_names, t_type *field_types, int val_dyn_type) {

  g_out_blocks = (output_holder *)calloc(1, sizeof(output_holder));
  g_out_blocks->val_types = field_types;
  g_out_blocks->val_names = field_names;
  g_out_blocks->vals_size = num_fields;

  g_out_blocks->min_blocks_used = min_num_blocks;
  g_out_blocks->max_blocks_size = max_num_blocks;



  /* g_out_blocks->blocks */
  g_out_blocks->blocks_used = 0;
  g_out_blocks->blocks_size = (max_num_blocks != 0) ? max_num_blocks : 20;

  g_out_blocks->blocks = (memory_holder **)calloc(g_out_blocks->blocks_size,
                                                  sizeof(memory_holder *));
  g_out_blocks->block_names =
      (char **)calloc(g_out_blocks->blocks_size, sizeof(char *));

  /* for (size_t i=0; i<g_out_blocks->blocks_size; i++) { */

  /*   /\* g_defs[i] = (memory_holder *)malloc(sizeof(memory_holder)); *\/ */
  /*   /\* memoryInit(g_defs[i], ); *\/ */

  /* } */

}

void uniRead(const char *file_name, memory_holder *input,
             output_holder **output) {
  /* Initialize output blocks holder */
  *output = g_out_blocks;
  g_memory = (memory_holder *)malloc(sizeof(memory_holder));
  memoryInit(g_memory, 20);

  /* Set input block */
  if (input != NULL)
    g_input = input;
  else
    g_input = NULL;

  /* Initialize friendlies */
  g_friendlies = (friendly_names *)malloc(sizeof(friendly_names));
  friendlyInit(g_friendlies, 20);

  /* EXPERIMENTAL: null item and null pointer */
  memory_item *null_item = (memory_item*)malloc(sizeof(memory_item));
  null_item->str = strdup("NULL");
  memory_address null_item_address = memoryInsert(g_memory, null_item, t_str);
  friendlyInsert(g_friendlies, strdup("NULL"), null_item_address);

  /* TODO: this next line might be useless */
  defsInitArr(&progDefs, 5);

  /* open file for reading and parse until end of file
   * NOTE: each call of yyparse is each token and not line by line */
  yyin = fopen(file_name, "r");
  do {
		yyparse();
	} while (!feof(yyin));

  /* For debugging purposes, print an illustration
     of the current memory state */
  if (verboseMode) {
    printf("\n");
    memoryIllustrateMap(g_friendlies, g_memory, 999);
    printf("\n");
  }

  /* Moved frees and destroys to uniDestroy()
   * friendlyFree(g_friendlies);
   * free(g_friendlies);
   * 
   * memoryFree(g_memory);
   * free(g_memory); */

  fclose(yyin);
  yylex_destroy();

  /* Error msg if not reaching minimum required output blocks
   * NOTE: is this a runtime error? */
  if (g_out_blocks->blocks_used < g_out_blocks->min_blocks_used) {
    ERROR_PRINT("not enough output block definitions in %s", file_name);
    yyerror_count++;
  }

  /* Print num or occured error */
  VERBOSE_PRINT("%lu error(s) occured", (unsigned long)yyerror_count);
}

/* DESTROY THE EVIDENCE!
 * Frees memory allocated during reading and free unitheme var memory */
void uniDestroy() {
  VERBOSE_PRINT("DESTROYING");

  /* Destroy memory allocated during built-in function calls */
  uni_garbage_collect();

  /* Destroy input blocks */
  if (g_input != NULL)
    memoryFree(g_input);

  /* Destroy output blocks */
  for (size_t i=0; i<g_out_blocks->blocks_used;i++) {
    VERBOSE_PRINT("FREEING MEMORY BLOCK: %lu", i)
    memoryFree(outputGetBlockById(i));
    free(outputGetBlockById(i));
    g_out_blocks->blocks[i] = NULL;
    outputSetBlockById(i, +0, NULL);
    freennull(g_out_blocks->block_names[i]);
  }

  for (size_t i=0; i<g_out_blocks->vals_size;i++) {
    freennull(g_out_blocks->val_names[i]);
  }
  freennull(g_out_blocks->val_names);
  freennull(g_out_blocks->val_types);
  freennull(g_out_blocks->blocks);
  freennull(g_out_blocks->block_names);
  freennull(g_out_blocks);

  /* Destroy friendly names */
  friendlyFree(g_friendlies);
  free(g_friendlies);

  /* Destroy memory
   * TODO: implement better memory optimization
   *       than just freeing it at the end of execution */
  memoryFree(g_memory);
  free(g_memory);
}

memory_holder *outputGetBlockById(int block_num) {
  if (block_num < 0) {
    block_num += g_out_blocks->blocks_used;
  }
  return g_out_blocks->blocks[block_num];
}

void outputSetBlockById(int block_num, int offset, memory_holder *new_block) {
  if (block_num < 0) {
    block_num += g_out_blocks->blocks_used;
  }
  g_out_blocks->blocks[block_num + offset] = new_block;
}

char *outputGetBlockNameById(int block_num) {
  if (block_num < 0) {
    block_num += g_out_blocks->blocks_used;
  }
  return g_out_blocks->block_names[block_num];
}

void outputSetBlockNameById(int block_num, int offset, char *new_block_name) {
  if (block_num < 0) {
    block_num += g_out_blocks->blocks_used;
  }
  g_out_blocks->block_names[block_num + offset] = new_block_name;
}

size_t outputGetFieldIdByName(char *field_name) {
  for (size_t i=0; i < g_out_blocks->vals_size; i++)
    if (strcmp(g_out_blocks->val_names[i], field_name) == 0)
      return i;

  WARNING_PRINT("Couldn't find %s in defined field names list! (returning 0)",
                field_name);
  return 0;
}

/* Create and initialize new block */
void outputCreateMemBlock(char *block_name) {
  /* if will run out of space */
  if (g_out_blocks->blocks_size <= g_out_blocks->blocks_used+1) {

    /* if making room for one more block is allowed (under or equal to max) */
    if (g_out_blocks->blocks_size+1 <= g_out_blocks->max_blocks_size) {

      g_out_blocks->blocks = (memory_holder **)
        realloc(g_out_blocks->blocks, g_out_blocks->blocks_size+1);

      g_out_blocks->block_names = (char **)
        realloc(g_out_blocks->block_names, g_out_blocks->blocks_size+1);

      g_out_blocks->blocks_size++;
    } else {
      WARNING_PRINT("Reached maximum number of output blocks");
    }
  }

  /* NOTE: id of -1 : last item */
  /* NOTE: offset of +1 : item after last item (new item) */

  /* Initialize new block */
  outputSetBlockById(-1, +1, (memory_holder *)calloc(1, sizeof(memory_holder)));

  /* Set the name */
  outputSetBlockNameById(-1, +1, block_name);

  /* Increment num of used blocks */
  g_out_blocks->blocks_used++;
}

/* Insert field value into specified output block */
void outputInsertFieldVal(const char *dest_block_name, char * dest_field_name,
                          memory_address value) {

  /* Find the id of the block to insert to by finding the name */
  size_t block_id = 0;
  bool block_id_unset = true;
  for (size_t i=0; i<g_out_blocks->blocks_used; i++) {
    if (strcmp(outputGetBlockNameById(i), dest_block_name) == 0) {
      block_id = i;
      block_id_unset = false;
      break;
    }
    if (block_id_unset) {
      WARNING_PRINT("Couldn't find %s in defined block names list!",
                    dest_block_name);
      return;
    }
  }

  /* Set value to destination field id */
  outputGetBlockById(block_id)->
    content[outputGetFieldIdByName(dest_field_name)]->address = value;

}

/* void outputInsertMemBlock(memory_holder *block, char *block_name) { */
/*   if (g_out_blocks->blocks_size <= g_out_blocks->blocks_used+1) { */
/*     if (g_out_blocks->max_blocks_size <= g_out_blocks->blocks_size+1) { */
/*       g_out_blocks->blocks = (memory_holder **)realloc(g_out_blocks->blocks, g_out_blocks->blocks_size+1); */
/*       g_out_blocks->block_names = (char **)realloc(g_out_blocks->block_names, g_out_blocks->blocks_size+1); */

/*       g_out_blocks->blocks_size++; */
/*     } else { */
/*       WARNING_PRINT("Reached maximum number of output blocks"); */
/*     } */
/*   } */

/*   g_out_blocks->blocks[g_out_blocks->blocks_used] = block; */
/*   g_out_blocks->block_names[g_out_blocks->blocks_used] = block_name; */

/*   g_out_blocks->blocks_used++; */
/* } */

/* void unithemeInsert(memory_address addr, ) */

void runLex(const char *filename) {
  g_memory = (memory_holder *)malloc(sizeof(memory_holder));
  memoryInit(g_memory, 20);

  g_friendlies = (friendly_names *)malloc(sizeof(friendly_names));
  friendlyInit(g_friendlies, 20);

  memory_item *null_item = (memory_item*)malloc(sizeof(memory_item));
  null_item->str = strdup("NULL");
  memory_address null_item_address = memoryInsert(g_memory, null_item, t_str);
  friendlyInsert(g_friendlies, strdup("NULL"), null_item_address);

  defsInitArr(&progDefs, 5);
  /* g_variables = (variableList *)malloc(sizeof(variableList)); */
  /* varsInit(g_variables, 20); */

  yyin = fopen(filename, "r");
  do {
		yyparse();
	} while (!feof(yyin));
  /* yylex(); */

  if (verboseMode) {
    printf("\n");
    memoryIllustrateMap(g_friendlies, g_memory, 999);
    printf("\n");
  }

  fclose(yyin);
  yylex_destroy();
  /* varsFree(g_variables); */
  /* free(g_variables); */

  friendlyFree(g_friendlies);
  free(g_friendlies);

  memoryFree(g_memory);
  free(g_memory);

  if (g_out_blocks->blocks_used < g_out_blocks->min_blocks_used)
    fprintf(stderr, BKRED "Error: not enough output block definitions in %s" KDEFAULT, filename);
  VERBOSE_PRINT("%lu error(s) occured", (unsigned long)yyerror_count);
}

void handlePointerAssig(char *to_name, char *from_name) {
  memory_item *mem_item = malloc(sizeof(memory_item));
  mem_item->address = memoryGetAddresByFriendly(g_friendlies, from_name);
  if (strcmp(from_name, "NULL") != 0 && mem_item->address == 0)
    yyerror("(runtime error) identifier does not exist");


  memory_address address = memoryInsert(g_memory, mem_item, t_addr);
  friendlyInsert(g_friendlies, to_name, address);

  memoryGetRootAddress(g_memory, address);

  free(from_name);
}

void handleAssigDef(char *var_name, memory_address content) {
  memory_address address;

  /* keep parsing but dont actually handle anything if errors occure */
  /* if (yyerror_count != 0) { */
  /* /\* if (1) { *\/ */
  /*   /\* free(content); *\/ */
  /*   free(var_name); */
  /*   return; */
  /* } */

  /* create mem_item for insertion into memory */
  memory_item *mem_item = malloc(sizeof(memory_item));
  mem_item->address = content;

  /* insert item and keep address */
  address = memoryInsert(g_memory, mem_item, t_addr);
  /* if (str_type == T_STRING) */
  /*   address = memoryInsert(g_memory, mem_item, t_str); */
  /* /\* else if (str_type == T_REGEX) *\/ */
  /* else */
  /*   address = memoryInsert(g_memory, mem_item, t_rgx); */


  /* generate friendly name pointer */
  friendlyInsert(g_friendlies, var_name, address);

  /* /\* remove surrounding delimiters from strings or regexprs *\/ */
  /* utilUnstring(&content, (DELIM_TYPE)str_type); */

  /* /\* handling built-in variables is different than user-created ones *\/ */
  /* if (strcmp(var_name, "prg_name") == 0) { */
  /*   currentProg->name = content; */
  /* } else if (strcmp(var_name, "prg_path") == 0) { */
  /*   currentProg->path = content; */
  /* } else if (strcmp(var_name, "prg_exec_before") == 0) { */
  /*   currentProg->execBefore = content; */
  /* } else if (strcmp(var_name, "prg_exec_after") == 0) { */
  /*   currentProg->execAfter = content; */
  /* } else { */
  /*   varsInsertVar(g_variables, content, var_name, str_type); */
  /* } */


  /* VERBOSE_PRINT(BKYEL "variable %s is:\t %s", var_name, content); */
}

/* void handleListDef(char *list_name, t_ptr_list *content, STRING_TYPE str_type) {
 *   memory_address address;
 *   /\* keep parsing but dont actually handle anything if errors occure *\/
 *   if (yyerror_count != 0) {
 *     ptrListFree(content);
 *     free(content);
 *     free(list_name);
 *     return;
 *   }
 * 
 *   /\* create mem_item for insertion into memory *\/
 *   memory_item *mem_item = malloc(sizeof(memory_item));
 *   mem_item->list = content;
 * 
 *   /\* insert item and keep address *\/
 *   address = memoryInsert(g_memory, mem_item, t_list);
 * 
 *   /\* generate friendly name pointer *\/
 *   friendlyInsert(g_friendlies, list_name, address);
 * 
 *   /\* handling built-in variables is different than user-created ones *\/
 *   /\* if (strcmp(list_name, "tokens") == 0) { *\/
 *   /\*   currentProg->tokens = content; *\/
 *   /\* } else if (strcmp(list_name, "snippets") == 0) { *\/
 *   /\*   currentProg->snippets = content; *\/
 *   /\* } else { *\/
 *   /\*   varsInsertList(g_variables, content, list_name, str_type); *\/
 *   /\* } *\/
 * 
 *   for (size_t i = 0; i < g_memory->content[address]->list->used; i++) {
 *     VERBOSE_PRINT(BKYEL "%s %s content is: %lu %s",
 *                   (str_type == T_STRING ? "string" : str_type == T_REGEX ? "regex" : "unknown"),
 *                   memoryGetFriendlyByAddress(g_friendlies, address),
 *                   (unsigned long)g_memory->content[address]->list->pointers[i],
 *                   g_memory->content[memoryGetRootAddress(g_memory, g_memory->content[address]->list->pointers[i])]->str
 *                   );
 *   }
 * } */


void handleFuncCall(char *func_name, memory_address param_list_address) {
  /* keep parsing but dont actually handle anything if errors occur */
  if (yyerror_count != 0) {
    /* if (1) { */
    /* free(content); */
    free(func_name);
    return;
  }

  /* Built-in functions (written here in C) */
  if (strcmp(func_name, "mkblock") == 0)
    uni_mkblock(param_list_address);
  else if (strcmp(func_name, "print") == 0)
    uni_print(param_list_address);
  else if (strcmp(func_name, "set_color") == 0)
    uni_set_color(param_list_address);
  else {
    yyerror("Runtime error while running function: function does not exist");
    free(func_name);
    return;
  }

  /* User-defined functions (written in Unitheme file in uth)
   * TODO: add support for user-defined functions */
  if (1){;}

  free(func_name);
}

memory_address handleOperation(memory_address operand1, char operation, memory_address operand2) {
  if (memoryGetRootType(g_memory, operand1) !=
      memoryGetRootType(g_memory, operand2)) {
    ERROR_PRINT("Error while handling operation %c, operand types (%d and %d) "
                "did not match!",
                operation, memoryGetRootType(g_memory, operand1),
                memoryGetRootType(g_memory, operand2));
    yyerror("runtime error (operation type mismatch)");
  }

  /* memory_item *result_item = malloc(sizeof(memory_item)); */
  type_attrs ty_at;
  for (size_t i=0; i<sizeof(*g_types_attrs); i++) {
    if (g_types_attrs[i]->t_type==memoryGetRootType(g_memory, operand1)) {
      ty_at = *g_types_attrs[i];
      break;
    }
  }

  switch (operation){
  case '+':
    return ty_at.add(operand1, operand2);
    break;
  case '-':
    return ty_at.subtract(operand1, operand2);
    break;
  case '*':
    return ty_at.multiply(operand1, operand2);
    break;
  case '/':
    return ty_at.divide(operand1, operand2);
    break;
  default:
    ERROR_PRINT("operation '%c' does not exist", operation);
    yyerror_count++;
    /* EXIT(1); */
    break;
  }
  /* free(result_item); */
  return operand1;
}

memory_address handleListIndex(memory_address list, memory_address index) {
  memory_address root_list_a = memoryGetRootAddress(g_memory, list);
  memory_address root_index_a = memoryGetRootAddress(g_memory, index);
  if (memoryGetType(g_memory, root_list_a) != t_list) {
    yyerror("semantic error: pointer does not represent a list!");
    return list;
  }
  if (memoryGetType(g_memory, root_index_a) != t_int) {
    yyerror("semantic error: index in square brackets does not represent an integer!");
    return list;
  }

  memory_item *out_item = malloc(sizeof(memory_item));
  out_item->address =
    memoryGetItem(g_memory, root_list_a)
    ->list->pointers[memoryGetItem(g_memory, root_index_a)->integer];
  memory_address out = memoryInsert(g_memory, out_item, t_addr);

  VERBOSE_PRINT("value of %lu index %d is: %lu",
                list, memoryGetItem(g_memory, root_index_a)->integer,
                out_item->address);
  printf("out is: ");
  memoryIllustrateItem(g_friendlies, g_memory, out, 999, true);

  return out;
}

memory_address handleListSublist(memory_address list, memory_address from, memory_address to) {
  memory_address root_list_a = memoryGetRootAddress(g_memory, list);
  memory_item *root_list_i = memoryGetItem(g_memory, root_list_a);
  memory_address root_from_a = memoryGetRootAddress(g_memory, from);
  memory_item *root_from_i = memoryGetItem(g_memory, root_from_a);
  memory_address root_to_a = memoryGetRootAddress(g_memory, to);
  memory_item *root_to_i = memoryGetItem(g_memory, root_to_a);
  if (memoryGetType(g_memory, root_list_a) != t_list) {
    ERROR_PRINT("ERROR: trying to get sublist from non-list memory item!");
    yyerror("(semantic error) wrong type for argument 'list'");
    return list;
  }
  if (memoryGetType(g_memory, root_from_a) != t_int) {
    ERROR_PRINT("ERROR: trying to get sublist with value 'from' from non-integer memory item!");
    yyerror("(semantic error) wrong type for argument 'from'");
    return list;
  }
  if (memoryGetType(g_memory, root_to_a) != t_int) {
    ERROR_PRINT("ERROR: trying to get sublist with value 'to' from non-integer memory item!");
    yyerror("(semantic error) wrong type for argument 'to'");
    return list;
  }
  if (root_from_i->integer>(int)root_list_i->list->size) {
    ERROR_PRINT("ERROR: parameter 'from' is larger than list size");
    yyerror("(semantic error) argument 'from' out of bounds");
    return list;
  }
  if (root_to_i->integer>(int)root_list_i->list->size) {
    ERROR_PRINT("ERROR: parameter 'to' is larger than list size");
    yyerror("(semantic error) argument 'to' out of bounds");
    return list;
  }

  memory_item *sublist = malloc(sizeof(memory_item));
  sublist->list = (t_ptr_list *)malloc(sizeof(t_ptr_list));
  ptrListInit(sublist->list, 20);
  for (size_t i=(size_t)root_from_i->integer; i<=(size_t)root_to_i->integer; i++) {
    ptrListInsert(sublist->list, root_list_i->list->pointers[i]);
  }
  memory_address sublist_a = memoryInsert(g_memory, sublist, t_list);

  return sublist_a;
}

/* /\* Allocates memory for the variableList content and initializes some values *\/ */
/* void varsInit(variableList *vars, size_t initial_size) { */
/*   /\* *vars = (variableList *)calloc(1, sizeof(variableList)); *\/ */

/*   vars->list_names = (list *)malloc(sizeof(list)); */
/*   vars->var_names = (list *)malloc(sizeof(list)); */
/*   vars->vars= (list *)malloc(sizeof(list)); */

/*   defsInitList(vars->list_names, initial_size); */
/*   defsInitList(vars->var_names, initial_size); */
/*   defsInitList(vars->vars, initial_size); */

/*   vars->list_string_types = (STRING_TYPE *)calloc(initial_size, sizeof(STRING_TYPE)); */
/*   vars->var_string_types = (STRING_TYPE *)calloc(initial_size, sizeof(STRING_TYPE)); */


/*   vars->lists = (list **)calloc(initial_size, sizeof(list *)); */
/*   vars->lists_used = 0; */
/*   vars->lists_size = initial_size; */
/*   memset(&vars->lists[0], 0, sizeof(list *) * initial_size); */
/* } */

/* /\* If needed expand allocated memory to make way for new item. Then inserts item. *\/ */
/* void varsInsertList(variableList *vars, list *list, char *list_name, STRING_TYPE str_type) { */
/*   /\* resize string type array if needed *\/ */
/*   if (vars->list_names->used == vars->list_names->size-1) */
/*     vars->list_string_types = realloc(vars->list_string_types, vars->list_names->size+20); */

/*   /\* set name *\/ */
/*   defsInsertList(vars->list_names, list_name); */

/*   /\* set string type *\/ */
/*   vars->list_string_types[vars->list_names->used-1] = str_type; */

/*   /\* add list *\/ */
/*   vars->lists[vars->lists_used] = list; */
/*   vars->lists_used++; */
/* } */

/* /\* If needed expand allocated memory to make way for new item. Then inserts item. *\/ */
/* void varsInsertVar(variableList *vars, char *value, char* var_name, STRING_TYPE str_type) { */
/*   /\* resize string type array if needed *\/ */
/*   if (vars->var_names->used == vars->var_names->size-1) */
/*     vars->var_string_types = realloc(vars->var_string_types, vars->var_names->size+20); */

/*   /\* set var name *\/ */
/*   defsInsertList(vars->var_names, var_name); */

/*   /\* set string type *\/ */
/*   vars->var_string_types[vars->var_names->used-1] = str_type; */

/*   /\* add var value*\/ */
/*   defsInsertList(vars->vars, value); */
/* } */

/* /\* If found, returns the index of the list_name. If not, returns -1 *\/ */
/* int varsLookupList(variableList *vars, const char *list_name) { */
/*   /\* loop all var names until search term found and index returned *\/ */
/*   for (size_t i=0; i<vars->list_names->used; i++) { */
/*     if (strcmp(vars->list_names->items[i], list_name)) */
/*       return i; */
/*   } */

/*   /\* return -1 if search term not found *\/ */
/*   return -1; */
/* } */

/* /\* If found, returns the index of the var_name-> If not, returns -1 *\/ */
/* int varsLookupVar(variableList *vars, const char *var_name) { */
/*   /\* loop all var names until search term found and index returned *\/ */
/*   for (size_t i=0; i<vars->var_names->used; i++) { */
/*     if (strcmp(vars->var_names->items[i], var_name)) */
/*       return i; */
/*   } */

/*   /\* return -1 if search term not found *\/ */
/*   return -1; */
/* } */

/* /\* Frees all memory allocated by varsInit or modified by any of the insert functions *\/ */
/* void varsFree(variableList *vars) { */
/*   defsFreeList(vars->list_names); */
/*   free(vars->list_names); */
/*   for (size_t i=0; i<vars->lists_used; i++) { */
/*     defsFreeList(vars->lists[i]); */
/*     free(vars->lists[i]); */
/*   } */
/*   free(vars->lists); */
/*   free(vars->list_string_types); */

/*   defsFreeList(vars->var_names); */
/*   free(vars->var_names); */
/*   defsFreeList(vars->vars); */
/*   free(vars->vars); */
/*   free(vars->var_string_types); */


/*   /\* free(vars); *\/ */
/* } */




/*
  Hopefully if the conversion to flex + bison is successful
  then most of the following code will be unused and eventually deleted
*/

void loadUniTheme(const char *filename) {
  char *buff = malloc(256);
  char *tmp;
  FILE *UniThemeFile = fopen(filename, "r");
  currentLine = 0;
  initRegExpressions();
  defsInitArr(&progDefs, 5);
  currentProg = calloc(1, sizeof(programDef));
  defsInitDef(currentProg, 20);

  if (UniThemeFile == NULL) {
    fprintf(stderr, "Error: file %s not found!", filename);

    free(buff);
    defsFree(&progDefs);
    defsFreeDef(currentProg);
    free(currentProg);
    return;
  }

  while (fgets(buff, 256, UniThemeFile) != NULL) {
    currentLine++;
    /* TODO: Implement #include <> like functionality to Unitheme files */

    /* Skip empty lines */
    if (buff[0] == '\n' || buff[0] == '#') {
      continue;
    } else {
      /* Handle non-empty lines */
      getFullLine(&buff, UniThemeFile);
      strTrimStrAware(buff);
      rmEscape(&buff);
      evalLine(buff);
    }

    tmp = realloc(buff, 256);
    if (tmp == NULL) {
      fprintf(stderr, BKRED "Error: Failed to realocate memory for the buff variable in the unitheme reader!" KDEFAULT);
      EXIT(1);
    } else {
      buff = tmp;
      tmp = NULL;
    }
  }
  free(buff);
  fclose(UniThemeFile);
  destroyRegExpressions();
  /* defsFree(&progDefs); */

  /* No need to free currentProg because it will be freed by defsFree */
  /* defsFreeDef(currentProg); */
  /* free(currentProg); */
}

void runExecUitheme() {
  execUnitheme(&progDefs);
  freeProgs();
}

void freeProgs() {
  defsFree(&progDefs);
  free(currentProg);
}

/* This fucntion compiles the regular expressions at the beggining */
/* of the program so that it isnt done multiple times at runtime */
/*
  Mega expression for detecting valid def list:
  https://regex101.com/r/kY5iB8/2
  https://regex101.com/r/kY5iB8/3
  https://regex101.com/r/kY5iB8/4
  https://regex101.com/r/kY5iB8/5

  def\s+(string)\s+\S+(?<s>\s*\n*\s*)\{\g<s>((?<str>("((\\")|[^"(\\")])+"|""))\g<s>,\g<s>)*(\g<str>\g<s>)?\};?
  def\s+(regex)\s+\S+(?<s>\s*\n*\s*)\{\g<s>((?<str>(\/((\\\/)|[^\/(\\\/)])+\/|\/\/))\g<s>,\g<s>)*(\g<str>\g<s>)?\};?

  Universal (detects valid )
  (def\s+(regex)\s+\S+(?<s>\s*\n*\s*)\{\g<s>((?<reg>(\/((\\\/)|[^\/(\\\/)])+\/|\/\/))\g<s>,\g<s>)*(\g<reg>\g<s>)?\};?)|(def\s+(string)\s+\S+\g<s>\{\g<s>((?<str>("((\\")|[^"(\\")])+"|""))\g<s>,\g<s>)*(\g<str>\g<s>)?\};?)
*/
void initRegExpressions() {
  int re_err_code = 0;
  size_t re_err_offset = 0;
  PCRE2_UCHAR *re_err_msg = NULL;

  g_re_exps = calloc(g_re_exps_size, sizeof(char *));

  g_re_exp_is_def_string_list =
    strMkCpy("def\\s+" DEF_STRING_KEYWORD "\\s+(\\S+)(?<s>[\\s\\n]*)\\{\\g<s>((?<str>" RE_STRING_LITTERAL ")\\g<s>,\\g<s>)*(\\g<str>\\g<s>)?\\};?");
  /* free(g_re_exp_is_def_regex_list); */

  g_re_exp_is_def_regex_list =
    strMkCpy("def\\s+" DEF_REGEX_KEYWORD "\\s+(\\S+)(?<s>[\\s\\n]*)\\{\\g<s>((?<reg>" RE_REGEX_LITTERAL ")\\g<s>,\\g<s>)*(\\g<reg>\\g<s>)?\\};?");

  g_re_exp_is_def_string_assig =
    strMkCpy("^\\s*\\n*\\s*(?<s>[\\s\\n]*)string\\g<s>\\S+\\g<s>=\\g<s>" RE_STRING_LITTERAL "\\g<s>;");

  g_re_exp_is_def_regex_assig =
    strMkCpy("^\\s*\\n*\\s*(?<s>[\\s\\n]*)regex\\g<s>\\S+\\g<s>=\\g<s>" RE_REGEX_LITTERAL "\\g<s>;");

  g_re_codes = calloc(g_re_codes_size, sizeof(pcre2_code *));

  for (size_t i=0; i<g_re_exps_size&&i<g_re_codes_size; i++) {
    g_re_codes[i] = pcre2_compile((PCRE2_SPTR)g_re_exps[i], PCRE2_ZERO_TERMINATED, 0, &re_err_code, &re_err_offset, NULL);
    if (g_re_codes[i] == NULL) {
      fprintf(stderr, BKRED "REGEX COMPILATION FAILED!!\n" KDEFAULT);
      re_err_msg = calloc(256, sizeof(PCRE2_UCHAR));
      pcre2_get_error_message(re_err_code, re_err_msg, 256);
      fprintf(stderr, BKRED "%s\n" KDEFAULT, re_err_msg);
      free(re_err_msg);
    }
  }

}

/* This function free the expressions compiled by initRegExpressions() */
void destroyRegExpressions() {
    for (size_t i=0; i<g_re_exps_size; i++)
      free(g_re_exps[i]);
    free(g_re_exps);
    g_re_exps = NULL;

    for (size_t i=0; i<g_re_codes_size; i++)
      pcre2_code_free(g_re_codes[i]);
    free(g_re_codes);
    g_re_codes= NULL;
}

//--------------------------------------------------------------------
void defsInitDef(const programDef *def, const size_t initial_tok_size) {
  /* def->tokens = (list *)calloc(1, sizeof(list *)); */
  /* def->tokens.items = (char **)calloc(initial_tok_size, sizeof(char *)); */
  /* def->tokens.used = 0; */
  /* def->tokens.size = initial_tok_size; */
  /* memset(&def->tokens.items[0], 0, sizeof(char *) * initial_tok_size); */

  /* /\* def->snippets = (list *)calloc(1, sizeof(list *)); *\/ */
  /* def->snippets.items = (char **)calloc(initial_tok_size, sizeof(char *)); */
  /* def->snippets.used = 0; */
  /* def->snippets.size = initial_tok_size; */
  /* memset(&def->snippets.items[0], 0, sizeof(char *) * initial_tok_size); */


}


void defsInitList(list *listp, size_t inital_list_size) {
  /* listp = (list *)calloc(1, sizeof(list)); */
  listp->items = (char **)calloc(inital_list_size, sizeof(char *));
  listp->used = 0;
  listp->size = inital_list_size;
  memset(&listp->items[0], 0, sizeof(char *) * inital_list_size);

}

/* DO NOT FREE element AFTER INSERTING */
void defsInsertList(list *list, char *element) {
  /* resize list allocated size if needed */
  if (list->used == list->size) {
    list->size *= 2;
    list->items = (char **)realloc(list->items, list->size * sizeof(char *));
    // Initialize the last/new elements of the reallocated progs
    for (size_t i = list->used; i < list->size; i++) {
      memset(&list->items[i], 0, sizeof(char *));
    }
  }

  /* insert the item */
  list->items[list->used] = element;
  list->used++;
}

void defsFreeList(list *list) {
  for (size_t i = 0; i < list->used; i++) {
    free(list->items[i]);
    list->items[i] = NULL;
  }
  free(list->items);
  list->items = NULL;
  list->used = 0;
  list->size = 0;
}

void defsInitArr(programDefs *a, size_t initialSize) {
  // Allocate initial space
  a->progs = (programDef **)calloc(initialSize, sizeof(programDef *));

  a->used = 0;           // no elements used
  a->size = initialSize; // available nr of elements

  // Initialize all values of the array to 0
  for (size_t i = 0; i < initialSize; i++) {
    memset(&a->progs[i], 0, sizeof(programDef *));
  }


}

// Add element to array
void defsInsert(programDefs *a, programDef *element) {
  if (a->used == a->size) {
    a->size *= 2;
    a->progs = (programDef **)realloc(a->progs, a->size * sizeof(programDef *));
    // Initialize the last/new elements of the reallocated progs
    for (size_t i = a->used; i < a->size; i++) {
      memset(&a->progs[i], 0, sizeof(programDef *));
    }
  }

  a->progs[a->used] = element;
  /* a->progs[a->used] = calloc(1, sizeof(programDef)); */

  /* a->progs[a->used]->name = element->name; */
  /* a->progs[a->used]->tokens = element->tokens; */
  /* a->progs[a->used]->path = element->path; */
  /* a->progs[a->used]->execBefore = element->execBefore; */
  /* a->progs[a->used]->execAfter = element->execAfter; */

  a->used++;
}

void defsFree(programDefs *a) {
  // Free all name variables of each array element first
  for (size_t i = 0; i < a->used; i++) {
    VERBOSE_PRINT_VALUE(%s, a->progs[i]->name);
    VERBOSE_PRINT_VALUE(%p, a->progs[i]->name);

    if (a->progs[i]->tokens != NULL) {
      VERBOSE_PRINT_VALUE(%d, (int)a->progs[i]->tokens->used);
      for (size_t ii = 0; ii < a->progs[i]->tokens->used; ii++)
        VERBOSE_PRINT_VALUE(%s, a->progs[i]->tokens->items[ii]);
    } else {
      VERBOSE_PRINT_VALUE(%p, a->progs[i]->tokens);
    }

    if (a->progs[i]->snippets != NULL) {
      VERBOSE_PRINT_VALUE(%d, (int)a->progs[i]->snippets->used);
      for (size_t ii = 0; ii < a->progs[i]->snippets->used; ii++)
        VERBOSE_PRINT_VALUE(%s, a->progs[i]->snippets->items[ii]);
    } else {
      VERBOSE_PRINT_VALUE(%p, a->progs[i]->snippets);
    }

    VERBOSE_PRINT_VALUE(%s, a->progs[i]->path);
    VERBOSE_PRINT_VALUE(%s, a->progs[i]->execBefore);
    VERBOSE_PRINT_VALUE(%s, a->progs[i]->execAfter);
    if (verboseMode)
      printf("=================================\n");

    free(a->progs[i]->name);
    if (a->progs[i]->tokens != NULL) {
      defsFreeList(a->progs[i]->tokens);
      free(a->progs[i]->tokens);
    }
    if (a->progs[i]->snippets != NULL) {
      defsFreeList(a->progs[i]->snippets);
      free(a->progs[i]->snippets);
    }
    free(a->progs[i]->path);
    free(a->progs[i]->execBefore);
    free(a->progs[i]->execAfter);
    a->progs[i]->name = NULL;
    a->progs[i]->tokens = NULL;
    a->progs[i]->snippets = NULL;
    a->progs[i]->path = NULL;
    a->progs[i]->execBefore = NULL;
    a->progs[i]->execAfter = NULL;

    free(a->progs[i]->beginDef);
    free(a->progs[i]->endDef);
    a->progs[i]->beginDef = NULL;
    a->progs[i]->endDef = NULL;
    /* a->progs[1]->beginDef=0; */
    /* a->progs[1]->endDef=0; */
    /* free(a->progs[i]->toklen); */
    /* a->progs[i]->toklen= NULL; */

    free(a->progs[i]);
  }

  // Now free the array
  free(a->progs);
  a->progs = NULL;

  a->used = 0;
  a->size = 0;
}

void defsFreeDef(programDef* prg) {
  free(prg->name);
  /* free(prg->tokens); */
  /* free(prg->snippets); */
  free(prg->path);
  free(prg->execBefore);
  free(prg->execAfter);
  prg->name = NULL;
  /* prg->tokens = NULL; */
  /* prg->snippets = NULL; */
  prg->path = NULL;
  prg->execBefore = NULL;
  prg->execAfter = NULL;

  free(prg->beginDef);
  free(prg->endDef);
  prg->beginDef = NULL;
  prg->endDef = NULL;
}
//--------------------------------------------------------------------

void rmComment(char *in) {
  char *src = in;
  while (*src != '\0' && *src != '\n') {
    if (*src == '#' && *(src - 1) != '\\')
      *src = '\0';
    src++;
  }
}

void rmEscape(char **currentBuffer) { /* Apply's only to everything outside of strings and regex's */
  char *original = malloc(strlen(*currentBuffer) + 1);
  strcpy(original, *currentBuffer);
  char *src = original;
  while (*src != '\0') {
    if (!isInsideOfStr(original, src) && !isInsideOfRegEx(original, src)) {
      if (*src == '\\' && (src[1] == '\\' || src[1] == '#')) {
        strOverlap(original, original, (src - 1), (src + 1), NULL);
        src++;
      } else if (*src == '\\' && (src[1] != '\\' && src[1] != '#')) {
        fprintf(stderr, BKRED "Error: Found escape character outside of string "
                              "with invalid successor \"%c\": \n\t%s\n\t%s\n",
                src[1], original, genWrongUnderline(original, src, src + 1));
        EXIT(1);
      }
    }
    src++;
  }

  *currentBuffer = realloc(*currentBuffer, strlen(original) + 1);
  strcpy(*currentBuffer, original);
  free(original);
}

bool hasLineExtension(char **currentBuffer) {
  char *src = *currentBuffer;
  while (*src != '\0')
    src++;

  if (*(src - 1) == ';' || (*(src - 2) == ';' && *(src - 1) == '\n')) {
    src = *currentBuffer;
    while (*src != ';')
      src++;
    *(src + 1) = '\0';
    *currentBuffer = realloc(*currentBuffer, strlen(*currentBuffer) + 1);
    return false;
  }

  return true;
}

void getFullLine(char **currentBuffer, FILE *UniThemeFile) {
  // char *holder = malloc(strlen(currentBuffer) + 1);
  char *holder = calloc(256, sizeof(char));
  char *tmp = NULL;
  // char *nextLine = malloc(256);
  strcpy(holder, *currentBuffer);
  rmComment(holder);
  while (hasLineExtension(&holder)) {
    currentLine++;
    // line+=new;
    fgets(*currentBuffer, 256, UniThemeFile);
    tmp = realloc(holder, strlen(holder) + 1 + strlen(*currentBuffer) + 1);
    if (tmp == NULL) {
      fprintf(stderr, "Error: reallocation failed in %s at line %d", __FILE__, __LINE__);
    } else {
      holder = tmp;
      tmp = NULL;
    }
    strcat(holder, " ");
    strcat(holder, *currentBuffer);
    rmComment(holder);
  }
  *currentBuffer = realloc(*currentBuffer, strlen(holder) + 1);
  strcpy(*currentBuffer, holder);

  // free(nextLine);
  if (tmp != NULL)
    free(tmp);
  free(holder);
}

/* Returns the datatype of the def as well as running the compiled rexeprs and returning T_NULL if didnt match anything */
STRING_TYPE getDefType(char *in, pcre2_code *re_code_is_def_string, pcre2_code *re_code_is_def_regex) {
  STRING_TYPE str_type = T_NULL;

  int re_err_code = 0;
  PCRE2_UCHAR *re_err_msg = NULL;

  pcre2_match_data *re_match_data = NULL;

  re_match_data = pcre2_match_data_create_from_pattern(re_code_is_def_string, NULL); /* Create match data */
  re_err_code = pcre2_match(re_code_is_def_string,                                   /* Test for match */
                            (PCRE2_SPTR)in,
                            strlen(in),
                            0,
                            0,
                            re_match_data,
                            NULL);
  pcre2_match_data_free(re_match_data);                                          /* Dispose of the match data right away */
  if (re_err_code <= 0 && re_err_code != -1){                                    /* Error code -1 means just no match */
    VERBOSE_PRINT_VALUE(%d, re_err_code);
    re_err_msg = malloc(256);
    pcre2_get_error_message(re_err_code, re_err_msg, 256);
    fprintf(stderr, BKRED "%s\n" KDEFAULT, re_err_msg);
    free(re_err_msg);
  } else if (re_err_code > 0) {
    WARNING_PRINT("VALID STRING DEF");
    str_type = T_STRING;
  } else if (re_err_code == -1) {
    re_match_data = pcre2_match_data_create_from_pattern(re_code_is_def_regex, NULL); /* Create match data */
    re_err_code = pcre2_match(re_code_is_def_regex,                                   /* Test for match */
                              (PCRE2_SPTR)in,
                              strlen(in),
                              0,
                              0,
                              re_match_data,
                              NULL);
    pcre2_match_data_free(re_match_data);                                          /* Dispose of the match data right away */
    if (re_err_code <= 0 && re_err_code != -1){                                    /* Error code -1 means just no match */
      VERBOSE_PRINT_VALUE(%d, re_err_code);
      re_err_msg = malloc(256);
      pcre2_get_error_message(re_err_code, re_err_msg, 256);
      fprintf(stderr, BKRED "%s\n" KDEFAULT, re_err_msg);
      free(re_err_msg);
    } else if (re_err_code > 0) {
      WARNING_PRINT("VALID REGEX DEF");
      str_type = T_REGEX;
    } else if (re_err_code == -1) {
      /* WARNING_PRINT("INVALID DEF"); */
      return T_NULL;
    }
  }

  return str_type;
}

void getListAttr(char *in, char **outListName, list **outListItems, STRING_TYPE str_type) {
  char *begin_item = NULL;
  char *end_item = NULL;
  char *src = NULL;
  char *tmpc = NULL;
  char *tok = NULL;
  char *k_word;

  /* ==== Set type keyword ==== */
  switch (str_type) {
  case T_STRING:
    k_word = strMkCpy(DEF_STRING_KEYWORD);
    break;
  case T_REGEX:
    k_word = strMkCpy(DEF_REGEX_KEYWORD);
    break;
  default:
    k_word = NULL;
    fprintf(stderr, BKRED "Error: Unknown STRING_TYPE: %d passed to getListAttr!", str_type);
    EXIT(1);
    break;
  }

  /* ==== Get list name ==== */
  tmpc = strMkCpy(in);
  tok = strtok(tmpc, " \t\n");
  while (tok) {
    if (strcmp(tok, "def") && strcmp(tok, k_word)) {
      *outListName = strMkCpyInRange(tok, ((tok[strlen(tok)-1] == '{') ? strlen(tok)-1 : 0));
      VERBOSE_PRINT_VALUE(%s, *outListName);
      break;
    }
    tok = strtok(NULL, " \t\n");
  }
  free(tmpc);
  tmpc = NULL;
  tok = NULL;

  /* ==== Get list items ==== */
  /* Init list */
  *outListItems = (list *)calloc(1, sizeof(list));
  defsInitList(*outListItems, 20);

  /* Skip to char after opening { char */
  src = in;
  for (;*src!='{';src++);
  src++;

  /* Find and insert items one by one */
  VERBOSE_PRINT_VALUE(%s, src);
  if (str_type == T_STRING) {
    while (getNextStr(src, &begin_item, &end_item)) {
      defsInsertList(*outListItems, strMkCpyInRange(begin_item, end_item-begin_item+1));
      src = end_item+1;

      VERBOSE_PRINT_VALUE(%s, (*outListItems)->items[(*outListItems)->used-1]);
  }
  } else if (str_type == T_REGEX) {
    while (getNextRegEx(src, &begin_item, &end_item)) {
      defsInsertList(*outListItems, strMkCpyInRange(begin_item, end_item-begin_item+1));
      src = end_item+1;

      VERBOSE_PRINT_VALUE(%s, (*outListItems)->items[(*outListItems)->used-1]);
    }

  }

  free(k_word);
  k_word = NULL;
}

bool isList(char *in, char **outListName, list **outListItems, STRING_TYPE *str_type) {

  *str_type = getDefType(in, g_re_code_is_def_string_list, g_re_code_is_def_regex_list);

  if (*str_type != T_NULL) {
    getListAttr(in, outListName, outListItems, *str_type);
    return true;
  } else {
    return false;
  }
}

void getAssigAttr(char *in, char **outTok, char **outValue, STRING_TYPE str_type) {
  char *tmpc = NULL;
  char *tok = NULL;
  char *k_word;

  char *begin_val = NULL;
  char *end_val = NULL;

  /* ==== Set type keyword ==== */
  switch (str_type) {
  case T_STRING:
    k_word = strMkCpy(DEF_STRING_KEYWORD);
    break;
  case T_REGEX:
    k_word = strMkCpy(DEF_REGEX_KEYWORD);
    break;
  default:
    k_word = NULL;
    fprintf(stderr, BKRED "Error: Unknown STRING_TYPE: %d passed to getListAttr!", str_type);
    EXIT(1);
    break;
  }

  /* ==== Get assig name ==== */
  tmpc = strMkCpy(in);
  tok = strtok(tmpc, " \t\n");
  while (tok) {
    /* if (strcmp(tok, "def") && strcmp(tok, k_word)) { */
    if (strcmp(tok, k_word)) {
      *outTok = strMkCpyInRange(tok, ((tok[strlen(tok)-1] == '{') ? strlen(tok)-1 : 0));
      VERBOSE_PRINT_VALUE(%s, *outTok);
      break;
    }
    tok = strtok(NULL, " =\t\n");
  }
  free(tmpc);
  tmpc = NULL;
  tok = NULL;

  /* ==== Get assig value ==== */
  if (str_type == T_STRING)
    getNextStr(in, &begin_val, &end_val);
  else if (str_type == T_REGEX)
    getNextRegEx(in, &begin_val, &end_val);
  *outValue = strMkCpyInRange(begin_val, end_val-begin_val+1);

  free(k_word);
  k_word = NULL;
}

bool isAssignation(char *in, char **outTok, char **outValue, STRING_TYPE* str_type) {
  *str_type = getDefType(in, g_re_code_is_def_string_assig, g_re_code_is_def_regex_assig);

  if (*str_type != T_NULL) {
    getAssigAttr(in, outTok, outValue, *str_type);
    return true;
  } else {
    return false;
  }
}

bool isStatement(char *in, char **outCall, char **outArg) {
  char *copyIn = strMkCpyInRange(in, strlen(in) - 1);
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
  if (words != 2) {
    free(*outCall);
    *outCall = NULL;
    free(*outArg);
    *outArg = NULL;
    return false;
  }
  for (size_t i = 0; i < strlen(in); i++) {
    if (in[i] == '=' && !isInsideOfStr(in, in+i)) {
      free(*outCall);
      *outCall = NULL;
      free(*outArg);
      *outArg = NULL;
      return false;
    }
  }

  strTrimStrAware(*outCall);
  /* strTrimStrAware(*outArg); */

  return true;
}

void evalLine(char *currentBuffer) {
  char *src = currentBuffer;
  char *statCall = NULL;
  char *statArg = NULL;
  char *assigTok = NULL;
  char *assigValue = NULL;
  STRING_TYPE assig_type = T_NULL;
  char *listName = NULL;
  list *listItems = NULL;
  STRING_TYPE list_item_type = T_NULL;
  while (*src == ' ' || *src == '\t')
    src++;
  VERBOSE_PRINT("Evaluating line bufer...");
  VERBOSE_PRINT_VALUE(%s, src);

  if (isStatement(currentBuffer, &statCall, &statArg)) {
    printf("ITS A STATEMENT!!!\n");
    /*evalStatement*/;
    evalStatement(currentBuffer, statCall, statArg);
    statCall = NULL;
    statArg = NULL;
  } else if (isAssignation(currentBuffer, &assigTok, &assigValue, &assig_type)) {
    printf("ITS AN ASSIGNATION!!!\n");
    /* evalAssignation; */
    evalAssig(currentBuffer, assigTok, assigValue, assig_type);
    assigTok = NULL;
    assigValue = NULL;
  } else if (isList(currentBuffer, &listName, &listItems, &list_item_type)) {
    printf("ITS A LIST!!! ###%s###%d###\n", listName, (int)listItems->used);
    /*evalList*/;
    evalList(currentBuffer, listName, listItems, list_item_type);
    listName = NULL;
    listItems = NULL;
  } else {
    fprintf(stderr, BKRED "Error: Could not recognize (line #%d): %s",
            (int)currentLine, currentBuffer);
    exit(1);
  }

  if (verboseMode)
    printf("=====================================\n");
}

void evalStatement(char *currentBuffer, char *statCall, char *statArg) {
  bool freeArg = false;
  strUnstring(&statArg);
  if (strcmp(statCall, "begindef") == 0) {
    if (currentProg->name == NULL) {
      currentProg->beginDef = calloc(1, sizeof(size_t));
      *currentProg->beginDef = currentLine;
      currentProg->name = statArg;
    } else {
      fprintf(stderr,
              BKRED "Error: Called begindef twice! : check line #%d\n" KDEFAULT,
              (int)currentLine);
      exit(1);
    }
  } else if (strcmp(statCall, "enddef") == 0) {
    VERBOSE_PRINT("ENDING DEF SECTION");
    /* store program detail of struct somehow */
    currentProg->endDef = calloc(1, sizeof(size_t));
    *currentProg->endDef = currentLine;
    defsInsert(&progDefs, currentProg);
    /* free(currentProg); */
    currentProg = NULL;

    currentProg = calloc(1, sizeof(programDef));
    defsInitDef(currentProg, 20);
    currentProg->name = NULL;
    freeArg = true;
  } else {
    fprintf(stderr,
            BKRED "Error: Called undefined function %s! : check line #%d\n" KDEFAULT,
            statCall, (int)currentLine);
    exit(1);

    /* nothing matched */
    freeArg  = true;
  }
  printf("%s\n", statCall);
  printf("%s\n", statArg);
  free(statCall);
  if (freeArg)
    free(statArg);
  VERBOSE_PRINT_VALUE(%p, statArg);
}

void evalAssig(char *currentBuffer, char *tok, char *value, STRING_TYPE str_type) {
  bool freeVal = false;
  strTrim(tok);
  strTrimStrAware(value);
  strUnstring(&value);
  if (currentProg->name != NULL) {
    if (strcmp(tok, "path") == 0) {
      printf("FOUND PATH ASSIGNATION!!!\n");
      currentProg->path = value;
    } else if (strcmp(tok, "exec-before") == 0) {
      currentProg->execBefore = value;
    } else if (strcmp(tok, "exec-after") == 0) {
      currentProg->execAfter = value;
    } else {
    /* TODO: Add extra else if for user declared variables */
      fprintf(stderr,
              BKRED "Error: Assigned value to undefined variable %s! : check line #%d\n" KDEFAULT,
              tok, (int)currentLine);
      exit(1);

      freeVal = true;
    }
  } else {
    fprintf(stderr,
            BKRED "Error: Detected assignation outside of programdef scope! : check line #%d\n" KDEFAULT,
            (int)currentLine);
    exit(1);
  }

  free(tok);
  if (freeVal)
    free(value);
}

void evalList(char *currentBuffer, char *listName, list *content, STRING_TYPE str_type) {
  if (currentProg->name != NULL) {
    if (str_type == T_REGEX) { /* Automaticaly treat as regular expressions and not strings (delimiters: // and different escape chars) */
      for (size_t i = 0; i < content->used; i++) {
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
        strTrimStrAware(content->items[i]); //TODO: Replace this line with appropriate regex equivalent
        regexUnregex(&content->items[i]); //DONE: Replace this line with appropriate regex equivalent
        strRealloc(&content->items[i]);
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
      }
      currentProg->tokens = content;
    } else if (str_type == T_STRING) { /* Automatically treat as strings (delimiters: "" and different escape chars) */
      for (size_t i = 0; i < content->used; i++) {
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
        strTrimStrAware(content->items[i]);
        strUnstring(&content->items[i]);
        strRealloc(&content->items[i]);
        VERBOSE_PRINT_VALUE(%s, content->items[i]);
      }
      currentProg->snippets = content;
    } else {
      fprintf(stderr,
              BKRED "Error: Assigned value to undefined list %s! : check line #%d\n" KDEFAULT,
              listName, (int)currentLine);
      exit(1);
    }
  } else {
    fprintf(stderr,
            BKRED "Error: Detected list definition outside of programdef scope! : check line #%d\n" KDEFAULT,
            (int)currentLine);
    exit(1);
  }

  free(listName);
  //defsFreeList(content);
}
