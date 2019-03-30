#include "./server.h"

int Accept(int sockfd) {
  struct sockaddr_in client_addr;
  socklen_t len = static_cast<socklen_t>(sizeof client_addr);
  const int clientSockfd = accept(
      sockfd,
      reinterpret_cast<struct sockaddr*>(&client_addr),
      &len);
  return clientSockfd;
}


// OLD IMPLEMENTATION


// /**VISITED. Can be used as it is.
//  * Accepts a client connection. The server fd is passed as a para
//  * @param  server_fd
//  * @return client_fd
//  */
// int accept_connection(int server_fd){
// 	struct sockaddr_storage their_addr; // connector's address information
// 	char s[INET6_ADDRSTRLEN];
// 	socklen_t sin_size = sizeof their_addr;

// 	// Accept connections
// 	int client_fd = accept(server_fd, (struct sockaddr *)&their_addr, &sin_size);
// 	if (client_fd == -1)
// 	{
// 		perror("accept");
// 		return -1;
// 	}

// 	// Print out IP address. Getting IP address of client
// 	// remove get_in_addr by standard stuff
// 	inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
// 	printf("server: got connection from %s\n", s);
	
// 	// Setting Timeout. WHY ? Required ? Can be removed.
// 	struct timeval tv;
// 	tv.tv_sec = 120; /* 120 Secs Timeout */ // default is 60 seconds
// 	tv.tv_usec = 0;	// Not init'ing this can cause strange errors
// 	setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
// 	return client_fd;
// }
