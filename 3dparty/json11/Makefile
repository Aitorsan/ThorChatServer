

# Build and put json11 obj file in the temp dir to be linked against the executable

CXX:=g++
CC:=gcc
CXX=g++
CXXFLAGS:=-std=c++11 -Wall
INCLUDE:=-I./include
OBJ_DIR:=../../temp
SRC:=json11.cpp
OBJ_NAME:= json11.o

$(OBJ_NAME): $(SRC)
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $^ -o $(OBJ_DIR)/$@