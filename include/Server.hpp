
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
#include <map>
#include "./Client.hpp"
#include "./Replies.hpp"
#include "./Channel.hpp"

#define RED "\e[1;31m" 
#define WHI "\e[0;37m" 
#define GRE "\e[1;32m" 
#define YEL "\e[1;33m"
#define MAX_CLIENTS 100
class Channel;

class Server{

    private : 
        int _port;
        std::string _name;
        std::string _password;
        int SerSocketFd;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<struct pollfd> fds;
        std::map<std::string, Channel> channels;
    
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
        bool IsValidNick(std::string nickname, int fd);


        void cmd_auth(std::string cmd, int fd);
        void cmd_quit(std::string cmd, int fd);
        void cmd_user(std::string cmd, int fd);
        void cmd_nick(std::vector<std::string> splitted_cmd, int fd);
        void cmd_join(std::vector<std::string> splitted_cmd, int fd);
		void cmd_part(std::vector<std::string> splitted_cmd, int fd);
		void cmd_privmsg(std::string command_full, int fd);
		void cmd_kick(std::vector<std::string> splitted_cmd, std::string command_full, int fd);
        void cmd_mode(std::vector<std::string> splitted_cmd, int fd);
        void cmd_invite(std::vector<std::string> splitted_cmd, int fd);
		void cmd_topic(std::vector<std::string> splitted_cmd, int fd);

        static void SignalHandler(int signum);

        std::string getName();
        bool checkExist_name(std::string name);
        bool checkExist_chan(std::string name);
        Client  *getClient(int fd);
        Client *getClientFromNickname(std::string nickname);
        Channel *getChan(std::string name);
        std::string getPassword();
        
        void CloseFds();
        void ClearClients(int fd);

		int     getfdfromNickname(std::string nickname);
        std::string  getNicknameFromFd(int fd);
        void	chan_mode(std::vector<std::string> splitted_cmd, int fd, Channel *chan);
        void	invite_mode(char c, Channel *chan, int fd, std::vector<std::string> splitted_cmd);
        void	limit_mode(char c, Channel *chan, int fd, std::vector<std::string> splitted_cmd);
        void	topic_mode(char c, Channel *chan, int fd, std::vector<std::string> splitted_cmd);
        void	password_mode(char c, Channel *chan, int fd, std::string pass);
        bool    isvalid_limit(std::string& limit);
};