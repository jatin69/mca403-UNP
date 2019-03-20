/*  A Simple TCP Server which runs forever

    A simple server in the internet domain using TCP
    The port number is passed as an argument 
    Example usage : ./infinite-server 4545

    Note : Can serve mutliple clients at once by making new process for each request.
*/

// standard header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for read system call 
#include <unistd.h>

/*  Socket Programming Header files
  
    sys/types.h
    This header file contains definitions of a number of data types used in system calls.

    sys/sockets.h
    The header file socket.h includes a number of definitions of structures needed for sockets.

    netinet/in.h
    The header file netinet/in.h contains constants and structures needed for internet domain addresses.
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// constants

#define bufferSize 2048
#define sizeOfBacklogQueue 5
#define defaultPort 4545

void dostuff(int); /* function prototype */

// function to print error and exit
void error(char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){

/*  Port number should be passed as command line argument.
    The user needs to pass in the port number on which the server will accept connections. 
*/
    if (argc < 2){
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    int portno = atoi(argv[1]);

/*  The socket() system call creates a new socket. 

    It takes three arguments.
    The first is the address domain of the socket. Here it is the internet IPv4 domain.
    The second argument is the type of socket. Here it is the stream socket.
    The third argument is the protocol. 0 is passed to let the OS decide the best protocol.

    The socket system call returns an entry into the file descriptor table (i.e. a small integer). 
    This value is used for all subsequent references to this socket.
    If the socket call fails, it returns -1. 
*/
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }

/*  A sockaddr_in is a structure containing an internet address. 
    
    This structure is defined in <netinet/in.h>. Here is the definition:
    struct sockaddr_in {
            short   sin_family;
            u_short sin_port;
            struct  in_addr sin_addr;
            char    sin_zero[8];
    };

    The variable serv_addr is a structure of type struct sockaddr_in. 
    This structure has four fields.

    The first field is short sin_family, which contains a code for the address family. 
    It should always be set to the symbolic constant AF_INET.

    The second field of serv_addr is unsigned short sin_port, which contain the port number. 
    However, instead of simply copying the port number to this field, 
    it is necessary to convert this to network byte order using the function htons() 
    which converts a port number in host byte order to a port number in network byte order.

    The third field of `sockaddr_in` is a structure of type `struct in_addr` 
    which contains only a single field unsigned long s_addr. 
    This field contains the IP address of the host. 
    
    For server code, 
    this will always be the IP address of the machine on which the server is running, 
    and there is a symbolic constant INADDR_ANY which gets this address.

*/
    // The variable serv_addr will contain the address of the server.
    struct sockaddr_in serv_addr; 

    // The function bzero() sets all values in a buffer to zero.
    bzero((char *)&serv_addr, sizeof(serv_addr)); 
    
    // server address family as Internet IPv4
    serv_addr.sin_family = AF_INET;          
    
    // port no. Converted from host byte order to network byte order
    serv_addr.sin_port = htons(portno);

    // Get the IP address of server     
    serv_addr.sin_addr.s_addr = INADDR_ANY;  

/*  The bind() system call binds a socket to an address
    
    In this case the address of the current host and port number on which the server will run.
    It takes three arguments. 
    
    The socket file descriptor, 
    the address to which is bound, 
    and the size of the address to which it is bound. 
    
    The second argument is a pointer to a structure of type sockaddr, 
    but what is passed in is a structure of type sockaddr_in, 
    so this must be cast to the correct type. 
    
    This can fail for a number of reasons, 
    the most obvious being that this socket is already in use on this machine.
*/
    int bindStatus = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if ( bindStatus < 0){
        error("ERROR on binding");
    }

/*  The listen system call allows the process to listen on the socket for connections.

    The first argument is the socket file descriptor, 
    and the second is the size of the backlog queue, i.e., 
    the number of connections that can be waiting while the process is handling a particular connection. 
    This should be set to 5, the maximum size permitted by most systems. 
    
    If the first argument is a valid socket, this call cannot fail, 
    and so the code doesn't check for errors. 
*/
    printf("Server started. \nListening on Port %d\n\n", portno);
    listen(sockfd, sizeOfBacklogQueue);
    
/*  The accept() system call causes the process to block until a client connects to the server. 
    Thus, it wakes up the process when a connection from a client has been successfully established. 
    
    The first argument, 
    is the socket file descriptor. The socket which should be used for connection.

    The second argument,
    is a reference pointer to the address of the client on the other end of the connect0ion, 
    
    The third argument,
    is the size of this structure.

    It returns a new file descriptor, 
    and all communication on this connection should be done using the new file descriptor. 
    
*/
    struct sockaddr_in cli_addr ;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd;
    int pid; 
    
    // run forever
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0){
            error("ERROR on accept");
        }

        // create a new process for this request
        pid = fork();
        if (pid < 0){
            error("ERROR on fork");
        }

        // inside process, serve the client
        if (pid == 0)  {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        }
        else{ 
            close(newsockfd);
        }
    }

    return 0;
}

/*************************** DOSTUFF() **************************
 There is a separate instance of this function for each connection.  
 It handles all communication once a connnection has been established.
 *****************************************************************/
void dostuff (int newsockfd)
{
    /* A client has now successfully connected to our server. */ 
    printf("New Process : A new client with sockfd %d has been connected.\n", newsockfd);
    

    //  The server reads characters from the socket connection into this buffer.
    char buffer[bufferSize];
    bzero(buffer, bufferSize);

/*  Reading from the socket using the fd returned by accept.

    Note that the read call uses the new file descriptor, the one returned by accept(),
    not the original file descriptor returned by socket(). 
    
    Note also that the read() will block until there is something for it to read in the socket, 
    i.e. after the client has executed a write(). 
    It will read either the total number of characters in the socket or 255, whichever is less, 
    and return the number of characters read. 
*/
    int n ;
    n = read(newsockfd, buffer, bufferSize-1);
    if (n < 0){
        error("ERROR reading from socket");
    }
    printf("Here is the message: %s\n", buffer);


/*  Server writing to client. Testing 2-way communication.
*/
    char serverMsg[20] = "I got your message.";
    n = write(newsockfd, serverMsg , strlen(serverMsg));
    if (n < 0){
        error("ERROR writing to socket");
    }

}
