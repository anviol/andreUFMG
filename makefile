all: cliente servidor

cliente: cliente.o biblioteca.o
	gcc -o cliente cliente.o biblioteca.o

cliente.o: cliente.c
	gcc -c cliente.c

servidor: servidor.o biblioteca.o
	gcc -o servidor servidor.o biblioteca.o

servidor.o: servidor.c	
	gcc -c servidor.c

biblioteca.o: biblioteca.c
	gcc -o biblioteca.o -c biblioteca.c


clear:
	rm cliente
	rm cliente.o
	rm servidor
	rm servidor.o
	rm biblioteca.o