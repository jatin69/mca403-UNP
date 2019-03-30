#include "client.h"

/************** Exactly same as SERVER ************* */

/** VISITED. Same as server
 * A wrapper function on send() socket all which tries to send all the data that is in the buffer
 * @param  int socket
 * @param  const void *buffer
 * @param  size_t length
 * @return
 */
int send_all(int socket,const void *buffer, size_t length) {
    size_t i = 0;
    for (i = 0; i < length;){
    	int bytesSent = send(socket, buffer, length - i,MSG_NOSIGNAL);
    	if(bytesSent==-1){
    		return errno;
    	}else{
    		i+=bytesSent;
    	}
    }
    return 0;
}

int Send(int sockfd, string msg){
	msg += DELIM;
	return send_all(sockfd, msg.c_str(), msg.size());
}

/**VISITED. Same as server
 * Sends the to serverfd using binary mode reading from fd and 
 * @param  serverfd 
 * @param  FILE *fd       
 * @param  int size - size of the file    
 * @return -1 error, 0 success
 */
int send_all_binary(int serverfd, FILE *fd,int size){
	unsigned char buf[100001];
	int bytesSent=0;
	while(size>0){
		int bytesRead = fread(buf,1,100000,fd);
		int stat = send_all(serverfd, buf, bytesRead);
		if(stat != 0 ){
			cout<<"ERROR IN SENDING"<<endl;
			return -1;
		}
		
		size = size - bytesRead;
		
	}
	return 0;	
}

