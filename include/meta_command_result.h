#ifndef _META_COMMAND_RESULT_H_
#define _META_COMMAND_RESULT_H_

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

MetaCommandResult do_meta_command(InputBuffer* input_buffer);

#endif
