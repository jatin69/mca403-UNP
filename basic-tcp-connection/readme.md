# A Simple TCP client-server app

This is a sample app showing 2 way communication between server and client via a tcp connection.
Refer [this resource](http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html) to know more about socket programming and a 2-way TCP commuication.

## todo

- [X] infinite-server working locally
- [ ] internet testing
  - [X] basic functionality
  - [X] message length very long? Failing. Limit seems to be 113 bytes
  - [ ] perfectly working
- [ ] Stream sockets time outs for long message. Find alternative approach.

## How to run

- compile with `make` command
- or you can compile individually. Example - `cd client-side && gcc client.c -o client`
- Run the server first by giving a port `server-side/infinite-server 4545`
- Run the client by giving host and port `client-side/client localhost 4545`

## Files

- `makefile` : To compile files
- `server.c` : single run server for the app
- `infinite-server.c` : forever running server for the app
- `client.c` : client for the app

## Testing - Internet

- Tested on a VM from google cloud computer engine.
- Paid. Credits used for payment.
- `ssh root@35.200.210.239` : login to server
- goto project folder `basic-tcp-connection/`
- `make` : compile
- `server-side/infinite-server 4444` : start server on the remote computer 
- `client-side/client 35.200.210.239 4444` : then run client on local system
- It works.
- A firewall rule was added to allow incoming traffic from all protocols/ports.
