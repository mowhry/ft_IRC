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
		channel.addOperator(*cli); 
	}
	std::string msg = ":" + cli->getNickname() + " JOIN " + chan_name + "\r\n";
	SendResponse(fd, msg);
	channel.sendToAll(msg, fd, *this);
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
	if (splitted_cmd.size() < 3)
		return;
	if (splitted_cmd[1][0] == '#')
	{
		if(checkExist_chan(splitted_cmd[1]) && (splitted_cmd.size() == 4 || splitted_cmd.size() == 3) )
			chan_mode(splitted_cmd, fd, getChan(splitted_cmd[1]));
		else if (splitted_cmd.size() <= 4)
			SendResponse(fd, ERR_NOTENOUGHPARAM(getNicknameFromFd(fd)));	
		else
			SendResponse(fd, ERR_NOSUCHCHANNEL(splitted_cmd[1]));
	}
		return;


}

void	Server::chan_mode(std::vector<std::string> splitted_cmd, int fd, Channel *chan){
	if (splitted_cmd[2].size() != 2){

		SendResponse (fd, "Error on format\n");
		return;
	}
	if (chan->isOperator(getNicknameFromFd(fd)))
	{
		if ((splitted_cmd[2][0] != '+' ||  splitted_cmd[2][0] != '-') && (splitted_cmd[2].size() == 2))
		{

			if (splitted_cmd[2][1] == 'o'){
				if(splitted_cmd[2][0]== '+')
					SendResponse(fd, chan->addOperator(*getClientFromNickname(splitted_cmd[3])));
				else
					SendResponse(fd, chan->removeOperator(*getClientFromNickname(splitted_cmd[3])));
			}
			else if (splitted_cmd[2][1] == 'i')
				invite_mode(splitted_cmd[2][0], chan, fd);
			else if (splitted_cmd[2][1]== 'l')
			{
				if(splitted_cmd.size() > 3)
					limit_mode(splitted_cmd[2][0], chan, fd, splitted_cmd[3]);
				else
					SendResponse(fd, ERR_NEEDMODEPARAM(chan->getName(),"l"));
			}
			else if (splitted_cmd[2][1] == 't')
				topic_mode(splitted_cmd[2][0], chan, fd);


		}
	}
	else
		SendResponse(fd, ERR_NOTOPERATOR(chan->getName()));
}


void	Server::invite_mode(char c, Channel *chan, int fd)
{
	if (c =='+')
	{
		chan->setInviteOnly(true);
		SendResponse(fd, RPL_CHANGEMODE(getClient(fd)->getHostname(), this->_name, "+i", ""));	//sendtoall
	}

	else if (c =='-')
	{
		chan->setInviteOnly(false);
		SendResponse(fd, RPL_CHANGEMODE(getClient(fd)->getHostname(), this->_name, "-i", ""));	//sendtoall
	}
}

void Server::topic_mode(char c, Channel *chan, int fd)
{
	if (c =='+')
	{
		chan->setTopicRestricted(true);
		SendResponse(fd, RPL_CHANGEMODE(getClient(fd)->getHostname(), this->_name, "+t", ""));	//sendtoall
	}

	else if (c =='-')
	{
		chan->setTopicRestricted(false);
		SendResponse(fd, RPL_CHANGEMODE(getClient(fd)->getHostname(), this->_name, "-t", ""));	//sendtoall
	}
}

bool Server::isvalid_limit(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

void	Server::limit_mode(char c, Channel *chan, int fd, std::string limit)
{
	if (c == '+')
	{
		if (!isvalid_limit(limit))
			SendResponse(fd, ERR_INVALIDMODEPARAM(chan->getName(),"l"));
		else 
		{
			chan->setUserLimit(std::atoi(limit.c_str()));
			SendResponse(fd, RPL_CHANGEMODE(getClient(fd)->getHostname(), this->_name, "+l", limit));	//sendtoall
		}
	}
	else
	{
		chan->setUserLimit(0);
		SendResponse(fd, RPL_CHANGEMODE(getClient(fd)->getHostname(), this->_name, "+l", "0"));	//sendtoall
	}
}


