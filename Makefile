EXEC_NAME:=server
EXEC_DIR:=./bin
LINKER:=g++
CC:=gcc
CXX=g++
CXXFLAGS:= -g -std=c++11 -Wall
LDFLAGS:= -l
SRC_DIR:= ./src
INCLUDE:=-I./src/include
OBJ_DIR:=./temp


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c* $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $^ -o $@

$(EXEC_NAME): $(OBJ_DIR)/*.o $(EXEC_DIR)
	$(LINKER) $< -o $(EXEC_DIR)/$@

$(OBJ_DIR):
	mkdir -p $@

$(EXEC_DIR):
	mkdir -p $@

.PHONY: clean run clean_dirs

run: $(EXEC_NAME)
	 $(EXEC_DIR)/$^

clean:
	rm -rf $(EXEC_DIR)$(EXEC_NAME) $(OBJ_DIR)*.o

clean_dirs:
	rm -rf $(EXEC_DIR) $(OBJ_DIR)
