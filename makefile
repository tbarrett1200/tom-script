DIR = $(patsubst src/%, obj/%, $(wildcard src/**))
SRC = $(wildcard src/**/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

$(shell mkdir -p $(DIR))
$(shell mkdir bin > /dev/null)

CXX = clang++
CXXFLAGS = -std=c++14 -c -g -Wall -I include

all: $(OBJ)
	clang++ -std=c++14 -g $^ -o bin/tomscript

obj/%.o: src/%.cpp
	$(CXX) $< $(CXXFLAGS) -o $@

clean:
	rm -r obj
	rm -r bin
