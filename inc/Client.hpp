#pragma once
#include <string>
#include <set>

// Avoid forward declaration
class Channel;

class Client
{
	private:
		int _fd;
		std::string _buffer;

		std::string _nickname;
		std::string _username;
		std::string _host;

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

		void setNickname(const std::string& nick);
		void setUsername(const std::string& user);
		void setHasPass(bool value);
		void tryRegister();
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
};