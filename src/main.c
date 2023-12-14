#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "input_buffer.h"
#include "meta_command_result.h"
#include "table.h"
#include "statement.h"

void print_prompt(void);
void read_input(InputBuffer* input_buffer);

int
main(void)
{
    Table* table = new_table();
    InputBuffer* input_buffer = new_input_buffer();

    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_NEGATIVE_ID):
                puts("ID must be positive.");
                continue;
            case (PREPARE_STRING_TOO_LONG):
                puts("String is too long.");
                continue;
            case (PREPARE_SYNTAX_ERROR):
                puts("Syntax error. Could not parse statement.");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", 
                        input_buffer->buffer);
                continue;
        }

        switch (execute_statement(&statement, table))
        {
            case (EXECUTE_SUCCESS):
                puts("Executed.");
                break;
            case (EXECUTE_TABLE_FULL):
                puts("Error: Table full.");
                break;
        }
    }


    return 0;
}

void 
print_prompt(void)
{
    printf("db > ");
}

void 
read_input(InputBuffer* input_buffer)
{
    ssize_t bytes_read = getline(&(input_buffer->buffer), 
            &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}
