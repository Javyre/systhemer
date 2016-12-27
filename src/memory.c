/* Helper functions and type defs for memory management is unitheme */

#include <stdio.h>
#include "memory.h"

/* Initializes memory holder */
void memoryInit(memory_holder *mem, size_t initial_size) {
  mem->content = (memory_item **)calloc(initial_size, sizeof(memory_item*));
  mem->content_type = (t_type *)calloc(initial_size, sizeof(t_type));

  mem->used = 0;
  mem->size = initial_size;
}

/* Inserts mem item and return address to this item once its in memory */
memory_address memoryInsert(memory_holder *mem, memory_item *item, t_type ptr_type) {
  memory_item **holder1;
  t_type *holder2;
  if (mem->size <= mem->used+1) {
    holder1 = (memory_item **)realloc(mem->content, sizeof(memory_item *) * (mem->size + 20));
    if (holder1 != NULL) mem->content = holder1;
    else fprintf(stderr, BKRED "realloc failed!" KDEFAULT);

    holder2 = (t_type *)realloc(mem->content_type, sizeof(t_type) * (mem->size + 20));
    if (holder2 != NULL) mem->content_type = holder2;
    else fprintf(stderr, BKRED "realloc failed!" KDEFAULT);

    mem->size += 20;
  }

  mem->content[mem->used] = item;
  mem->content_type[mem->used] = ptr_type;

  mem->used++;

  return mem->used-1;
}

/* Frees memory allocated for the memory_holder */
void memoryFree(memory_holder *mem) {
  for (size_t i=0; i<mem->used; i++) {
    if (mem->content[i] == NULL)
      continue;

    if (mem->content_type[i] == t_str || mem->content_type[i] == t_rgx) {
      if (mem->content[i]->str != NULL) {
        free(mem->content[i]->str);
        mem->content[i]->str = NULL;
      }
    } else if (mem->content_type[i] == t_list) {
      if (mem->content[i]->list != NULL) {
        ptrListFree(mem->content[i]->list);
        free(mem->content[i]->list);
        mem->content[i]->list = NULL;
      }
    }
    free(mem->content[i]);
    mem->content[i] = NULL;
  }
  free(mem->content);
  free(mem->content_type);
}

memory_item *memoryGetItem(memory_holder *mem, memory_address addr) {
  return mem->content[addr];
}

memory_item *memoryGetRootItem(memory_holder *mem, memory_address addr) {
  return mem->content[memoryGetRootAddress(mem, addr)];
}

t_type memoryGetType(memory_holder *mem, memory_address addr) {
  return mem->content_type[addr];
}

t_type memoryGetRootType(memory_holder *mem, memory_address addr) {
  return mem->content_type[memoryGetRootAddress(mem, addr)];
}

memory_address memoryGetRootAddress(memory_holder *mem, memory_address mem_addr) {
  printf(BKGRN);

  if (verboseMode)
    memoryIllustrateItem(NULL, mem, mem_addr, 999, true);

  while (mem->content_type[mem_addr] == t_addr) {
    mem_addr = mem->content[mem_addr]->address;
  }
  return mem_addr;
}

void memoryIllustrateMap(friendly_names *friendly, memory_holder *mem,
                          size_t depth) {
  for (memory_address i=0; i<mem->used; i++) {
    if (memoryGetFriendlyByAddress_se(friendly, i) != NULL || o_illustrate_print_non_friendlied)
      memoryIllustrateItem(friendly, mem, i, depth, true);
  }
}

