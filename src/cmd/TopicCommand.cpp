#include "TopicCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"
#include <sstream>
#include <ctime>

TopicCommand::TopicCommand() {}
TopicCommand::~TopicCommand() {}

void TopicCommand::execute(Server* server, Client* client, const Message& msg)
{
    if (msg.getArgCount() < 1) 
    {
        server->sendToClient(client, Reply::errNeedMoreParams(client->getNickname(), "TOPIC").stringify());
        return;
    }

    std::string channelName = msg.getArg(0);
    Channel* chan = server->findChannel(channelName);

    if (!chan) 
    {
        server->sendToClient(client, Reply::errNoSuchChannel(client->getNickname(), channelName).stringify());
        return;
    }

    if (!msg.hasSuffix())
    {
        if (chan->getTopic().empty()) 
        {
            server->sendToClient(client, Reply::noTopic(client->getNickname(), channelName).stringify());
        } 
        else 
        {
            server->sendToClient(client, Reply::topic(client->getNickname(), channelName, chan->getTopic()).stringify());
            
            Message whoTime;
            std::stringstream ss;
            ss << chan->getTopicTime();
            whoTime.setPrefix(SERVER_NAME).setReplyCode(333)
                   .pushArg(client->getNickname()).pushArg(channelName)
                   .pushArg(chan->getTopicSetter()).pushArg(ss.str());
            server->sendToClient(client, whoTime.stringify());
        }
        return;
    }

    if (!chan->isOperator(client)) 
    {
        server->sendToClient(client, Reply::errChanOpIsNeeded(client->getNickname(), channelName).stringify());
        return;
    }

    std::string newTopic = msg.suffix();
    chan->setTopic(newTopic);
    chan->setTopicSetter(client->getNickname());
    chan->setTopicTime(time(NULL));
    
    Message topicNotify;
    topicNotify.setPrefix(client->getPrefix())
               .setCommand("TOPIC")
               .pushArg(channelName)
               .pushSuffix(newTopic);

    std::string rawNotify = topicNotify.stringify();
    server->sendToClient(client, rawNotify);
    server->broadcastToChannel(chan, rawNotify, client->getFd());

    server->sendToClient(client, Reply::topic(client->getNickname(), channelName, newTopic).stringify());

    Message whoTime;
    std::stringstream ss;
    ss << chan->getTopicTime();

    whoTime.setPrefix(SERVER_NAME)
           .setReplyCode(333)
           .pushArg(client->getNickname())
           .pushArg(channelName)
           .pushArg(client->getNickname()) 
           .pushArg(ss.str());  

    server->sendToClient(client, whoTime.stringify());
}
