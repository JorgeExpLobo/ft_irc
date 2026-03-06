#include "cmd/TopicCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

TopicCommand::TopicCommand() {}
TopicCommand::~TopicCommand() {}

void TopicCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() == 0)
        return;

    std::string channelName = msg.getArg(0);

    Channel* chan = server->findChannel(channelName);

    if (!chan)
        return;

    if (msg.suffix().empty())
    {
        server->sendToClient(client,
            Reply::topic(client->getNickname(), channelName, chan->getTopic()).toString());
    }
    else
    {
        chan->setTopic(msg.suffix());

        server->broadcastToChannel(chan,
            ":" + client->getPrefix() + " TOPIC " + channelName + " :" + msg.suffix(), client->getFd());
    }
}