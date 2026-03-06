#include "cmd/AwayCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

AwayCommand::AwayCommand() {}
AwayCommand::~AwayCommand() {}

void AwayCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.suffix().empty())
    {
        client->removeAway();
        server->sendToClient(client, Reply::unaway().toString());
    }
    else
    {
        client->setAway(msg.suffix());
        server->sendToClient(client, Reply::nowAway().toString());
    }
}