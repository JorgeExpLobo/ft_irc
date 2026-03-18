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
        server->sendToClient(client, err.stringify());
        return;
    }

    std::string nick = msg.getArg(0);

    if (server->nickExists(nick))
    {
        Message err = Reply::errNicknameInUse(client->getNickname(), nick);
        server->sendToClient(client, err.stringify());
        return;
    }

    std::string oldNick = client->getNickname();

    client->setNickname(nick);

    std::cout << "[NICK CHANGE] " << oldNick << " -> " << nick << std::endl;

    Message nickUpdate;
    nickUpdate.setPrefix(oldNick + "!" + client->getUsername() + "@" + client->getHost())
              .setCommand("NICK")
              .pushSuffix(nick);

    std::string finalMsg = nickUpdate.stringify();

    server->sendToClient(client, finalMsg);

    const std::map<int, Client*>& clients = server->getClients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        Client* other = it->second;
        if (other != client)
            server->sendToClient(other, finalMsg);
    }

    client->tryRegister(server);
}