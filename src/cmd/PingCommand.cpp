#include "cmd/PingCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

PingCommand::PingCommand() {}
PingCommand::~PingCommand() {}

void PingCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.argsCount() == 0 && !msg.hasSuffix())
        return; // ERR_NEEDMOREPARAMS opcional

    // El token viene en el primer argumento o en el sufijo
    std::string token = msg.hasSuffix() ? msg.suffix() : msg.arg(0);

    // Formato IRC correcto:
    // :server.name PONG clientNick :token
    std::string reply = std::string(":") + SERVER_NAME + " PONG " + client->getNickname() + " :" + token;

    server->sendToClient(client, reply);
}
