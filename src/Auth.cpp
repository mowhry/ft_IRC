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

