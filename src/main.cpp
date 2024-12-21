#include "../include/Server.hpp"
#include "../include/Client.hpp"

bool Server::Signal = false;

int main()
{
Server ser;
std::cout <<" -----SERVER----- "<< std::endl;

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