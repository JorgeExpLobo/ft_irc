#include "cmd/PartCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

PartCommand::PartCommand() {}
PartCommand::~PartCommand() {}

void PartCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() == 0)
        return;

    std::string channelName = msg.getArg(0);

    Channel* chan = server->getChannel(channelName);

    if (!chan)
    {
        server->sendToClient(client,
            Reply::errNoSuchChannel(client->getNickname(), channelName).toString());
        return;
    }

    chan->removeClient(client);
    client->leaveChannel(chan);

    server->broadcastToChannel(chan,
        ":" + client->getPrefix() + " PART " + channelName, client->getFd());
}