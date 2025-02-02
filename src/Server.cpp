#include "../include/Server.hpp"


//####### CONSTRUCTORS & DESTRUCTOR

Server::Server(Server const &src){
    *this = src;
}


Server::Server(std::string name, std::string port, std::string password) 
{
    this->_name = name;
    this->_port = atoi(port.c_str());
    this->_password = password;
    this->clients.reserve(MAX_CLIENTS);
}

Server::~Server(){

}

//####### OPERATOR OVERLOAD

Server & Server::operator=(Server const & src){
    this->_port = src._port;
    this->SerSocketFd = src.SerSocketFd;
    this->Signal = src.Signal;
    this->clients = src.clients;
    this->fds = src.fds;
    return (*this);
}

//####### GETTERS

std::string Server::getName(){
    return(this->_name);
}

Client *Server::getClient(int fd){
    for (size_t i = 0; i < this->clients.size(); i++){
        if (this->clients[i].getFd() == fd){
            return &clients[i]; 
        }
    }
    return NULL;
}

std::string Server::getPassword(){
    return this->_password;
}

Channel  * Server::getChan(std::string name)
{
    std::map<std::string, Channel>::iterator it = channels.find(name);
    if (it != channels.end())
        return &(it->second);
    return NULL;
}

Client * Server::getClientFromNickname(std::string nickname){
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].getNickname() == nickname)
            return &clients[i];
    }
    return NULL ;
}

std::string  Server::getNicknameFromFd(int fd){
    return(getClient(fd)->getNickname());
}


//####### FUNCTIONS

void Server::SerInit(){
   // this->_port = 4444;
    SerSocket();

    std::cout << GRE << "Server " << SerSocketFd << " connected" << std::endl;
    std::cout << WHI << "Waiting for connections..." << std::endl;

    while (Server::Signal == false) {
        if((poll(&fds[0], fds.size() , -1) == -1) && Server::Signal == false)
            throw (std::runtime_error("poll function failed"));
        for(size_t i = 0; i < fds.size(); i++)
        {
            if(fds[i].revents & POLLIN){
                if(fds[i].fd == SerSocketFd)
                    AcceptNewClient();
                else
                    ReceiveNewData((fds[i].fd));
            }
        }
    }
    CloseFds();
}

