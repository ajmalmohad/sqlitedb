#ifndef CURSOR_H
#define CURSOR_H

#include "constants.h"

Cursor *table_start(Table *table);
Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key);
Cursor *table_find(Table *table, uint32_t key);
void cursor_advance(Cursor *cursor);
void *cursor_value(Cursor *cursor);

#endif