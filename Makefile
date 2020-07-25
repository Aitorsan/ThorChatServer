EXEC_NAME:=server
EXEC_DIR:=./bin
LINKER:=g++
CC:=gcc
CXX=g++
CXXFLAGS:=-g -std=c++11 -Wall
LDFLAGS:=-pthread -ldl
SRC_DIR:=src
INCLUDE:=-I./src/include -I./src/3dparty
OBJ_DIR:=temp

ALLCPPS := $(shell find $(SRC_DIR) -type f -iname "*.cpp")# find all .cpp files
ALLCS := $(shell find $(SRC_DIR) -type f -iname "*.c")# find all .c files
ALLCSOBJS:= $(patsubst %.c,%.o,$(ALLCS))
ALLCPPSOBJS := $(patsubst %.cpp,%.o,$(ALLCPPS))
ALLSOURCES_SUBDIRS := $(shell find $(SRC_DIR) -type d -not -path $(SRC_DIR)/include )
OBJECTS_SUBDIRS := $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(ALLSOURCES_SUBDIRS))

.PHONY: clean run clean_dirs $(OBJECTS_SUBDIRS)
  
$(EXEC_NAME): $(ALLCSOBJS) $(ALLCPPSOBJS) $(EXEC_DIR)
	$(CXX) $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(ALLCPPSOBJS) $(ALLCSOBJS)) $(LDFLAGS) -o  $(EXEC_DIR)/$@

%.o: %.cpp $(OBJECTS_SUBDIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$@)

%.o: %.c $(OBJECTS_SUBDIRS)
	$(CC) $(INCLUDE) -c $< -o $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$@)

$(OBJECTS_SUBDIRS):
	mkdir -p $@

$(EXEC_DIR):
	mkdir -p $@

run: $(EXEC_NAME)
	 $(EXEC_DIR)/$^

dbg:
	$(info $(ALLSOURCES_SUBDIRS))
	$(info $(OBJECTS_SUBDIRS))
	$(info $(ALLCPPSOBJS))
	$(info $(ALLCSOBJS))
	$(info $(ALLCPPS))
	$(info $(ALLCS))

clean:
	rm -rf $(EXEC_DIR)/$(EXEC_NAME) $(OBJ_DIR)

#############################################################
# Build the default  test client ( This client is deprecated)
#############################################################
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