# FTP server and client

Implementation of FTP client-server application as per RFC-959

## todo

Check out the todolist [here](./todo.md)

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
