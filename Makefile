SRC_DIR=src
SRC_FILES=$(SRC_DIR)/*.c
INCL_DIR=include
TARGET=database
# Temporarily not using -Werror
CFLAGS=-Wall -Wpedantic 

all: $(TARGET) test

database: 
	gcc $(SRC_FILES) -I./$(INCL_DIR) -o $(TARGET) $(CFLAGS)

test:
	behave

clean:
	rm $(TARGET)
