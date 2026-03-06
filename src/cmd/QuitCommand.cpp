#include "cmd/QuitCommand.hpp"
#include "Server.hpp"

QuitCommand::QuitCommand() {}
QuitCommand::~QuitCommand() {}

void QuitCommand::execute(Server* server, Client* client, const Message& msg)
{
    std::string reason = "Client Quit";

    if (!msg.suffix().empty())
        reason = msg.suffix();

    server->terminateClientConnection(client->getFd());
}