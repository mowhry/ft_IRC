
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Replies.hpp"
#include "../include/Channel.hpp"

//##### QUIT #####

void Server::cmd_quit(std::string cmd, int fd){

        // retirer le fd dans tous les channels etc
        (void) cmd;
        std::cout << RED << "Client " << fd << " disconnected" << WHI << std::endl;
        ClearClients(fd);
        close(fd);
}

// ##### JOIN #####
void Server::cmd_join(std::vector<std::string> splitted_cmd, int fd)
{
	Client *cli = getClient(fd);
	std::string nick = cli->getNickname();

	if (splitted_cmd.size() < 2 || splitted_cmd[1].empty())
	{
		SendResponse(fd, ERR_NOTENOUGHPARAM(nick));
		return;
	}

	std::string chan_name = splitted_cmd[1];

	if (!cli->getLog())
	{
		SendResponse(fd, ERR_NOTREGISTERED(nick));
		return;
	}

	if (channels.find(chan_name) == channels.end())
		channels.insert(std::make_pair(chan_name, Channel(chan_name)));

	Channel &channel = channels[chan_name];
	const std::vector<Client*> &members = channel.getUser();

	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i] == cli)
		{
			SendResponse(fd, ERR_USERONCHANNEL(nick, chan_name));
			return;
		}
	}

	channel.addUser(cli);
	if (channel.getUser().size() == 1) 
	{
		channel.addOperator(cli); 
	}
	std::string msg = ":" + cli->getNickname()+"@localhost" + " JOIN " + chan_name + "\r\n";
	SendResponse(fd, msg);
	
	// 353 response
	std::ostringstream userList;
    userList << ":localhost 353 " << cli->getNickname() << " = " << chan_name << " :";
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (channel.isOperator(members[i]->getNickname()) == true)
		{
			std::cout << "OPERATOR FOUND" << std::endl;
			userList << "@" << members[i]->getNickname() << " ";
		}
		else
			userList <<members[i]->getNickname() << " ";
	}
	userList << "\r\n";
	SendResponse(fd, userList.str().c_str());

	/// 366 response
	std::ostringstream endOfNames;
	endOfNames << ":localhost 366 " << cli->getNickname() << " " << chan_name << " :End of /NAMES list\r\n";
	SendResponse(fd,endOfNames.str().c_str());
	
	channel.sendToAll(msg, fd, *this);

	//////
	std::cout << "Channel :" << channel.getName() << ": User List :" << std::endl;
	for (size_t i = 0; i < channel.getUser().size() ; i++)
	{
		std::cout << channel.getUser()[i]->getNickname() << std::endl;
	}
	/////
}


/// ### PART ###
void Server::cmd_part(std::vector<std::string> splitted_cmd, int fd)
{
	Client *cli = getClient(fd);
	std::string nick = cli->getNickname();

	if (splitted_cmd.size() < 2 || splitted_cmd[1].empty())
	{
		SendResponse(fd, ERR_NOTENOUGHPARAM(nick));
		return;
	}

	if (!cli->getLog())
	{
		SendResponse(fd, ERR_NOTREGISTERED(nick));
		return;
	}

	//check if channel exists
	if (checkExist_chan(splitted_cmd[1]) == false)
	{
		std::ostringstream msg;
		msg << ":localhost 403 " << nick << " " << splitted_cmd[1] << " :No such channel\r\n";
		SendResponse(fd,msg.str().c_str());
		return;
	}
	
	Channel &channel = channels[splitted_cmd[1]];
	//check if client is in the channel
	const std::vector<Client*> &members = channel.getUser();
	int is_present = 0;
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i] == cli)
			is_present = 1;
	}
	if (is_present == 0)
	{
		std::ostringstream msg;
		msg << ":localhost 442 " << nick << " " << splitted_cmd[1] << " :You're not on that channel\r\n";
		SendResponse(fd,msg.str().c_str());
		return;
	}

	//part message
	std::ostringstream msg;
	msg << ":" << nick << "@localhost PART " << splitted_cmd[1];
	if (splitted_cmd.size() > 2)
		msg << " " << splitted_cmd[2];
	msg << "\r\n";
	SendResponse(fd, msg.str().c_str());
	channel.sendToAll(msg.str().c_str(), fd, *this);

	//remove fron operator if present
	if (channel.isOperator(nick) == true)
		channel.removeOperator(cli);
	// remove from the channel client list
	channel.removeUser(cli);
	//delete channel if empty
	if (channel.is_channel_empty())
		channels.erase(splitted_cmd[1]);
}



