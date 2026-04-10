#pragma once
#include <string>
#include <set>
#include <ctime>
#include "Server.hpp"

class Client;

class Channel
{
private:

    std::string _name;
    std::string _topic;
    std::set<Client*> _clients;
    std::set<Client*> _operators;
    std::set<Client*> _invited;

    bool _inviteOnly;      // MODE +i
    bool _topicRestricted; // MODE +t
    bool _hasKey;          // MODE +k
    std::string _key;
    bool _hasUserLimit;    // MODE +l
    size_t _userLimit;

    std::string _topicSetter;
    time_t _topicTime;

public:

    Channel(const std::string& name);
    ~Channel();

    const std::string& getName() const;

    const std::string& getTopic() const;
    void setTopic(const std::string& topic);

    const std::string& getTopicSetter() const;
    time_t getTopicTime() const;
    void setTopicSetter(const std::string& setter);
    void setTopicTime(time_t t);

    void addClient(Client* client);
    void removeClient(Client* client);
    bool hasClient(Client* client) const;
    const std::set<Client*>& getClients() const;
    size_t getClientCount() const;
    bool isEmpty() const;

    void addOperator(Client* client);
    void removeOperator(Client* client);
    bool isOperator(Client* client) const;

    bool isInviteOnly() const;
    void setInviteOnly(bool value);

    bool hasKey() const;
    void setKey(const std::string& key);
    const std::string& getKey() const;
    void removeKey();

    bool hasUserLimit() const;
    size_t getUserLimit() const;
    void setUserLimit(size_t limit);
    void removeUserLimit();

    bool isInvited(Client* client) const;
    void invite(Client* client);
    void removeInvite(Client* client);

    void setTopicRestricted(bool b);
	std::string getModesString() const;

    void notifyQuit(Client& client, Server& server, const std::string& reason);
};
