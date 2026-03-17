#include "cmd/PrivmsgCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

PrivmsgCommand::PrivmsgCommand() {}
PrivmsgCommand::~PrivmsgCommand() {}

void PrivmsgCommand::execute(Server* server, Client* client, const Message& msg)
{
    
    if (msg.getArgCount() < 1) 
    {
        server->sendToClient(client, Reply::errNoRecipient(client->getNickname(), "PRIVMSG").stringify());
        return;
    }
    
    if (!msg.hasSuffix() || msg.suffix().empty()) 
    {
        server->sendToClient(client, Reply::errNoTextToSend(client->getNickname()).stringify());
        return;
    }

    std::string target = msg.getArg(0);
    std::string text = msg.suffix();


    Message out;
    out.setPrefix(client->getPrefix())
       .setCommand("PRIVMSG")
       .pushArg(target)
       .pushSuffix(text);

    std::string raw = out.stringify();


    if (target[0] == '#')
    {
        Channel* chan = server->findChannel(target);
        if (!chan) 
        {
            server->sendToClient(client, Reply::errNoSuchChannel(client->getNickname(), target).stringify());
            return;
        }

        
        if (!chan->hasClient(client)) 
        {
            server->sendToClient(client, Reply::errCannotSendToChan(client->getNickname(), target).stringify());
            return;
        }

        server->broadcastToChannel(chan, raw, client->getFd());
    }
    
    else
    {
        Client* dest = server->findClient(target);
        if (!dest) 
        {
            server->sendToClient(client, Reply::errNoSuchNick(client->getNickname(), target).stringify());
            return;
        }

        server->sendToClient(dest, raw);
    }
}