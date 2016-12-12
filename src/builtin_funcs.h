#ifndef BUILTIN_FUNCS_H
#define BUILTIN_FUNCS_H

#include "memory.h"
#include "unitheme.h"
#include "utils.h"

void uni_mkblock  (memory_address block_name_addr);
void uni_set_color(memory_address color_name_addr);
void uni_print    (memory_address block_name_addr);

void uni_garbage_collect();

#endif
