#include "PartCommand.hpp"
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
            Reply::errNoSuchChannel(client->getNickname(), channelName).stringify());
        return;
    }


    Message partMsg;
    partMsg.setPrefix(client->getPrefix());
    partMsg.setCommand("PART");
    partMsg.pushArg(channelName);

    if (msg.getArgCount() > 1)
    {
        std::string reason = msg.suffix();
        partMsg.pushSuffix(reason);
    }

    std::string raw = partMsg.stringify();

    server->broadcastToChannel(chan, raw, -1);

    server->removeClientFromChannel(client, channelName);

}