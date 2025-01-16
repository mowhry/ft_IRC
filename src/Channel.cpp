#include "../include/Channel.hpp"
#include "algorithm"

Channel::Channel() : _name(""), _users()
{
}

Channel::Channel(const std::string &name) : _name(name), _users()
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

std::string Channel::getName() const
{
	return this->_name;
}

void Channel::setName(const std::string &name)
{
	this->_name = name;
}


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

