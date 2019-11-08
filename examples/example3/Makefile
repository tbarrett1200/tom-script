BLC = ../../bin/tomscript
LD_FLAGS = -e _main -macosx_version_min 10.13 -lSystem -lc

all: array

array: array.o
	ld $^ ${LD_FLAGS} -o $@

array.o: array.bul
	${BLC} $^ -o $@

clean:
	rm array.o array