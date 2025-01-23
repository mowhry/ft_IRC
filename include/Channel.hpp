#pragma once

#include "./Client.hpp"
#include "./Replies.hpp"
#include "./Server.hpp"
#include <vector>
class Server;

class Channel{
private:
	std::string				_name;
	std::vector<Client *>	_users;


	int						_userLimit;
	std::string				_password;
	std::vector<Client> 	_operators;
	bool					_isInviteOnly;


public:
	Channel();
	Channel(const Channel &copy);
	Channel(const std::string &name);
	~Channel();

	Channel& operator=(const Channel &os);

	std::string	getName() const;
	int 		getUserLimit();
	std::string getPassword();
	bool		getInviteOnly();
	const std::vector<Client *>& getUser() const;
	



	void		setName(const std::string &name);
	void		setUserLimit(int limit);
	void		setPassword (std::string pass);
	void		setInviteOnly(bool invite);

	void		addUser(Client *user);
	std::string	addOperator(Client user);
	void		removeUser(Client *user);
	std::string	removeOperator(Client user);
	bool		isOperator(std::string nick);

	void sendToAll(const std::string &msg, int senderFd, Server &server);

};