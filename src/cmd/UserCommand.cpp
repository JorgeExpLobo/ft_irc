#include "cmd/UserCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"

UserCommand::UserCommand() {}
UserCommand::~UserCommand() {}

void UserCommand::execute(Server* server, Client* client, const Message& msg)
{
    
    if (client->isRegistered())
    {
        server->sendToClient(client,
            Reply::errAlreadyRegistered(client->getNickname()).stringify());
        return;
    }

   
    if (msg.getArgCount() < 4)
    {
        server->sendToClient(client,
            Reply::errNeedMoreParams(client->getNickname(), "USER").stringify());
        return;
    }

    
    client->setUsername(msg.getArg(0));
    client->setRealName(msg.suffix());

    
    client->tryRegister(server);

   
    if (client->isRegistered())
    {
        std::cout << "[REGISTER] "
                  << "nick=" << client->getNickname()
                  << " user=" << client->getUsername()
                  << " realName=" << client->getRealName()
                  << " host=" << client->getHost()
                  << std::endl;

        server->sendToClient(client, Reply::welcome(client->getNickname(), client->getUsername(), client->getHost()).stringify());

    }
}