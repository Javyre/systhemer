#include "builtin_funcs.h"
extern int yyerror(const char *p);
extern size_t yyerror_count;

char *g_current_print_color = NULL;

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

void uni_set_color(memory_address color_name_addr, STRING_TYPE str_type) {
  if (str_type != T_STRING) {
    yyerror("");
    return;
  }

  if (g_current_print_color != NULL)
    free(g_current_print_color);

  char *tmp = memoryGetRootItem(g_memory, color_name_addr)->str;
  if (strcmp(tmp, "default") == 0)
    g_current_print_color = strdup(KDEFAULT);
  else if (strcmp(tmp, "red") == 0)
    g_current_print_color = strdup(KRED);
  else if (strcmp(tmp, "bold_red") == 0)
    g_current_print_color = strdup(BKRED);
  else if (strcmp(tmp, "green") == 0)
    g_current_print_color = strdup(KGRN);
  else if (strcmp(tmp, "bold_green") == 0)
    g_current_print_color = strdup(BKGRN);
  else if (strcmp(tmp, "blue") == 0)
    g_current_print_color = strdup(KBLU);
  else if (strcmp(tmp, "bold_blue") == 0)
    g_current_print_color = strdup(BKBLU);
  else
    yyerror("runtime error (color name not found)");
}

void uni_print(memory_address string_addr, STRING_TYPE str_type) {
  if (str_type != T_STRING) {
    yyerror("");
    return;
  }

  printf(KDEFAULT "%s%s" KDEFAULT,
         g_current_print_color != NULL ? g_current_print_color : "",
         memoryGetRootItem(g_memory, string_addr)->str);
}

void uni_garbage_collect() {
  if (g_current_print_color != NULL)
    free(g_current_print_color);
  g_current_print_color = NULL;
}
