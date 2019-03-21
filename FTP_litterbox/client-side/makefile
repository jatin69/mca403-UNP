TARGET=client 
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
client: client.c
	$(CC) $(CFLAGS) client.c -o client
server: server.c
	$(CC) $(CFLAGS) server.c -o server
infinite-server: infinite-server.c
	$(CC) $(CFLAGS) infinite-server.c -o infinite-server
clean:
	$(RM) $(TARGET)