#include "InviteCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

InviteCommand::InviteCommand() {}
InviteCommand::~InviteCommand() {}

void InviteCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() < 2)
        return;

    std::string nick = msg.getArg(0);
    std::string channelName = msg.getArg(1);

    Channel* chan = server->findChannel(channelName);
    Client* target = server->findClient(nick);

    if (!chan) {
        server->sendToClient(client, Reply::errNoSuchChannel(client->getNickname(), channelName).stringify());
        return;
    }

    if (!target) {
        server->sendToClient(client, Reply::errNoSuchNick(client->getNickname(), nick).stringify());
        return;
    }

    if (!chan->hasClient(client)) {
        server->sendToClient(client, Reply::errNotOnChannel(client->getNickname(), channelName).stringify());
        return;
    }

    if (chan->hasClient(target)) {
        server->sendToClient(client, Reply::errUserOnChannel(client->getNickname(), nick, channelName).stringify());
        return;
    }

    if (chan->isInviteOnly() && !chan->isOperator(client)) {
        server->sendToClient(client, Reply::errChanOPrivsNeeded(client->getNickname(), channelName).stringify());
        return;
    }

    chan->invite(target);

    server->sendToClient(target,
        Reply::invite(client->getPrefix(), channelName).stringify());

    server->sendToClient(client,
        Reply::inviting(client->getNickname(), nick, channelName).stringify());
}