void memoryIllustrateItem(friendly_names *friendly, memory_holder *mem,
                          memory_address i, size_t depth, bool do_newline) {
  char buff[(256 * 8) + 1];
  char list_content[(256*8)+1];
  char *name, *name2;
  char fname[256], fname2[256];
  memory_address a;
  /* for (memory_address i=0; i<mem->used; i++) { */

  /* Print friendly name (if it exists) as well as the address */
  if (friendly != NULL)
    name = memoryGetFriendlyByAddress_se(friendly, i);
  else
    name = NULL;
  sprintf(fname, "%s%s" BKCYN, (name != NULL ? " - " BKYEL : ""),
          (name != NULL ? name : ""));
  if (name != NULL)
    /* if there is a friendly name */
    sprintf(buff, "[" BKGRN "%lu" BKCYN "%s]->", (unsigned long)i, fname);
  else
    /* if not */
    sprintf(buff, "[%lu%s]->", (unsigned long)i, fname);

  /* if it's a string of any type */
  if (mem->content_type[i] == t_str || mem->content_type[i] == t_rgx) {
    /* if has a name then highlight value */
    if (name != NULL) {
      strcatf(buff, BKYEL "{%s}" BKCYN, mem->content[i]->str);
    } else {
      strcatf(buff, "{%s}", mem->content[i]->str);
    }
    /* we treat t_str and t_rgx as final values
     * so we terminate the buff string and return */
    if (do_newline)
      printf(BKCYN "%s\n" KDEFAULT, buff);
    else
      printf(BKCYN "%s" KDEFAULT, buff);
    buff[0] = '\0';
    list_content[0] = '\0';
    fname[0] = '\0';
    return;

  /* if it's a list */
  } else if (mem->content_type[i] == t_list) {
    /* following line is to avoid appending to non-initialized string */
    /* list_content[0] = '\0'; */

    if (name != NULL) {
      strcatf(buff, BKYEL ILISTO BKCYN);
    } else {
      strcatf(buff, ILISTO);
    }
    printf(BKCYN "%s" KDEFAULT, buff);

    /* Print contents of list (not expanding like for addresses section) */
    for (size_t ii=0; ii<mem->content[i]->list->used; ii++) {
      /* strcatf(list_content, "%lu, ", (unsigned long)mem->content[i]->list->pointers[ii]); */
      if (o_illustrate_recursive_list)
        memoryIllustrateItem(friendly, mem, mem->content[i]->list->pointers[ii],
                             depth - 1, false);
      else
        printf(BKCYN "[%lu]", (unsigned long)mem->content[i]->list->pointers[ii]);
      if (ii != mem->content[i]->list->used-1)
        printf(BKCYN ", ");
    }

    if (name != NULL) {
      printf(BKYEL ILISTC "%s" KDEFAULT, do_newline ? "\n" : "");
    } else {
      printf(BKCYN ILISTC "%s" KDEFAULT, do_newline ? "\n" : "");
    }
    /* finish off the string */
    /* list_content[strlen(list_content)-2] = '\0'; */
    /* if has a friendly name then highlight it while adding to the buff string */
    /* if (name != NULL) { */
    /*   strcatf(buff, BKYEL "[%s]" BKCYN, list_content); */
    /* } else { */
    /*   strcatf(buff, "[%s]", list_content); */
    /* } */

    /* for now, we treat list as final item and return */
    /* printf(BKCYN "%s\n" KDEFAULT, buff); */
    buff[0] = '\0';
    list_content[0] = '\0';
    fname[0] = '\0';
    return;


  /* if it's a pointer to another memoryitem */
  } else if (mem->content_type[i] == t_addr) {
    a = i;

    printf(BKCYN "%s", buff);
    buff[0] = '\0';
    list_content[0] = '\0';
    fname[0] = '\0';

    a = mem->content[a]->address;
    if ((depth-1)>0) {
      memoryIllustrateItem(friendly, mem, a, depth-1, do_newline);
      printf(KDEFAULT);
    } else {
      if (do_newline)
        printf(BKCYN "\n" KDEFAULT);
      else
        printf(KDEFAULT);
      return;
    }
  }
}

memory_address memoryGetAddresByFriendly(friendly_names *friendly, char *identifier) {
  for (size_t i=0; i<friendly->used; i++) {
    if (strcmp(friendly->identifiers[i], identifier) == 0) {
      return friendly->addresses[i];
    }
  }

  fprintf(stderr, BKRED "Error: memoryGetAddresByFriendly is returning 0: %s does not exist!\n", identifier);
  return 0;
}

char *memoryGetFriendlyByAddress_m(friendly_names *friendly, memory_address address, bool suppress_errors) {
  for (size_t i=0; i<friendly->used; i++) {
    if (friendly->addresses[i] == address) {
      return friendly->identifiers[i];
    }
  }

  if (!suppress_errors)
    fprintf(stderr, BKRED "Error: memoryGetFriendlyByAddress is returning NULL: friendly name for %lu does not exist!", (unsigned long)address);
  return NULL;
}
char *memoryGetFriendlyByAddress(friendly_names *friendly, memory_address address) {
  return memoryGetFriendlyByAddress_m(friendly, address, false);
}
char *memoryGetFriendlyByAddress_se(friendly_names *friendly, memory_address address) {
  return memoryGetFriendlyByAddress_m(friendly, address, true);
}

void ptrListInit(t_ptr_list *ptr_list, size_t initial_size) {
  ptr_list->pointers = (memory_address *)calloc(initial_size, sizeof(memory_address));
  ptr_list->size = initial_size;
  ptr_list->used = 0;
}

void ptrListInsert(t_ptr_list *ptr_list, memory_address address) {
  if (ptr_list->size == ptr_list->used+1) {
    ptr_list->pointers = realloc(ptr_list->pointers, ptr_list->size + 20);
  }

  ptr_list->pointers[ptr_list->used] = address;
  ptr_list->used++;
}

void ptrListFree(t_ptr_list *ptr_list) {
  free(ptr_list->pointers);
}

void friendlyInit(friendly_names *friendly, size_t initial_size) {
  friendly->identifiers = (char **)calloc(initial_size, sizeof(char *));
  friendly->addresses = (memory_address *)calloc(initial_size, sizeof(memory_address));
  friendly->size = initial_size;
  friendly->used = 0;
}

void friendlyInsert(friendly_names *friendly, char *identifier, memory_address address) {
  if (friendly->size == friendly->used+1) {
    friendly->identifiers = realloc(friendly->identifiers, friendly->size + 20);
    friendly->addresses = realloc(friendly->addresses, friendly->size + 20);
  }

  friendly->addresses[friendly->used] = address;
  friendly->identifiers[friendly->used] = identifier;
  friendly->used++;
}

void friendlyFree(friendly_names *friendly) {
  free(friendly->addresses);
  for (size_t i=0; i<friendly->used; i++)
    free(friendly->identifiers[i]);
  free(friendly->identifiers);
}
