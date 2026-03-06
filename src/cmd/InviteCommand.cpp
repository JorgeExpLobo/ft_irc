#include "cmd/InviteCommand.hpp"
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

    if (!chan || !target)
        return;

    chan->invite(target);

    server->sendToClient(client,
        Reply::inviting(client->getNickname(), nick, channelName).toString());
}