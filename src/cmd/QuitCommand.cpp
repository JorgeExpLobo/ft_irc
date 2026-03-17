#include "cmd/QuitCommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

QuitCommand::QuitCommand() {}
QuitCommand::~QuitCommand() {}

void QuitCommand::execute(Server* server, Client* client, const Message& msg)
{
    std::string reason = msg.hasSuffix() ? msg.suffix() : "Client Quit";

    Message quitMsg;
    quitMsg.setPrefix(client->getPrefix())
           .setCommand("QUIT")
           .pushSuffix(reason);

    std::string rawQuit = quitMsg.stringify();

    
    const std::set<Channel*>& channels = client->getChannels();
    std::set<int> notifiedFds;

    for (std::set<Channel*>::const_iterator itChan = channels.begin(); itChan != channels.end(); ++itChan)
    {
        Channel* chan = *itChan;
        const std::set<Client*>& recipients = chan->getClients();
        
        for (std::set<Client*>::const_iterator itCli = recipients.begin(); itCli != recipients.end(); ++itCli)
        {
            Client* other = *itCli;
            int targetFd = other->getFd();

            if (targetFd != client->getFd() && notifiedFds.find(targetFd) == notifiedFds.end())
            {
                server->sendToClient(other, rawQuit);
                notifiedFds.insert(targetFd);
            }
        }
    }

    server->terminateClientConnection(client->getFd());
}