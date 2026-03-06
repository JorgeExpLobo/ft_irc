#include "cmd/UserCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

UserCommand::UserCommand() {}
UserCommand::~UserCommand() {}

void UserCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() < 4)
    {
        Message err = Reply::errNeedMoreParams(client->getNickname(), "USER");
        server->sendToClient(client, err.toString());
        return;
    }

    client->setUsername(msg.getArg(0));
    client->tryRegister();

    if (client->isRegistered())
    {
		std::cout << "[REGISTER] "
              << "nick=" << client->getNickname()
              << " user=" << client->getUsername()
              << " host=" << client->getHost()
              << std::endl;
        Message welcome = Reply::welcome(
            client->getNickname(),
            client->getUsername(),
            client->getHost());

        server->sendToClient(client, welcome.toString());
    }
}