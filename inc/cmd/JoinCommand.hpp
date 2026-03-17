#pragma once

#include "../ACommand.hpp"
#include <sstream>

class JoinCommand : public ACommand
{
public:
	JoinCommand();
	~JoinCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};