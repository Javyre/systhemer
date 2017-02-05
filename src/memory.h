#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
/* #include "unitheme.h" */

typedef size_t memory_address;

/* type declarations */
typedef enum {
  t_int,
  t_str,
  t_rgx,
  t_list,
  t_addr,
} t_type;

typedef struct {
  char **identifiers;
  memory_address *addresses;
  size_t used;
  size_t size;
} friendly_names;

/* array of pointers */
typedef struct {
  memory_address *pointers;
  size_t used;
  size_t size;

  bool yy_has_last_item; /* used in the parser (ignore this variable) */
} t_ptr_list;

typedef union {
  int integer;
  t_ptr_list *list;
  char *str;
  memory_address address;
} memory_item;

typedef struct {
  memory_item **content;
  t_type *content_type;
  size_t used;
  size_t size;
} memory_holder;



typedef memory_address (*mem_operation_func) (const memory_address a, const memory_address b);
memory_address mem_operation_func_unsupported(const memory_address a, const memory_address b);

typedef struct {
  const char esc_char;      /* would be '\\' */
  /* const char *escaped[205];   /\* would be "n" for the newline character ("\\n") *\/
   * const char *unescaped[5]; /\* would be the actual newline character          *\/ */
  const char *escaped[26];   /* would be "n" for the newline character ("\\n") */
  const char *unescaped[26]; /* would be the actual newline character          */
  const size_t num_escs;
  const char delim_char;
  const bool allow_unrecognized_escs;
  mem_operation_func add;
  mem_operation_func subtract;
  mem_operation_func multiply;
  mem_operation_func divide;
  mem_operation_func negate;
  /* memory_address (*add)(memory_address a, memory_address b); */
  /* memory_address (*subtract)(memory_address a, memory_address b); */
  /* memory_address (*multiply)(memory_address a, memory_address b); */
  /* memory_address (*divide)(memory_address a, memory_address b); */

  bool is_string_type;
  STRING_TYPE s_type;
  t_type t_type;
} type_attrs;

/* #include "uni_string.h" */
/* #include "uni_regex.h" */
/* #include "uni_int.h" */
/* extern type_attrs g_string_attrs; */
/* extern type_attrs g_regex_attrs; */
/* extern type_attrs g_int_attrs; */

extern type_attrs *g_types_attrs[];


void memoryInit(memory_holder *mem, size_t initial_size);

memory_address memoryInsert(memory_holder *mem, memory_item *item, t_type ptr_type);

void memoryFree(memory_holder *mem);

memory_item *memoryGetItem(memory_holder *mem, memory_address addr);

memory_item *memoryGetRootItem(memory_holder *mem, memory_address addr);

t_type memoryGetType(memory_holder *mem, memory_address addr);

t_type memoryGetRootType(memory_holder *mem, memory_address addr);

memory_address memoryGetRootAddress(memory_holder *mem, memory_address mem_addr);


void memoryIllustrateMap(friendly_names *friendly, memory_holder *mem, size_t depth);

void memoryIllustrateItem(friendly_names *friendly, memory_holder *mem, memory_address i, size_t depth, bool do_newline);

memory_address memoryGetAddresByFriendly(friendly_names *friendly, char *identifier);

char *memoryGetFriendlyByAddress_m(friendly_names *friendly, memory_address address, bool suppress_errors);
char *memoryGetFriendlyByAddress(friendly_names *friendly, memory_address address);
char *memoryGetFriendlyByAddress_se(friendly_names *friendly, memory_address address);

void ptrListInit(t_ptr_list *ptr_list, size_t initial_size);

void ptrListInsert(t_ptr_list *ptr_list, memory_address address);

void ptrListFree(t_ptr_list *ptr_list);

void friendlyInit(friendly_names *friendly, size_t initial_size);

void friendlyInsert(friendly_names *friendly, char *identifier, memory_address address);

void friendlyFree(friendly_names *friendly);

#endif
