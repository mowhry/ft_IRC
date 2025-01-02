#include "../include/Client.hpp"

//####### CONSTRUCTORS & DESTRUCTOR

Client::Client() : _isRegistered(0){

}

Client::Client(Client const &src) {
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

//####### METHODS

void Client::clearBuffer() {
    this->_buffer.clear();
}

//####### GETTERS & SETTERS

int Client::getFd(){
    return (this->fd);
}

std::string Client::getBuffer(){
    return this->_buffer;
}

bool    Client::getRegister(){
    return (this->_isRegistered);
}

void Client::setFd(int fd){
    this->fd = fd;
}

void Client::setRegister(bool Registered){
    this->_isRegistered = Registered;
}
void Client::setIpAdd(std::string ipadd){
    this->IPadd = ipadd;
}

void Client::setBuffer(std::string received){
    this->_buffer += received; 
}