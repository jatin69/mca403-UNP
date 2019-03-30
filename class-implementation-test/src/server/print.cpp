#include "./server.hpp"

void great(){
    std::cout << "LOL NICE";
}
void Server::print() {
    std::cout << "Hey. The value of A is : " << a;
    great();
}


