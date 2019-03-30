# mca403 - Unix Network Programming

Related coursework for mca403 - Unix Network Programming

## TODO

### Reading

- [ ] read TCP, telnet spec and rfc959 FTP spec
- [ ] figure out properties of stream sockets and block and compressed sockets
- [ ] figure out why len is limited in TCP transmission. MTU 1500 maybe? How can it be resolved.

### Connection

- [x] Test on 2 VMs. find a working code.
- [x] Connect my system directly to ethernet. Then try. This will confirm the NAT theory. Tried. Not working. IDK WHY?
- [ ] figure out why my system is not working. NAT ?

### Coding

- [ ] Modify rfc code to remove HARD coded IP address.
- [ ] Read then entire code. Understand it, comment it.
- [ ] Break it and start coding new app.

## Resources

### VMs used for testing

- VM on Digital ocean `ssh root@139.59.79.252`
- VM on Google cloud `ssh root@35.200.210.239`
- Strangely enough. Things are not working on my wifi. Tried without the router as well. Even with single IP source, it says no route to host. Now before coding any solution, verify that `telnet <my system IP>` is working. This will tell if there's no path or connection refusing.

### Docs

- [Beej's guide to Network Programming](https://beej.us/guide/bgnet/html/multi/index.html)
- [Tutorial based on Beej's guide](https://www.bogotobogo.com/cplusplus/sockets_server_client.php)
- [RFC-793 TCP](https://tools.ietf.org/html/rfc793), [RFC-854 Telnet](https://tools.ietf.org/html/rfc854)
- [RFC-959 FTP](https://www.w3.org/Protocols/rfc959/6_State.html)
- [D. J. Bernstein's guide to FTP's practical Implemention](http://cr.yp.to/ftp.html)

### others

- [FTP Server - modern cpp](https://github.com/DaHoC/ftpserver)
- [Apples ftp implementation](http://simplestcodings.blogspot.com/2012/09/ftp-implementation-in-c.html)

## Available options

### FTP

1. [`FTP_rfc959-complaint`](https://github.com/harshil93/FTP-Server-Client)

   1. dummy login
   2. responses and response codes
   3. major functionality implemented and follows rfc959
   4. Passive port implemented
   5. Tested the internet working. Works Now. On two VM's. Does not work from my system because of NAT maybe.
   6. TODO : Figure out a way to remove HARDCODED IP, probably by reading the spec sheet and conforming that IP address can be removed from the PORT command

2. [`FTP_functioning`](https://github.com/gudipati/FTP-ClientServer)

   1. Basic functionality of commands
   2. No response codes
   3. No login
   4. TODO : There is a strange problem. Maybe it works in only one packet burst, but only half the data is transferred. Limit seems to be 1401 bytes, maybe because Linux MTU is 1500 bytes. FIGURE OUT if possible.
   5. Works on VMs, but only limited bytes are transferred, as said in point 4.
   6. Doesn't work from system to internet. Port bind error.

3. [`FTP_litterbox`](https://www.armi.in/wiki/FTP_Server_and_Client_Implementation_in_C/C%2B%2B)
   1. Basic functionality works.
   2. But leaves a temp file whereever it goes
   3. Not ready for use

### TCP

1. `TCP_demo`
   1. Works as intended.
   2. Works over the internet.
   3. Somehow has a transfer limit.
   4. Works as a tcp-demo hello-world app
   5. TODO : figure out why limit
