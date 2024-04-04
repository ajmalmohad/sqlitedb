#include "btree.h"
#include "node.h"
#include "pager.h"
#include "serialize.h"

/*
 * Initializes a leaf node.
 *
 * Parameters:
 * - node: A pointer to the node to be initialized.
 *
 * The function sets the type of the node to NODE_LEAF using the set_node_type
 * function. It then sets the number of cells in the leaf node to 0, indicating
 * that the leaf node is empty.
 *
 * Does not return a value.
 */
void initialize_leaf_node(void *node) {
  set_node_type(node, NODE_LEAF);
  *leaf_node_num_cells(node) = 0;
}

/*
 * Inserts a key-value pair into a leaf node.
 *
 * Parameters:
 * - cursor: A pointer to the Cursor structure, which indicates where to insert
 * the key-value pair.
 * - key: The key to be inserted.
 * - value: A pointer to the Row structure to be inserted.
 *
 * The function first retrieves the leaf node where the key-value pair is to be
 * inserted. It then checks if the leaf node is full. If it is, the function
 * prints an error message and exits.
 *
 * If the insertion point is not at the end of the leaf node, the function makes
 * room for the new cell by shifting the existing cells to the right.
 *
 * The function then increments the number of cells in the leaf node, sets the
 * key of the new cell, and serializes the value into the new cell.
 *
 * Does not return a value.
 */
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value) {
  void *node = get_page(cursor->table->pager, cursor->page_num);

  uint32_t num_cells = *leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    // Node full
    printf("Need to implement splitting a leaf node.\n");
    exit(EXIT_FAILURE);
  }

  if (cursor->cell_num < num_cells) {
    // Make room for new cell
    for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
      memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1),
             LEAF_NODE_CELL_SIZE);
    }
  }

  *(leaf_node_num_cells(node)) += 1;
  *(leaf_node_key(node, cursor->cell_num)) = key;
  serialize_row(value, leaf_node_value(node, cursor->cell_num));
}