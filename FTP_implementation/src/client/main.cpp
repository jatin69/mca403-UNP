#include "./client.h"

int debug;
int main(int argc, char **argv) {
  // initialise remBuf -> Figure out its importance
  remBuf = "";
  debug = 0;
  if (argc < 3) {
    cout << "The correct format is $./client <host> <port>";
    return 0;
  }
  if (argc == 4) {
    string msg = string(argv[3]);
    if (msg == "-d") {
      // Debug Mode
      debug = 1;
    }
  }

  // Starting work now

  int serverfd;
  if ((serverfd = make_client_connection(argv[1], argv[2])) > 0) {
    // server has now connected to client

    // Sending out Authentication Information.
    string res, user, pass;
    Recv(serverfd, res);
    cout << "Response: " << res << endl;
    cout << "Enter Your Username" << endl;
    getline(std::cin, user);
    string userstr = "USER " + user + "\r\n";
    if (debug == 1)
      cout << "Request: " << userstr << endl;
    send_all(serverfd, userstr.c_str(), userstr.size());
    Recv(serverfd, res);
    cout << "Response: " << res << endl;
    cout << "Enter Your Pass" << endl;
    getline(std::cin, pass);
    string passstr = "PASS " + pass + "\r\n";
    if (debug == 1)
      cout << "Request: " << passstr << endl;
    send_all(serverfd, passstr.c_str(), passstr.size());
    Recv(serverfd, res);
    cout << "Response: " << res << endl;
    // TODO - handle auth

    /*
            ADDON Implementation -
            Also implement a PORT command here, and keep track of previous
       command.
            If user did not issue PORT command, use default [the way it is
       working right now]
            Else,
            if the previous command was PORT, do not issue the explicit PORT
       command in each command
    */

    while (1) {
      cout << "ftp>";
      string userinput;
      getline(std::cin, userinput);
      ltrim(userinput);

      // put command -> transfer happens in new process
      if (userinput.compare(0, strlen("put"), "put") == 0) {
        // cout<<"put"<<endl;
        int pid = fork();
        if (pid != 0) {
          int stat;
          wait(&stat);
          Recv(serverfd, res);
          cout << "Response: " << res << endl;
        } else {
          string path =
              userinput.substr(3); // Gets the filename from put command
          path = trim(path);

          // Gets the file size
          struct stat st;
          int statcode = stat(path.c_str(), &st);
          int size = st.st_size;
          if (statcode == -1) {
            cout << strerror(errno) << endl;
            continue;
          }
          // Switching to Binary Mode -> JUST A DUMMY
          string typei = "TYPE I\r\n";
          if (debug == 1)
            cout << "Request: " << typei << endl;
          send_all(serverfd, typei.c_str(), typei.size());
          Recv(serverfd, res);
          cout << "Response: " << res << endl;

          // Getting a random port and corresponding PORT command
          string portstr, port;
          // Needs a mechanism
          getportstring(getownip(serverfd), portstr, port);

          // Listening to data server
          // int dataportserverfd = server_listen(port.c_str());
          int dataportserverfd = Socket();
          Bind(dataportserverfd, atoi(port.c_str()));
          Listen(dataportserverfd, backlog);
        
          if (debug == 1)
            cout << "Request: " << portstr << endl;
          // send_all(serverfd, portstr.c_str(), portstr.size());
          Send(serverfd, portstr);
          Recv(serverfd, res);
          cout << "Response: " << res << endl;

          // Sending out the STOR command
          string storstr = "STOR " + path + "\r\n";
          if (debug == 1)
            cout << "Request: " << storstr << endl;
          send_all(serverfd, storstr.c_str(), storstr.size());
          Recv(serverfd, res);
          cout << "Response: " << res << endl;

          int dataportclientfd = accept_connection(dataportserverfd);

          // Opening file and sending Data
          FILE *filew;
          int numw;
          filew = fopen(path.c_str(), "rb");
          cout << "DATA TRANSFER" << endl;
          int len = send_all_binary(dataportclientfd, filew, size);
          Recv(serverfd, res);
          cout << "Response: " << res << endl;
          
          fclose(filew);
          close(dataportclientfd);
          close(dataportserverfd);
          return 0;
        }
      }
      // get command -> transfer happens in new process
      else if (userinput.compare(0, strlen("get"), "get") == 0) {
        cout << "get" << endl;
        int pid = fork();
        if (pid != 0) {
          int stat;
          wait(&stat);
          Recv(serverfd, res);
          cout << "Response: " << res << endl;
        } else {
          // Switching to Binary Mode

          string typei = "TYPE I\r\n";
          if (debug == 1)
            cout << "Request: " << typei << endl;
          send_all(serverfd, typei.c_str(), typei.size());
          Recv(serverfd, res);
          cout << "Response: " << res << endl;

          // Getting a random port and corresponding PORT command
          string portstr, port;
          // Needs a mechanism
          getportstring(getownip(serverfd), portstr, port);

          // Listening to data server
          // i am a client and i want `get` at this port
          // this port is random. client is free to choose any
          
          // int dataportserverfd = server_listen(port.c_str());
          int dataportserverfd = Socket();
          Bind(dataportserverfd, atoi(port.c_str()));
          Listen(dataportserverfd, backlog);

          if (debug == 1)
            cout << "Request: " << portstr << endl;
          // you need to throw data at this IP and this port
          send_all(serverfd, portstr.c_str(), portstr.size());
          Recv(serverfd, res);
          cout << "Response: " << res << endl;

          string path =
              userinput.substr(3); // Gets the filename from get command
          path = trim(path);

          string getstr = "RETR " + path + "\r\n";
          if (debug == 1)
            cout << "Request: " << getstr << endl;
          send_all(serverfd, getstr.c_str(), getstr.size());
          Recv(serverfd, res);

          cout << "Response: " << res << endl;
          if (res.compare(0, strlen("550"), "550") == 0) {
            close(dataportserverfd);
            continue;
          }
          // server threws what we wanted
          // we are accepting data at this IP and this port
          int dataportclientfd = accept_connection(dataportserverfd);

          FILE *filew;
          int numw;
          // Receiving data and storing it in file.
          filew = fopen(path.c_str(), "wb");
          cout << "DATA TRANSFER" << endl;
          int len = recv_all_binary(dataportclientfd, filew);
          cout << "Bytes Received : " << len << endl;
          fclose(filew);
          close(dataportclientfd);
          close(dataportserverfd);
          return 0;
        }
      }
      // LIST command
      else if (userinput.compare(0, strlen("ls"), "ls") == 0) {
        int pid = fork();
        if (pid != 0) {
          int stat;
          wait(&stat);
          Recv(serverfd, res);
          cout << "Response: " << res << endl;
        } else {
          // child which will receive data
          string portstr, port;

          // Needs a mechanism
          getportstring(getownip(serverfd), portstr, port);
          // int dataportserverfd = server_listen(port.c_str());
          int dataportserverfd = Socket();
          Bind(dataportserverfd, atoi(port.c_str()));
          Listen(dataportserverfd, backlog);


          if (debug == 1)
            cout << "Request: " << portstr << endl;
          send_all(serverfd, portstr.c_str(), portstr.size());
          Recv(serverfd, res);
          cout << "Response: " << res << endl;
          if (debug == 1)
            cout << "Request: "
                 << "LIST\r\n"
                 << endl;
          send_all(serverfd, "LIST\r\n", strlen("LIST\r\n"));
          Recv(serverfd, res);
          cout << "Response: " << res << endl;
          int dataportclientfd = accept_connection(dataportserverfd);
          recv_all(dataportclientfd, res);
          cout << "------------DATA---------" << endl;
          cout << "Response: " << res << endl;
          cout << "-------END-DATA---------" << endl;
          close(dataportclientfd);
          close(dataportserverfd);
          return 0;
        }
      }
      // cd command
      else if (userinput.compare(0, strlen("cd"), "cd") == 0) {
        cout << "cd" << endl;
        string path = userinput.substr(2);
        path = trim(path);
        string cwdstr = "CWD " + path + "\r\n";
        if (debug == 1)
          cout << "Request: " << cwdstr << endl;
        send_all(serverfd, cwdstr.c_str(), cwdstr.size());
        Recv(serverfd, res);
        cout << "Response: " << res << endl;
      }
      // normal mkdir command
      else if (userinput.compare(0, strlen("mkdir"), "mkdir") == 0) {
        cout << "mkdir" << endl;
        string path = userinput.substr(5);
        path = trim(path);
        string cwdstr = "MKD " + path + "\r\n";
        if (debug == 1)
          cout << "Request: " << cwdstr << endl;
        send_all(serverfd, cwdstr.c_str(), cwdstr.size());
        Recv(serverfd, res);
        cout << "Response: " << res << endl;
      }
      // normal rmdir command
      else if (userinput.compare(0, strlen("rmdir"), "rmdir") == 0) {
        cout << "rmdir" << endl;
        string path = userinput.substr(5);
        path = trim(path);
        string cwdstr = "RMD " + path + "\r\n";
        if (debug == 1)
          cout << "Request: " << cwdstr << endl;
        send_all(serverfd, cwdstr.c_str(), cwdstr.size());
        Recv(serverfd, res);
        cout << "Response: " << res << endl;
      }
      // pwd command
      else if (userinput.compare(0, strlen("pwd"), "pwd") == 0) {
        string pwdstr = "PWD\r\n";
        if (debug == 1)
          cout << "Request: " << pwdstr << endl;
        send_all(serverfd, pwdstr.c_str(), pwdstr.size());
        Recv(serverfd, res);
        cout << "Response: " << res << endl;
      }
      // !ls
      else if (userinput.compare(0, strlen("!ls"), "!ls") == 0) {
        DIR *dp;
        struct dirent *ep;
        dp = opendir("./");
        if (dp != NULL) {
					// being explicit. putting NULL here
          while ((ep = readdir(dp)) != NULL)
            cout << ep->d_name << endl;
          closedir(dp);
        } else
          perror("Couldn't open the directory");
      }
      // !cd
      else if (userinput.compare(0, strlen("!cd"), "!cd") == 0) {
        string path = userinput.substr(3);
        path = trim(path);
        int stat = chdir(path.c_str());
        if (stat == 0)
          cout << "Directory Successfully Changed" << endl;
        else
          cout << strerror(errno) << endl;
      }
      // !pwd
      else if (userinput.compare(0, strlen("!pwd"), "!pwd") == 0) {
        cout << "!pwd" << endl;
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
          cout << "Current Dir: " << cwd << endl;
        else
          perror("getcwd() error");
      }
      // quit
      else if (userinput.compare(0, strlen("quit"), "quit") == 0) {
        close(serverfd);
        exit(0);
      }
      // help
      else if (userinput.compare(0, strlen("help"), "help") == 0) {
        cout << "Look at the README file supplied" << endl;
      }
      // default
      else {
        cout << "UNKNOWN COMMAND" << endl;
      }
    }
  } else {
    cerr << "Cannot connect to server" << endl;
  }

  return 0;
}
