libgetos.so:libgetos.o
	gcc -shared libgetos.o -lc -o libgetos.so

libgetos.o:getos.c
	gcc -FPIC -c $^ -o $@

all:libgetos.so

clean:
	@rm -f *.o *.so
