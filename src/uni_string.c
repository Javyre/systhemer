/* #include "uni_string.h" */
#include "memory.h"
#include "utils.h"
#include "unitheme.h"
#include <string.h>

memory_address g_string_attrs_add(const memory_address a, const memory_address b);
memory_address g_string_attrs_subtract(const memory_address a, const memory_address b);
memory_address g_string_attrs_multiply(const memory_address a, const memory_address b);
memory_address g_string_attrs_divide(const memory_address a, const memory_address b);

type_attrs g_string_attrs = {
  .esc_char                = '\\',
  .escaped                 = {"\\", "\"", "n", "t"},   /* escaped   */
  .unescaped               = {"\\", "\"", "\n", "\t"}, /* unescaped */
  .num_escs                = 4,
  .delim_char              = '\"',
  .allow_unrecognized_escs = false,
  .add                     = g_string_attrs_add,
  .subtract                = g_string_attrs_subtract,
  .multiply                = g_string_attrs_multiply,
  .divide                  = g_string_attrs_divide,
  .is_string_type          = true,
  .s_type                  = T_STRING,
  .t_type                  = t_str
};

memory_address g_string_attrs_add(const memory_address a, const memory_address b) {
  memory_item *result_item = malloc(sizeof(memory_item));

  memory_item *root_a = memoryGetRootItem(g_memory, a);
  memory_item *root_b = memoryGetRootItem(g_memory, b);

  result_item->str=calloc(strlen(root_a->str)+strlen(root_b->str)+1, sizeof(char));
  strcpy(result_item->str, root_a->str);
  strcat(result_item->str, root_b->str);

  return memoryInsert(g_memory, result_item, t_str);
}

memory_address g_string_attrs_subtract(const memory_address a, const memory_address b) {
  ERROR_PRINT("Invalid operation on type string!");
  return a;
}

memory_address g_string_attrs_multiply(const memory_address a, const memory_address b) {
  ERROR_PRINT("Invalid operation on type string!");
  return a;
}

memory_address g_string_attrs_divide(const memory_address a, const memory_address b) {
  ERROR_PRINT("Invalid operation on type string!");
  return a;
}
