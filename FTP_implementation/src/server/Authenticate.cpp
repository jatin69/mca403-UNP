#include "server.h"

/** Authenticate the client connected to clientControlfd
 * 
 * Returns 
 *        0 : failed authentication
 *        1 : User authenticated
 *       -1 : Error occured
 * 
 * Handling authentication ( As stated in RFC-959)
 **/ 

int AuthenticateUser(int clientControlfd) {
    /*
        Implement authentication as commands. and do not give unathenticated access.
        Maybe abstract this entire authentication part in a single function.

    */
    // receive username from client
    string username;
    Recv(clientControlfd, username);

    if (username.compare(0, strlen("USER"), "USER") == 0){
        //do authentication
        string res = "331 Please specify password\r\n";
        send_all(clientControlfd, res.c_str(), res.size());
        string password;

        Recv(clientControlfd, password);
        // do pass authentication.
        res = "230 Login Successfuly\r\n";
        send_all(clientControlfd, res.c_str(), res.size());
        return 1;
    }
    else{
        string res = "430 Invalid Username\r\n";
        send_all(clientControlfd, res.c_str(), res.size());
        return 0;
    }
}

