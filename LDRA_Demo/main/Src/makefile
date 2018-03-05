# build an executable named program from program.c
all:
	gcc -c program.c
	gcc program.o -o program

clean:
	if [ -a program.exe ]; then rm program.exe; fi
	if [ -a program.o ]; then rm program.o; fi