#define once


#define RPL_CONNECTED(nickname) (": 001 " + nickname + " : Welcome to the IRC server!\r\n")

#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " : You have not registered\r\n")