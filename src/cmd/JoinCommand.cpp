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
        server->sendToClient(client, 
            Reply::errNeedMoreParams(client->getNickname(), "JOIN").stringify() + "\r\n");
        return;
    }

    std::string channelName = msg.getArg(0);

    Channel* chan = server->getOrCreateChannel(channelName, client);

    chan->addClient(client);
    client->joinChannel(chan);

    if (chan->getClientCount() == 1)
        chan->addOperator(client);

    Message joinNotify;
    joinNotify.setPrefix(client->getPrefix())
              .setCommand("JOIN")
              .pushSuffix(channelName);

    std::string rawJoin = joinNotify.stringify() + "\r\n";

    server->sendToClient(client, rawJoin);
    server->broadcastToChannel(chan, rawJoin, client->getFd());

    server->sendToClient(client, 
        Reply::nameReply(client->getNickname(), *chan).stringify() + "\r\n");

    server->sendToClient(client, 
        Reply::endOfNames(client->getNickname(), channelName).stringify() + "\r\n");
}
