#include "builtin_funcs.h"
extern int yyerror(const char *p);
extern size_t yyerror_count;

#define FUNC_CALL_ERR(...) do {} while (0);

void uni_mkblock (memory_address block_name_addr, STRING_TYPE str_type){
  if (str_type != T_STRING) {
    yyerror("");
    return;
  }

  /* TODO: Make it tell the difference between string litteral and variable to
   * then decide if it should dup it or just use and then free the place in
   * memory (also TODO: make memory esable by item for optimization) */
  outputCreateMemBlock(
      strdup(memoryGetRootItem(g_memory, block_name_addr)->str));
}
