#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include <algorithm>

Channel::Channel(const std::string& name)
    : _name(name),
      _topic(""),
      _inviteOnly(false),
      _topicRestricted(false),
      _hasKey(false),
      _key(""),
      _hasUserLimit(false),
      _userLimit(0),
      _topicSetter(""),
      _topicTime(0)
{
}

Channel::~Channel() {}

const std::string& Channel::getName() const 
{
    return _name;
}

const std::string& Channel::getTopic() const 
{
    return _topic;
}

void Channel::setTopic(const std::string& topic) 
{
    _topic = topic;
}

const std::string& Channel::getTopicSetter() const 
{
    return _topicSetter;
}

time_t Channel::getTopicTime() const 
{
    return _topicTime;
}

void Channel::setTopicSetter(const std::string& setter) 
{
    _topicSetter = setter;
}

void Channel::setTopicTime(time_t t) 
{
    _topicTime = t;
}

void Channel::addClient(Client* client) 
{
    _clients.insert(client);
}

void Channel::removeClient(Client* client) 
{
    _clients.erase(client);
    _operators.erase(client);
    _invited.erase(client);
}

bool Channel::hasClient(Client* client) const 
{
    return _clients.find(client) != _clients.end();
}

const std::set<Client*>& Channel::getClients() const 
{
    return _clients;
}

size_t Channel::getClientCount() const 
{
    return _clients.size();
}

bool Channel::isEmpty() const 
{
    return _clients.empty();
}

void Channel::addOperator(Client* client) 
{
    _operators.insert(client);
}

void Channel::removeOperator(Client* client) 
{
    _operators.erase(client);
}

bool Channel::isOperator(Client* client) const 
{
    return _operators.find(client) != _operators.end();
}

bool Channel::isInviteOnly() const 
{
    return _inviteOnly;
}

void Channel::setInviteOnly(bool value) 
{
    _inviteOnly = value;
}

bool Channel::hasKey() const 
{
    return _hasKey;
}

void Channel::setKey(const std::string& key) 
{
    _hasKey = true;
    _key = key;
}

const std::string& Channel::getKey() const 
{
    return _key;
}

void Channel::removeKey() 
{
    _hasKey = false;
    _key = "";
}

bool Channel::hasUserLimit() const 
{
    return _hasUserLimit;
}

size_t Channel::getUserLimit() const
{
    return _userLimit;
}

void Channel::setUserLimit(size_t limit)
{
    _hasUserLimit = true;
    _userLimit = limit;
}

void Channel::removeUserLimit() 
{
    _hasUserLimit = false;
    _userLimit = 0;
}

bool Channel::isInvited(Client* client) const 
{
    return _invited.find(client) != _invited.end();
}

void Channel::invite(Client* client) 
{
    _invited.insert(client);
}

void Channel::removeInvite(Client* client) 
{
    _invited.erase(client);
}

void Channel::setTopicRestricted(bool b) 
{
    _topicRestricted = b;
}

std::string Channel::getModesString() const
{
    std::string modes = "+";

    if (_inviteOnly)      modes += "i";
    if (_topicRestricted) modes += "t";
    if (_hasKey)          modes += "k";
    if (_hasUserLimit)    modes += "l";

    return modes;
}

//no more zombies
void Channel::notifyQuit(Client& client, Server& server, const std::string& reason) {
    Message quitMsg;
    quitMsg.setPrefix(client.getPrefix())
           .setCommand("QUIT")
           .pushSuffix(reason);
    std::string rawQuit = quitMsg.stringify();

    const std::set<Client*>& recipients = getClients();
    for (std::set<Client*>::const_iterator itCli = recipients.begin(); itCli != recipients.end(); ++itCli) {
        Client* other = *itCli;
        if (other->getFd() != client.getFd())
            server.sendToClient(other, rawQuit);
    }
}