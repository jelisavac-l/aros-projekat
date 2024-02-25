CC=gcc
LIBS=extern/printC.o

server: server.c
	$(CC) -o server server.c $(LIBS)

client: client.c
	$(CC) -o client client.c $(LIBS)

libs: extern/printC.c
	$(CC) -o extern/printC.o -c extern/printC.c
