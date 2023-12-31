#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_buffer.h"
#include "meta_command_result.h"
#include "table.h"

void
indent (uint32_t level)
{
  for (uint32_t i = 0; i < level; i++)
    {
      printf ("  ");
    }
}

void
print_tree (Pager *pager, uint32_t page_num, uint32_t indentation_level)
{
  void *node = get_page (pager, page_num);
  uint32_t num_keys, child;

  switch (get_node_type (node))
    {
    case (NODE_LEAF):
      num_keys = *leaf_node_num_cells (node);
      indent (indentation_level);
      printf ("- leaf (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++)
        {
          indent (indentation_level + 1);
          printf ("- %d\n", *leaf_node_key (node, i));
        }
      break;
    case (NODE_INTERNAL):
      num_keys = *internal_node_num_keys (node);
      indent (indentation_level);
      printf ("- internal (size %d)\n", num_keys);
      if (num_keys > 0)
        {
          for (uint32_t i = 0; i < num_keys; i++)
            {
              child = *internal_node_child (node, i);
              print_tree (pager, child, indentation_level + 1);

              indent (indentation_level + 1);
              printf ("- key %d\n", *internal_node_key (node, i));
            }
          child = *internal_node_right_child (node);
          print_tree (pager, child, indentation_level + 1);
        }
      break;
    }
}

void
print_constants (void)
{
  printf ("ROW_SIZE: %d\n", ROW_SIZE);
  printf ("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
  printf ("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
  printf ("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
  printf ("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
  printf ("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

MetaCommandResult
do_meta_command (InputBuffer *input_buffer, Table *table)
{
  if (strcmp (input_buffer->buffer, ".exit") == 0)
    {
      db_close (table);
      exit (EXIT_SUCCESS);
    }
  else if (strcmp (input_buffer->buffer, ".btree") == 0)
    {
      puts ("Tree:");
      print_tree (table->pager, 0, 0);
      return META_COMMAND_SUCCESS;
    }
  else if (strcmp (input_buffer->buffer, ".constants") == 0)
    {
      puts ("Constants:");
      print_constants ();
      return META_COMMAND_SUCCESS;
    }
  else
    {
      return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
