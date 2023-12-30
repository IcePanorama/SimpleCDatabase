SRC_DIR=src
SRC_FILES=$(SRC_DIR)/*.c
INCL_DIR=include
TARGET=main
# Temporarily not using -Werror
CFLAGS=-Wall -Wpedantic 

all: main

main: 
	gcc $(SRC_FILES) -I./$(INCL_DIR) -o $(TARGET) $(CFLAGS)

clean:
	rm $(TARGET)
