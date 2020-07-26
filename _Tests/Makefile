EXEC_NAME:=tests
LINKER:=g++
CC:=gcc
CXX=g++
CXXFLAGS:=-g -std=c++11 -Wall
LDFLAGS:=-L./googletest/lib -lgtest -lgtest_main -pthread 
INCLUDE:=-I../src/include -I./googletest/googletest/include -I./googletest/googlemock/include

SRCS:= $(wildcard *.cpp)
OBJS:= $(patsubst %.cpp,%.o,$(SRCS))

.PHONY: clean run 
  
$(EXEC_NAME): $(OBJS)
	$(LINKER) $^ $(LDFLAGS) -o  $@

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.c 
	$(CC) $(INCLUDE) -c $< -o $@


run_tests: $(EXEC_NAME)
	./$^

info:
	$(info $(SRCS))
	$(info $(OBJS))

clean:
	rm -rf *.o $(EXEC_NAME) out.jpg out.png

