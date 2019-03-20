TARGET=client server 
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
client: client-side/client.c
	$(CC) $(CFLAGS) client-side/client.c -o client-side/client
server: server-side/server.c
	$(CC) $(CFLAGS) server-side/server.c -o server-side/server
infinite-server: server-side/infinite-server.c
	$(CC) $(CFLAGS) server-side/infinite-server.c -o server-side/infinite-server
clean:
	$(RM) $(TARGET)