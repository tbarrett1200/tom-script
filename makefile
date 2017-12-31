src = $(wildcard src/*.cpp)
obj = $(patsubst src/%.cpp, obj/%.o, $(src))
inc = $(wildcard include/*.h)

cc = clang++

project: $(obj)
	$(cc) $^ -o $@

obj/%.o: src/%.cpp $(inc)
	$(cc) $< -c -std=c++11 -o $@ -Iinclude

clean:
	rm obj/*.o