int Server::getfdfromNickname(std::string nickname)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].getNickname() == nickname)
			return (clients[i].getFd());
	}
	return (-1);
}

void Server::cmd_kick(std::vector<std::string> splitted_cmd ,std::string command_full, int fd)
{
	Client *cli = getClient(fd);
	std::string nick = cli->getNickname();

	if (splitted_cmd.size() < 3 || splitted_cmd[1].empty())
	{
		SendResponse(fd, ERR_NOTENOUGHPARAM(nick));
		return;
	}

	//parsing
	std::istringstream str(command_full);
	std::string cmd, channel_name, target, comment;
	
	str >> cmd >> channel_name >> target;
	std::getline(str, comment);
	size_t start = comment.find_first_not_of(" \t\r\n");
	if (start != std::string::npos) {
		comment = comment.substr(start);  
	} else {
		comment = "";  
	}
	if (comment[0] == ':')
	{
		comment = comment.substr(1);
	}

	//check if channel exists
	if (checkExist_chan(splitted_cmd[1]) == false)
	{
		std::ostringstream msg;
		msg << ":localhost 403 " << nick << " " << splitted_cmd[1] << " :No such channel\r\n";
		SendResponse(fd,msg.str().c_str());
		return;
	}

	Channel &channel = channels[splitted_cmd[1]];

	//check if client is in the channel
	const std::vector<Client*> &members = channel.getUser();
	int is_present = 0;
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i] == cli)
			is_present = 1;
	}
	if (is_present == 0)
	{
		std::ostringstream msg;
		msg << ":localhost 442 " << nick << " " << splitted_cmd[1] << " :You're not on that channel\r\n";
		SendResponse(fd,msg.str().c_str());
		return;
	}

	// Check if target user exists in the channel
	is_present = 0;
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i]->getNickname() == target)
			is_present = 1;
	}
	if (is_present == 0)
	{
		std::ostringstream msg;
		msg << ":localhost 441 " << nick << " " << splitted_cmd[1] << " :They aren't on that channel\r\n";
		SendResponse(fd,msg.str().c_str());
		return;
	}


	//check if client is operator or the target is an operator
	if (channel.isOperator(nick) == false || channel.isOperator(target) == true)
	{
		std::ostringstream msg;
		msg << ":localhost 482 " << nick << " " << splitted_cmd[1] << " :You're not channel operator or the target is an operator\r\n";
		SendResponse(fd,msg.str().c_str());
		return;
	}
	
	//send kick message to all user in the channel
	std::ostringstream msg;
	msg << ":" << nick << "@localhost KICK " << channel_name << " " << target;
	if (!comment.empty())
		msg << " :" << comment;
	msg << "\r\n";
	SendResponse(fd, msg.str().c_str());
	channel.sendToAll(msg.str().c_str(), fd, *this);

	//remove target user from channel
	channel.removeUser(getClientFromNickname(target));

}

void Server::cmd_privmsg(std::string command_full, int fd)
{
	Client *cli = getClient(fd);
	std::string nick = cli->getNickname();

	std::istringstream str(command_full);
	std::string cmd, targetList, message;

	str >> cmd >> targetList;

	if (targetList.empty())
	{
		SendResponse(fd, ERR_NORECIPIENT(nick));
		return;
	}

	std::getline(str, message);


	size_t start = message.find_first_not_of(" \t\r\n");

	if (start != std::string::npos) {
		message = message.substr(start);  
	} else {
		message = "";  
	}

	if (message.empty())
	{
		SendResponse(fd, ERR_NOTEXTTOSEND(nick));
		return;
	}

	if (message[0] == ':')
	{
		message = message.substr(1);
	}

	
	std::istringstream targetStream(targetList);
	std::string target;
	while (std::getline(targetStream, target, ','))
	{
		std::cout << "target:" << target << ":END" << std::endl; // debug purpose

		if (target[0] == '#')
		{
			Channel *channel = getChan(target);
			if (!channel)
			{
				SendResponse(fd, ERR_NOSUCHCHANNEL(target));
				continue;
			}
			std::string messageToSend = ":" + nick + " PRIVMSG " + target + " :" + message + "\r\n";

			channel->sendToAll(messageToSend, fd, *this);
		}
		else if (getfdfromNickname(target) != -1)
		{
			std::string messageToSend = ":" + nick + " PRIVMSG " + target + " :" + message + "\r\n";
			SendResponse(getfdfromNickname(target), messageToSend);
		}
		else
		{
			SendResponse(fd, ERR_NOSUCHNICK(target));
		}
	}
}


