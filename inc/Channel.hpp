#pragma once
#include <string>
#include <set>

// Avoid forward declaration
class Client;

class Channel
{
	private:

		std::string _name;
		std::string _topic;
		std::set<Client*> _clients;
		std::set<Client*> _operators;

		bool _inviteOnly;      // MODE +i
		bool _topicRestricted; // MODE +t
		bool _hasKey;          // MODE +k
		std::string _key;
		bool _hasUserLimit;    // MODE +l
		size_t _userLimit;

		std::set<Client*> _invited;

	public:

		Channel(const std::string& name);
		~Channel();
		const std::string& getName() const;
		const std::string& getTopic() const;
		void setTopic(const std::string& topic);
		void addClient(Client* client);
		void removeClient(Client* client);
		bool hasClient(Client* client) const;
		const std::set<Client*>& getClients() const;

		void addOperator(Client* client);
		void removeOperator(Client* client);
		bool isOperator(Client* client) const;

		bool isInviteOnly() const;
		void setInviteOnly(bool value);

		bool hasKey() const;
		void setKey(const std::string& key);
		const std::string& getKey() const;

		bool hasUserLimit() const;
		size_t getUserLimit() const;
		void setUserLimit(size_t limit);

		bool isInvited(Client* client) const;
		void invite(Client* client);
		void removeInvite(Client* client);
};