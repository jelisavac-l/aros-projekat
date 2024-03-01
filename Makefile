CC=gcc
LIBS=extern/printC.o

server: server.c
	$(CC) -o server server.c uiplusplus.o $(LIBS)

client: client.c
	$(CC) -o client client.c uiplusplus.o $(LIBS)

libs: extern/printC.c
	$(CC) -o extern/printC.o -c extern/printC.c

uipp: uiplusplus.c
	$(CC) -o uiplusplus.o -c uiplusplus.c
