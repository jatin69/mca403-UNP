/*  A Simple TCP Client

    Note : server must be running for this to work.
    A simple client in the internet domain using TCP
    The host and port number is passed as an argument 
    Example usage : ./client localhost 4545
    Works on internet by providing IP and port
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
#include <netdb.h> 

// constants
#define bufferSize 2048
#define defaultHost 127.0.0.1
#define defaultPort 4545

// function to print error and exit
void error(char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){

    char buffer[bufferSize];

/*  Hostname and portname are required to connect to server
    Example usage - 
    ./client localhost 4545
*/  
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    // get port no
    int portno  = atoi(argv[2]);

    // get socket descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }

/*  `server` defines a host computer on the Internet

    The variable server is a pointer to a structure of type hostent. 
    This structure is defined in the header file netdb.h as follows:
    struct  hostent {
        char    *h_name;              // official name of host 
        char    **h_aliases;          // alias list 
        int     h_addrtype;           // host address type 
        int     h_length;             // length of address 
        char    **h_addr_list;        // list of addresses from name server 
    #define h_addr  h_addr_list[0]    // address, for backward compatiblity 
    };

*/
    struct hostent *server;

/*  gethostbyname
    Takes such a name as an argument and 
    returns a pointer to a hostent containing information about that host. 
    The field char *h_addr contains the IP address. 
    If this structure is NULL, the system could not locate a host with this name.
    
    The mechanism by which this function works is complex, 
    often involves querying large databases all around the country.
*/
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

/*  serv_addr holds server address
    This structure is defined in <netinet/in.h>. Here is the definition:
    struct sockaddr_in {
            short   sin_family;
            u_short sin_port;
            struct  in_addr sin_addr;
            char    sin_zero[8];
    };

    Below lines sets the fields in serv_addr
*/
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    bcopy((char *)server->h_addr_list[0], 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    
/*  The connect function is called by the client to establish a connection to the server. 
    It takes three argumeh_addrnts.
    
    The socket file descriptor, 
    the address of the host to which it wants to connect (including the port number), 
    and the size of this address. 
    
    This function returns 0 on success and -1 if it fails. 

    Notice that the client needs to know the port number of the server, 
    but it does not need to know its own port number. 
    This is typically assigned by the system when connect is called.
*/
    int connectStatus = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) ;
    if (connectStatus< 0){
        error("ERROR connecting");
    }

    /* Client is now connected to server */

    int n;

    printf("Please enter the message: ");
    bzero(buffer, bufferSize);
    fgets(buffer, bufferSize-1, stdin); // uses fgets to read the message from stdin
    n = write(sockfd,buffer,strlen(buffer)); // writes the message to the socket
    if (n < 0){
        error("ERROR writing to socket");
    }

    bzero(buffer,bufferSize);
    n = read(sockfd,buffer,bufferSize-1); // reads the reply from the socket
    if (n < 0){
        error("ERROR reading from socket");
    } 
    printf("%s\n",buffer);

    return 0;
}
