#include <stdio.h>
#include <string.h>

#include "input_buffer.h"
#include "statement.h"

PrepareResult 
prepare_statement(InputBuffer* input_buffer, Statement* statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS; 
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void 
execute_statement(Statement* statement)
{
    switch (statement->type)
    {
        case (STATEMENT_INSERT):
            puts("This is where we would do an insert.");
            break;
        case (STATEMENT_SELECT):
            puts("This is where we would do a select.");
            break;
    }
}
