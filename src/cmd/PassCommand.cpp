#include "cmd/PassCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

PassCommand::PassCommand() {}
PassCommand::~PassCommand() {}

void PassCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() == 0)
    {
        Message err = Reply::errNeedMoreParams(client->getNickname(), "PASS");
        server->sendToClient(client, err.toString());
        return;
    }

    std::string pass = msg.getArg(0);

    if (pass != server->getPassword())
    {
        Message err = Reply::errPassWdMissMatch(client->getNickname());
        server->sendToClient(client, err.toString());
        return;
    }

    client->setHasPass(true);
    client->tryRegister();
}