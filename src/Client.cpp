#include "../include/Client.hpp"

//####### CONSTRUCTORS & DESTRUCTOR

Client::Client(){

}

Client::Client(Client const &src){
    *this = src;
}

Client::~Client(){

}

//####### OPERATOR OVERLOAD

Client & Client::operator=(Client const &src){
    this->fd=src.fd;
    this->IPadd = src.IPadd;
    return (*this);
}

//####### GETTERS & SETTERS

int Client::getFd(){
    return (this->fd);
}

void Client::setFd(int fd){
    this->fd = fd;
}

void Client::setIpAdd(std::string ipadd){
    this->IPadd = ipadd;
}