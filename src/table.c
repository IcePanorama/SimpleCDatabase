#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pager.h"
#include "table.h"

uint32_t *node_parent (void *node);
void update_internal_node_key (void *node, uint32_t old_key, uint32_t new_key);
void internal_node_insert (Table *table, uint32_t parent_page_num,
                           uint32_t child_page_num);
void internal_node_split_and_insert (Table *table, uint32_t parent_page_num,
                                     uint32_t child_page_num);

#define INVALID_PAGE_NUM UINT32_MAX

const uint32_t PAGE_SIZE = 4096; // 4k
const uint32_t ID_SIZE = size_of_attribute (Row, id);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_SIZE = size_of_attribute (Row, username);
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_SIZE = size_of_attribute (Row, email);
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t NODE_TYPE_SIZE = sizeof (uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof (uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof (uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t COMMON_NODE_HEADER_SIZE
    = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof (uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof (uint32_t);
const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET
    = LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE
                                       + LEAF_NODE_NUM_CELLS_SIZE
                                       + LEAF_NODE_NEXT_LEAF_SIZE;
const uint32_t LEAF_NODE_KEY_SIZE = sizeof (uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET
    = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS
    = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT
    = (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;

const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof (uint32_t);
const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof (uint32_t);
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET
    = INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE
                                           + INTERNAL_NODE_NUM_KEYS_SIZE
                                           + INTERNAL_NODE_RIGHT_CHILD_SIZE;
const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof (uint32_t);
const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof (uint32_t);
const uint32_t INTERNAL_NODE_CELL_SIZE
    = INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;

/* Keep this small for testing */
const uint32_t INTERNAL_NODE_MAX_KEYS = 3;

void
serialize_row (Row *source, void *destination)
{
  memcpy (destination + ID_OFFSET, &(source->id), ID_SIZE);
  strncpy (destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
  strncpy (destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void
deserialize_row (void *source, Row *destination)
{
  memcpy (&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy (&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy (&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void
print_row (Row *row)
{
  printf ("(%d, %s, %s)\n", row->id, row->username, row->email);
}

Table *
db_open (const char *filename)
{
  Pager *pager = pager_open (filename);

  Table *table = (Table *)malloc (sizeof (Table));
  table->pager = pager;
  table->root_page_num = 0;

  if (pager->num_pages == 0)
    {
      // New database file. Init page 0 as a leaf node.
      void *root_node = get_page (pager, 0);
      initialize_leaf_node (root_node);
      set_node_root (root_node, true);
    }

  return table;
}

void
db_close (Table *table)
{
  Pager *pager = table->pager;

  for (uint32_t i = 0; i < pager->num_pages; i++)
    {
      if (pager->pages[i] == NULL)
        {
          continue;
        }

      pager_flush (pager, i);
      free (pager->pages[i]);
      pager->pages[i] = NULL;
    }

  int result = close (pager->file_descriptor);
  if (result == -1)
    {
      puts ("Error closing db file.");
      exit (EXIT_FAILURE);
    }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
      void *page = pager->pages[i];
      if (page)
        {
          free (page);
          pager->pages[i] = NULL;
        }
    }

  free (pager);
  free (table);
}

uint32_t *
leaf_node_num_cells (void *node)
{
  return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *
leaf_node_cell (void *node, uint32_t cell_num)
{
  return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *
leaf_node_key (void *node, uint32_t cell_num)
{
  return leaf_node_cell (node, cell_num);
}

void
initialize_leaf_node (void *node)
{
  set_node_type (node, NODE_LEAF);
  set_node_root (node, false);
  *leaf_node_num_cells (node) = 0;
  *leaf_node_next_leaf (node) = 0; // 0 represents no sibling.
}

void
initialize_internal_node (void *node)
{
  set_node_type (node, NODE_INTERNAL);
  set_node_type (node, false);
  *internal_node_num_keys (node) = 0;
  /*
   *  Necessary b/c the root page num is 0; by not initializing an internal
   *  node's right child to an invalid page number when initializing the
   *  node, we may end up with 0 as the node's right child, which makes the
   *  node a parent of the root.
   */
  *internal_node_right_child (node) = INVALID_PAGE_NUM;
}

void *
leaf_node_value (void *node, uint32_t cell_num)
{
  return leaf_node_cell (node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void
leaf_node_insert (Cursor *cursor, uint32_t key, Row *value)
{
  void *node = get_page (cursor->table->pager, cursor->page_num);

  uint32_t num_cells = *leaf_node_num_cells (node);
  if (num_cells >= LEAF_NODE_MAX_CELLS)
    {
      // Node full
      leaf_node_split_and_insert (cursor, key, value);
      return;
    }

  if (cursor->cell_num < num_cells)
    {
      // Make room for new cell
      for (uint32_t i = num_cells; i > cursor->cell_num; i--)
        {
          memcpy (leaf_node_cell (node, i), leaf_node_cell (node, i - 1),
                  LEAF_NODE_CELL_SIZE);
        }
    }

  *(leaf_node_num_cells (node)) += 1;
  *(leaf_node_key (node, cursor->cell_num)) = key;
  serialize_row (value, leaf_node_value (node, cursor->cell_num));
}

NodeType
get_node_type (void *node)
{
  uint8_t value = *((uint8_t *)(node + NODE_TYPE_OFFSET));
  return (NodeType)value;
}

void
set_node_type (void *node, NodeType type)
{
  uint8_t value = type;
  *((uint8_t *)(node + NODE_TYPE_OFFSET)) = value;
}

void
leaf_node_split_and_insert (Cursor *cursor, uint32_t key, Row *value)
{
  /*
   *  Create a new node and move half the cells over.
   *  Insert the new value in one of the two nodes.
   *  Update parent or create a new parent.
   */
  void *old_node = get_page (cursor->table->pager, cursor->page_num);
  uint32_t old_max = get_node_max_key (cursor->table->pager, old_node);
  uint32_t new_page_num = get_unused_page_num (cursor->table->pager);
  void *new_node = get_page (cursor->table->pager, new_page_num);
  initialize_leaf_node (new_node);
  *node_parent (new_node) = *node_parent (old_node);
  *leaf_node_next_leaf (new_node) = *leaf_node_next_leaf (old_node);
  *leaf_node_next_leaf (old_node) = new_page_num;

  /*
   *  All existing keys plus new key should be divided evenly between
   *  old (left) and new (right) nodes.
   *  Starting from the right, move each key to the correct position.
   */
  for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--)
    {
      void *destination_node;
      if (i >= LEAF_NODE_LEFT_SPLIT_COUNT)
        {
          destination_node = new_node;
        }
      else
        {
          destination_node = old_node;
        }

      uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
      void *destination = leaf_node_cell (destination_node, index_within_node);

      if (i == cursor->cell_num)
        {
          serialize_row (
              value, leaf_node_value (destination_node, index_within_node));
          *leaf_node_key (destination_node, index_within_node) = key;
        }
      else if (i > cursor->cell_num)
        {
          memcpy (destination, leaf_node_cell (old_node, i - 1),
                  LEAF_NODE_CELL_SIZE);
        }
      else
        {
          memcpy (destination, leaf_node_cell (old_node, i),
                  LEAF_NODE_CELL_SIZE);
        }
    }

  /* Update cell counts on both leaf nodes */
  *(leaf_node_num_cells (old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
  *(leaf_node_num_cells (new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

  if (is_node_root (old_node))
    {
      return create_new_root (cursor->table, new_page_num);
    }
  else
    {
      uint32_t parent_page_num = *node_parent (old_node);
      uint32_t new_max = get_node_max_key (cursor->table->pager, old_node);
      void *parent = get_page (cursor->table->pager, parent_page_num);

      update_internal_node_key (parent, old_max, new_max);
      internal_node_insert (cursor->table, parent_page_num, new_page_num);
      return;
    }
}

uint32_t
get_unused_page_num (Pager *pager)
{
  return pager->num_pages;
}

void
create_new_root (Table *table, uint32_t right_child_page_num)
{
  /*
   *  Handle splitting the root.
   *  Old root copied to new page, becomes left child.
   *  Address of right child passed in.
   *  Re-initialize root page to contain the new root node.
   *  New root node points to two children.
   */
  void *root = get_page (table->pager, table->root_page_num);
  void *right_child = get_page (table->pager, right_child_page_num);
  uint32_t left_child_page_num = get_unused_page_num (table->pager);
  void *left_child = get_page (table->pager, left_child_page_num);

  if (get_node_type (root) == NODE_INTERNAL)
    {
      initialize_internal_node (right_child);
      initialize_internal_node (left_child);
    }

  /* Left child has data copied from old root */
  memcpy (left_child, root, PAGE_SIZE);
  set_node_root (left_child, false);

  if (get_node_type (left_child) == NODE_INTERNAL)
    {
      void *child;
      for (int i = 0; i < *internal_node_num_keys (left_child); i++)
        {
          child
              = get_page (table->pager, *internal_node_child (left_child, i));
          *node_parent (child) = left_child_page_num;
        }
      child = get_page (table->pager, *internal_node_right_child (left_child));
      *node_parent (child) = left_child_page_num;
    }

  /* Root node is a new internal node w/ one key and two children. */
  initialize_internal_node (root);
  set_node_root (root, true);
  *internal_node_num_keys (root) = 1;
  *internal_node_child (root, 0) = left_child_page_num;
  uint32_t left_child_max_key = get_node_max_key (table->pager, left_child);
  *internal_node_key (root, 0) = left_child_max_key;
  *internal_node_right_child (root) = right_child_page_num;
  *node_parent (left_child) = table->root_page_num;
  *node_parent (right_child) = table->root_page_num;
}

uint32_t *
internal_node_num_keys (void *node)
{
  return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t *
internal_node_right_child (void *node)
{
  return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t *
internal_node_cell (void *node, uint32_t cell_num)
{
  return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t *
internal_node_child (void *node, uint32_t child_num)
{
  uint32_t num_keys = *internal_node_num_keys (node);
  if (child_num > num_keys)
    {
      printf ("Tried to access child_num %d > num_keys %d\n", child_num,
              num_keys);
      exit (EXIT_FAILURE);
    }
  else if (child_num == num_keys)
    {
      uint32_t *right_child = internal_node_right_child (node);
      if (*right_child == INVALID_PAGE_NUM)
        {
          puts ("Tried to access right child of node, but was invalid page.");
          exit (EXIT_FAILURE);
        }
      return right_child;
    }
  else
    {
      uint32_t *child = internal_node_cell (node, child_num);
      if (*child == INVALID_PAGE_NUM)
        {
          printf ("Tried to access child %d of node, but was invalid page.\n",
                  child_num);
          exit (EXIT_FAILURE);
        }
      return child;
    }
}

uint32_t *
internal_node_key (void *node, uint32_t key_num)
{
  return (void *)internal_node_cell (node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

uint32_t
get_node_max_key (Pager *pager, void *node)
{
  if (get_node_type (node) == NODE_LEAF)
    {
      return *leaf_node_key (node, *leaf_node_num_cells (node) - 1);
    }
  void *right_child = get_page (pager, *internal_node_right_child (node));
  return get_node_max_key (pager, right_child);
}

bool
is_node_root (void *node)
{
  uint8_t value = *((uint8_t *)(node + IS_ROOT_OFFSET));
  return (bool)value;
}

void
set_node_root (void *node, bool is_root)
{
  uint8_t value = is_root;
  *((uint8_t *)(node + IS_ROOT_OFFSET)) = value;
}

uint32_t *
leaf_node_next_leaf (void *node)
{
  return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

uint32_t *
node_parent (void *node)
{
  return node + PARENT_POINTER_OFFSET;
}

void
update_internal_node_key (void *node, uint32_t old_key, uint32_t new_key)
{
  uint32_t old_child_index = internal_node_find_child (node, old_key);
  *internal_node_key (node, old_child_index) = new_key;
}

// Should this be here or in Cursor.c?
void
internal_node_insert (Table *table, uint32_t parent_page_num,
                      uint32_t child_page_num)
{
  /*
   *  Add a new child/key pair to parent that corresponds to child.
   */

  void *parent = get_page (table->pager, parent_page_num);
  void *child = get_page (table->pager, child_page_num);
  uint32_t child_max_key = get_node_max_key (table->pager, child);
  uint32_t index = internal_node_find_child (parent, child_max_key);

  uint32_t original_num_keys = *internal_node_num_keys (parent);

  if (original_num_keys >= INTERNAL_NODE_MAX_KEYS)
    {
      internal_node_split_and_insert (table, parent_page_num, child_page_num);
      return;
    }

  uint32_t right_child_page_num = *internal_node_right_child (parent);
  /*
   *  An internal node with a right child of INVALID_PAGE_NUM is empty.
   */
  if (right_child_page_num == INVALID_PAGE_NUM)
    {
      *internal_node_right_child (parent) = child_page_num;
      return;
    }

  void *right_child = get_page (table->pager, right_child_page_num);
  /*
   *  If we are already at the max number of cells for a node, we cannot
   *  increment before splitting. Incrememnting without inserting a new
   *  key/child pair and immediately calling internal_node_split_and_insert
   *  has the effect of creating a new key at (max_cells + 1) w/ an
   *  uninitialized value;
   */
  *internal_node_num_keys (parent) = original_num_keys + 1;
  if (child_max_key > get_node_max_key (table->pager, right_child))
    {
      /* Replace right child. */
      *internal_node_child (parent, original_num_keys) = right_child_page_num;
      *internal_node_key (parent, original_num_keys)
          = get_node_max_key (table->pager, child);
      *internal_node_right_child (parent) = child_page_num;
    }
  else
    {
      /* Make room for the new cell. */
      for (uint32_t i = original_num_keys; i > index; i--)
        {
          void *destination = internal_node_cell (parent, i);
          void *source = internal_node_cell (parent, i - 1);
          memcpy (destination, source, INTERNAL_NODE_CELL_SIZE);
        }
      *internal_node_child (parent, index) = child_page_num;
      *internal_node_key (parent, index) = child_max_key;
    }
}

void
internal_node_split_and_insert (Table *table, uint32_t parent_page_num,
                                uint32_t child_page_num)
{
  uint32_t old_page_num = parent_page_num;
  void *old_node = get_page (table->pager, parent_page_num);
  uint32_t old_max = get_node_max_key (table->pager, old_node);

  void *child = get_page (table->pager, child_page_num);
  uint32_t child_max = get_node_max_key (table->pager, child);

  uint32_t new_page_num = get_unused_page_num (table->pager);

  /*
   *  Declaring a flag before updating pointers which records whether this
   *  operation involves splittingg the root--if it does, we will insert our
   *  newly created node during the step where the table's new root is
   *  created. If it does not, we have to insert the newly created node into
   *  its parent after the old node's keys have been transfered over. We are
   *  not able to do this if the newly created node's parent is not a newly
   *  initialized root node, because in that case its parent may have
   *  existing keys aside from our old node which we are splitting. If that
   *  is true, we need to find a place for our newlly created node in its
   *  parent, and we cannot insert it at the correct index if it does not
   *  yet have any keys.
   */
  uint32_t splitting_root = is_node_root (old_node);

  void *parent;
  void *new_node;
  if (splitting_root)
    {
      create_new_root (table, new_page_num);
      parent = get_page (table->pager, table->root_page_num);
      /*
       *  If we are splitting the root, we need to update old_node to point
       *  to the new root's left child, new_page_numm will already point to
       *  the new root's right child.
       */
      old_page_num = *internal_node_child (parent, 0);
      old_node = get_page (table->pager, old_page_num);
    }
  else
    {
      parent = get_page (table->pager, *node_parent (old_node));
      new_node = get_page (table->pager, new_page_num);
      initialize_internal_node (new_node);
    }

  uint32_t *old_num_keys = internal_node_num_keys (old_node);

  uint32_t cur_page_num = *internal_node_right_child (old_node);
  void *cur = get_page (table->pager, cur_page_num);

  /*
   *  First put right child into new node and set right child of old node to
   *  invalid pagte number.
   */
  internal_node_insert (table, new_page_num, cur_page_num);
  *node_parent (cur) = new_page_num;
  *internal_node_right_child (old_node) = INVALID_PAGE_NUM;
  /*
   *  For each key until you get to the middle key, move the key and the
   *  child to the new node.
   */
  for (int i = INTERNAL_NODE_MAX_KEYS - 1; i > INTERNAL_NODE_MAX_KEYS / 2; i--)
    {
      cur_page_num = *internal_node_child (old_node, i);
      cur = get_page (table->pager, cur_page_num);

      internal_node_insert (table, new_page_num, cur_page_num);
      *node_parent (cur) = new_page_num;

      (*old_num_keys)--;
    }

  /*
   *  Set child before middle key, which is now the highest key, to be node's
   *  right child, then decrement number of keys.
   */
  *internal_node_right_child (old_node)
      = *internal_node_child (old_node, *old_num_keys - 1);
  (*old_num_keys)--;

  /*
   *  Determine which of the two nodes after the split should contain the
   *  child to be inserted, and insert the child.
   */
  uint32_t max_after_split = get_node_max_key (table->pager, old_node);

  uint32_t destination_page_num
      = child_max < max_after_split ? old_page_num : new_page_num;
  internal_node_insert (table, destination_page_num, child_page_num);
  *node_parent (child) = destination_page_num;

  update_internal_node_key (parent, old_max,
                            get_node_max_key (table->pager, old_node));

  if (!splitting_root)
    {
      internal_node_insert (table, *node_parent (old_node), new_page_num);
      *node_parent (new_node) = *node_parent (old_node);
    }
}
