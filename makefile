
CC = clang

# Compilation flags
CFLAGS = -g -O0 -std=c99 -Wall -Wextra -pedantic -I/opt/homebrew/include -Wc23-extensions

# Linker flags
LDFLAGS = -L/opt/homebrew/lib -lraylib -lm \
          -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Final executable
EXEC = $(BIN_DIR)/pomodoro

# Link the executable
$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if missing
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


# Clean command
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: clean

