EXEC_NAME:=server
EXEC_DIR:=./bin
LINKER:=g++
CC:=gcc
CXX=g++
CXXFLAGS:=-g -std=c++11 -Wall
LDFLAGS:=-pthread
SRC_DIR:=src
INCLUDE:=-I./src/include -I./3dparty/json11/include
OBJ_DIR:=./temp
3DOBJECTS:= $(OBJ_DIR)/json11.o

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

.PHONY: clean run clean_dirs 
  
$(EXEC_NAME): $(OBJECTS) | $(EXEC_DIR)
	$(CXX) $(LDFLAGS) $(3DOBJECTS) $^ -o $(EXEC_DIR)/$(EXEC_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $^ -o $@

$(OBJ_DIR):
	mkdir -p $@

$(EXEC_DIR):
	mkdir -p $@

run: $(EXEC_NAME)
	 $(EXEC_DIR)/$^

dbg:
	$(info $(SOURCES))

clean:
	rm -rf $(EXEC_DIR)/$(EXEC_NAME) $(OBJ_DIR)/*.o

######################################################
# Build the default client
#####################################################
CLIENT_SRC_DIR:=./client/src
CLIENT_OBJS_DIR:=./client/temp
CLIENT_NAME:=client

CLIENT_SOURCES:= $(wildcard $(CLIENT_SRC_DIR)/*.cpp)
CLIENT_OBJS:=$(patsubst $(CLIENT_SRC_DIR)/%.cpp,$(CLIENT_OBJS_DIR)/%.o,$(CLIENT_SOURCES))

.PHONY: run_client clean_client $(CLIENT_OBJS_DIR)

$(CLIENT_NAME): $(CLIENT_OBJS) | $(EXEC_DIR)
	$(CXX) $(LDFLAGS) $^ -o $(EXEC_DIR)/$@

$(CLIENT_OBJS_DIR)/%.o: $(CLIENT_SRC_DIR)/%.cpp | $(CLIENT_OBJS_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $^ -o $@

$(CLIENT_OBJS_DIR):
	mkdir -p $@

clean_client:
	rm -rf $(EXEC_DIR)/$(CLIENT_NAME) $(CLIENT_OBJS_DIR)

run_client: $(CLIENT_NAME)
	$(EXEC_DIR)/$^

clean_dirs:
	rm -rf $(EXEC_DIR) $(OBJ_DIR) $(CLIENT_OBJS_DIR)