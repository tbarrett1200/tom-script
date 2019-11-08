BLC = ../../bin/tomscript
LD_FLAGS = -e _main -macosx_version_min 10.13 -lSystem -lc

all: hello_world

hello_world: hello_world.o
	ld $^ ${LD_FLAGS} -o $@

hello_world.o: hello_world.bul
	${BLC} $^ -o $@ > /dev/null

clean:
	rm hello_world.o hello_world