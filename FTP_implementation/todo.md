# todo

- [ ] Handle authentication - SERVER Side
  - [X] Abstract as a function
  - [ ] Support `USER`, `PASS`, `HELP` and all other commands are invalid
  - [ ] Allow only certain users to passthrough
- [ ] Handle Authentication - client side
  - [ ] support `USER` and `PASS` command
- [X] abstraction of BIND and ACCEPT
- [ ] Rename all functions
- [ ] support enum for commands
  - [X] basic test
  - [ ] entire porting
- [X] Abstract serverlisten and break into Socket, Bind and Listen
  - [X] Abstract and test
- [ ] See what changed. support for IPv6 etc ?
- [X] Take out common things from client and server and put them in utils. NO. Compiler optimisation fucks this up. The call stack gets messed up.
- [ ] Clear out the confusion between `PORT` and `PASV` and define clearly in readme
- [ ] A normal TCP connection over WAN is working. Figure out a way to bypass the NAT thingy.
- [ ] If cannot, atleast test if it works if the same connection is forked

- [ ] Once everything is at a good place. TEST if we can move on to classes.
  - [ ] It will handle the floating functions
  - [ ] It will handle the floating header files
  - [ ] It will help me store variables
  - [ ] Will help us deal with initialisations
  - [ ] But will also need a significant porting work.
 
- [ ] Make IPv6 compatible? Required these days. Atleast read about how that'll be possible.

### Endgoals

- [ ] A proper github repo with readme, screenshots and source code. Possibly a demo v
- [ ] 3 Working modes explicitly asked from user
  - [ ] localhost
  - [ ] LAN
  - [ ] WAN
- [ ] A proper `makefile` : Learn more about makefile
- [ ] See how to make it work in browser? Making it browser compatible

