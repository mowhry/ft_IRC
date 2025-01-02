
#pragma once

#include <iostream>
#include <unistd.h> 


#define RED "\e[1;31m" 
#define WHI "\e[0;37m" 
#define GRE "\e[1;32m" 
#define YEL "\e[1;33m"

class Client
{
    private:
        int fd;
        std::string IPadd;
        std::string _nickname;
        std::string _username;
        std::string _buffer;

        //bool _isRegistered;
   
    public : 
        Client();
        Client(Client const &src);
        ~Client();

        Client & operator=(Client const &src);

        int getFd();

        void setFd(int fd);
        void setIpAdd(std::string ipadd);
        void setBuffer(std::string received);
};