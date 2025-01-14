#include "../include/Client.hpp"

//####### CONSTRUCTORS & DESTRUCTOR

Client::Client() : _nickname ("*"),  _isRegistered(false){

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

std::string Client::getNickname(){
    return this->_nickname;
} 

std::string Client::getUser(){
    return this->_username;
} 

bool    Client::getRegister(){
    return (this->_isRegistered);
}

bool    Client::getLog(){
    return (this->_isLogged);
}

void Client::setFd(int fd){
    this->fd = fd;
}

void Client::setRegister(bool Registered){
    this->_isRegistered = Registered;
}


void Client::setLog(bool Logged){
    this->_isLogged = Logged;
}

void Client::setIpAdd(std::string ipadd){
    this->IPadd = ipadd;
}

void Client::setBuffer(std::string received){
    this->_buffer += received; 
}

void Client::setUser(std::string username){
    this->_username = username;
}

void Client::setNickname(std::string nickname){
    this->_nickname = nickname;
}