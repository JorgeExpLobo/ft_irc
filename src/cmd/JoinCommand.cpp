#include "cmd/JoinCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"
#include "Channel.hpp"

JoinCommand::JoinCommand() {}
JoinCommand::~JoinCommand() {}

void JoinCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() == 0)
    {
        Message err = Reply::errNeedMoreParams(client->getNickname(), "JOIN");
        server->sendToClient(client, err.toString());
        return;
    }

    std::string channelName = msg.getArg(0);

    Channel* chan = server->getOrCreateChannel(channelName, client);

    chan->addClient(client);
    client->joinChannel(chan);

    if (chan->getClientCount() == 1)
        chan->addOperator(client);

    std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + channelName;
    server->sendToClient(client, joinMsg);

    server->broadcastToChannel(chan, joinMsg, client->getFd());


    server->sendToClient(client,
        Reply::nameReply(client->getNickname(), *chan).toString());

    server->sendToClient(client,
        Reply::endOfNames(client->getNickname(), channelName).toString());
}
