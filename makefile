src = $(wildcard src/*.cpp)
obj = $(patsubst src/%.cpp, obj/%.o, $(src))
inc = $(wildcard include/*.h)

CC = clang++
CFLAGS = -c -std=c++11
HDIR = include

project: $(obj)
	$(CC) $^ -o $@

obj/%.o: src/%.cpp $(inc)
	$(CC) $< $(CFLAGS) -o $@ -I$(HDIR)

clean:
	rm obj/*.o

tst_src = $(wildcard test/src/*.cpp)
tst_obj = $(patsubst test/src/%.cpp, test/obj/%.o, $(tst_src))

test: $(tst_obj) $(obj)
	$(CC) $^ -o -lpthread -lgtest -lgtest_main -o test/$@

test/obj/%.o: test/src/%.cpp $(inc)
		$(CC) $< $(CFLAGS) -o $@ -I$(HDIR)
