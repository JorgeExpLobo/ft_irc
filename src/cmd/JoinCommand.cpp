#include "JoinCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"
#include "Channel.hpp"

JoinCommand::JoinCommand() {}
JoinCommand::~JoinCommand() {}

void JoinCommand::execute(Server* server, Client* client, const Message& msg)
{
	if (msg.getArgCount() == 0)
	{
		server->sendToClient(client, 
			Reply::errNeedMoreParams(client->getNickname(), "JOIN").stringify());
		return;
	}

	std::string channelName = msg.getArg(0);

	 if (channelName.empty() || channelName[0] != '#')
	{
		server->sendToClient(client,
			Reply::errNoSuchChannel(client->getNickname(), channelName).stringify());
		return;
	}

	Channel* chan = server->getOrCreateChannel(channelName, client);
	
	if (chan->hasClient(client))
		return;

	
    bool isInvited = chan->isInvited(client);

    if (chan->isInviteOnly() && !isInvited)
    {
        server->sendToClient(client, Reply::errInviteOnlyChan(client->getNickname(), channelName).stringify());
        return;
    }
    
    if (!isInvited)
    {
        
        if (chan->hasKey())
        {
            std::string providedKey = (msg.getArgCount() > 1) ? msg.getArg(1) : "";
            if (providedKey != chan->getKey())
            {
                server->sendToClient(client, Reply::errBadChannelKey(client->getNickname(), channelName).stringify());
                return;
            }
        }

        if (chan->hasUserLimit())
        {
            if (chan->getClientCount() >= chan->getUserLimit())
            {
                server->sendToClient(client, Reply::errChannelIsFull(client->getNickname(), channelName).stringify());
                return;
            }
        }
    }

	chan->addClient(client);
	client->joinChannel(chan);

	if (chan->isInviteOnly())
		chan->removeInvite(client);

	if (chan->getClientCount() == 1)
		chan->addOperator(client);

	Message joinNotify;
	joinNotify.setPrefix(client->getPrefix())
			  .setCommand("JOIN")
			  .pushArg(channelName);

	std::string rawJoin = joinNotify.stringify();
	server->sendToClient(client, rawJoin);
	server->broadcastToChannel(chan, rawJoin, client->getFd());

if (chan->getTopic().empty()) 
	{
		server->sendToClient(client,
			Reply::noTopic(client->getNickname(), channelName).stringify());
	} 
	else 
	{

		server->sendToClient(client,
			Reply::topic(client->getNickname(), channelName, chan->getTopic()).stringify());

		Message whoTime;
		std::stringstream ss;
		ss << chan->getTopicTime();
		whoTime.setPrefix(SERVER_NAME)
			.setReplyCode(333)
			.pushArg(client->getNickname())
			.pushArg(channelName)
			.pushArg(chan->getTopicSetter())
			.pushArg(ss.str());
		
		server->sendToClient(client, whoTime.stringify());
	}

	server->sendToClient(client,
		Reply::nameReply(client->getNickname(), *chan).stringify());

	server->sendToClient(client,
		Reply::endOfNames(client->getNickname(), channelName).stringify());
}
