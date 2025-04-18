CC := /usr/bin/gcc

all: clean | task1 task2

task1:
	$(CC) -o serverito.o src/serverito.c
task2:
	$(CC) -o cliento.o src/cliento.c

clean: 
	rm -rf *.o
