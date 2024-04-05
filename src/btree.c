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
  set_node_root(node, false);
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
    leaf_node_split_and_insert(cursor, key, value);
    return;
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

/*
 * Initializes an internal node.
 *
 * Parameters:
 * - node: A pointer to the node to be initialized.
 *
 * The function sets the type of the node to NODE_INTERNAL using the
 * set_node_type function. It then sets the root flag of the node to false,
 * indicating that this node is not a root node.
 *
 * The function also sets the number of keys in the internal node to 0,
 * indicating that the internal node is empty.
 *
 * Does not return a value.
 */
void initialize_internal_node(void *node) {
  set_node_type(node, NODE_INTERNAL);
  set_node_root(node, false);
  *internal_node_num_keys(node) = 0;
}

/*
 * Retrieves the number of keys in an internal node.
 *
 * Parameters:
 * - node: A pointer to the internal node.
 *
 * The function calculates the memory address of the number of keys in the
 * internal node by adding the offset of the number of keys to the base address
 * of the node.
 *
 * Returns a pointer to the number of keys in the internal node.
 */
uint32_t *internal_node_num_keys(void *node) {
  return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

/*
 * Retrieves the right child of an internal node.
 *
 * Parameters:
 * - node: A pointer to the internal node.
 *
 * The function calculates the memory address of the right child in the internal
 * node by adding the offset of the right child to the base address of the node.
 *
 * Returns a pointer to the right child of the internal node.
 */
uint32_t *internal_node_right_child(void *node) {
  return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

/*
 * Retrieves a cell from an internal node.
 *
 * Parameters:
 * - node: A pointer to the internal node.
 * - cell_num: The number of the cell to be retrieved.
 *
 * The function calculates the memory address of the cell in the internal node
 * by adding the offset of the cell (calculated as the header size plus the cell
 * number times the cell size) to the base address of the node.
 *
 * Returns a pointer to the cell in the internal node.
 */
uint32_t *internal_node_cell(void *node, uint32_t cell_num) {
  return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

/*
 * Retrieves a child from an internal node.
 *
 * Parameters:
 * - node: A pointer to the internal node.
 * - child_num: The number of the child to be retrieved.
 *
 * The function first retrieves the number of keys in the internal node.
 * It then checks if the requested child number is greater than the number of
 * keys. If it is, the function prints an error message and exits, as this
 * indicates an invalid request.
 *
 * If the requested child number is equal to the number of keys, the function
 * retrieves the right child of the node. Otherwise, the function retrieves the
 * cell corresponding to the child number.
 *
 * Returns a pointer to the child in the internal node.
 */
uint32_t *internal_node_child(void *node, uint32_t child_num) {
  uint32_t num_keys = *internal_node_num_keys(node);
  if (child_num > num_keys) {
    printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
    exit(EXIT_FAILURE);
  } else if (child_num == num_keys) {
    return internal_node_right_child(node);
  } else {
    return internal_node_cell(node, child_num);
  }
}

/*
 * Retrieves a key from an internal node.
 *
 * Parameters:
 * - node: A pointer to the internal node.
 * - key_num: The number of the key to be retrieved.
 *
 * The function first retrieves the cell corresponding to the key number.
 * It then calculates the memory address of the key in the cell by adding the
 * size of the child pointer to the base address of the cell.
 *
 * Returns a pointer to the key in the internal node.
 */
uint32_t *internal_node_key(void *node, uint32_t key_num) {
  return internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

/*
 * Retrieves the maximum key from a node.
 *
 * Parameters:
 * - node: A pointer to the node.
 *
 * The function first determines the type of the node using the get_node_type
 * function.
 *
 * If the node is an internal node, the function retrieves the last key in the
 * node (which is the maximum key as the keys are sorted in ascending order).
 *
 * If the node is a leaf node, the function retrieves the last key in the node
 * (which is the maximum key as the keys are sorted in ascending order).
 *
 * Returns the maximum key in the node.
 */
uint32_t get_node_max_key(void *node) {
  switch (get_node_type(node)) {
  case NODE_INTERNAL:
    return *internal_node_key(node, *internal_node_num_keys(node) - 1);
  case NODE_LEAF:
    return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
  }
}

/*
 * Checks if a node is a root node.
 *
 * Parameters:
 * - node: A pointer to the node.
 *
 * The function retrieves the root flag from the node by calculating the memory
 * address of the flag (by adding the offset of the flag to the base address of
 * the node) and dereferencing the pointer.
 *
 * Returns true if the node is a root node, and false otherwise.
 */
bool is_node_root(void *node) {
  uint8_t value = *((uint8_t *)(node + IS_ROOT_OFFSET));
  return (bool)value;
}

/*
 * Sets the root flag of a node.
 *
 * Parameters:
 * - node: A pointer to the node.
 * - is_root: The value to be set for the root flag.
 *
 * The function sets the root flag of the node by calculating the memory address
 * of the flag (by adding the offset of the flag to the base address of the
 * node) and setting the value at that address.
 *
 * Does not return a value.
 */
void set_node_root(void *node, bool is_root) {
  uint8_t value = is_root;
  *((uint8_t *)(node + IS_ROOT_OFFSET)) = value;
}

/*
 * Creates a new root for a B-Tree when the old root is split.
 *
 * Parameters:
 * - table: A pointer to the Table structure, which contains the B-Tree.
 * - right_child_page_num: The page number of the right child of the new root.
 *
 * The function first retrieves the old root and the right child.
 * It then allocates a new page for the left child and copies the old root to
 * the left child. The left child is not a root node, so its root flag is set to
 * false.
 *
 * The function then re-initializes the old root to be the new root node and
 * sets its root flag to true. The new root has one key, which is the maximum
 * key of the left child. The new root points to the left child and the right
 * child.
 *
 * Does not return a value.
 */
void create_new_root(Table *table, uint32_t right_child_page_num) {
  void *root = get_page(table->pager, table->root_page_num);
  void *right_child = get_page(table->pager, right_child_page_num);
  uint32_t left_child_page_num = get_unused_page_num(table->pager);
  void *left_child = get_page(table->pager, left_child_page_num);

  memcpy(left_child, root, PAGE_SIZE);
  set_node_root(left_child, false);

  initialize_internal_node(root);
  set_node_root(root, true);
  *internal_node_num_keys(root) = 1;
  *internal_node_child(root, 0) = left_child_page_num;
  uint32_t left_child_max_key = get_node_max_key(left_child);
  *internal_node_key(root, 0) = left_child_max_key;
  *internal_node_right_child(root) = right_child_page_num;
}

/*
 * Splits a leaf node and inserts a key-value pair into the appropriate node.
 *
 * Parameters:
 * - cursor: A pointer to the Cursor structure, which indicates where to insert
 * the key-value pair.
 * - key: The key to be inserted.
 * - value: A pointer to the Row structure to be inserted.
 *
 * The function first retrieves the old node and creates a new node.
 * It then moves half of the cells from the old node to the new node.
 *
 * The function then inserts the new key-value pair into the appropriate node.
 * If the old node is a root node, the function creates a new root.
 * Otherwise, the function updates the parent of the nodes.
 *
 * The function also updates the number of cells in both nodes.
 *
 * Does not return a value.
 */
void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row *value) {
  void *old_node = get_page(cursor->table->pager, cursor->page_num);
  uint32_t new_page_num = get_unused_page_num(cursor->table->pager);
  void *new_node = get_page(cursor->table->pager, new_page_num);
  initialize_leaf_node(new_node);

  for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
    void *destination_node;
    if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
      destination_node = new_node;
    } else {
      destination_node = old_node;
    }
    uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
    void *destination = leaf_node_cell(destination_node, index_within_node);

    if (i == cursor->cell_num) {
      serialize_row(value, destination);
    } else if (i > cursor->cell_num) {
      memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
    } else {
      memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
    }
  }

  *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
  *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

  if (is_node_root(old_node)) {
    return create_new_root(cursor->table, new_page_num);
  } else {
    printf("Need to implement updating parent after split\n");
    exit(EXIT_FAILURE);
  }
}