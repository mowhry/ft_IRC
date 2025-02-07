#include "../include/Client.hpp"

//####### CONSTRUCTORS & DESTRUCTOR

Client::Client() : fd(-1), IPadd(""), _nickname(""), _username(""), _buffer(""), _isRegistered(false), _isLogged(false), _nickaccepted(false) {
    _chaninvitations.clear();
}

Client::Client(Client const &src) : fd(src.fd), IPadd(src.IPadd), _nickname(src._nickname), _username(src._username),
      _buffer(src._buffer), _isRegistered(src._isRegistered), _isLogged(src._isLogged), _nickaccepted(src._nickaccepted){
    _chaninvitations = src._chaninvitations; 
}

Client::~Client(){

}

//####### OPERATOR OVERLOAD

Client & Client::operator=(Client const &src){
        if (this != &src) {
        this->fd = src.fd;
        this->IPadd = src.IPadd;
        this->_nickname = src._nickname;
        this->_username = src._username;
        this->_buffer = src._buffer;
        this->_isRegistered = src._isRegistered;
        this->_isLogged = src._isLogged;
        this->_nickaccepted = src._nickaccepted;
        this->_chaninvitations = src._chaninvitations;
    }
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

std::string Client::getHostname(){
	std::string hostname = this->getNickname() + "!" + this->getUser();
	return hostname;
}

 bool Client::getChanInvitation( std::string name){
    for (size_t i = 0; i < _chaninvitations.size(); i++)
    {
        if (name == _chaninvitations[i])
            return(true);
    }
    return(false);
 }

bool Client::getNickAcceptance(){
    return this->_nickaccepted;
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

void Client::setNickAcceptance(bool accepted){
    this->_nickaccepted = accepted;
}

 void  Client::addChannelInvitation(std::string name){
    for(size_t i = 0; i < _chaninvitations.size(); i++)
    {
        if (name == _chaninvitations[i])
            return;
    }
    _chaninvitations.push_back(name);
 }

 bool  Client::isInvitedByChannel(std::string name)
 {
	for (size_t i = 0; i < _chaninvitations.size(); i++)
	{
		if(name == _chaninvitations[i])
			return (true);
	}
	return (false);
 }

 void Client::removeChannelInvitation(std::string name)
{
    for (size_t i = 0; i < _chaninvitations.size(); i++)
    {
        if (_chaninvitations[i] == name)
        {
            _chaninvitations.erase(_chaninvitations.begin() + i);
            return;
        }
    }
}
