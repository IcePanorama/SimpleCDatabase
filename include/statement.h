#ifndef _STATEMENT_H_
#define _STATEMENT_H_

#include "input_buffer.h"

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_TABLE_FULL
} ExecuteResult;

#include "table.h"

typedef struct {
    StatementType type;
    Row row_to_insert;  // only used by insert statement
} Statement;

ExecuteResult execute_statement(Statement* statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);
ExecuteResult execute_insert(Statement* statement, Table* table);
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement);

PrepareResult prepare_statement(InputBuffer* input_buffer, 
        Statement* statement);

#endif
