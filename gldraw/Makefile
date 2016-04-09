CFLAGS=`pkg-config --cflags gtkglext-1.0 gtk+-2.0 glew`
LDFLAGS=`pkg-config --libs gtkglext-1.0 gtk+-2.0 glew`
DEBUG=-g
C99=-std=c99

default: gldraw
debug: gldraw-dbg
gldraw: matrix.o utils.o gldraw.o app.o
	gcc $(C99) -o gldraw matrix.o utils.o gldraw.o app.o -lm ${LDFLAGS}
utils.o: gldraw.h matrix.h utils.c
	gcc $(C99) -w -c -o utils.o utils.c ${CFLAGS}
matrix.o: matrix.h matrix.c
	gcc $(C99) -c -o matrix.o matrix.c ${CFLAGS}
gldraw.o: gldraw.h gldraw.c
	gcc $(C99) -c -o gldraw.o gldraw.c ${CFLAGS}
app.o: gldraw.h app.c
	gcc $(C99) -c -o app.o app.c ${CFLAGS}
gldraw-dbg: matrix-dbg.o utils-dbg.o gldraw-dbg.o app-dbg.o
	gcc $(C99) ${DEBUG} -o gldraw-dbg matrix-dbg.o utils-dbg.o gldraw-dbg.o app-dbg.o -lm ${LDFLAGS}
utils-dbg.o: gldraw.h matrix.h utils.c
	gcc $(C99) ${DEBUG} -c -o utils-dbg.o utils.c ${CFLAGS}
matrix-dbg.o: matrix.h matrix.c
	gcc $(C99) -c -o matrix-dbg.o matrix.c ${CFLAGS}
gldraw-dbg.o: gldraw.h gldraw.c
	gcc $(C99) ${DEBUG} -c -o gldraw-dbg.o gldraw.c ${CFLAGS}
app-dbg.o: gldraw.h app.c
	gcc $(C99) ${DEBUG} -c -o app-dbg.o app.c ${CFLAGS}
clean:
	rm *.o
	rm gldraw
	rm gldraw-dbg
install: gldraw
	mv gldraw build/
	rm *.o
