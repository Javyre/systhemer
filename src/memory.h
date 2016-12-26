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
} t_ptr_list;

typedef union {
  t_ptr_list *list;
  char *str;
  size_t address;
} memory_item;

typedef struct {
  memory_item **content;
  t_type *content_type;
  size_t used;
  size_t size;
} memory_holder;

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
