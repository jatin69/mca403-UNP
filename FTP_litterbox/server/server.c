/*FTP server*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* for read system call*/
#include <unistd.h>

/*for getting file size using stat()*/
#include <sys/stat.h>

/*for sendfile()*/
#include <sys/sendfile.h>

/*for O_RDONLY*/
#include <fcntl.h>

int main(int argc, char *argv[]){

	struct sockaddr_in server, client;
	struct stat obj;

	int sock1, sock2;
	char buf[100], command[5], filename[20];
	int k, i, size, c;
	int filehandle;

	// create a socket for server
	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock1 < 0){
		printf("Socket creation failed");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = INADDR_ANY;
	
	// bind socket to port
	k = bind(sock1, (struct sockaddr *)&server, sizeof(server));
	if (k == -1){
		printf("Binding error");
		exit(1);
	}

	// start listening on this socket
	k = listen(sock1, 1);
	if (k == -1){
		printf("Listen failed");
		exit(1);
	}
	socklen_t len = sizeof(client);
	
	// accept client connections on new socket
	sock2 = accept(sock1, (struct sockaddr *)&client, &len);
	i = 1;

	// infinite running
	while (1){

		recv(sock2, buf, 100, 0);		// read buffer from socket
		sscanf(buf, "%s", command);		// parse string for command

		// if command is `LIST`
		if (!strcmp(command, "ls")){
			system("ls >temps.ftp");
			i = 0;
			stat("temps.ftp", &obj);
			size = obj.st_size;
			send(sock2, &size, sizeof(int), 0);
			filehandle = open("temps.ftp", O_RDONLY);
			sendfile(sock2, filehandle, NULL, size);
		}
		// if commands if `RETR`
		else if (!strcmp(command, "get")){
			sscanf(buf, "%s%s", filename, filename);
			stat(filename, &obj);
			filehandle = open(filename, O_RDONLY);
			size = obj.st_size;
			if (filehandle == -1)
				size = 0;
			send(sock2, &size, sizeof(int), 0);
			if (size)
				sendfile(sock2, filehandle, NULL, size);
		}
		// if command is `STOR`
		else if (!strcmp(command, "put")){
			int c = 0;
			char *f;
			sscanf(buf + strlen(command), "%s", filename);
			recv(sock2, &size, sizeof(int), 0);
			i = 1;
			while (1){
				filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
				if (filehandle == -1){
					sprintf(filename + strlen(filename), "%d", i);
				}
				else{
					break;
				}
			}
			f = malloc(size);
			recv(sock2, f, size, 0);
			c = write(filehandle, f, size);
			close(filehandle);
			send(sock2, &c, sizeof(int), 0);
		}
		// if command is `PWD`
		else if (!strcmp(command, "pwd")){
			system("pwd>temp.ftp");
			i = 0;
			FILE *f = fopen("temp.ftp", "r");
			while (!feof(f))
				buf[i++] = fgetc(f);
			buf[i - 1] = '\0';
			fclose(f);
			send(sock2, buf, 100, 0);
		}
		// if command is `CWD` or `CDUP`
		else if (!strcmp(command, "cd")){
			if (chdir(buf + 3) == 0){
				c = 1;
			}
			else{
				c = 0;
			}
			send(sock2, &c, sizeof(int), 0);
		}
		// if command is `QUIT`
		else if (!strcmp(command, "bye") || !strcmp(command, "quit")){
			printf("FTP server quitting..\n");
			i = 1;
			send(sock2, &i, sizeof(int), 0);
			exit(0);
		}
	}
	return 0;
}