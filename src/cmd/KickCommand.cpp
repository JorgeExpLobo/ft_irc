#include "cmd/KickCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

KickCommand::KickCommand() {}
KickCommand::~KickCommand() {}

void KickCommand::execute(Server* server, Client* client, const Message& msg)
{
    
    if (msg.getArgCount() < 2) 
    {
        server->sendToClient(client, Reply::errNeedMoreParams(client->getNickname(), "KICK").stringify());
        return;
    }

    std::string channelName = msg.getArg(0);
    std::string targetNick = msg.getArg(1);
    std::string reason = msg.hasSuffix() ? msg.suffix() : client->getNickname();

    Channel* chan = server->findChannel(channelName);
    Client* target = server->findClient(targetNick);

    
    if (!chan) 
    {
        server->sendToClient(client, Reply::errNoSuchChannel(client->getNickname(), channelName).stringify());
        return;
    }
    if (!target || !chan->hasClient(target))
    {
        server->sendToClient(client, Reply::errUserNotInChannel(client->getNickname(), channelName, targetNick).stringify());
        return;
    }
    if (!chan->isOperator(client)) 
    {
        server->sendToClient(client, Reply::errChanOpIsNeeded(client->getNickname(), channelName).stringify());
        return;
    }

    Message kickMsg;
    kickMsg.setPrefix(client->getPrefix())
           .setCommand("KICK")
           .pushArg(channelName)
           .pushArg(targetNick)
           .pushSuffix(reason);

    std::string rawKick = kickMsg.stringify();

    
    server->sendToClient(target, rawKick); 
    server->broadcastToChannel(chan, rawKick, target->getFd());

   
    chan->removeClient(target);
    target->leaveChannel(chan);

    std::cout << "[KICK] " << client->getNickname() << " has kicked " << targetNick << " from " << channelName << "\n";
}