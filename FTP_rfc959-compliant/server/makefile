TARGET=server
CC=g++ -std=c++14
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server
clean:
	$(RM) $(TARGET)