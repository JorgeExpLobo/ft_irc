#include "PrivmsgCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

PrivmsgCommand::PrivmsgCommand() {}
PrivmsgCommand::~PrivmsgCommand() {}




std::vector<std::string> split(const std::string& str, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delim))
        result.push_back(item);

    return result;
}

void PrivmsgCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() < 1)
    {
        server->sendToClient(client,
            Reply::errNoRecipient(client->getNickname(), "PRIVMSG").stringify());
        return;
    }

    if (!msg.hasSuffix() || msg.suffix().empty())
    {
        server->sendToClient(client,
            Reply::errNoTextToSend(client->getNickname()).stringify());
        return;
    }

    std::string text = msg.suffix();
    std::vector<std::string> targets = split(msg.getArg(0), ',');

    for (size_t i = 0; i < targets.size(); ++i)
    {
        const std::string& target = targets[i];

        if (target.empty())
            continue;

        Message out;
        out.setPrefix(client->getPrefix())
           .setCommand("PRIVMSG")
           .pushArg(target)
           .pushSuffix(text);

        std::string raw = out.stringify();

        // CANAL 
        if (target[0] == '#')
        {
            Channel* chan = server->findChannel(target);

            if (!chan)
            {
                server->sendToClient(client,
                    Reply::errNoSuchChannel(client->getNickname(), target).stringify());
                continue;
            }

            if (!chan->hasClient(client))
            {
                server->sendToClient(client,
                    Reply::errCannotSendToChan(client->getNickname(), target).stringify());
                continue;
            }

            server->broadcastToChannel(chan, raw, client->getFd());
        }
        // USUARIO 
        else
        {
            Client* dest = server->findClient(target);

            if (!dest)
            {
                server->sendToClient(client,
                    Reply::errNoSuchNick(client->getNickname(), target).stringify());
                continue;
            }

            server->sendToClient(dest, raw);

            // AWAY
            if (dest->isAway())
            {
                server->sendToClient(client, Reply::away(client->getNickname(),dest->getNickname(), dest->getAwayMessage()).stringify());
            }
        }
    }
}