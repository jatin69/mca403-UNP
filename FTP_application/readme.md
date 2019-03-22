# FTP server and client

Implementation of FTP client-server application as per RFC-959

## todo list

- [ ] read TCP and telnet spec
- [ ] read rfc959 FTP spec
- [ ] figure out properties of stream sockets and block and compressed sockets
- [ ] figure out why len is limited in TCP transmission. MTU 1500 maybe? How can it be resolved.
- [X] Test on 2 VMs. find a working code.
- [ ] figure out why my system is not working. NAT ?
- [ ] Connect my system directly to ethernet. Then try. This will confirm the NAT theory.
- [ ] Then modify rfc code to remove HARD coded IP address.

## Resources 

- VM on Digital ocean `ssh root@139.59.79.252`
- VM on Google cloud `ssh root@35.200.210.239`
- Official FTP spec sheet rfc959 [here](https://www.w3.org/Protocols/rfc959/6_State.html)
- [FTP commands](http://www.nsftools.com/tips/RawFTP.htm)

## Bare Minimum inclusions

In order to make FTP workable without needless error messages, thefollowing minimum implementation is required for all servers:

```text
TYPE - ASCII Non-print
MODE - Stream
STRUCTURE - File, Record
COMMANDS - USER, QUIT, PORT, TYPE, MODE, STRU, for the default values RETR, STOR,NOOP.
```

The default values for transfer parameters are:
```
TYPE - ASCII Non-print
MODE - Stream
STRU - File
```

All hosts must accept the above as the standard defaults.

## Commands

### Required commands

- `USER <SP> <username> <CRLF>`
  - user root
- `PASS <SP> <password> <CRLF>`
  - pass toor
- `MKD  <SP> <pathname> <CRLF>`
  - mkd 
  - mkd ../hello
- `RMD  <SP> <pathname> <CRLF>`
  - rmd ../hello/
- `PWD  <CRLF>`
  - pwd
- `CWD  <SP> <pathname> <CRLF>`
  - cwd ..
  - cwd ../hello/
- `LIST [<SP> <pathname>] <CRLF>`
  - list hello
- `RETR <SP> <pathname> <CRLF>`
  - retr ../hello/welcome.txt
- `STOR <SP> <pathname> <CRLF>`
  - stor ../hello/thankyou.txt
- `QUIT <CRLF>`  - exit
  - quit
- `ABOR <CRLF>` - stop previous command is not completed yet
  - abor

### Good to have commands

- `CDUP <CRLF>`
  - cdup
  - cwd ~
- `APPE <SP> <pathname> <CRLF>`
  - appe public-keys.txt
- `DELE <SP> <pathname> <CRLF>`
  - dele ../hello/thankyou.txt
- `HELP [<SP> <string>] <CRLF>`
  - help
  - help send
  - help usage
  - help receive
- `NOOP <CRLF>`
  - noop

### Additional

- `NLST [<SP> <pathname>] <CRLF>`
- `RNFR <SP> <pathname> <CRLF>`
- `RNTO <SP> <pathname> <CRLF>`

## Replies

### Required Reply codes

- `200 Command okay`
- `530 Not logged in`
- `331 User name okay, need password`
- `225 Data connection open; no transfer in progress`
- `551 Requested action aborted: page type unknown`
- `502 Command not implemented`
- `503 Bad sequence of commands`
- `504 Command not implemented for that parameter`

### Good to have reply codes

- `214 Help message`
- `220 Service ready for new user`
- `230 User logged in, proceed`
- `500 Syntax error, command unrecognized`
- `501 Syntax error in parameters or arguments`
- `550 Requested action not taken` - in case of file not found
