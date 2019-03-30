#include "./server.h"

int Connect(int sockfd, const std::string &host, int port) {
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  inet_pton(AF_INET, host.c_str(), &(server_addr.sin_addr));

  const int res = connect(
    sockfd,
    reinterpret_cast<struct sockaddr*>(&server_addr),
    sizeof(server_addr));
  if (res == -1) {
    throw std::runtime_error("Error in making connection");
  }
  return sockfd;
}


// OLD IMPLEMENTATION

// /**VISITED. Server initiates the connection on given port.
//  * connects to remote host using supplied sock_fd.
//  * @param const char *host  - Host's domain name or IP address 
//  * @param const char *port - The port to which we have to make connection. 
//  * @returns fd of socket, <0 if error
//  */
// int make_client_connection_with_sockfd(int sock_fd, const char *host, const char *port){
	
// 	// new
// 	// 	stringstream strValue;
// 	// 	strValue << port;

// 	// 	int intport;
// 	// 	strValue >> intport;

// 	// 	struct sockaddr_in server_addr;
// 	//   	server_addr.sin_family = AF_INET;
// 	//   	server_addr.sin_port = htons(intport);

// 	//   inet_pton(AF_INET, host, &(server_addr.sin_addr));
// 		cout << "\nhost is " << host << " and port is " << port << "\n";
// 	//   const int res = connect(
// 	//     sock_fd,
// 	//     reinterpret_cast<struct sockaddr*>(&server_addr),
// 	//     sizeof(server_addr));
// 	//   if (res == -1) {
// 	//    	cout << "FUCK Connect Error " << strerror(errno) << endl;
// 	// 		return -1;
// 	//   }

// 	// old
// 	struct addrinfo hints, *res;

// 	// first, load up address structs with getaddrinfo():

// 	memset(&hints, 0, sizeof hints);
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;
// 	cout << "sock fd in make_client_connection_with_Sockfd is  " << sock_fd  << "\n";
	
// 	// pinpointed here - next 2 lines are causing - meh
// 	getaddrinfo(host, port, &hints, &res);
// 	// connect!
// 	// cout << "Requested address is " << res->ai_addr->sa_data << endl;
// 	int stat = connect(sock_fd, res->ai_addr, res->ai_addrlen);
// 	if (stat == -1)
// 	{ 
// 		cout << "Connect Error " << strerror(errno) << endl;
// 		return -1;
// 	}

// 	return sock_fd;
// }

