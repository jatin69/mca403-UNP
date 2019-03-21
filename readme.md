# mca403 - Unix Network Programming

Related coursework for mca403 - Unix Network Programming

## Resources

- VM on Digital ocean `ssh root@139.59.79.252`
- VM on Google cloud `ssh root@35.200.210.239`

## Available options

### FTP

1. `FTP_rfc959-complaint` 
   1. dummy login
   2. responses and response codes
   3. major functionality implemented
2. `FTP_functioning`
   1. Basic functionality of commands
   2. No response codes
   3. No login
3. `FTP_litterbox`
   1. Basic functionality works.
   2. But leaves a temp file whereever it goes
   3. Not ready for use
   4. Limit is 1401 bytes somehow. Data gets corrupted after that.


### TCP

1. `TCP_demo`
   1. Works as intended.
   2. Works over the internet.
   3. Somehow has a transfer limit.
   4. Works as a tcp-demo hello-world app 