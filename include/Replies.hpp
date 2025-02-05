#define once


#define RPL_CONNECTED(nickname) (": 001 " + nickname + " : Welcome to Serveur 2 Ouf\r\n")
#define RPL_NICKCHANGE(oldnickname, nickname) (":" + oldnickname + " NICK " + nickname + "\r\n")
#define RPL_CHANGEMODE(hostname, channelname, mode, arguments) (":" + hostname + " MODE " + channelname + " " + mode + " " + arguments + "\r\n")

#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " : You have not registered\r\n")
#define ERR_INVALIDNICK(nickname)(": 432 " + nickname + " : Erroneus nickname\r\n")
#define ERR_NICKALREADYUSED(nickname) (": 433 " + nickname + " : Nickname is already in use\r\n")
#define ERR_NOTENOUGHPARAM(nickname) (": 461 " + nickname + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You are already registered!\r\n")
#define ERR_CMDNOTFOUND(nickname, command) (": 421 " + nickname + " " + command + " : Command not found\r\n")
#define ERR_INCORRECTPASS(nickname) (": 464 " + nickname + " :Password incorrect !\r\n" )
#define ERR_USERONCHANNEL(nickname, channel) ("443 " + nickname + " " + channel + " :is already on channel\r\n")
#define ERR_NOTOPERATOR(channelname) (": 482 " + channelname + " :You're not a channel operator" + "\r\n")
#define ERR_NORECIPIENT(nickname) (": 411 " + nickname + " :No recipient given (PRIVMSG)\r\n")
#define ERR_NOTEXTTOSEND(nickname) (": 412 " + nickname + " :No text to send\r\n")
#define ERR_NOSUCHNICK(name) (": 401 " + name + " :No such nick\r\n")
#define ERR_NOSUCHCHANNEL(name) (": 403 " + name + " :No such channel\r\n")
#define ERR_UNKNOWNMODE(nickname, channelname, mode) (": 472 " + nickname + " #" + channelname + " " + mode + " :is not a recognised channel mode" + "\r\n")
#define ERR_INVALIDMODEPARAM(nickname, channelname, mode, value) (": 696 "+nickname+" "+ channelname +" "+mode+" "+value+ " :Invalid mode parameter.\r\n")
#define ERR_NEEDMODEPARAM(nickname,channelname, mode) (": 696 " + nickname +" " + channelname +" "+mode + " * :You must specify a parameter for the key mode.\r\n")
#define ERR_KEYSET(nickname,channelname) (": 467 "+ nickname + " " + channelname + " :Channel key already set.\r\n")