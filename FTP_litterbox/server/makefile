TARGET=server 
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
server: server.c
	$(CC) $(CFLAGS) server.c -o server
clean:
	$(RM) $(TARGET)