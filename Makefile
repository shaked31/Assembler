CC = gcc
CFLAGS = -Wall -ansi -pedantic -Iinclude

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

BIN = assembler

VALGRIND = valgrind
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
FILE = tests/test1

.PHONY: clean debug valgrind

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g -O0
debug: clean $(BIN)

valgrind: debug
	$(VALGRIND) $(VFLAGS) ./$(BIN) $(FILE)

clean:
	rm -rf $(OBJ_DIR) $(BIN) *.am *.ob *.ent *.ext