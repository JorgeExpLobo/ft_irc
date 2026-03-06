#include "cmd/NickCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

NickCommand::NickCommand() {}
NickCommand::~NickCommand() {}

void NickCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() == 0)
    {
        Message err = Reply::errNoNicknameGiven(client->getNickname());
        server->sendToClient(client, err.toString());
        return;
    }

    std::string nick = msg.getArg(0);

    if (server->nickExists(nick))
    {
        Message err = Reply::errNicknameInUse(client->getNickname(), nick);
        server->sendToClient(client, err.toString());
        return;
    }

    client->setNickname(nick);
    client->tryRegister();
}