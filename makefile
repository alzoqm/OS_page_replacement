a.out : OS4.o
	gcc -o a.out OS4.o

OS4.o : OS4.c
	gcc -c -o OS4.o OS4.c
