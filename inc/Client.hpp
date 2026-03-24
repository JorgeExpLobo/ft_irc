#pragma once
#include <iostream>
#include <string>
#include <set>


class Channel;
class Server;

class Client
{
	private:
		int _fd;
		std::string _buffer;

		std::string _nickname;
		std::string _username;
		std::string _host;
		std::string _realName;

		bool _has_pass;
		bool _has_nick;
		bool _has_user;
		bool _registered;

		std::set<Channel*> _channels;

		bool _away;
		std::string _awayMessage;

	public:
		Client();
		Client(int fd, const std::string& host);
		~Client();

		void setFd(int fd);
		int getFd() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getHost() const;
		bool getHasPass() const;

		void setNickname(const std::string& nick);
		void setUsername(const std::string& user);
		void setHasPass(bool value);
		void tryRegister(Server* server);
		bool isRegistered() const;

		void appendBuffer(const std::string& data);
		std::string& getBuffer();

		std::string getPrefix() const;

		void joinChannel(Channel* channel);
		void leaveChannel(Channel* channel);
		const std::set<Channel*>& getChannels() const;

		bool isAway() const;
		const std::string& getAwayMessage() const;
		void setAway(const std::string& msg);
		void removeAway();
		void setRealName(const std::string& realName);
    	const std::string& getRealName() const;
};