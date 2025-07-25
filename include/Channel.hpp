#pragma once

#include "./Client.hpp"
#include "./Replies.hpp"
#include "./Server.hpp"
#include <vector>
class Server;

class Channel{
private:
	std::string				_name;
	std::string				_topic;
	std::vector<Client *>	_users;


	int						_userLimit;
	std::string				_password;
	std::vector<Client *> 	_operators;
	bool					_isInviteOnly;
	bool					_isTopicRestricted;



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
	bool 		getTopicRestricted();
	const std::vector<Client *>& getUser() const;
	std::string getTopic() const;




	void		setName(const std::string &name);
	void		setUserLimit(int limit);
	void		setPassword (std::string pass);
	void		setInviteOnly(bool invite);
	void		setTopicRestricted(bool restriction);
	void 		setTopic(const std::string &topic);

	void		addUser(Client *user);
	std::string	addOperator(Client *user);
	void		removeUser(Client *user);
	std::string	removeOperator(Client *user);
	bool		isOperator(std::string nick);
	bool 		is_channel_empty();
	bool 		isUserInChannel(const std::string &nickname);

	void sendToAll(const std::string &msg, int senderFd, Server &server);

};