#include "ModeCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"


ModeCommand::ModeCommand() {}
ModeCommand::~ModeCommand() {}

void ModeCommand::execute(Server* server, Client* client, const Message& msg)
{
	if (msg.getArgCount() < 1)
		return;
	
	std::string target = msg.getArg(0);

	if (target[0] != '#')
		return;

	Channel* chan = server->findChannel(target);
	if (!chan)
	{
		server->sendToClient(client, Reply::errNoSuchChannel(client->getNickname(), target).stringify());
		return;
	}

	if (!chan->hasClient(client))
	{
		server->sendToClient(client, Reply::errNotOnChannel(client->getNickname(), target).stringify());
		return;
	}
	
	if (!chan->isOperator(client))
	{
		server->sendToClient(client, Reply::errChanOpIsNeeded(client->getNickname(), target).stringify());
		return;
	}

	if (msg.getArgCount() == 1)
	{
		std::string modes = chan->getModesString();
		std::string reply = std::string(":") + SERVER_NAME + " 324 " +
			client->getNickname() + " " + target + " " + modes;

		server->sendToClient(client, reply);
		return;
	}

	std::string modeStr = msg.getArg(1);
	bool adding = true;
	size_t argIndex = 2;

	for (size_t i = 0; i < modeStr.size(); i++)
	{
		char c = modeStr[i];

		if (c == '+') { adding = true; continue; }
		if (c == '-') { adding = false; continue; }

		switch (c)
		{
			case 't':
				chan->setTopicRestricted(adding);
				break;

			case 'i':
				chan->setInviteOnly(adding);
				break;

			case 'o':
			{
				if (argIndex >= static_cast<size_t>(msg.getArgCount()))
				{
					server->sendToClient(client, Reply::errNeedMoreParams(client->getNickname(), "MODE").stringify());
					return;
				}
				std::string nick = msg.getArg(argIndex++);
				Client* targetClient = server->findClient(nick);
				if (!targetClient)
				{
					server->sendToClient(client, Reply::errNoSuchNick(client->getNickname(), nick).stringify());
					return;
				}

				if (!chan->hasClient(targetClient))
				{
					server->sendToClient(client, Reply::errUserNotInChannel(client->getNickname(), chan->getName(), nick).stringify());
					return;
				}

				if (adding)
					chan->addOperator(targetClient);
				else
					chan->removeOperator(targetClient);

				break;
			}

			case 'k':
			{
				if (adding)
				{
					if (argIndex >= static_cast<size_t>(msg.getArgCount()))
					{
						server->sendToClient(client, Reply::errNeedMoreParams(client->getNickname(), "MODE +k").stringify());
						return;
					}
					chan->setKey(msg.getArg(argIndex++));
				}
				else
					chan->removeKey();
				break;
			}

			case 'l':
			{
				if (adding)
				{
					if (argIndex >= static_cast<size_t>(msg.getArgCount()))
					{
						server->sendToClient(client, Reply::errNeedMoreParams(client->getNickname(), "MODE +l").stringify());
						return;
					}
					std::string limitStr = msg.getArg(argIndex++);
					int limit = std::atoi(limitStr.c_str());
					if (limit > 0)
					{
						chan->setUserLimit(static_cast<size_t>(limit));
					}
					else
						return;
				}
				else
				{
					// Si es '-l', simplemente borramos el límite
					chan->removeUserLimit();
				}
				break;
			}
		}
	}

	std::string modeMsg = std::string(":") + client->getPrefix() +
	" MODE " + target + " " + modeStr;

		for (size_t i = 2; i < static_cast<size_t>(msg.getArgCount()); i++)
		{
			modeMsg += " " + msg.getArg(i);
		}


	server->broadcastToChannel(chan, modeMsg, client->getFd());
	server->sendToClient(client, modeMsg);
}
