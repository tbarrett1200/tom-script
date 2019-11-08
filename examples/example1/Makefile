BLC = ../../bin/tomscript
LD_FLAGS = -e _main -macosx_version_min 10.13 -lSystem -lc

all: functions

functions: functions.o
	ld $^ ${LD_FLAGS} -o $@

functions.o: functions.bul
	${BLC} $^ -o $@ 

clean:
	rm functions.o functions