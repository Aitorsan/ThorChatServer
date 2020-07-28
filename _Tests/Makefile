EXEC_NAME:=tests
LINKER:=g++
CC:=gcc
CXX=g++
CXXFLAGS:=-g -std=c++11 -Wall
LDFLAGS:=-L./googletest/lib -lgtest -lgtest_main -pthread 
INCLUDE:=-I../src/include -I./googletest/googletest/include -I./googletest/googlemock/include -I../src/3dparty


SRCS:= $(wildcard *.cpp)
OBJS:= $(patsubst %.cpp,%.o,$(SRCS))

.PHONY: clean run 

$(EXEC_NAME): $(OBJS) *.o
	$(LINKER) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $^ -o $@

%.o: %.c 
	$(CC) $(INCLUDE) -c $< -o $@

compile_test_cpps:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(TEST_SRC)

compile: $(EXEC_NAME)

run_tests: $(EXEC_NAME)
	./$^

info:
	$(info $(SRCS))
	$(info $(OBJS))
	$(info $(TEST_SRC))

clean:
	rm -rf *.o $(EXEC_NAME) out.jpg out.png

clean_objs:
	rm -rf *.o out.jpg out.png