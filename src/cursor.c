#include "cursor.h"
#include "pager.h"
#include "node.h"

/*
 * Initializes a cursor to the start of the table.
 *
 * Parameters:
 * - table: A pointer to the Table structure.
 *
 * Allocates memory for a new Cursor, sets it to the start of the table.
 * Checks if the table is empty, and set end_of_table to true if it is.
 * Returns a pointer to the newly created Cursor.
 */
Cursor *table_start(Table *table) {
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  cursor->cell_num = 0;

  void *root_node = get_page(table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells(root_node);
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

/*
 * Finds the position of a key in a leaf node of a table.
 *
 * Parameters:
 * - table: A pointer to the Table structure.
 * - page_num: The page number of the leaf node.
 * - key: The key to find.
 *
 * The function performs a binary search in the leaf node for the given key.
 * It initializes a new Cursor and sets its 'table' and 'page_num' fields.
 * If the key is found, it sets the 'cell_num' field of the cursor to the index of the key.
 * If the key is not found, it sets 'cell_num' to the index where the key should be inserted.
 *
 * Returns a pointer to the newly created Cursor.
 */
Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key) {
  void *node = get_page(table->pager, page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);

  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = page_num;

  // Binary search
  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;
  while (one_past_max_index != min_index) {
    uint32_t index = (min_index + one_past_max_index) / 2;
    uint32_t key_at_index = *leaf_node_key(node, index);
    if (key == key_at_index) {
      cursor->cell_num = index;
      return cursor;
    }
    if (key < key_at_index) {
      one_past_max_index = index;
    } else {
      min_index = index + 1;
    }
  }

  cursor->cell_num = min_index;
  return cursor;
}

/*
 * Finds a key in the table and returns a cursor pointing to it.
 *
 * Parameters:
 * - table: A pointer to the Table structure.
 * - key: The key to find.
 *
 * The function retrieves the root node of the table and checks its type.
 * If the root node is a leaf node, it calls leaf_node_find to find the key.
 * If the root node is an internal node, it searches the internal nodes. 
 * TODO : searching in internal nodes is not yet implemented
 *
 * Returns a pointer to a Cursor that points to the key if it's found, or where it should be inserted if not.
 */
Cursor *table_find(Table *table, uint32_t key) {
  uint32_t root_page_num = table->root_page_num;
  void *root_node = get_page(table->pager, root_page_num);

  if (get_node_type(root_node) == NODE_LEAF) {
    return leaf_node_find(table, root_page_num, key);
  } else {
    printf("Need to implement searching an internal node\n");
    exit(EXIT_FAILURE);
  }
}

/*
 * Advances a cursor to the next cell in the table.
 *
 * Parameters:
 * - cursor: A pointer to the Cursor structure.
 *
 * Increments the 'cell_num' field of the cursor.
 * If the cursor is at the end of the table, it sets 'end_of_table' to true.
 * 
 * Note: This function currently only handles tables that fit within a single page. 
 * It would need to be modified to handle tables that span multiple pages.
 */
void cursor_advance(Cursor *cursor) {
  uint32_t page_num = cursor->page_num;
  void *node = get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
    cursor->end_of_table = true;
  }
}

/*
 * Returns a pointer to the value at the cursor's current position in the table.
 *
 * Parameters:
 * - cursor: A pointer to the Cursor structure.
 *
 * The function retrieves the page that the cursor is on by calling get_page.
 * It then calls leaf_node_value to get a pointer to the value of the cell that the cursor is pointing to.
 *
 * Returns a pointer to the value at the cursor's current position.
 */
void *cursor_value(Cursor *cursor) {
  uint32_t page_num = cursor->page_num;
  void *page = get_page(cursor->table->pager, page_num);

  return leaf_node_value(page, cursor->cell_num);
}
