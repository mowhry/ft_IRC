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

// ##### JOIN (TEMPPORARY) #####
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

	if 	(channels.find(chan_name) == channels.end())
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

	channels[chan_name].addUser(cli);
	std::string msg = ":" + cli->getNickname() + " JOIN " + chan_name + "\r\n";
	SendResponse(fd, msg);

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

			const std::vector<Client *> &members = channel->getUser();
			for (size_t i = 0; i < members.size(); ++i)
			{
				if (members[i]->getFd() != fd)
				{
					std::string messageToSend = ":" + nick + " PRIVMSG " + target + " :" + message + "\r\n";
					SendResponse(members[i]->getFd(), messageToSend);
				}
			}
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
		if(checkExist_chan(splitted_cmd[1]) && splitted_cmd.size() == 4 )
			chan_mode(splitted_cmd, fd, getChan(splitted_cmd[1]));
		else
		{
			SendResponse(fd, "Channel does not exist\n");
			return;
		}
	}
	else
	{
		if(checkExist_name(splitted_cmd[1])){
			SendResponse(fd, "User modes are not mandatory\n");
			return;
		}
		else
		{
			SendResponse(fd, "Client does not exist\n");
			return;
		}	
	}	


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
		}
	}
	else
		SendResponse(fd, "Error on the MODE command\n");
}
