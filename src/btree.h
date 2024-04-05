#ifndef BTREE_H
#define BTREE_H

#include "constants.h"

void initialize_leaf_node(void *node);
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);
void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row *value);
void create_new_root(Table *table, uint32_t right_child_page_num);
void initialize_internal_node(void *node);
uint32_t *internal_node_num_keys(void *node);
uint32_t *internal_node_right_child(void *node);
uint32_t *internal_node_cell(void *node, uint32_t cell_num);
uint32_t *internal_node_child(void *node, uint32_t child_num);
uint32_t *internal_node_key(void *node, uint32_t key_num);
uint32_t get_node_max_key(void *node);
bool is_node_root(void *node);
void set_node_root(void *node, bool is_root);

#endif