//##### MODE #####

bool Server::checkExist_name(std::string name){
	for (size_t i = 0; i < clients.size(); i++){
		if (clients[i].getNickname() == name)
			return (true);
	}
	return (false);
}

bool Server::checkExist_chan(std::string name){

	std::map<std::string, Channel>::iterator it = channels.find(name);
	if(it != channels.end() )
			return (true);
	return (false);
}

void Server::cmd_mode(std::vector<std::string> splitted_cmd, int fd)
{
	Client *cli = getClient(fd);
	std::string nick = cli->getNickname();

	if (splitted_cmd.size() < 2)
		return;
	// when no modstring is given
	if (splitted_cmd.size() == 2 && checkExist_chan(splitted_cmd[1]))
	{
		Channel &channel = channels[splitted_cmd[1]];
		std::ostringstream msg;
		msg << ":localhost 324 " << nick << " " << splitted_cmd[1] <<" +";
		if (channel.getInviteOnly())
			msg << "i";
		if (channel.getTopicRestricted())
			msg << "t";
		if (channel.getPassword() != "")
			msg << "k";
		if (channel.getUserLimit() != -1)
			msg << "l";
		if (channel.getUserLimit() != -1 && channel.getPassword() != "")
			msg << " " << channel.getPassword() << " :" << channel.getUserLimit();
		else if (channel.getUserLimit() == -1 && channel.getPassword() != "")
			msg << " :" << channel.getPassword();
		else if (channel.getUserLimit() != -1 && channel.getPassword() == "")
			msg << " :" << channel.getUserLimit();
		msg << "\r\n";
		SendResponse(fd, msg.str().c_str());
		return;
	}


	if (splitted_cmd[1][0] == '#')
	{
		if(checkExist_chan(splitted_cmd[1]) && (splitted_cmd.size() == 4 || splitted_cmd.size() == 3) )
			chan_mode(splitted_cmd, fd, getChan(splitted_cmd[1]));
		//else if (splitted_cmd.size() <= 4)
		//	SendResponse(fd, ERR_NOTENOUGHPARAM(getNicknameFromFd(fd)));	
		else
			SendResponse(fd, ERR_NOSUCHCHANNEL(splitted_cmd[1]));
	}
		return;
}

void	Server::chan_mode(std::vector<std::string> splitted_cmd, int fd, Channel *chan){

	Client *cli = getClient(fd);
	std::string nick = cli->getNickname();

	if (splitted_cmd[2].size() != 2){

		SendResponse(fd, ": 472 "+ nick +" "+splitted_cmd[2]+" :is not a recognized channel mode.\r\n");
		return;
	}
	if (chan->isOperator(getNicknameFromFd(fd)))
	{
		if ((splitted_cmd[2][0] == '+' ||  splitted_cmd[2][0] == '-') && (splitted_cmd[2].size() == 2))
		{
			if (splitted_cmd[2][1] == 'o'){

				//check if there is a target
				if (splitted_cmd.size() == 3)
				{
					SendResponse(fd,": 696 "+nick+" "+splitted_cmd[1]+" o * :You must specify a parameter for the op mode. Syntax: <nick>.\r\n");
					return;
				}

				//check if target is valid
				if (chan->isUserInChannel(splitted_cmd[3]) == false)
				{
					std::cout << "Enter" << std::endl;
					SendResponse(fd,": 401 "+nick+" "+splitted_cmd[3]+" :No such nick\r\n");
					return;
				}

				if(splitted_cmd[2][0]== '+')
				{
					chan->addOperator(getClientFromNickname(splitted_cmd[3]));
					SendResponse(fd, ":"+nick+"@localhost MODE "+splitted_cmd[1]+" +o :"+splitted_cmd[3]+"\r\n");
					chan->sendToAll(":"+nick+"@localhost MODE "+splitted_cmd[1]+" +o :"+splitted_cmd[3]+"\r\n",fd, *this);
				}
				else
				{
					chan->removeOperator(getClientFromNickname(splitted_cmd[3]));
					SendResponse(fd, ":"+nick+"@localhost MODE "+splitted_cmd[1]+" -o :"+splitted_cmd[3]+"\r\n");
					chan->sendToAll(":"+nick+"@localhost MODE "+splitted_cmd[1]+" -o :"+splitted_cmd[3]+"\r\n",fd, *this);
				}
			}
			else if (splitted_cmd[2][1] == 'i')
				invite_mode(splitted_cmd[2][0], chan, fd, splitted_cmd);
			else if (splitted_cmd[2][1]== 'l')
			{
				limit_mode(splitted_cmd[2][0], chan, fd, splitted_cmd);
			}
			else if (splitted_cmd[2][1] == 't')
				topic_mode(splitted_cmd[2][0], chan, fd, splitted_cmd);
			else if (splitted_cmd[2][1] == 'k')
			{
				if(splitted_cmd.size() > 3)
					password_mode(splitted_cmd[2][0], chan, fd, splitted_cmd[3]);
				else
					SendResponse(fd, ERR_NEEDMODEPARAM(nick,chan->getName(),"k"));
			}
			else
				SendResponse(fd, ": 472 "+ nick +" "+splitted_cmd[2]+" :is not a recognized channel mode.\r\n");

		}
	}
	else
		SendResponse(fd, ERR_NOTOPERATOR(chan->getName()));
}


