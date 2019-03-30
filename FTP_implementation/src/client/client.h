#ifndef SRC_CLIENT_CLIENT_H_
#define SRC_CLIENT_CLIENT_H_

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
extern int debug;
extern string remBuf; // to handle buffer

int Socket();
void Bind(int sockfd, int port);
void Listen(int sockfd, int backlog);
int Accept(int sockfd);

// below function can be removed
// int server_listen(const char *port);


void *get_in_addr (struct sockaddr *sa);

int accept_connection(int server_fd);
int make_client_connection (const char *host, const char *port);
int Send(int sockfd, string msg);
int send_all(int socket,const void *buffer, size_t length);
int send_all_binary(int serverfd, FILE *fd,int size);
int recv_all(int serverfd, string& result);
int Recv(int serverfd, string& result);
int recv_all_binary(int serverfd, FILE *fd);
string exec(const char* cmd);
string int2str(int k);
string getownip(int m_sd);
void getportstring(string ownip,string& portstr, string& port);
void printError();
const string DELIM("\r\n");
const int backlog(5);

// trimming functions
std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);

#endif  // SRC_CLIENT_CLIENT_H_
