BLC = ../../bin/tomscript
LD_FLAGS = -e _main -macosx_version_min 10.13 -lSystem -lc
NAME = struct

all: ${NAME}

${NAME}: ${NAME}.o
	ld $^ ${LD_FLAGS} -o $@

${NAME}.o: ${NAME}.bul
	${BLC} $^ -o $@

clean:
	rm ${NAME}.o ${NAME}