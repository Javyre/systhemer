/* #include "uni_regex.h" */
#include "memory.h"
#include "utils.h"
#include "unitheme.h"
#include <string.h>

memory_address g_regex_attrs_add(const memory_address a, const memory_address b);
memory_address g_regex_attrs_subtract(const memory_address a, const memory_address b);
memory_address g_regex_attrs_multiply(const memory_address a, const memory_address b);
memory_address g_regex_attrs_divide(const memory_address a, const memory_address b);
RETURN_CODE g_regex_attrs_compare(const memory_address a, const memory_address b);

type_attrs g_regex_attrs = {
  .esc_char                = '\\',
  .escaped                 = {"/"},   /* escaped   */
  .unescaped               = {"/"}, /* unescaped */
  .num_escs                = 1,
  .delim_char              = '/',
  .allow_unrecognized_escs = true,
  .add                     = g_regex_attrs_add,
  .subtract                = g_regex_attrs_subtract,
  .multiply                = g_regex_attrs_multiply,
  .divide                  = g_regex_attrs_divide,
  .negate                  = mem_operation_func_unsupported,
  .compare                 = g_regex_attrs_compare,
  .is_string_type          = true,
  .s_type                  = T_REGEX,
  .t_type                  = t_rgx
};

memory_address g_regex_attrs_add(const memory_address a, const memory_address b) {
  ERROR_PRINT("Invalid operation on type regex!");
  return a;
}

memory_address g_regex_attrs_subtract(const memory_address a, const memory_address b) {
  ERROR_PRINT("invalid operation on type regex!");
  return a;
}

memory_address g_regex_attrs_multiply(const memory_address a, const memory_address b) {
  ERROR_PRINT("invalid operation on type regex!");
  return a;
}

memory_address g_regex_attrs_divide(const memory_address a, const memory_address b) {
  ERROR_PRINT("invalid operation on type regex!");
  return a;
}

RETURN_CODE g_regex_attrs_compare(const memory_address a, const memory_address b) {
  if (strcmp(memoryGetRootItem(g_memory, a)->str,memoryGetRootItem(g_memory, b)->str) == 0)
    return EQUAL;
  return UNEQUAL;
}
