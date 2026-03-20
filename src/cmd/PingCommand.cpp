
#include "Server.hpp"
#include "Reply.hpp"
#include "PingCommand.hpp"

PingCommand::PingCommand() {}
PingCommand::~PingCommand() {}

void PingCommand::execute(Server* server, Client* client, const Message& msg)
{
    
    if (msg.argsCount() == 0 && !msg.hasSuffix())
    {
        server->sendToClient(client, Reply::errNoOrigin(client->getNickname()).stringify());
        return;
    }

    std::string token = msg.hasSuffix() ? msg.suffix() : msg.arg(0);

    Message pong;
    pong.setPrefix(SERVER_NAME)   
        .setCommand("PONG")
        //.pushArg(SERVER_NAME)
        .pushSuffix(token);

    server->sendToClient(client, pong.stringify());
}
