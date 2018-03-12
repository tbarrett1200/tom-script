DIR = $(patsubst src/%, obj/%, $(wildcard src/**))
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

SRC_OBJ = $(wildcard ../obj/AST/*.o) $(wildcard ../obj/Basic/*.o) $(wildcard ../obj/Parse/*.o)


$(shell mkdir -p $(DIR))
$(shell mkdir bin > /dev/null)

CXX = clang++
CXXFLAGS = -std=c++1z -c -g -Wall -I ../include

all: $(OBJ)
	clang++ -std=c++1z -g $^ $(SRC_OBJ) -lpthread -lgtest -lgtest_main -o bin/test

obj/%.o: src/%.cpp
	$(CXX) $< $(CXXFLAGS) -o $@

clean:
	rm -r obj
	rm -r bin
