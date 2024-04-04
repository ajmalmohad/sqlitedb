#ifndef BTREE_H
#define BTREE_H

#include "constants.h"

void initialize_leaf_node(void *node);
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);

#endif