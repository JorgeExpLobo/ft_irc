#include "cmd/PrivmsgCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

PrivmsgCommand::PrivmsgCommand() {}
PrivmsgCommand::~PrivmsgCommand() {}

void PrivmsgCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() < 2)
    {
        server->sendToClient(client,
            Reply::errNoTextToSend(client->getNickname()).toString());
        return;
    }

    std::string target = msg.getArg(0);
    std::string text = msg.suffix();

    if (target[0] == '#')
    {
        Channel* chan = server->findChannel(target);

        if (!chan)
        {
            server->sendToClient(client,
                Reply::errNoSuchChannel(client->getNickname(), target).toString());
            return;
        }

        server->broadcastToChannel(chan,
            ":" + client->getPrefix() + " PRIVMSG " + target + " :" + text, client->getFd());
    }
    else
    {
        Client* dest = server->findClient(target);

        if (!dest)
        {
            server->sendToClient(client,
                Reply::errNoSuchNick(client->getNickname(), target).toString());
            return;
        }

        server->sendToClient(dest,
            ":" + client->getPrefix() + " PRIVMSG " + target + " :" + text);
    }
}