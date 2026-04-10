#include "PassCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

PassCommand::PassCommand() {}
PassCommand::~PassCommand() {}

void PassCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() == 0)
    {
        server->sendToClient(
            client,
            Reply::errNeedMoreParams(client->getNickname(), "PASS").stringify()
        );
        return;
    }

    std::string pass = msg.getArg(0);

    if (pass != server->getPassword())
    {
        client->setHasPass(false);
        server->sendToClient(client,
            Reply::errPassWdMissMatch(client->getNickname()).stringify());

        return;
    }

    client->setHasPass(true);
    client->tryRegister(server);
}