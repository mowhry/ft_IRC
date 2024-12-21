
#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <poll.h> 
#include <csignal> 
#include <stdio.h>
#include <string.h>
#include "./Client.hpp"

#define RED "\e[1;31m" 
#define WHI "\e[0;37m" 
#define GRE "\e[1;32m" 
#define YEL "\e[1;33m"

class Server{

    private : 
        int Port;
        int SerSocketFd;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<struct pollfd> fds;
    
    public : 
        Server();
        Server(Server const &src);
        ~Server();

        Server & operator=(Server const &src);

        void SerInit();
        void SerSocket();
        void AcceptNewClient();
        void ReceiveNewData(int fd);

        static void SignalHandler(int signum);

        void CloseFds();
        void ClearClients(int fd);
};