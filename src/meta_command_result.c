#include <stdlib.h>
#include <string.h>

#include "input_buffer.h"
#include "meta_command_result.h"
#include "table.h"

MetaCommandResult 
do_meta_command(InputBuffer* input_buffer, Table* table)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
