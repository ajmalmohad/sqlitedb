#include "node.h"
#include "constants.h"

/*
 * Returns a pointer to the number of cells in a leaf node.
 *
 * Parameters:
 * - node: A pointer to the leaf node.
 *
 * The function calculates the memory address where the number of cells is
 * stored by adding the LEAF_NODE_NUM_CELLS_OFFSET to the node pointer.
 *
 * Returns a pointer to the number of cells in the leaf node.
 */
uint32_t *leaf_node_num_cells(void *node) {
  return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

/*
 * Returns a pointer to a specific cell in a leaf node.
 *
 * Parameters:
 * - node: A pointer to the leaf node.
 * - cell_num: The index of the cell in the leaf node.
 *
 * The function calculates the memory address of the cell by adding the
 * LEAF_NODE_HEADER_SIZE and the product of cell_num and LEAF_NODE_CELL_SIZE to
 * the node pointer.
 *
 * Returns a pointer to the cell.
 */
void *leaf_node_cell(void *node, uint32_t cell_num) {
  return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

/*
 * Returns a pointer to the key of a specific cell in a leaf node.
 *
 * Parameters:
 * - node: A pointer to the leaf node.
 * - cell_num: The index of the cell in the leaf node.
 *
 * The function retrieves a pointer to the cell by calling leaf_node_cell.
 * Since the key is at the start of the cell, no additional offset is needed.
 *
 * Returns a pointer to the key of the cell.
 */
uint32_t *leaf_node_key(void *node, uint32_t cell_num) {
  return leaf_node_cell(node, cell_num);
}

/*
 * Returns a pointer to the value of a specific cell in a leaf node.
 *
 * Parameters:
 * - node: A pointer to the leaf node.
 * - cell_num: The index of the cell in the leaf node.
 *
 * The function first retrieves a pointer to the cell by calling leaf_node_cell.
 * It then adds LEAF_NODE_KEY_SIZE to this pointer to skip over the key and get
 * to the value.
 *
 * Returns a pointer to the value of the cell.
 */
void *leaf_node_value(void *node, uint32_t cell_num) {
  return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

/*
 * Returns the type of a specific node.
 *
 * Parameters:
 * - node: A pointer to the node.
 *
 * The function works by first casting the node pointer + NODE_TYPE_OFFSET to a
 * uint8_t pointer. The value at this memory location is then dereferenced to
 * get the node type as a uint8_t value, which is then cast to NodeType and
 * returned.
 *
 * Returns the type of the node as a NodeType enum value.
 */
NodeType get_node_type(void *node) {
  uint8_t value = *((uint8_t *)(node + NODE_TYPE_OFFSET));
  return (NodeType)value;
}

/*
 * Sets the type of a specific node.
 *
 * Parameters:
 * - node: A pointer to the node.
 * - type: The type to be set for the node.
 *
 * The function works by first casting the NodeType to a uint8_t value.
 * It then adds the NODE_TYPE_OFFSET to the node pointer, and then casts the
 * result to a uint8_t pointer. The value at this memory location is set to the
 * uint8_t value.
 *
 * Does not return a value.
 */
void set_node_type(void *node, NodeType type) {
  uint8_t value = type;
  *((uint8_t *)(node + NODE_TYPE_OFFSET)) = value;
}