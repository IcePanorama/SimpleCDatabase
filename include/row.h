#ifndef _ROW_H_
#define _ROW_H_

#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;  // need to def row before including statement

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#include "table.h"

void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* row_slot(Table* table, uint32_t row_num);
void print_row(Row* row);

#endif
