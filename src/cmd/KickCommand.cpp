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

    // Debug: punteros y tamaños antes
    std::cout << "[DEBUG] KICK from " << client->getNickname()
              << " -> " << target->getNickname() << " in channel " << channelName << "\n";
    std::cout << "[DEBUG] channel clients before: " << chan->getClients().size() << "\n";
    std::cout << "[DEBUG] target channels before: " << target->getChannels().size() << "\n";

    if (!chan->isOperator(client))
    {
        server->sendToClient(client,
            Reply::errChanOpIsNeeded(client->getNickname(), channelName).toString());
        return;
    }

    chan->removeClient(target);
    target->leaveChannel(chan);

    // Debug: tamaños después
    std::cout << "[DEBUG] channel clients after: " << chan->getClients().size() << "\n";
    std::cout << "[DEBUG] target channels after: " << target->getChannels().size() << "\n";

    server->broadcastToChannel(chan,
        ":" + client->getPrefix() + " KICK " + channelName + " " + nick, client->getFd());
}