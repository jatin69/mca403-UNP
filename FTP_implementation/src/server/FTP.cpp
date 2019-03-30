#include "./server.h"


/**VISITED
 * CLEAN IT ALL UP WITH ENUMS
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

	if(AuthenticateUser(clientControlfd) == 0){
		throw std::runtime_error("Failed authentication");
	}

	/* *********** AUTHENTICATION IS NOW DONE **************** */
	
		// commands work like this

	string cmdString = "NOOP\r\n";
	cmd::Command type = getCommandType(cmdString);
	// cout << "got " << type << "\n";
	switch(type) {
		case cmd::NOOP : {
			cmd_NOOP();
			printError();
		}break;
		default : {
			cout << "Weird";
		}break;
	}

	// clientDatafd is for data connection, datasocket is the socketfd of the data server
	int clientDatafd = 0, datasocket = 0; 
	// For binary mode
	int binarymode = 0;										

	while (1){
		string command;
		
		// receive command from client -> by getting one line from socket
		// Size of that one line is hardcoded for now. Shift to constant.
		Recv(clientControlfd, command);
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
			
			datasocket = Socket();
  			Bind(datasocket, atoi(FTP_SERVER_DATA_PORT));
			// datasocket = bindsocket(FTP_SERVER_DATA_PORT); 
			
			cout << "Data socket is " << datasocket << "\n";
			// then uses this socket to connect to remote host on a specific port

			// make connection with client using this datasocket 
			// and at this Ip and port. Here you will find receiver
			// @todo : done
			// clientDatafd = make_client_connection_with_sockfd(datasocket, ip.c_str(), port.c_str());
			clientDatafd = Connect(datasocket, ip.c_str(), atoi(port.c_str()));
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
			
			filew = fopen(path.c_str(), "rb");
			int len = send_all_binary(clientDatafd, filew, size);
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
			

			filew = fopen(path.c_str(), "wb");
			int len = recv_all_binary(clientDatafd, filew);
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