void Server::SerSocket(){
    struct sockaddr_in add;
    struct pollfd NewPoll;

    add.sin_family = AF_INET;
    add.sin_port = htons(this->_port);
    add.sin_addr.s_addr = INADDR_ANY;

    SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    	int en = 1;
	if(setsockopt(SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
		throw(std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	if (fcntl(SerSocketFd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		throw(std::runtime_error("failed to set option (O_NONBLOCK) on socket"));
	if (bind(SerSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1) //-> bind the socket to the address
		throw(std::runtime_error("failed to bind socket"));
	if (listen(SerSocketFd, SOMAXCONN) == -1) //-> listen for incoming connections and making the socket a passive socket
		throw(std::runtime_error("listen() faild"));

	NewPoll.fd = SerSocketFd; //-> add the server socket to the pollfd
	NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
	NewPoll.revents = 0; //-> set the revents to 0
	fds.push_back(NewPoll); //-> add the server socket to the pollfd
}


void Server::AcceptNewClient(){
    if (clients.size() >= MAX_CLIENTS) {
        std::cout << RED << "Client limit reached (" << MAX_CLIENTS << ")" << WHI << std::endl;
        return;
    }
    Client cli;
    struct sockaddr_in cliadd;
    struct pollfd NewPoll;
    socklen_t len = sizeof(cliadd);

    int incomingfd = accept(SerSocketFd, (sockaddr *)&(cliadd), &len);
    if (incomingfd == -1)
        std::cout << "Failed to accept new client" << std::endl;
    if (fcntl(incomingfd, F_SETFL, O_NONBLOCK) == -1)
        std::cout << "Failed to set up the client socket" << std::endl;

    NewPoll.fd = incomingfd;
    NewPoll.events = POLLIN;
    NewPoll.revents = 0;

    cli.setFd(incomingfd);
    cli.setIpAdd(inet_ntoa((cliadd.sin_addr)));
    clients.push_back(cli);
    fds.push_back(NewPoll);

    //SendResponse(incomingfd, RPL_CONNECTED((getClient(incomingfd)->getNickname())));
   // std::cout << GRE << "Client " << incomingfd << " connected" << WHI << std::endl;


}

void Server::ReceiveNewData(int fd){
    std::vector<std::string> cmd;
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    Client *cli = getClient(fd);

    ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);
    if(bytes <= 0){
        std::cout << RED << "Client " << fd << " disconnected" << WHI << std::endl;
        ClearClients(fd);
        close(fd);
    }
    else
    {
        cli->setBuffer(buff);
        if (cli->getBuffer().find_first_of("\r\n") == std::string::npos)
                return;
        cmd = ParseData(cli->getBuffer());
        for (size_t i = 0; i < cmd.size(); i++)
           this->exec(cmd[i], fd);
           // std::cout << i << ": " << this->(cmd[i]) << std::endl;
        if (getClient(fd))
            getClient(fd)->clearBuffer();
        //buff[bytes] = '\0';
        //std::cout << YEL << "Client " << fd << " data : " << WHI << buff;
        //PROCESS THE DATA HERE

    }


}

//##### PARSING
std::vector<std::string> Server::split(std::string &str){
    std::vector<std::string> cmd;
    std::istringstream stm(str);
    std::string token;

    while(stm >> token){
        cmd.push_back(token);
        token.clear();
    }
    return cmd;
}

void Server::exec(std::string &cmd, int fd){
    if (cmd.empty())
        return;
    std::vector<std::string> splitted_cmd;
    splitted_cmd = split(cmd);

    //UNCOMMENT TO SEE WHAT YOU RECEIVE FROM THE CLIENT
   /*for(size_t i = 0; i < splitted_cmd.size(); i++)
    {
        std::cout << fd << ": " << i << ": " << splitted_cmd[i] << std::endl;
    }*/
   /////////////
   size_t check = cmd.find_first_not_of(" \t\r");
    if (check != std::string::npos)
        cmd = cmd.substr(check);
    if(splitted_cmd.size() && (splitted_cmd[0] == "CAP" || splitted_cmd[0] == "cap"))
		return;
    if(splitted_cmd.size() && (splitted_cmd[0] == "PASS" || splitted_cmd[0] == "pass"))
        cmd_auth(cmd, fd);
    else if(splitted_cmd.size() && (splitted_cmd[0] == "NICK" || splitted_cmd[0] == "nick"))
        cmd_nick(splitted_cmd, fd);
    else if(splitted_cmd.size() && (splitted_cmd[0] == "USER" || splitted_cmd[0] == "user"))
        cmd_user(cmd, fd);
    else if(splitted_cmd.size() && (splitted_cmd[0] == "QUIT" || splitted_cmd[0] == "quit"))
        cmd_quit(cmd, fd);
    else if(splitted_cmd.size() && (splitted_cmd[0] == "PING" || splitted_cmd[0] == "ping"))
        SendResponse(fd, "PONG\r\n");
    else if (getClient(fd)->getLog() == true)
    {
        // FUNCTIONS THAT NEED YOU TO BE LOGGED IN
        if (splitted_cmd.size() && (splitted_cmd[0] == "JOIN" || splitted_cmd[0] == "join"))
            cmd_join(splitted_cmd, fd);
		else if(splitted_cmd.size() && (splitted_cmd[0] == "PART" || splitted_cmd[0] == "part"))
        	cmd_part(splitted_cmd, fd);
        else if(splitted_cmd.size() && (splitted_cmd[0] == "PRIVMSG" || splitted_cmd[0] == "privmsg"))
            cmd_privmsg(cmd, fd);
        else if(splitted_cmd.size() && (splitted_cmd[0] == "MODE" || splitted_cmd[0] == "mode"))
            cmd_mode(splitted_cmd, fd);
        else
            SendResponse(fd, ERR_CMDNOTFOUND(getClient(fd)->getNickname(),splitted_cmd[0]));
    }
    else if (getClient(fd)->getRegister() == false)
    {
        SendResponse(fd, ERR_NOTREGISTERED(getClient(fd)->getNickname()));
    }
    return;
    
}

std::vector<std::string>   Server::ParseData(std::string buff){
    std::vector<std::string> cmd;

    std::istringstream stream(buff);
    std::string str;
    while(std::getline(stream, str)){
        size_t pos = str.find_first_of("\n\r");
        if (pos != std::string::npos){
            str = str.substr(0, pos);
        }
        cmd.push_back(str);
    }
    return (cmd);
}

//####### REPLY

void Server::SendResponse(int fd, std::string str){
 if(send(fd, str.c_str(), str.size(), 0) == -1)
  std::cerr << "reply send() failed : " << str << std::endl;
    
}

//####### SIGNALS

void Server::SignalHandler(int signum){
    (void) signum;
    std::cout << std::endl << RED << "Signal Received" << WHI << std::endl;
    Server::Signal = true;
}

//####### CLOSE & EXIT

void Server::CloseFds(){
    for (size_t i = 0; i < clients.size(); i++)
    {
        std::cout << RED << "Client " << clients[i].getFd() << " disconnected" << std::endl;
        close(clients[i].getFd());
    }
    if (SerSocketFd != -1)
    {
        std::cout << RED << "Server " << SerSocketFd << " disconnected" << WHI << std::endl;
        close(SerSocketFd);
    }
}

void Server::ClearClients(int fd){
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == fd)
        {
            fds.erase(fds.begin() + i);
            break;
        }
    }
    for (size_t i = 0; i < clients.size(); i++){
        if (clients[i].getFd() == fd)
        {
            clients.erase(clients.begin() + i);
            break;
        }
    }
}

