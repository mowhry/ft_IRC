
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

        bool _isRegistered;
        bool _isLogged;
   
    public : 
        Client();
        Client(Client const &src);
        ~Client();

        Client & operator=(Client const &src);

        void clearBuffer();

        int getFd();
        std::string getBuffer();
        bool getRegister();
        bool getLog();
        std::string getNickname();
        std::string getUser();

        void setFd(int fd);
        void setRegister(bool Registered);
        void setLog (bool Logged);
        void setIpAdd(std::string ipadd);
        void setBuffer(std::string received);
        void setUser(std::string username);
        void setNickname(std::string nickname);

};