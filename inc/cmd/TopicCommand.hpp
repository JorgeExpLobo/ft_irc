#pragma once

#include "../ACommand.hpp"

class TopicCommand : public ACommand
{
public:
	TopicCommand();
	~TopicCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};