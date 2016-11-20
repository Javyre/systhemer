#ifndef UNITHEME_H
#define UNITHEME_H

#include <stdbool.h>
#include <stdio.h>
#include "utils.h"
#include "memory.h"

typedef char *uni_string;
typedef char *uni_regexp;


typedef struct
{
  char **items;
  size_t used;
  size_t size;
} list;

typedef struct
{
  char *name;
  list *tokens;
  list *snippets;
  char *path;
  char *execBefore;
  size_t *beginDef;
  size_t *endDef;
  char *execAfter;
} programDef;
/* programDef *currentProg; */

typedef struct
{
  programDef **progs;
  size_t used;
  size_t size;
} programDefs;
/* programDefs progDefs; */
/*
typedef struct
{
  list *list_names;
  list *var_names;
  list **lists;
  STRING_TYPE *list_string_types;
  STRING_TYPE *var_string_types;
  size_t lists_used;
  size_t lists_size;
  list *vars;
} variableList;
 */

enum {
  /* num_b_type */
  /*
    used in uniInit for setting the type of number set for how many wanted blocks for the output :
        maximum number of blocks: (declares the number set as
                                  the maximum number of blocks)

        number of blocks:         (declares the number set as
                                  the number of blocks)

        minimum number of blocks: (declares the number set as
                                  the minimum number of blocks)
   */
  UNI_MAX_NUM_OUT_BLOCKS,
  UNI_NUM_OUT_BLOCKS,
  UNI_MIN_NUM_OUT_BLUCKS,


  /* val_dyn_type */
  /*
    used in uniInit for setting wether the values in the output blocks should be :
        dynamic:  (can change as it reads the file)

        constant: (can not change and are duplicate
                  constants of the value at the time
                  it was scanned)
   */
  UNI_DYNAMIC_OUT_VALS,
  UNI_CONSTANT_OUT_VALS,
};

/* ---------------------------------------------------------------------- */

/* extern variableList *g_variables; */

extern memory_holder *g_memory;

extern friendly_names *g_friendlies;

/* void varsInit(variableList *vars, size_t initial_size); */

/* void varsInsertList(variableList *vars, list *list, char* list_name, STRING_TYPE str_type); */

/* void varsInsertVar(variableList *vars, char *value, char* var_name, STRING_TYPE str_type); */

/* int varsLookupList(variableList *vars, const char *list_name); */

/* int varsLookupVar(variableList *vars, const char *var_name); */

/* void varsFree(variableList *vars); */

/* ---------------------------------------------------------------------- */

/*
  block_names: names of each output block (memory_holder) (defined at .uth file runtime)
  val_types: value t_types for each value in a block (defined by api user)
  val_names: value names for each value in a block (defined by api user)

  blocks: actual array of output blocks. These blocks are each a memory_holder
          but can only contain address type memory_items (defined at .uth file runtime)

  blocks_size: to keep track of the size of the array of output blocks (internal - defined at
               .uth file runtime if number of blocks is set as flexible)

  blocks_used: to keep track of the number of used blocks in the array (internal - dynamic and
               changes during .uth runtime)

  max_blocks_size: maximum number of allowed output blocks
  min_blocks_used: minimum number of allowed output blocks
  NOTE: if max and min are the same, than the number of allowed output blocks is not flexible

 */
typedef struct {
  char **block_names;
  t_type *val_types;
  char **val_names;
  size_t vals_size;

  memory_holder **blocks;
  size_t blocks_size;
  size_t blocks_used;
  size_t max_blocks_size;
  size_t min_blocks_used;
} output_holder;

/* ---------------------------------------------------------------------- */

/* void uniInit(size_t num_blocks, char **field_names, t_type *flied_types, PARAM_HELPER num_b_type, PARAM_HELPER val_dyn_type); */
/* void uniInit(size_t min_num_blocks, size_t max_num_blocks, size_t num_fields, char **field_names, t_type *field_types, PARAM_HELPER val_dyn_type); */
void uniInit(size_t min_num_blocks, size_t max_num_blocks, size_t num_fields,
             char **field_names, t_type *field_types, int val_dyn_type);

void uniRead(const char *file_name, memory_holder *input, output_holder **output);

void uniDestroy();

/* ---------------------------------------------------------------------- */

memory_holder *outputGetBlockById(int block_num);

void outputSetBlockById(int block_num, int offset, memory_holder *new_block);

char *outputGetBlockNameById(int block_num);

void outputSetBlockNameById(int block_num, int offset, char *new_block_name);

size_t outputGetFieldIdByName(char *field_name);

void outputCreateMemBlock(char *block_name);

void outputInsertFieldVal(const char *dest_block_name, char * dest_field_name,
                          memory_address value);

/* ---------------------------------------------------------------------- */

void runLex(const char *filename);

void handlePointerAssig(char *to_name, char *from_name);

void handleAssigDef(char *var_name, memory_address content, STRING_TYPE str_type);

void handleListDef(char *list_name, t_ptr_list *content, STRING_TYPE str_type);

/* ---------------------------------------------------------------------- */

void loadUniTheme(const char *filename);

void runExecUitheme();

void freeProgs();

void defsInitDef(const programDef *def, const size_t initial_tok_size);

void defsInitArr(programDefs *a, size_t initialSize);

void defsInitList(list *list, size_t inital_list_size);

void defsInsert(programDefs *a, programDef *element);

void defsInsertList(list *list, char *element);

void defsFree(programDefs *a);

void defsFreeDef(programDef* prg);

void defsFreeList(list *list);

#endif