void	Server::invite_mode(char c, Channel *chan, int fd, std::vector<std::string> splitted_cmd)
{
	if (c =='+')
	{
		chan->setInviteOnly(true);
		SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :+i\r\n");
		chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :+i\r\n",fd, *this);
	}

	else if (c =='-')
	{
		chan->setInviteOnly(false);
		SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :-i\r\n");
		chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :-i\r\n",fd, *this);
	}
}

void Server::topic_mode(char c, Channel *chan, int fd, std::vector<std::string> splitted_cmd)
{
	if (c =='+')
	{
		chan->setTopicRestricted(true);
		SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :+t\r\n");
		chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :+t\r\n",fd, *this);
	}

	else if (c =='-')
	{
		chan->setTopicRestricted(false);
		SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :-t\r\n");
		chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :-t\r\n",fd, *this);
	}
}

bool Server::isvalid_limit(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) >= 0);
}

void	Server::limit_mode(char c, Channel *chan, int fd, std::vector<std::string> splitted_cmd)
{
	if (c == '+')
	{
		if (splitted_cmd.size() == 3)
		{
			SendResponse(fd, ERR_NEEDMODEPARAM(getNicknameFromFd(fd),chan->getName(),"l"));
			return ;
		}
		
		if (!isvalid_limit(splitted_cmd[3]))
			SendResponse(fd, ERR_INVALIDMODEPARAM(getNicknameFromFd(fd),chan->getName(),"l",splitted_cmd[3]));
		else 
		{
			chan->setUserLimit(std::atoi(splitted_cmd[3].c_str()));
			SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" +l :"+splitted_cmd[3]+"\r\n");
			chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" +l :"+splitted_cmd[3]+"\r\n",fd, *this);
		}
	}
	else
	{
		chan->setUserLimit(-1);
		SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :-l\r\n");
		chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+splitted_cmd[1]+" :-l\r\n",fd, *this);
	}
}

bool isvalidPassword(std::string password)
{
	if(password.empty())
		return false;
	for(size_t i = 0; i < password.size(); i++)
	{
		if(!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}


void	Server::password_mode(char c, Channel *chan, int fd, std::string pass){
	if (!isvalidPassword(pass))
	{
		SendResponse(fd, ERR_INVALIDMODEPARAM(getNicknameFromFd(fd),chan->getName(),"k", pass));
		return ;
	}
	if (c == '+')
	{
		//if there is already a pass , we do nothing
		if (!chan->getPassword().empty())
			return;
		chan->setPassword(pass);
		SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+chan->getName()+" +k :"+pass+"\r\n");
		chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+chan->getName()+" +k :"+pass+"\r\n",fd, *this);
	}
	else if (c == '-' && !chan->getPassword().empty())
	{
		if(pass == chan->getPassword())
		{
			chan->setPassword("");
			SendResponse(fd, ":"+getNicknameFromFd(fd)+"@localhost MODE "+chan->getName()+" -k :"+pass+"\r\n");
			chan->sendToAll(":"+getNicknameFromFd(fd)+"@localhost MODE "+chan->getName()+" -k :"+pass+"\r\n",fd, *this);
		}
		else
			SendResponse(fd, ERR_KEYSET(getNicknameFromFd(fd),chan->getName()));
	}	
}

