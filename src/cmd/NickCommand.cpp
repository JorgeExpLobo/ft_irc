#include "NickCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

NickCommand::NickCommand() {}
NickCommand::~NickCommand() {}


bool isValidNick(const std::string& nick)
{
	if (nick.empty())
		return false;

	if (!std::isalpha(nick[0]))
		return false;

	for (size_t i = 1; i < nick.size(); i++)
	{
		if (!std::isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_')
			return false;
	}

	return true;
}

void NickCommand::execute(Server* server, Client* client, const Message& msg)
{
	if (msg.getArgCount() == 0)
	{
		Message err = Reply::errNoNicknameGiven(client->getNickname());
		server->sendToClient(client, err.stringify());
		return;
	}

	std::string nick = msg.getArg(0);

	if (client->getNickname() == nick)
		return;

	if (!isValidNick(nick))
	{
		Message err = Reply::errErroneousNickname(client->getNickname(), nick);
		server->sendToClient(client, err.stringify());
		return;
	}

	if (server->nickExists(nick))
	{
		Message err = Reply::errNicknameInUse(client->getNickname(), nick);
		server->sendToClient(client, err.stringify());
		return;
	}

	std::string oldNick = client->getNickname();

	if (!oldNick.empty() && oldNick != "*")
    	server->removeNick(oldNick);
	client->setNickname(nick);
	server->addNick(nick, client);

	std::cout << "[NICK CHANGE] " << oldNick << " -> " << nick << std::endl;

	Message nickUpdate;
	nickUpdate.setPrefix(oldNick + "!" + client->getUsername() + "@" + client->getHost())
			  .setCommand("NICK")
			  .pushSuffix(nick);

	std::string finalMsg = nickUpdate.stringify();

	server->sendToClient(client, finalMsg);

	const std::set<Channel*>& channels = client->getChannels();

	for (std::set<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it)
	{
		server->broadcastToChannel(*it, finalMsg, client->getFd());
	}

	client->tryRegister(server);
}