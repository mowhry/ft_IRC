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
    if (!cli->getRegister())
    {
        if (cmd == getPassword()){
            cli->setRegister(true);
            SendResponse(fd, "Connected\n");
        }
        else
            SendResponse(fd, "Wrong Password\n");
    }
    else
        SendResponse(fd, "Already Registered\n");
}

bool Server::IsValidNick(std::string nickname, int fd){

    if(nickname.empty() || nickname[0] == '&' || nickname[0] == ':' || nickname[0] == '#')
        return false;
    for (size_t i = 0; i < nickname.size(); i++)
    {
        if(!std::isalnum(nickname[i] && nickname[i] != '_'))
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
}
