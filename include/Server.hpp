
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
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "./Client.hpp"
#include "./Replies.hpp"

#define RED "\e[1;31m" 
#define WHI "\e[0;37m" 
#define GRE "\e[1;32m" 
#define YEL "\e[1;33m"

class Server{

    private : 
        int _port;
        std::string _name;
        std::string _password;
        int SerSocketFd;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<struct pollfd> fds;
    
    public : 
        Server();
        Server(std::string name, std::string port, std::string password);
        Server(Server const &src);
        ~Server();

        Server & operator=(Server const &src);

        void SerInit();
        void SerSocket();
        void AcceptNewClient();
        void ReceiveNewData(int fd);
        void SendResponse(int fd, std::string str);
        std::vector<std::string> ParseData(std::string buff);
        std::vector<std::string> split(std::string &cmd);
        void exec(std::string &cmd, int fd);

        void cmd_auth(std::string cmd, int fd);
        void cmd_quit(std::string cmd, int fd);


        static void SignalHandler(int signum);

        std::string getName();
        Client  *getClient(int fd);
        std::string getPassword();
        
        void CloseFds();
        void ClearClients(int fd);
};