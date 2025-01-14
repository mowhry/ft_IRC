#include "../include/Server.hpp"
#include "../include/Client.hpp"


void Server::cmd_quit(std::string cmd, int fd){
        (void) cmd;
        std::cout << RED << "Client " << fd << " disconnected" << WHI << std::endl;
        ClearClients(fd);
        close(fd);
}