EXEC_EXT = out
RUN = ./

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

make server: $(BIN_DIR)/server.$(EXEC_EXT)
	$(RUN)$^

make client: $(BIN_DIR)/client.$(EXEC_EXT)
	$(RUN)$^

$(BIN_DIR)/%.$(EXEC_EXT): $(OBJ_DIR)/%.o | bin
	gcc $^ -o $@

bin: 
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc -c $^ -o $@

obj: 
	mkdir -p $@