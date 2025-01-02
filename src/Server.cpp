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

    std::cout << GRE << "Client " << incomingfd << " connected" << WHI << std::endl;

}

void Server::ReceiveNewData(int fd){
    char buff[1024];
    memset(buff, 0, sizeof(buff));

    ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);
    if(bytes <= 0){
        std::cout << RED << "Client " << fd << " disconnected" << WHI << std::endl;
        ClearClients(fd);
        close(fd);
    }
    else
    {
        buff[bytes] = '\0';
        std::cout << YEL << "Client " << fd << " data : " << WHI << buff;
        //PROCESS THE DATA HERE

    }


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

