#define once


#define RPL_CONNECTED(nickname) (": 001 " + nickname + " : Welcome to Serveur 2 Ouf\r\n")
#define RPL_NICKCHANGE(oldnickname, nickname) (":" + oldnickname + " NICK " + nickname + "\r\n")

#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " : You have not registered\r\n")
#define ERR_INVALIDNICK(nickname)(": 432 " + nickname + " : Erroneus nickname\r\n")
#define ERR_NICKALREADYUSED(nickname) (": 433 " + nickname + " : Nickname is already in use\r\n")
#define ERR_NOTENOUGHPARAM1(nickname) (": 461 " + nickname + " :Not enough parameters\r\n")
#define ERR_NOTENOUGHPARAM() (": 461 * : Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You are already registered!\r\n")