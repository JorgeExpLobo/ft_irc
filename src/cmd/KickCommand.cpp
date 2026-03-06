#include "cmd/KickCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

KickCommand::KickCommand() {}
KickCommand::~KickCommand() {}

void KickCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() < 2)
        return;

    std::string channelName = msg.getArg(0);
    std::string nick = msg.getArg(1);

    Channel* chan = server->findChannel(channelName);
    Client* target = server->findClient(nick);

    if (!chan || !target)
        return;

    if (!chan->isOperator(client))
    {
        server->sendToClient(client,
            Reply::errChanOpIsNeeded(client->getNickname(), channelName).toString());
        return;
    }

    chan->removeClient(target);
    target->leaveChannel(chan);

    server->broadcastToChannel(chan,
        ":" + client->getPrefix() + " KICK " + channelName + " " + nick, client->getFd());
}