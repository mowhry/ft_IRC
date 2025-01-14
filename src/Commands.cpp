#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Server::cmd_user(std::vector<std::string> splitted_cmd, int fd){

    getClient(fd)->setUser(splitted_cmd[1]);
    std::cout << "username changed to " << getClient(fd)->getUser() << std::endl;

}


void Server::cmd_nick(std::vector<std::string> splitted_cmd, int fd){
    getClient(fd)->setNickname(splitted_cmd[1]);
    std::cout << "nickname changed to " << getClient(fd)->getNickname() << std::endl;
}

void Server::cmd_quit(std::string cmd, int fd){
        (void) cmd;
        std::cout << RED << "Client " << fd << " disconnected" << WHI << std::endl;
        ClearClients(fd);
        close(fd);
}