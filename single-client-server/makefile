EXEC_EXT = out
RUN = ./

SRC_DIR = src
OBJ_DIR = obj

make server: server.$(EXEC_EXT)
	$(RUN)server.$(EXEC_EXT)

make client: client.$(EXEC_EXT)
	$(RUN)client.$(EXEC_EXT)

server.$(EXEC_EXT): $(OBJ_DIR)/server.o
	gcc $^ -o $@ $(LIBS)

client.$(EXEC_EXT): $(OBJ_DIR)/client.o
	gcc $^ -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	gcc -c $^ -o $@ $(LIBS)