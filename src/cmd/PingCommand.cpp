#include "cmd/PingCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"


PingCommand::PingCommand() {}
PingCommand::~PingCommand() {}

void PingCommand::execute(Server* server, Client* client, const Message& msg)
{
    (void)server;

    if (msg.argsCount() == 0 && !msg.hasSuffix())
    {
        // Opcional: enviar un ERR_NEEDMOREPARAMS (461)
        return;
    }

    // El identificador suele ser el primer argumento o el sufijo
    std::string token = msg.hasSuffix() ? msg.suffix() : msg.arg(0);

    
    Message res = Reply::pong(SERVER_NAME, token);

    
    std::string out = res.stringify() + "\r\n";
    send(client->getFd(), out.c_str(), out.size(), 0);
}