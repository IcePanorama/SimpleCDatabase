#ifndef _LEAF_NODE_H_
#define _LEAF_NODE_H_

#include <stdint.h>

#include "table.h"
#include "node.h"

const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = 
    COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE; 
const uint32_t LEAF_NODE_MAX_CELLS = 
    LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

uint32_t leaf_node_num_cells(void* node);
uint32_t leaf_node_cell(void* node, uint32_t cell_num);
uint32_t leaf_node_key(void* node, uint32_t cell_num);
uint32_t leaf_node_value(void* node, uint32_t cell_num);
void initialize_leaf_node(void* node);

#endif
