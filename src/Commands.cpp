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



int Server::searchNicknameInClient(std::string nickname)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].getNickname() == nickname)
			return (1);
	}
	return (0);
}


void Server::cmd_privmsg(std::string command_full, int fd)
{
	Client *cli = getClient(fd);
	std::string nick = cli->getNickname();

	std::istringstream str(command_full);
	std::string cmd, targetList, message;

	str >> cmd >> targetList;

	if(targetList.empty())
	{
		SendResponse(fd, ERR_NORECIPIENT(nick));
		return;
	}

	std::getline(str, message);

	if(message.empty())
	{
		SendResponse(fd, ERR_NOTEXTTOSEND(nick));
		return;
	}

	if(message[0] == ':')
	{
		message = message.substr(1);
	}

	std::istringstream targetStream(targetList);
	std::string target;
	while (std::getline(targetStream, target,','))
	{
		if(searchNicknameInClient(target) == 1)
		{
			std::string messageToSend = ":" + nick + " PRIVMSG " + target + " :" + message + "\r\n";
			SendResponse(fd,messageToSend);
		}
		///else if(if target exists in channel then broadcast 
		///to everyone except the sender
		///a implementer
		else
		{
			SendResponse(fd, ERR_NOSUCHNICK(target));
		}
	}

}