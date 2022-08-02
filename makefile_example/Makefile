SRC:=test.c
OBJ:=test
LIBPATH=./libgetos
LIB=getos
CFLAGS+=-Wall -O -g

all:${SRC}
	@cd ${LIBPATH} && make all
	gcc ${CFLAGS} -o ${OBJ} ${SRC} -L${LIBPATH} -l${LIB}

clean:
	@rm -f ${OBJ}
	@cd ${LIBPATH} && make clean
