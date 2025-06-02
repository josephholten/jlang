AS=as
LD=ld

all: main test

main: main.c
	$(CC) main.c -o main

test: test.s
	$(AS) test.s -o test.o
	$(LD) test.o -o test
