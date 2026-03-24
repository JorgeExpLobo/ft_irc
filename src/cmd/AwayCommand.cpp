#include "AwayCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

AwayCommand::AwayCommand() {}
AwayCommand::~AwayCommand() {}

void AwayCommand::execute(Server* server, Client* client, const Message& msg)
{
    if  (!msg.hasSuffix() || msg.suffix().empty())
    {
        client->removeAway();
        server->sendToClient(client, Reply::unaway(client->getNickname()).stringify());
    }
    else
    {
        client->setAway(msg.suffix());
        server->sendToClient(client, Reply::nowAway(client->getNickname()).stringify());
    }

}