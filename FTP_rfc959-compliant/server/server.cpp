// header files - server.h
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

#define PR(x) cout << #x << " = " << x << "\n";
#define DEBUG 1
#define LOG 1
#define BACKLOG 100	// No. of backlog reqeusts
#define BUFSIZE 2048 // BufferSize

char *FTP_SERVER_CONTROL_PORT;
char *FTP_SERVER_DATA_PORT;


// utils - next 3 functions - util/trim.cpp

// trim from start
static inline std::string &ltrim(std::string &s){
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s){
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s){
	return ltrim(rtrim(s));
}

/********************************** FUNCTIONS ***********************************/

/**VISITED.
 * This is the normal BIND AND LISTEN. 
 * CAN BE BROKEN IN 2 Parts. Code from socket-chat can be used.
 * Return sockfd
 * 
 * server_listen - bind to the supplied port and listen
 * @param  char* port - a string
 * @return int the fd if no error otherwise <0 value which indicates error
 */
int server_listen(const char *port)
{
	// Create address structs
	struct addrinfo hints, *res;
	int sock_fd;
	// Load up address structs
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
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

	// Start listening
	if (listen(sock_fd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	return sock_fd;
}

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

/**VISITED. Should this be eliminated ?? Maybe. Not required in any near future.
 * Because i'm gonna use AF_INET
 * When the world is ready for migration. We'll modify the code at once to support IPv4
 * 
 * A function wrapper to wrap both IPv4 and IPv6
 * @param  struct sockaddr *sa
 */
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

/**VISITED. Can be used as it is.
 * Accepts a client connection. The server fd is passed as a para
 * @param  server_fd
 * @return client_fd
 */
int accept_connection(int server_fd){
	struct sockaddr_storage their_addr; // connector's address information
	char s[INET6_ADDRSTRLEN];
	socklen_t sin_size = sizeof their_addr;

	// Accept connections
	int client_fd = accept(server_fd, (struct sockaddr *)&their_addr, &sin_size);
	if (client_fd == -1)
	{
		perror("accept");
		return -1;
	}

	// Print out IP address. Getting IP address of client
	// remove get_in_addr by standard stuff
	inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
	printf("server: got connection from %s\n", s);
	
	// Setting Timeout. WHY ? Required ? Can be removed.
	struct timeval tv;
	tv.tv_sec = 120; /* 120 Secs Timeout */ // default is 60 seconds
	tv.tv_usec = 0;	// Not init'ing this can cause strange errors
	setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	return client_fd;
}

/**VISITED. Server initiates the connection on given port.
 * connects to remote host using supplied sock_fd.
 * @param const char *host  - Host's domain name or IP address 
 * @param const char *port - The port to which we have to make connection. 
 * @returns fd of socket, <0 if error
 */
int make_client_connection_with_sockfd(int sock_fd, const char *host, const char *port){
	
	// new
	// 	stringstream strValue;
	// 	strValue << port;

	// 	int intport;
	// 	strValue >> intport;

	// 	struct sockaddr_in server_addr;
	//   	server_addr.sin_family = AF_INET;
	//   	server_addr.sin_port = htons(intport);

	//   inet_pton(AF_INET, host, &(server_addr.sin_addr));
		cout << "\nhost is " << host << " and port is " << port << "\n";
	//   const int res = connect(
	//     sock_fd,
	//     reinterpret_cast<struct sockaddr*>(&server_addr),
	//     sizeof(server_addr));
	//   if (res == -1) {
	//    	cout << "FUCK Connect Error " << strerror(errno) << endl;
	// 		return -1;
	//   }

	// old
	struct addrinfo hints, *res;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	cout << "sock fd in make_client_connection_with_Sockfd is  " << sock_fd  << "\n";
	
	// pinpointed here - next 2 lines are causing - meh
	getaddrinfo(host, port, &hints, &res);
	// connect!
	// cout << "Requested address is " << res->ai_addr->sa_data << endl;
	int stat = connect(sock_fd, res->ai_addr, res->ai_addrlen);
	if (stat == -1)
	{
		cout << "Connect Error " << strerror(errno) << endl;
		return -1;
	}

	return sock_fd;
}

/**VISITED. This function is doing a good Job.
 * 
 * Modify the wrapper initials to make work easier for us. 
 * OR maybe make a wrapper for this wrapper. Name - Send
 * Usage : Send(sockfd, "Initialising server");
 * 
 * A wrapper function on send() socket all which tries to send all the data that is in the buffer
 * @param  int socket
 * @param  const void *buffer
 * @param  size_t length
 * @return
 */
int send_all(int socket, const void *buffer, size_t length){
	size_t i = 0;
	for (i = 0; i < length;){
		int bytesSent = send(socket, buffer, length - i, MSG_NOSIGNAL);
		if (bytesSent == -1){
			return errno;
		}
		else{
			i += bytesSent;
		}
	}
	return 0;
}


/**VISITED. Nice function.
 * It receives the data from serverfd till socket is closed and writes the data to the given file.
 * @param int serverfd 
 * @param FILE - file descriptor of the file created
 * @param  result   sets the output to result
 * @return          status code
 */

int recvallbinary(int serverfd, FILE *fd){
	unsigned char buf[10001];
	int bytesRead = 0;
	int len = 0;
	while ((bytesRead = recv(serverfd, buf, 10000, 0)) > 0){
		len += bytesRead;
		fwrite(buf, 1, bytesRead, fd);
	}
	if (bytesRead < 0){
		cerr << "Error Occurred";
		return -1;
	}
	else{
		return len;
	}
}

/**VISITED. Almost perfect. Just a lil housekeeping is required.
 * A wrapper function on send() socket all which tries to send the supplied file in binary mode.
 * @param  int serverfd
 * @param  FILE *fd - The file descriptor of the file to be sent
 * @param  int size
 * @return -1 on error 0 on success
 */
int sendallbinary(int serverfd, FILE *fd, int size){
	unsigned char buf[100001];
	int bytesSent = 0;
	while (size > 0)
	{
		int bytesRead = fread(buf, 1, 100000, fd);
		int stat = send_all(serverfd, buf, bytesRead);
		if (stat != 0)
		{
			cout << "ERROR IN SENDING" << endl;
			return -1;
		}
		size = size - bytesRead;
	}
	return 0;
}

string remBuf;
/**VISITED. Nice function.
 * 
 * Receives one line(upto /r/n ) from serverfd.
 * @param  serverfd 
 * @param  result   - the received one line is assigned to result
 * @return  -1 on error, 0 on success.
 */
int recvoneline(int serverfd, string &result){
	
	// transfer this constant to CONSTANTS
	char buf[1001];

	int bytesRead = 0;

	// Notice how important is 'remBuf' here.
	// try to remove it from global handler and make part of the function somehow.
	result = remBuf;

	// can be converted to a simple while loop
	// SHIFT "\r\n" to LINE_DELIMETER constant
	do{
		result += string(buf, buf + bytesRead);
		int pos = result.find("\r\n");
		if (pos != string::npos)
		{
			//found
			remBuf = result.substr(pos + 2);
			result = result.substr(0, pos + 2);
			break;
		}
	} while ((bytesRead = recv(serverfd, buf, 1000, 0)) > 0);

	if (bytesRead < 0){
		cerr << "Error Occurred";
		return -1;
	}
	else{
		return 0;
	}
}

/**VISITED. Can be probably simplified.
 * 
 * Converts PORT command format to nice and good variables.
 * 
 * Gives the ip and port from the standard PORT command.
 * For eg. in = "PORT 127,0,0,1,35,40"
 * 			gives ipstr = "127.0.0.1" , portstr = "9000" (35*256 + 40)
 * @param  in      - input PORT command string
 * @param  ipstr   - output ip
 * @param  portstr - output port
 * @return -1 on error, 0 on success
 */
int reverseportstring(string &in, string &ipstr, string &portstr){
	int cnt = 0, pos;
	string ip = in;
	for (int i = 0; i < in.size(); ++i)
	{
		if (ip[i] == ',')
		{
			ip[i] = '.';
			cnt++;
			if (cnt == 4)
			{
				pos = i;
				break;
			}
		}
	}

	if (cnt != 4)
		return -1;
		
	ipstr = ip.substr(0, pos);

	/*
	Work in Progress [ WIP ]

	this works with 2 VMs because both have static IPs
	Fails in my router because it does not know how to route this from router to my system
	it will work from my system to a VM when telnet works from VM to my system
	*/
	// ipstr = "35.200.210.239";	// Google cloud
	// ipstr = "103.78.148.10";		// my system's IP for now
	// ipstr = "139.59.79.252";		// Digital Ocean
	// Use `curl ifconfig.me` to find IP address on the client machine

	string port = ip.substr(pos + 1);
	int val = 0;
	int i = 0;

	while (i < port.size())
	{
		if (port[i] == ',')
			break;
		val = 10 * val + (port[i] - '0');
		i++;
	}
	val = 256 * val;
	int portval = val;
	val = 0;
	i++;
	while (i < port.size())
	{
		val = 10 * val + (port[i] - '0');
		i++;
	}

	portval = portval + val;
	stringstream ss;
	ss << portval;
	portstr = ss.str();
	return 0;
}

/**VISITED. Works Perfectly.
 * 
 * can also be used to create and delete directories.
 * executes a command and returns its output
 * @param const char *cmd  - command to execute
 * @return The output of the command
 */
string exec(const char *cmd){

	FILE *pipe = popen(cmd, "r");
	if (!pipe)
		return "ERROR";
	char buffer[128];
	std::string result = "";
	while (!feof(pipe))
	{
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

/**VISITED
 * Function name needs renaming
 * 
 * The main function handling FTP
 * @param clientControlfd - clients control socket fd
 */
void doftp(int clientControlfd){

	// I am now inside the server

	// Will be replaced by Send(clientcontrolfd, "blah blah");
	// be careful of the carriage return and newline
	string progver = "220 (ftpserver0.1)\r\n";
	send_all(clientControlfd, progver.c_str(), progver.size());


	// Handling dummy authentication (This is done to conform to the RFC 959)
	
	/*
		Implement authentication as commands. and do not give unathenticated access.
		Maybe abstract this entire authentication part in a single function.

	*/
	// receive username from client
	string username;
	recvoneline(clientControlfd, username);

	if (username.compare(0, strlen("USER"), "USER") == 0){
		//do authentication
		string res = "331 Please specify password\r\n";
		send_all(clientControlfd, res.c_str(), res.size());
		string password;

		recvoneline(clientControlfd, password);
		// do pass authentication.
		res = "230 Login Successfuly\r\n";
		send_all(clientControlfd, res.c_str(), res.size());
	}
	else{
		string res = "430 Invalid Username\r\n";
		send_all(clientControlfd, res.c_str(), res.size());
	}

	/* *********** AUTHENTICATION IS NOW DONE **************** */
	
	// clientDatafd is for data connection, datasocket is the socketfd of the data server
	int clientDatafd = 0, datasocket = 0; 
	// For binary mode
	int binarymode = 0;										

	while (1){
		string command;
		
		// receive command from client -> by getting one line from socket
		// Size of that one line is hardcoded for now. Shift to constant.
		recvoneline(clientControlfd, command);
		// Also make a filter for command.
		/* LOGIC : Make a enum for commands supported.
			Make a function that returns the command used in current line
			use enum in switch case because that's just an integer

			Also make the actual line available, because we get arguments etc with command usually.
			// SHIFT THis if else river to a switch block using enum syntax
		*/
		// not required. Remove maybe
		if (command.compare(0, strlen("SYST"), "SYST") == 0){
			// simpleStringSend
			string res = "215 UNIX Type: L8\r\n";
			send_all(clientControlfd, res.c_str(), res.size());
		}
		/*	PORT h1,h2,h3,h4,p1,p2
			Binds server to supplied data port
			and client to supplied port by PORT command
			next connections will occur between these two dudes
			@todo figure out shit
			IT is OK to not use it in normal circumstances
			But we are following a convention where control and data ports are different.

			It is not compulsory for user to provide this.
			But in the backend. It is always used, either with default parameters, or with
			explicit parameters
		*/
		else if (command.compare(0, strlen("PORT"), "PORT") == 0){
			string portstr = command.substr(4); // Getting the ip-port string
			portstr = trim(portstr);
			string ip, port;
			reverseportstring(portstr, ip, port); // Parsing the string.

			// binds a socket with server's data port
			// i am a server, i want to transfer data
			// create a socket for me on FTP_SERVER_DATA_PORT -> was supplied by user
			// default is next to control-port [9000+1]
			datasocket = bindsocket(FTP_SERVER_DATA_PORT); 
			cout << "Data socket is " << datasocket << "\n";
			// then uses this socket to connect to remote host on a specific port

			// make connection with client using this datasocket 
			// and at this Ip and port. Here you will find receiver
			clientDatafd = make_client_connection_with_sockfd(datasocket, ip.c_str(), port.c_str());
			cout << "client Data FD is " << clientDatafd << "\n";
			string res = "200 PORT command successful\r\n";
			send_all(clientControlfd, res.c_str(), res.size());
		}
		// similar to `ls` command on linux system 
		else if (command.compare(0, strlen("LIST"), "LIST") == 0){
			if (clientDatafd > 0)
			{
				string res = "150 Here comes the directory listing.\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
				
				res = exec("ls -l");
				send_all(clientDatafd, res.c_str(), res.size());
				
				// Make wrappers - Close
				// wrapper will do the resetting work
				close(clientDatafd);
				close(datasocket);
				
				// close data FD once work is done
				clientDatafd = 0;
				datasocket = 0;
				
				res = "226 Directory send OK.\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
			}
			else
			{
				string res = "425 Use PORT\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
			}
		}
		// simple pwd
		else if (command.compare(0, strlen("PWD"), "PWD") == 0){
			char cwd[1024];
			getcwd(cwd, sizeof(cwd));
			string res(cwd);
			res = "257 " + res + "\r\n";
			send_all(clientControlfd, res.c_str(), res.size());
		}
		// simple cd
		else if (command.compare(0, strlen("CWD"), "CWD") == 0){
			string path = command.substr(3);
			path = trim(path);
			int stat = chdir(path.c_str());
			if (stat == 0)
			{
				string res = "250 Directory successfully changed.\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
			}
			else
			{
				string res = "550 Failed to change directory.\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
			}
		}
		// regular mkdir command
		else if (command.compare(0, strlen("MKD"), "MKD") == 0){
			string path = command.substr(3);
			path = trim(path);
			int stat = mkdir(path.c_str(), 0775);
			if( stat!=0){
				if(errno == EEXIST){
					string res = "Directory already exists. \r\n";
					send_all(clientControlfd, res.c_str(), res.size());
				}
				else{
					string res = "Failed to create directory.\r\n";
					send_all(clientControlfd, res.c_str(), res.size());	
				}
			}
			else{
				string res = "Directory " + path + " successfully created." + "\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
			}
		}
		// regular rmd command - removes empty directories
		else if (command.compare(0, strlen("RMD"), "RMD") == 0){
			string path = command.substr(3);
			path = trim(path);
			int stat = rmdir(path.c_str());
			if(stat!=0){
				if(errno == EEXIST || errno == ENOTEMPTY){
					string res = "Directory not empty. Can't be deleted. \r\n";
					send_all(clientControlfd, res.c_str(), res.size());
				}
				else if(errno == ENOTDIR){
					string res = "Not a directory. Can't be deleted. \r\n";
					send_all(clientControlfd, res.c_str(), res.size());
				}
				else{
					string res = "Failed to delete directory.\r\n";
					send_all(clientControlfd, res.c_str(), res.size());	
				}
			}
			else{
				string res = "Directory " + path + " successfully deleted." + "\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
			}
		}
		// switching to Binary Mode
		else if (command.compare(0, strlen("TYPE I"), "TYPE I") == 0){
			string res = "200 Switching to Binary mode.\r\n";
			binarymode = 1;	// is a dummy variable
			send_all(clientControlfd, res.c_str(), res.size());
		}
		else if (command.compare(0, strlen("RETR"), "RETR") == 0){
			// should not be compulsory ideally. But maybe it is compulsory in our tactics
			if (clientDatafd <= 0){
				string res = "425 Use PORT first\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
				continue;
			}

			string path = command.substr(4);
			path = trim(path);

			// Getting the filesize of the supplied file
			struct stat st;
			int statcode = stat(path.c_str(), &st);
			int size = st.st_size;
			if (statcode == -1)
			{
				close(clientDatafd);
				close(datasocket);
				clientDatafd = 0;
				datasocket = 0;
				string res = "550 " + string(strerror(errno)) + "\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
				binarymode = 0;
				continue;
			}

			string res = "150 Opening BINARY mode data connection for " + path + "\r\n";
			send_all(clientControlfd, res.c_str(), res.size());

			FILE *filew;
			int numw;
			filew = fopen(path.c_str(), "rb");
			int len = sendallbinary(clientDatafd, filew, size);
			cout << "Bytes Sent : " << size << endl;
			fclose(filew);
			close(clientDatafd);
			close(datasocket);
			clientDatafd = 0;
			datasocket = 0;
			res = "226 Transfer complete.\r\n";
			send_all(clientControlfd, res.c_str(), res.size());
			binarymode = 0;
		}
		else if (command.compare(0, strlen("STOR"), "STOR") == 0){
			
			// PORT is useless here. Server isn't sending anything
			// PASV command makes more sense here.
			if (clientDatafd <= 0)
			{
				string res = "425 Use PORT first\r\n";
				send_all(clientControlfd, res.c_str(), res.size());
				continue;
			}

			string path = command.substr(4);
			path = trim(path);

			string res = "150 Ok to send data.\r\n";
			send_all(clientControlfd, res.c_str(), res.size());

			FILE *filew;
			int numw;

			filew = fopen(path.c_str(), "wb");
			int len = recvallbinary(clientDatafd, filew);
			cout << "Bytes Received : " << len << endl;
			fclose(filew);

			close(clientDatafd);
			close(datasocket);
			clientDatafd = 0;
			datasocket = 0;
			res = "226 Transfer complete.\r\n";
			send_all(clientControlfd, res.c_str(), res.size());
			binarymode = 0;
		}
		// quit. This should ideally only close client connection and not server process.
		else if (command.compare(0, strlen("QUIT"), "QUIT") == 0){
			// simpleStringSend
			string res = "221 Goodbye.\r\n";
			send_all(clientControlfd, res.c_str(), res.size());
			// ? wait what ?
			close(clientDatafd);
			close(datasocket);
			close(clientControlfd);
			return;
		}
		// unknown command
		else{
			// simpleStringSend
			string res = "500 Unknown command.\r\n";
			send_all(clientControlfd, res.c_str(), res.size());
		}
	}	// end:while
}

// VISITED.
int main(int argc, char **argv){

	// get host and porn from command line arguments
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
	// Can be replaced by separated bind and listen from socket-chat code
	int server_fd;
	if ((server_fd = server_listen(FTP_SERVER_CONTROL_PORT)) < 0){
		fprintf(stderr, "%s\n", "Error listening to given port");
		return 0;
	}
	cout << "Server Listening at " << FTP_SERVER_CONTROL_PORT << endl;
	// now start accept incoming connections:

	// Everything below this can be transferred to one single function.
	// FTP_Server()
	while (1){
		// ACCEPT can be abstracted as in sc
		int clientControlfd;
		if ((clientControlfd = accept_connection(server_fd)) < 0)
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
