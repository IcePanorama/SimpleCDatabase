#ifndef _META_COMMAND_RESULT_H_
#define _META_COMMAND_RESULT_H_

#include "table.h"

typedef enum
{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

MetaCommandResult do_meta_command (InputBuffer *input_buffer, Table *table);

#endif
