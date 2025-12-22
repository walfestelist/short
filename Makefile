CC = gcc
CFLAGS = -O2
LDFLAGS = 

SRC_DIR = src
OBJ_DIR = obj
MAIN_SRC = main.c
EXEC = short

SRCS = $(wildcard $(SRC_DIR)/*.c)
ALL_SRCS = $(MAIN_SRC) $(SRCS)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) \
       $(patsubst %.c, $(OBJ_DIR)/%.o, $(MAIN_SRC))

all: $(EXEC)

$(EXEC): $(OBJS)
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

run: $(EXEC)
	@clear
	@./$(EXEC)

clean:
	@rm -rf $(OBJ_DIR) $(EXEC)

debug: CFLAGS += -g -O0
debug: $(EXEC)

info:
	@echo "Source files:"
	@echo $(ALL_SRCS)
	@echo ""
	@echo "Object files:"
	@echo $(OBJS)

install:
	@sudo cp $(EXEC) /usr/local/bin/$(EXEC)
	@echo "Installed at /usr/local/bin/$(EXEC)"

.PHONY: all clean run debug info
