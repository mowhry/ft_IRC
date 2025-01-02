#include "../include/Server.hpp"
#include "../include/Client.hpp"

bool Server::Signal = false;

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cout << "Usage : ./ircserv <port> <password>" << std::endl;
        return (-1);
    }
Server ser("SERVER DE OUF", av[1], av[2]);
std::cout <<" -----"<< ser.getName() <<"----- "<< std::endl;

    try{
        signal(SIGINT, Server::SignalHandler);
        signal(SIGQUIT, Server::SignalHandler);
        ser.SerInit();
    }

    catch(const std::exception &e){
        ser.CloseFds();
        std::cerr<< e.what() << std::endl;
    }
    std::cout << "The server closed" << std::endl;
}