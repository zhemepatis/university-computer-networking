EXEC_EXT = out
RUN = ./
LIBS = -lpthread

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

chat: $(BIN_DIR)/chat-server.$(EXEC_EXT)
	$(RUN)$^

file: $(BIN_DIR)/file-server.$(EXEC_EXT)
	$(RUN)$^

$(BIN_DIR)/chat-server.$(EXEC_EXT): $(OBJ_DIR)/chat-server.o $(OBJ_DIR)/socket.o $(OBJ_DIR)/files.o $(OBJ_DIR)/commands.o $(OBJ_DIR)/name.o | bin
	gcc $^ -o $@ -lpthread

$(BIN_DIR)/file-server.$(EXEC_EXT): $(OBJ_DIR)/file-server.o $(OBJ_DIR)/socket.o $(OBJ_DIR)/files.o $(OBJ_DIR)/commands.o | bin
	gcc $^ -o $@ -lpthread

$(BIN_DIR)/%.$(EXEC_EXT): $(OBJ_DIR)/%.o | bin
	gcc $^ -o $@ -lpthread

bin: 
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc -c $^ -o $@

obj: 
	mkdir -p $@