#include "./server.h"

char *FTP_SERVER_CONTROL_PORT;
char *FTP_SERVER_DATA_PORT;

// ALL NAMING CONVENTIONS NEEDS TO BE CORRECTED

// VISITED.
int main(int argc, char **argv){

	// get host and port from command line arguments
	if (argc == 3){
		FTP_SERVER_CONTROL_PORT = argv[1];
		FTP_SERVER_DATA_PORT = argv[2];
	}
	else{
		cout << "The format is ./server <control port> <data port>" << endl;
		cout << "If you are using ports <1024 , use sudo ./server <control port> <data port>" << endl;
		exit(0);
	}

	// make server bind and listen to the supplied port

	int sockfd = Socket();
  	Bind(sockfd, atoi(FTP_SERVER_CONTROL_PORT));
  	Listen(sockfd, backlog);
	int server_fd = sockfd;
	
	// Can be replaced by separated bind and listen from socket-chat code
	// int server_fd;
	// if ((server_fd = server_listen(FTP_SERVER_CONTROL_PORT)) < 0){
	// 	fprintf(stderr, "%s\n", "Error listening to given port");
	// 	return 0;
	// }
	// cout << "Server Listening at " << FTP_SERVER_CONTROL_PORT << endl;
	// now start accept incoming connections:

	// Everything below this can be transferred to one single function.
	// FTP_Server()
	while (1){
		// ACCEPT can be abstracted as in sc
		int clientControlfd = Accept(server_fd);
		if (clientControlfd < 0)
		{
			fprintf(stderr, "%s\n", "Error Accepting Connections");
		}
		else
		{
			// creating new process to handle this request.
			int pid = fork();

			if (pid == 0){
				//child
				doftp(clientControlfd);
				close(clientControlfd);
				return 0;
			}
		}
	}	//end:while
	return 0;
}
