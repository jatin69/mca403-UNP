#include "./server.h"

void Bind(int sockfd, int port) {
  struct sockaddr_in server_addr;
  std::fill_n(reinterpret_cast<char*>(&server_addr), sizeof server_addr, '\0');

  const int reuse = 1;
  if (setsockopt(
      sockfd,
      SOL_SOCKET,
      SO_REUSEADDR,
      reinterpret_cast<const char*>(&reuse),
      sizeof(reuse)) < 0) {
    throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  int res = bind(sockfd,
      reinterpret_cast<struct sockaddr*>(&server_addr),
      sizeof server_addr);
  if (res < 0) {
    printError();
    throw std::runtime_error("Error in binding socket");
  }

}


// OLD IMPLEMENTATION

/**VISITED
 * creates a new socket with supplied port on the server.
 * Usage : when data is to be supplied with new port.
 * 
 * This is very similar to accept connection from client. Figure out diff. and replace shit.
 * Binds to the supplied port.
 * @param char * port 
 * @return socket descriptor
 */
int bindsocket(const char *port){
	// Create address structs
	struct addrinfo hints, *res;
	int sock_fd;
	// Load up address structs
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;		// Shift to AF_INET
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int addr_status = getaddrinfo(NULL, port, &hints, &res);
	if (addr_status != 0)
	{
		fprintf(stderr, "Cannot get info\n");
		return -1;
	}

	// Loop through results, connect to first one we can
	struct addrinfo *p;
	for (p = res; p != NULL; p = p->ai_next)
	{
		// Create the socket
		sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sock_fd < 0)
		{
			perror("server: cannot open socket");
			continue;
		}

		// Set socket options
		int yes = 1;
		int opt_status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (opt_status == -1)
		{
			perror("server: setsockopt");
			exit(1);
		}

		// Bind the socket to the port
		int bind_status = bind(sock_fd, res->ai_addr, res->ai_addrlen);
		if (bind_status != 0)
		{
			close(sock_fd);
			perror("server: Cannot bind socket");
			continue;
		}

		// Bind the first one we can
		break;
	}

	// No binds happened
	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		return -2;
	}

	// Don't need the structure with address info any more
	freeaddrinfo(res);

	return sock_fd;
}
