#include "../include/Server.hpp"
#include "../include/Client.hpp"

int verif_args(int ac, char **av)
{
	if (ac != 3)
    {
        std::cout << "Usage : ./ircserv <port> <password>" << std::endl;
        return (-1);
    }

	if (std::string(av[2]).empty())
	{
		std::cout << "Password must not be empty" << std::endl;
		return (-1);
	}

	std::string port = av[1];
	for (int i = 0;port[i] != '\0';i++)
	{
		if (!std::isdigit(port[i]))
		{
			std::cout << "The port must be all digit" << std::endl;
			return (-1);
		}	
	}

	int i_port = atoi(av[1]);
	if (i_port < 1024 || i_port > 65535)
	{
		std::cout << "The port must be between  1024 and 65535" << std::endl;
		return (-1);
	}
	return (0);
}