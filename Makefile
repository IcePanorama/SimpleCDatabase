SRC_DIR=src
SRC_FILES=$(SRC_DIR)/*.c
INCL_DIR=include
INCL_FILES=$(INCL_DIR)/*.h
TARGET=database
# Temporarily not using -Werror
CFLAGS=-Wall -Wpedantic
FORMAT=GNU

all: format $(TARGET) test

$(TARGET): 
	gcc $(SRC_FILES) -I./$(INCL_DIR) -o $(TARGET) $(CFLAGS)

test: $(TARGET)
	behave

clean:
	rm $(TARGET)

format:
	clang-format -style=GNU -i $(SRC_FILES) $(INCL_FILES)
