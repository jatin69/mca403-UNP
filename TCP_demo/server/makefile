TARGET=server infinite-server
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
server: server.c
	$(CC) $(CFLAGS) server.c -o server
infinite-server: infinite-server.c
	$(CC) $(CFLAGS) infinite-server.c -o infinite-server
clean:
	$(RM) $(TARGET)