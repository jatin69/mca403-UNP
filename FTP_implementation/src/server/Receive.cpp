#include "./server.h"

/***************** EXACTLY SAME AS CLIENT **********/

/**VISITED.
 * Used in ls command
 * It receives all the data from the socket buffer
 * @param  serverfd 
 * @param  result   sets the output to result
 * @return          status code
 */

int recv_all(int serverfd, string& result){
	int len=0;
	while(1){
		char buf[10001];
		int bytesRead;
		if((bytesRead = recv(serverfd,buf,10000,0)) >0){
			result += string(buf,buf+bytesRead);
			len+=bytesRead;
		}else if(bytesRead<0){
			return -1;
		}else{
			return len;
		}
	}
}

/**VISITED. Nice function.
 * It receives the data from serverfd till socket is closed and writes the data to the given file.
 * @param int serverfd 
 * @param FILE - file descriptor of the file created
 * @param  result   sets the output to result
 * @return          status code
 */

int recv_all_binary(int serverfd, FILE *fd){
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

string remBuf;
/**VISITED. Nice function.
 * 
 * Receives one line(upto /r/n ) from serverfd.
 * @param  serverfd 
 * @param  result   - the received one line is assigned to result
 * @return  -1 on error, 0 on success.
 */
int Recv(int serverfd, string &result){
	
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
		size_t pos = result.find("\r\n");
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

