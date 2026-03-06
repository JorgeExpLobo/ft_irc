#include "../inc/Client.hpp"
#include "../inc/Channel.hpp"

Client::Client() 
		: _fd(-1),
		_has_pass(false),
		_has_nick(false),
		_has_user(false),
		_registered(false),
		_away(false)
{
}

Client::Client(int fd, const std::string& host)
		: _fd(fd),
		_host(host),
		_has_pass(false),
		_has_nick(false),
		_has_user(false),
		_registered(false),
		_away(false)
{
}

Client::~Client() {}

int Client::getFd() const
{
	return _fd;
}

void Client::setFd(int fd)
{
	_fd = fd;
}

const std::string& Client::getNickname() const
{
	return _nickname;
}

const std::string& Client::getUsername() const
{
	return _username;
}

const std::string& Client::getHost() const
{
	return _host;
}

void Client::setNickname(const std::string& nick)
{
	_nickname = nick;
	_has_nick = true;
}

void Client::setUsername(const std::string& user)
{
	_username = user;
	_has_user = true;
}

void Client::setHasPass(bool value)
{
	_has_pass = value;
}

void Client::tryRegister()
{
	if (_has_pass && _has_nick && _has_user)
		_registered = true;
}

bool Client::isRegistered() const
{
	return _registered;
}

void Client::appendBuffer(const std::string& data)
{
	_buffer += data;
}

std::string& Client::getBuffer()
{
	return _buffer;
}

std::string Client::getPrefix() const
{
	return _nickname + "!" + _username + "@" + _host;
}

void Client::joinChannel(Channel* channel)
{
	_channels.insert(channel);
}

void Client::leaveChannel(Channel* channel)
{
	_channels.erase(channel);
}

const std::set<Channel*>& Client::getChannels() const
{
	return _channels;
}

bool Client::isAway() const
{
	return _away;
}

const std::string& Client::getAwayMessage() const
{
	return _awayMessage;
}

void Client::setAway(const std::string& msg)
{
	_away = true;
	_awayMessage = msg;
}

void Client::removeAway()
{
	_away = false;
	_awayMessage.clear();
}