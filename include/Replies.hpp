#define once


#define RPL_CONNECTED(nickname) (": 001 " + nickname + " : Welcome to the IRC server!\r\n")

#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " : You have not registered\r\n")
#define ERR_INVALIDNICK(nickname)(": 432 " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKALREADYUSED(nickname) (": 433 " + nickname + " :Nickname is already in use\r\n")