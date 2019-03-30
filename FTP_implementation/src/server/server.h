#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

// @todo : meaning of each header file

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>		 // close() 
#include <errno.h>
#include <sys/wait.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

// quick print
#define PR(x) cout << #x " = " << x << "\n";
#define DEBUG 1
#define LOG 1
#define BACKLOG 100	// No. of backlog reqeusts
#define BUFSIZE 2048 // BufferSize

extern char *FTP_SERVER_CONTROL_PORT;
extern char *FTP_SERVER_DATA_PORT;

int Socket();
void Bind(int sockfd, int port);
void Listen(int sockfd, int backlog);
int Accept(int sockfd);
int Connect(int sockfd, const std::string &host, int port);

int AuthenticateUser(int clientControlfd);
// int server_listen(const char *port);
// int bindsocket(const char *port);
int Recv(int serverfd, string &result);
void *get_in_addr(struct sockaddr *sa);

int accept_connection(int server_fd);
// int make_client_connection_with_sockfd(int sock_fd, const char *host, const char *port);


int recv_all_binary(int serverfd, FILE *fd);
int Send(int sockfd, string msg);
int send_all(int socket, const void *buffer, size_t length);
int send_all_binary(int serverfd, FILE *fd, int size);

int reverseportstring(string &in, string &ipstr, string &portstr);
string exec(const char *cmd);
void doftp(int clientControlfd);
void printError();
const string DELIM("\r\n");
const int backlog(5);

// trimming functions
std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);

/******************** Commands Supported ****************/

namespace cmd {
  enum Command {
    NOOP, SYS,
    PORT, PASV,
    LIST, PWD,
    CWD, 
    MKD, RMD,
    STOR, RETR,
    QUIT, ABOR,
    INVALID, NO_DELIM_FOUND
  };
}

cmd::Command getCommandType(const std::string &msg);

// commands
int cmd_NOOP();


namespace color {
  const std::string red("\033[0;31m");
  const std::string green("\033[1;32m");
  const std::string yellow("\033[1;33m");
  const std::string cyan("\033[0;36m");
  const std::string magenta("\033[0;35m");
  const std::string reset("\033[0m");
}

#endif  // SRC_SERVER_SERVER_H_
