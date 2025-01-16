#pragma once
#include "./Client.hpp"
#include "./Server.hpp"

class Channel{
private:
	std::string			_name;
	std::vector<Client *>	_users;

public:
	Channel();
	Channel(const Channel &copy);
	Channel(const std::string &name);
	~Channel();

	Channel& operator=(const Channel &os);

	std::string	getName() const;
	void		setName(const std::string &name);

	void		addUser(Client *user);
	void		removeUser(Client *user);
	const std::vector<Client *>& getUser() const;

};