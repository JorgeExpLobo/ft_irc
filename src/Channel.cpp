#include "../inc/Channel.hpp"
#include "../inc/Client.hpp"

Channel::Channel(const std::string& name)
	: _name(name),
	  _inviteOnly(false),
	  _topicRestricted(false),
	  _hasKey(false),
	  _hasUserLimit(false),
	  _userLimit(0)
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

void Channel::addClient(Client* client)
{
	_clients.insert(client);
}

void Channel::removeClient(Client* client)
{
	_clients.erase(client);
	_operators.erase(client);
}

bool Channel::hasClient(Client* client) const
{
	return _clients.find(client) != _clients.end();
}

const std::set<Client*>& Channel::getClients() const
{
	return _clients;
}

bool Channel::isEmpty() const
{
	return _clients.empty();
}

size_t Channel::getClientCount() const
{
	return _clients.size();
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
	_key = key;
	_hasKey = true;
}

const std::string& Channel::getKey() const
{
	return _key;
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
	_userLimit = limit;
	_hasUserLimit = true;
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