#include "builtin_funcs.h"
extern int yyerror(const char *p);
extern size_t yyerror_count;

#define FUNC_CALL_ERR(...) do {} while (0);

void uni_mkblock (memory_address block_name_addr, STRING_TYPE str_type){
  if (str_type != T_STRING) {
    yyerror("");
    return;
  }

  outputCreateMemBlock(memoryGetRootItem(g_memory, block_name_addr)->str);
}
