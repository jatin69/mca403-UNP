#include "./client.h"

void printError() {
  if(errno == 0){
    cout << "No error. All good. " << endl;
    return; 
  }

  cout << "[ERROR]: " << strerror(errno) << endl;
}
