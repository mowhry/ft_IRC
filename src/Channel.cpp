#include "../include/Channel.hpp"
#include "algorithm"

Channel::Channel() : _name(""), _users(),  _userLimit(-1), _password(""), _operators(), _isInviteOnly(false)
{
}

Channel::Channel(const std::string &name) : _name(name), _users(), _userLimit(-1), _password(""), _operators(), _isInviteOnly(false)
{
}

Channel::Channel(const Channel &copy)
{
	this->_name = copy._name;
	this->_users = copy._users;
}

Channel::~Channel()
{
}

Channel& Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		this->_name = other._name;
		this->_users = other._users;
	}
	return *this;
}

//#### GETTERS 
std::string Channel::getName() const
{
	return this->_name;
}


int	Channel::getUserLimit(){
	return this->_userLimit;
}

std::string Channel::getPassword(){
	return this->_password;
}

bool Channel::getInviteOnly(){
	return this->_isInviteOnly;
}


//#### SETTERS

void Channel::setName(const std::string &name)
{
	this->_name = name;
}

void		Channel::setUserLimit(int limit){
	this->_userLimit = limit;
}

void		Channel::setPassword (std::string pass){
	this->_password = pass;
}

void		Channel::setInviteOnly(bool invite){
	this->_isInviteOnly = invite;
}

//#### FUNCTIONS

void Channel::addUser(Client *user)
{
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i] == user)
			return;
	}
	_users.push_back(user);
}

void Channel::removeUser(Client *user)
{
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i] == user)
		{
			_users.erase(_users.begin() + i);
			break;
		}
	}
}

const std::vector<Client *>& Channel::getUser() const
{
	return this->_users;
}


bool	Channel::isOperator(std::string nick){
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i].getNickname() == nick)
		 return true;
	}
	return false;
}

std::string	Channel::addOperator(Client user){
	if (isOperator(user.getNickname())){
		return("User is Already Operator");
	}
	for(size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i]->getFd() == user.getFd()){
			_operators.push_back(user);
			return("User added as Operator");
		}
	}
	return("User not in the Channel");
}

std::string Channel::removeOperator(Client user){
	for (size_t i = 0; i < _operators.size(); i++){
		if (_operators[i].getFd() == user.getFd())
		{
			_operators.erase(_operators.begin() + i);
			return ("User Not Operator anymore");
		}
	}
	return("User was not Operator");
}
