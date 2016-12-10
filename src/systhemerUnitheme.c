#include "utils.h"
#include "unitheme.h"
#include "systhemerUnitheme.h"
#include <stdlib.h>

void sysUniStart(const char *file_name) {
  char **field_names = (char**)malloc(sizeof(char *) * 1);
  t_type *field_types = (t_type *)malloc(sizeof(t_type) * 1);
  /* memory_holder *input = (memory_holder *)malloc(sizeof(memory_holder)); */
  memory_holder *input = NULL;
  output_holder *output;



  field_names[0] = strdup("firstField");
  field_types[0] = t_str;

  uniInit(1, 10, 1, field_names, field_types, -1);

  uniRead(file_name, input, &output);

  uniDestroy();
}
