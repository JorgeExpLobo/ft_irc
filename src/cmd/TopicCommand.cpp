#include "cmd/TopicCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"
#include <sstream>
#include <ctime>

TopicCommand::TopicCommand() {}
TopicCommand::~TopicCommand() {}

void TopicCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() == 0)
        return;

    std::string channelName = msg.getArg(0);
    Channel* chan = server->findChannel(channelName);

    if (!chan)
        return;

   
    if (msg.suffix().empty())
    {
        server->sendToClient(client,
            Reply::topic(client->getNickname(), channelName, chan->getTopic()).toString());

        // Enviar también el WHO/TIME (333)
        std::stringstream ss;
        ss << chan->getTopicSetter() << " " << chan->getTopicTime();

        server->sendToClient(client,
            std::string(":") + SERVER_NAME + " 333 " + client->getNickname() +
            " " + channelName + " " + ss.str());

        return;
    }

    std::string newTopic = msg.suffix();
    chan->setTopic(newTopic);
    chan->setTopicSetter(client->getNickname());
    chan->setTopicTime(time(NULL));

   
    std::string topicMsg =
        ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic;
    server->sendToClient(client, topicMsg);

 
    server->broadcastToChannel(chan, topicMsg, client->getFd());

    // Envia RPL_TOPIC (332)
    server->sendToClient(client,
        std::string(":") + SERVER_NAME + " 332 " + client->getNickname() +
        " " + channelName + " :" + newTopic);

    // Envia RPL_TOPICWHOTIME (333)
    std::stringstream ss;
    ss << client->getNickname() << " " << chan->getTopicTime();

    server->sendToClient(client,
        std::string(":") + SERVER_NAME + " 333 " + client->getNickname() +
        " " + channelName + " " + ss.str());
}
