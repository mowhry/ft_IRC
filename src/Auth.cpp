#include "../include/Server.hpp"
#include "../include/Client.hpp"


void Server::cmd_auth(std::string cmd, int fd){
    
    Client *cli = getClient(fd);
    cmd = cmd.substr(4);
    size_t pos = cmd.find_first_not_of("\t\v ");
    if (pos == std::string::npos || cmd.empty())
        SendResponse(fd, "Not enough arguments\n");
    else if (pos < cmd.size()){
        cmd = cmd.substr(pos);
        if (cmd[0] == ':')
            cmd.erase(cmd.begin());
    }
    if(pos == std::string::npos || cmd.empty()) 
		SendResponse(fd, ERR_NOTENOUGHPARAM(std::string("*")));
    if (!cli->getRegister())
    {
        if (cmd == getPassword()){
            cli->setRegister(true);
        }
        else
            SendResponse(fd, ERR_INCORRECTPASS(std::string("*")));
    }
    else
          SendResponse(fd, ERR_ALREADYREGISTERED(getClient(fd)->getNickname()));
}

bool Server::IsValidNick(std::string nickname, int fd){

    if(nickname.empty() || nickname[0] == '&' || nickname[0] == ':' || nickname[0] == '#')
        return false;
    for (size_t i = 0; i < nickname.size(); i++)
    {
        if(!std::isalnum(nickname[i]) && nickname[i] != '_')
        {
            SendResponse(fd, ERR_INVALIDNICK(nickname));
            return false;
        }
    }
    for(size_t i = 0; i < clients.size(); i++)
    {
        if (this->clients[i].getNickname() == nickname)
        {
            SendResponse(fd, ERR_NICKALREADYUSED(nickname));
            return false;
        }
    }
    return true;
}



void Server::cmd_nick(std::vector<std::string> splitted_cmd, int fd){

    Client *cli = getClient(fd);
    if (splitted_cmd.size() < 2 || splitted_cmd[1].empty()){
        SendResponse(fd, ERR_NOTENOUGHPARAM(std::string("*")));
        return;
    }
    if (!IsValidNick(splitted_cmd[1], fd))
        return;
    if(cli && cli->getRegister())
    {
        std::string old;
        cli->setNickname(splitted_cmd[1]);
        //std::cout << "nouveau nickname : " << cli->getNickname() << std::endl;
        //AJOUTER LE CHANGEMENT DE NICKNAME DANS TOUS LES CHANNELS
        if (!old.empty() && old != splitted_cmd[1])
        {
				if(old == "*" && !cli->getUser().empty())
				{
					cli->setLog(true);
					SendResponse(fd, RPL_CONNECTED(cli->getNickname()));
					SendResponse(fd, RPL_NICKCHANGE(cli->getNickname(),splitted_cmd[1]));
				}
				else
					SendResponse(fd, RPL_NICKCHANGE(old,splitted_cmd[1]));
				return;
			}
			
		}
	else if (cli && !cli->getRegister())
		SendResponse(fd, ERR_NOTREGISTERED(splitted_cmd[1]));

	if (cli && cli->getRegister() && !cli->getUser().empty() && !cli->getNickname().empty() && cli->getNickname() != "*" && !cli->getLog())
	{
		cli->setLog(true);
		SendResponse(fd, RPL_CONNECTED(cli->getNickname()));
	}
}

void Server::cmd_user(std::string cmd, int fd){

    std::vector<std::string> splitted_cmd = split(cmd);
	Client *cli = getClient(fd); 
	if((cli && splitted_cmd.size() < 5))
	{   
        SendResponse(fd, ERR_NOTENOUGHPARAM(cli->getNickname())); 
        return; 
    }
	if(!cli  || !cli->getRegister())
		SendResponse(fd, ERR_NOTREGISTERED(std::string("*")));
	else if (cli && !cli->getUser().empty())
		{SendResponse(fd, ERR_ALREADYREGISTERED(cli->getNickname())); return;}
	else
		cli->setUser(splitted_cmd[1]);
	if(cli && cli->getRegister() && !cli->getUser().empty() && !cli->getNickname().empty() && cli->getNickname() != "*"  && !cli->getLog())
	{
		cli->setLog(true);
		SendResponse(fd, RPL_CONNECTED(cli->getNickname()));
	}
}
